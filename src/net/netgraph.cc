/**
	\file "net/netgraph.cc"
	$Id: netgraph.cc,v 1.37 2011/05/03 19:20:53 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <sstream>		// for ostringstream
#include <iterator>		// for ostream_iterator
#include "net/netgraph.hh"
#include "net/netlist_options.hh"
#include "Object/global_entry.hh"
#include "Object/global_channel_entry.hh"
#include "Object/global_entry_context.hh"
#include "Object/def/footprint.hh"
#include "Object/def/process_definition.hh"
#include "Object/traits/instance_traits.hh"
#include "Object/lang/PRS_footprint.hh"
#include "Object/inst/physical_instance_placeholder.hh"
#include "Object/inst/physical_instance_collection.hh"
#include "Object/inst/port_alias_tracker.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/alias_actuals.hh"
#include "Object/inst/bool_port_collector.tcc"
#include "Object/inst/state_instance.hh"
#include "Object/inst/instance_pool.hh"
#if NETLIST_VERILOG
#include "Object/inst/collection_interface.hh"
#include "Object/type/canonical_type.hh"
#endif
#include "util/iterator_more.hh"		// for set_inserter
#include "util/unique_list.tcc"
#include "util/string.hh"		// for strgsub
#include "util/stacktrace.hh"
#include "util/indent.hh"

namespace HAC {
namespace NET {
#include "util/using_ostream.hh"
using entity::footprint_frame_map_type;
using entity::meta_type_port_collector;
using entity::class_traits;
using entity::bool_tag;
using entity::port_formals_manager;
using entity::process_definition;
using entity::physical_instance_collection;
using entity::port_alias_tracker;
using entity::port_alias_tracker_base;
using entity::alias_reference_set;
using entity::instance_alias_info;
using std::ostringstream;
using std::ostream_iterator;
using entity::directive_base_params_type;
using entity::preal_value_type;
using util::unique_list;
using util::strings::strgsub;
using util::memory::never_ptr;

//=============================================================================
bool
device_group::is_empty(void) const {
	return transistor_pool.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Marks whichnodes are used/driven from a transistor's perspective,
	and also constructs the (redundant) node-terminal information.
 */
void
device_group::mark_used_nodes(node_pool_type& node_pool) const {
	transistor_pool_type::const_iterator
		i(transistor_pool.begin()), e(transistor_pool.end());
	size_t ti = 0;
	for ( ; i!=e; ++i, ++ti) {
		i->mark_used_nodes(node_pool);
#if NETLIST_NODE_GRAPH
		i->mark_node_terminals(node_pool, ti);
#endif
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
device_group::add_transistor(const transistor& t) {
#if NETLIST_CHECK_NAME_COLLISIONS
	// only check user-overridden names, not generated ones
if (t.name.length()) {
	const pair<name_set_type::iterator, bool> p(names.insert(t.name));
	if (!p.second) {
		cerr << "Error: transistor name \"" << t.name <<
			"\" was already used, and cannot be re-used."
			<< endl;
		THROW_EXIT;
	}
}
#endif
	transistor_pool.push_back(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_CACHE_PARASITICS
/**
	Only after netlist subgraph is complete, can parasitics be
	accurately evaluated.  Need to know which nodes are end nodes.
 */
void
device_group::summarize_parasitics(node_pool_type& node_pool, 
		const netlist_options& nopt) {
	typedef	transistor_pool_type::iterator	iterator;
	iterator i(transistor_pool.begin()), e(transistor_pool.end());
	for ( ; i!=e; ++i) {
		node& s(node_pool[i->source]);
		node& d(node_pool[i->drain]);
		const bool s_ext = s.is_stack_end_node();
		const bool d_ext = d.is_stack_end_node();
		i->parasitic_values.update(*i, s_ext, d_ext, nopt);
#if NETLIST_NODE_CAPS
		node& g(node_pool[i->gate]);
		const transistor::parasitics& p(i->parasitic_values);
		g.cap.gate_area += i->gate_area();
		++g.cap.gate_terms;
		const bool N = (i->is_NFET());
		if (N) {
			s.cap.ndiff_area += p.source_area;
			s.cap.ndiff_perimeter += p.source_perimeter;
			d.cap.ndiff_area += p.drain_area;
			d.cap.ndiff_perimeter += p.drain_perimeter;
			++s.cap.ndrain_terms;
			++d.cap.ndrain_terms;
		} else {
			s.cap.pdiff_area += p.source_area;
			s.cap.pdiff_perimeter += p.source_perimeter;
			d.cap.pdiff_area += p.drain_area;
			d.cap.pdiff_perimeter += p.drain_perimeter;
			++s.cap.pdrain_terms;
			++d.cap.pdrain_terms;
		}
#endif
	}
}
#endif

//=============================================================================
// class netlist_common method definitions

netlist_common::netlist_common() : device_group(), 
		node_pool(), 
		passive_device_pool() {
	node_pool.reserve(8);	// reasonable pre-allocation
	// following order should match above universal node indices
	node_pool.push_back(netlist::void_node);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
netlist_common::~netlist_common() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
netlist_common::is_empty(void) const {
	// this doesn't check for fanin/fanout of nodes being used/driven
	return device_group::is_empty() && passive_device_pool.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Since these devices don't belong to a particular group, 
	don't use device_group::emit_devices.  
 */
ostream&
netlist_common::emit_devices(ostream& o, const netlist_options& nopt) const {
	// emit devices
#if ENABLE_STACKTRACE
	o << nopt.comment_prefix << "devices:" << endl;
#endif
#if !NETLIST_CACHE_ASSOC_UID && NETLIST_GROUPED_TRANSISTORS
	// preserve per-node device counters, just save the whole node pool
	typedef util::ptr_value_saver<index_type>	save_type;
	typedef	vector<pair<save_type, save_type> >	saves_type;
	saves_type __s;		// will auto-restore at end-of-scope
{
	__s.resize(node_pool.size());
	node_pool_type::const_iterator ni(node_pool.begin());
	saves_type::iterator i(__s.begin()), e(__s.end());
	for (++i; i!=e; ++i, ++ni) {
		index_type& c0(ni->device_count[0]);
		index_type& c1(ni->device_count[1]);
		i->first.bind(c0);
		i->second.bind(c1);
		c0 = 0; c1 = 0;		// locally reset counter
	}
}
#endif
	typedef	transistor_pool_type::const_iterator	const_iterator;
	const_iterator i(transistor_pool.begin()), e(transistor_pool.end());
	// TODO: print originating rule in comments
	// TODO: use optional label designations
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		i->emit(o, j, node_pool, nopt) << endl;
	}
	emit_passive_devices(o, nopt);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this code is *untested*, until we have some language hook
	for adding capacitors and passive components.  
 */
ostream&
netlist_common::emit_passive_devices(ostream& o,
		const netlist_options& nopt) const {
	typedef	passive_device_pool_type::const_iterator	const_iterator;
	const_iterator i(passive_device_pool.begin()),
		e(passive_device_pool.end());
	size_t C_count = 0;
	size_t R_count = 0;
	size_t L_count = 0;
	size_t other_count = 0;	// what other device is there?
	const bool paren =
		(nopt.instance_port_style == netlist_options::STYLE_SPECTRE);
for ( ; i!=e; ++i) {
	o << i->type;
	// default names of devices?
	switch (i->type) {
	case 'c':
	case 'C':
		o << C_count++; break;
	case 'r':
	case 'R':
		o << R_count++; break;
	case 'l':
	case 'L':
		o << L_count++; break;
	default:
		o << other_count++;
	}
	// terminals
	if (paren) o << " (";
	// FIXME: these indices may be wrong (netlist vs. local_netlist)
	const node& n0(node_pool[i->t[0]]);
	const node& n1(node_pool[i->t[1]]);
	n0.emit(o << ' ', nopt);
	n1.emit(o << ' ', nopt);
	if (paren) {
		o << " ) ";
		// for spectre, also print model name
		switch (i->type) {
		case 'c':
		case 'C':
			o << "capacitor"; break;
		case 'r':
		case 'R':
			o << "resistor"; break;
		case 'l':
		case 'L':
			o << "inductor"; break;
		default:
			o << "unknown-" << i->type;
		}
	}
	o << ' ';
	if (paren) {
		o << i->type << '=';
	}
	o << i->parameter_value;
	switch (i->type) {
		case 'c':
		case 'C':
			o << nopt.capacitance_unit; break;
		case 'r':
		case 'R':
			o << nopt.resistance_unit; break;
		case 'l':
		case 'L':
			o << nopt.inductance_unit; break;
		default:
			o << "?unit?";
	}
	o << endl;
}	// end for
	return o;
}	// end netlist_common::emit_passive_devices

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
device_group::dump_raw_devices(ostream& o) const {
	typedef	transistor_pool_type::const_iterator	const_iterator;
	const_iterator i(transistor_pool.begin()), e(transistor_pool.end());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		i->dump_raw(o << "  " << j << ": ") << endl;
	}
	return o;
}

//=============================================================================
// class local_netlist method definitions

local_netlist::local_netlist() : netlist_common(), name(), node_index_map(),
		transistor_index_offset(0) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
local_netlist::~local_netlist() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Summarizes subcircuit ports by looking at all used nodes.  
	TODO: passive devices
	This also transforms actual node indices to subcircuit-local indices.  
	\param nnp the master (supercircuit) netlist's node pool
 */
void
local_netlist::mark_used_nodes(node_pool_type& nnp) {
	STACKTRACE_VERBOSE;
{
	size_t ti = transistor_index_offset;
	transistor_pool_type::const_iterator
		i(transistor_pool.begin()), e(transistor_pool.end());
	for ( ; i!=e; ++i, ++ti) {
		// garbage values to be filled below...
		node_index_map[i->gate] = 0;
		node_index_map[i->source] = 0;
		node_index_map[i->drain] = 0;
		node_index_map[i->body] = 0;
		// flag as used, since this acts as both definition and instance
		i->mark_used_nodes(nnp);
#if NETLIST_NODE_GRAPH
		i->mark_node_terminals(nnp, ti);	// local node pool
#endif
	}
}{
	const size_t ns = node_index_map.size();
	STACKTRACE_INDENT_PRINT("ns = " << ns << endl);
	node_pool.reserve(ns +1);
	index_type j = netlist::void_index +1;
	node_index_map_type::iterator
		i(node_index_map.begin()), e(node_index_map.end());
	for ( ; i!=e; ++i, ++j) {
		i->second = j;
		const node& ref(nnp[i->first]);
		node_pool.push_back(ref);	// yes, copy node
	}
}{
	// transform indices from actual to local formal
	transistor_pool_type::iterator
		i(transistor_pool.begin()), e(transistor_pool.end());
	for ( ; i!=e; ++i) {
		const node_index_map_type::const_iterator
			ne(node_index_map.end()),
#if NETLIST_GROUPED_TRANSISTORS
			ai(node_index_map.find(i->assoc_node)),
#endif
			gi(node_index_map.find(i->gate)),
			si(node_index_map.find(i->source)),
			di(node_index_map.find(i->drain)),
			bi(node_index_map.find(i->body));
		INVARIANT(gi != ne);
		INVARIANT(si != ne);
		INVARIANT(di != ne);
		INVARIANT(bi != ne);
		i->gate = gi->second;
		i->source = si->second;
		i->drain = di->second;
		i->body = bi->second;
		// INVARIANT: associated node is used and in ports list
#if NETLIST_GROUPED_TRANSISTORS
		INVARIANT(ai != ne);
		i->assoc_node = ai->second;
#endif
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
local_netlist::dump_raw(ostream& o) const {
	o << name;
	typedef	node_pool_type::const_iterator	const_iterator;
	const_iterator i(++node_pool.begin()), e(node_pool.end());
	for ( ; i!=e; ++i) {
		i->dump_raw(o << ' ');
	}
	o << endl;
	o << "transistors:" << endl;
	dump_raw_devices(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Always emit these definitions' node ports.  
 */
ostream&
local_netlist::emit_definition(ostream& o, const netlist& n,
		const netlist_options& nopt) const {
{
switch (nopt.subckt_def_style) {
case netlist_options::STYLE_SPECTRE: 
	o << "subckt ";
	break;
default:
// case netlist_options::STYLE_SPICE: 
	o << ".subckt ";
}	// end switch
if (!nopt.nested_subcircuits) {
	// then use fully qualified type name, mangled
	o << n.get_name() << nopt.emit_scope();;
}
	o << name;
}
	// TODO: parameters would go here
{
	// ports, formals
	ostringstream oss;
	typedef	node_pool_type::const_iterator	const_iterator;
	const_iterator i(++node_pool.begin()), e(node_pool.end());
	for ( ; i!=e; ++i) {
		const node& nd(*i);
		// stack-node are local-only
		if (!nd.is_auxiliary_node()) {
			nd.emit(oss << ' ', nopt);
		}
	}
	string formals(oss.str());
	// already mangled during name caching
	o << formals << endl;
}
	// TODO: emit mangle map? only if not nested format?
	// TODO: emit node port-info
#if NETLIST_NODE_CAPS
if (nopt.emit_node_caps) {
	const node_pool_type& local_nodes_only(node_pool);
	node::emit_node_caps(o, local_nodes_only, nopt);
}
#endif
	// use supercircuit's names
	emit_devices(o, nopt);
	// end subcircuit
switch (nopt.subckt_def_style) {
case netlist_options::STYLE_SPECTRE: 
	o << "ends ";
if (!nopt.nested_subcircuits) {
	// then use fully qualified type name, mangled
	o << n.get_name() << nopt.emit_scope();;
}
	o << name << endl;
	break;
default:
// case netlist_options::STYLE_SPICE: 
	o << ".ends" << endl;
}	// end switch
	return o;
}	// end local_netlist::emit_definition

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
local_netlist::emit_instance_name(ostream& o,
		const netlist_options& nopt) const {
	o << nopt.subckt_instance_prefix;
	o << name << nopt.emit_colon() << "inst";
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiation uses the same names as the original ports.
	Always emit these instances' node ports.  
 */
ostream&
local_netlist::emit_instance(ostream& o, const netlist& n,
		const netlist_options& nopt) const {
	emit_instance_name(o, nopt);
{
	// actuals
switch (nopt.instance_port_style) {
case netlist_options::STYLE_SPECTRE:
	o << " (";	// spectre wraps ports in parentheses
	break;
default:
// case netlist_options::STYLE_SPICE:
	break;
}	// end switch
	ostringstream oss;
	typedef	node_pool_type::const_iterator	const_iterator;
	const_iterator i(++node_pool.begin()), e(node_pool.end());
	for ( ; i!=e; ++i) {
		const node& nd(*i);
		// stack-node are local-only
		if (!nd.is_auxiliary_node()) {
			nd.emit(oss << ' ', nopt);
		}
	}
	oss << ' ';
	string line(oss.str());
	// already mangled during name caching
	// type name is already mangled
	o << line;
switch (nopt.instance_port_style) {
case netlist_options::STYLE_SPECTRE:
	o << ") ";	// spectre wraps ports in parentheses
	break;
default:
// case netlist_options::STYLE_SPICE:
	break;
}	// end switch
}
if (!nopt.nested_subcircuits) {
	// then use fully qualified type name
	o << n.name << nopt.emit_scope();
}
	o << name;
	return o << endl;
}	// end local_netlist::emit_instance

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_CACHE_PARASITICS
/**
	Only after netlist subgraph is complete, can parasitics be
	accurately evaluated.  Need to know which nodes are end nodes.
 */
void
local_netlist::summarize_parasitics(const netlist_options& nopt) {
	device_group::summarize_parasitics(node_pool, nopt);
}
#endif

//=============================================================================
#if NETLIST_VERILOG
// class proc method definitions
ostream&
proc::emit(ostream& o) const {
	return o << name;
}

ostream&
proc::dump_raw(ostream& o) const {
	o << ':' << index;
	if (!used) o << " (unused)";
	return o;
}
#endif

//=============================================================================
// class node method definitions

/**
	How to format print each node's identity.  
	Name mangling is done outside of this procedure, 
	including special designators.
	\param fp the context of this node index, 
		used for named nodes and internal nodes.
 */
ostream&
node::emit(ostream& o, const netlist_options& n) const {
switch (type) {
case NODE_TYPE_LOGICAL:
	o << name;
	break;
case NODE_TYPE_INTERNAL:
	o << n.emit_internal_at() << name;
	break;
case NODE_TYPE_AUXILIARY:
	o << n.emit_auxiliary_pound();
	if (name.length()) {
		o << name;
	} else {
		o << index;
	}
	break;
default:
	DIE;
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For debugging interpretation.
 */
ostream&
node::dump_raw(ostream& o) const {
switch (type) {
case NODE_TYPE_LOGICAL:
	o << ':' << index;
	break;
case NODE_TYPE_INTERNAL:
	o << '@' << index;	// was name
	break;
case NODE_TYPE_AUXILIARY:
	o << '#';
	if (name.length()) {
		o << name;
	} else {
		o << index;
	}
	break;
default:
	o << "???";
}
	if (!used) o << " (unused)";
#if NETLIST_CHECK_CONNECTIVITY
	if (!driven) o << " (undriven)";
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print basic port direction information.
 */
ostream&
node::emit_port_summary(ostream& o, const netlist_options& opt) const {
	o << name << " : ";
	if (used) {
	if (driven) {
		o << "inout";	// bidirectional
	} else {
		o << "input";
	}
	} else {
	if (driven) {
		o << "output";
	} else {
		o << "unused";
	}
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_CHECK_CONNECTIVITY
error_status
node::check_connectivity(const netlist_options& opt) const {
	// check should apply to non-ports
	if (used && !driven) {
	// depend on opt
		cerr << (opt.undriven_node_policy == OPTION_ERROR ?
			"Error:" : "Warning:") << " node ";
		// do not mangle
		emit(cerr, netlist_options::default_value) <<
			" is used but not driven!" << endl;
		return opt.undriven_node_policy;
	}
	return STATUS_NORMAL;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_NODE_CAPS
ostream&
node_caps::emit(ostream& o, const netlist_options& nopt) const {
	return o <<
		"[ndiff_perim=" << ndiff_perimeter << nopt.length_unit <<
		", ndiff_area=" << ndiff_area << nopt.area_unit <<
		", ndrain_terms=" << ndrain_terms <<
		", pdiff_perim=" << pdiff_perimeter << nopt.length_unit <<
		", pdiff_area=" << pdiff_area << nopt.area_unit <<
		", pdrain_terms=" << pdrain_terms <<
		", gate_area=" << gate_area << nopt.area_unit <<
		", gate_terms=" << gate_terms <<
		", wire_area=" << wire_area << nopt.area_unit <<
		']';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
node::emit_node_caps(ostream& o, const node_pool_type& node_pool, 
		const netlist_options& nopt) {
	o << nopt.comment_prefix << "BEGIN node caps" << endl;
	typedef	node_pool_type::const_iterator	const_iterator;
	const_iterator i(node_pool.begin()), e(node_pool.end());
for (++i; i!=e; ++i) {
	// include all nodes, internal, stack, supply, etc...
	const node_caps& c(i->cap);
	i->emit(o << nopt.comment_prefix << "\t", nopt) << " ";
	c.emit(o, nopt) << endl;
}
	return o << nopt.comment_prefix << "END node caps" << endl;
}
#endif

//=============================================================================
// class instance method definitions

/**
	Mark all instance actuals as used automatically?
	\param inst_index the process id.
 */
void
instance::mark_used_nodes(node_pool_type& node_pool) const {
#if !NETLIST_CHECK_CONNECTIVITY
	node_actuals_list_type::const_iterator
		i(node_actuals.begin()), e(node_actuals.end());
	for ( ; i!=e; ++i) {
		node_pool[*i].used = true;
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
instance::is_empty(void) const {
	NEVER_NULL(type);
	return type->is_empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
instance::raw_identifier(const footprint& fp,
		const netlist_options& nopt) const {
	// process instance name
	ostringstream oss;
	fp.get_instance_pool<process_tag>()[index -1].get_back_ref()
		->dump_hierarchical_name(oss, nopt.__dump_flags);
	return oss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: support for instance/type parameters?
	\param fp is the footprint of the parent that contains this instance.
 */
ostream&
instance::emit(ostream& o, const node_pool_type& node_pool, 
#if NETLIST_VERILOG
		const proc_pool_type& proc_pool,
#endif
		const footprint& fp, const netlist_options& nopt) const {
	const string& pname(name);
	size_t line_length = 0;
{
	// process instance name
if (nopt.emit_mangle_map) {
	const string rname(raw_identifier(fp, nopt));
	o << nopt.comment_prefix << "instance: " << type->get_unmangled_name()
		<< ' ' << rname << endl;	// want original name
}
	o << nopt.subckt_instance_prefix;
	line_length += nopt.subckt_instance_prefix.length();
}
	vector<string> _actuals;
	_actuals.reserve(32);
// if include wire ports
if (nopt.node_ports) {
	node_actuals_list_type::const_iterator
		i(node_actuals.begin()), e(node_actuals.end());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		ostringstream oss;
		if (nopt.named_port_connections) {
			oss << '.' << type->get_node_port(j).name << '(';
		}
		node_pool[*i].emit(oss, nopt);
		if (nopt.named_port_connections) {
			oss << ')';
		}
		_actuals.push_back(oss.str());
	}
}
// if include struct ports
#if NETLIST_VERILOG
if (nopt.struct_ports) {
	STACKTRACE_INDENT_PRINT("proc_actuals.size() = " <<
		proc_actuals.size() << endl);
	proc_actuals_list_type::const_iterator
		i(proc_actuals.begin()), e(proc_actuals.end());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		ostringstream oss;
		if (nopt.named_port_connections) {
			oss << '.' << type->get_proc_port(j).name << '(';
		}
		proc_pool[*i].emit(oss);	// nopt
		if (nopt.named_port_connections) {
			oss << ')';
		}
		_actuals.push_back(oss.str());
	}
}
#endif
	const bool paren =
		(nopt.instance_port_style == netlist_options::STYLE_SPECTRE);
	vector<string>::const_iterator i(_actuals.begin()), e(_actuals.end());
switch (nopt.instance_port_style) {
case netlist_options::STYLE_SPICE:
case netlist_options::STYLE_SPECTRE:
	o << pname;
	line_length += pname.length();
{
	// actuals
	if (paren) { o << " ("; line_length += 2; }
{
	for ( ; i!=e; ++i) {
		const size_t wl = i->length() +1;
		line_length += wl;
		const bool wrap = (nopt.auto_wrap_length &&
			(line_length > nopt.auto_wrap_length));
		if (wrap) {
			nopt.line_continue(o);
			line_length = nopt.post_line_continue.length() +wl;
		}
		o << ' ' << *i;
	}
}
	// already mangled during name caching
	if (paren) { o << " )"; line_length += 2; }

	// type name is already mangled
	const size_t wl = type->get_name().length() +1;
	line_length += wl;
	const bool wrap = (nopt.auto_wrap_length &&
		(line_length > nopt.auto_wrap_length));
	if (wrap) {
		nopt.line_continue(o);
		line_length = nopt.post_line_continue.length() +wl;
	}
	o << ' ' << type->get_name();	// endl
	break;
}
case netlist_options::STYLE_VERILOG:
	o << type->get_name();
	o << ' ' << pname << '(';
	line_length += type->get_name().length() +pname.length() +2;
if (_actuals.size()) {
	o << *i;
	for (++i; i!=e; ++i) {
		o << ", ";
		const size_t wl = i->length() +2;
		line_length += wl;
		const bool wrap = (nopt.auto_wrap_length &&
			(line_length > nopt.auto_wrap_length));
		if (wrap) {
			nopt.line_continue(o);
			line_length = nopt.post_line_continue.length() +wl;
		}
		o << *i;
	}
}
	o << ");";
	break;
}
	return o;
}	// end instance::emit()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance::dump_raw(ostream& o) const {
	o << '[' << index << "]: " << type->get_name() << ": (";
	copy(node_actuals.begin(), node_actuals.end(),
		ostream_iterator<index_type>(o, ","));
	return o << ')';
}


//=============================================================================
// class transistor method definitions

ostream&
transistor_base::dump(ostream& o) const {
	o << (is_NFET() ? 'N' : 'P');
	o << " g:" << gate;
	emit_attribute_suffixes(o << " [", netlist_options::default_value)
		<< ']';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In a separate pass mark all nodes participating on transistor.
	Why?  Is possible that a supply node is not used.  
 */
void
transistor::mark_used_nodes(node_pool_type& node_pool) const {
	node_pool[source].used = true;
	node_pool[gate].used = true;
#if NETLIST_CHECK_CONNECTIVITY
	node_pool[drain].driven = true;
#else
	node_pool[drain].used = true;
#endif
	node_pool[body].used = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_NODE_GRAPH
void
transistor::mark_node_terminals(node_pool_type& node_pool, 
	const size_t ti) const {
	const size_t nps = node_pool.size();
// redundantly maintain node graph for adjacency and connectivity
	const node_terminal s('M', ti, 's');
	const node_terminal g('M', ti, 'g');
	const node_terminal d('M', ti, 'd');
	const node_terminal b('M', ti, 'b');
	INVARIANT(source < nps);
	INVARIANT(gate < nps);
	INVARIANT(drain < nps);
	INVARIANT(body < nps);
	node_pool[source].terminals.push_back(s);
	node_pool[gate].terminals.push_back(g);
	node_pool[drain].terminals.push_back(d);
	node_pool[body].terminals.push_back(b);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
transistor::emit_identifier(ostream& o,
#if NETLIST_GROUPED_TRANSISTORS
		const index_type, 	// unused
#else
		const index_type di, 
#endif
		const node_pool_type& node_pool,
		const netlist_options& nopt) const {
#if NETLIST_GROUPED_TRANSISTORS
	// don't use ordinal index di
	const node& n(node_pool[assoc_node]);
	INVARIANT(!n.is_auxiliary_node());
#if !NETLIST_CACHE_ASSOC_UID
	index_type& c(n.device_count[size_t(assoc_dir)]);
#endif
	o << nopt.transistor_prefix;	// spice card
	if (name.length()) {
		o << nopt.emit_colon() << name;
		// override name of device, entire basename
		// does NOT mangle the name, so give it a good string!
		// extra leading colon helps avoid conflict with 
		// default rule-named devices
	} else {
		// is logical or internal
		n.emit(o, nopt) << nopt.emit_colon() <<
			(assoc_dir ? "up" : "dn") << nopt.emit_colon();
#if NETLIST_CACHE_ASSOC_UID
#if 0
		if (assoc_uid.first) {
			// debug-only: print local subckt index
			o << assoc_uid.first << nopt.emit_colon();
		}
#endif
		o << assoc_uid.second;	// local transistor index
#else
		o << c;
#endif
	}
#if !NETLIST_CACHE_ASSOC_UID
	++c;	// unconditionally
#endif
#else
	o << nopt.transistor_prefix << di << '_';
#endif
	// attribute suffixes cannot be overridden
	return emit_attribute_suffixes(o, nopt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param di the device index, incrementing.
 */
ostream&
transistor::emit(ostream& o, const index_type di, 
		const node_pool_type& node_pool,
		const netlist_options& nopt) const {
	emit_identifier(o, di, node_pool, nopt) << ' ';
	const bool paren = 
		(nopt.instance_port_style == netlist_options::STYLE_SPECTRE);
	const node& s(node_pool[source]);
	const node& g(node_pool[gate]);
	const node& d(node_pool[drain]);
	const node& b(node_pool[body]);
{
	if (paren) o << "( ";
	// perform name-mangling
	ostringstream oss;
	s.emit(oss, nopt) << ' ';
	g.emit(oss, nopt) << ' ';
	d.emit(oss, nopt) << ' ';
	b.emit(oss, nopt) << ' ';
	string nodes(oss.str());
	// all nodes already mangled
	o << nodes;
	if (paren) o << ") ";
}
	string ftype("");
	switch (type) {
	case NFET_TYPE: ftype += "nfet"; break;
	case PFET_TYPE: ftype += "pfet"; break;
	// TODO: honor device type name overrides, including vt
	default:
		ftype += "<type?>";
	}
	if (attributes & IS_LOW_VT)
		ftype += "_lvt";
	else if (attributes & IS_HIGH_VT)
		ftype += "_hvt";
	else	ftype += "_svt";
	// else leave svt unmarked
	// TODO: query nopt.misc_options.find()
	netlist_options::misc_options_map_type::const_iterator
		f(nopt.misc_options_map.find(ftype));
	if (f != nopt.misc_options_map.end()) {
		o << f->second.front();
	} else {
		o << ftype;
	}

	// option 'width_by_nfin' controls how W_eff is calculated.
	// TODO: restrict lengths and widths, from tech/conf file
	if (nopt.width_by_nfin) { 
		// assign width to num_fin
		const real_type num_fins = width;
		// calculate the effective-width
		// note that lambda is not used as a scale factor here
		const real_type width_eff = ((num_fins - 1) * nopt.fin_pitch)
			+ nopt.fin_drawn_width;
		// emit parameters
		o << " NFIN=" << num_fins << 
			" W=" << width_eff << nopt.length_unit;
	} else {
		// for non-FinFet processes
		o << " W=" << width *nopt.lambda << nopt.length_unit;
	}

	o << " L=" << length *nopt.lambda << nopt.length_unit;

	if (nopt.emit_parasitics) {
#if !NETLIST_CACHE_PARASITICS
		const parasitics parasitic_values(*this,
			s.is_stack_end_node(), d.is_stack_end_node(), nopt);
#endif
		const real_type& asv(parasitic_values.source_area);
		const real_type& psv(parasitic_values.source_perimeter);
		const real_type& adv(parasitic_values.drain_area);
		const real_type& pdv(parasitic_values.drain_perimeter);
#if 0
		// debugging
		nopt.line_continue(o);
		o << "lambda=" << nopt.lambda <<
			" sl=" << sl <<
			" dl=" << dl;
#endif
		nopt.line_continue(o);
		o <<	" AS=" << asv << nopt.area_unit <<
			" PS=" << psv << nopt.length_unit <<
			" AD=" << adv << nopt.area_unit <<
			" PD=" << pdv << nopt.length_unit;
	}
	return o;
}	// end transistor::emit()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
transistor_base::emit_attribute_suffixes(ostream& o, 
		const netlist_options& nopt) const {
	if (is_pass())
		o << nopt.emit_colon() << "pass";
	if (is_precharge()) {
		o << nopt.emit_colon() <<
			(is_non_restoring() ? "pchgnr" : "pchg");
	}
	if (is_weak_keeper())
		o << nopt.emit_colon() << "keeper";
	if (is_comb_keeper())
		o << nopt.emit_colon() << "ckeeper";
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
transistor::dump_raw(ostream& o) const {
	switch (type) {
	case NFET_TYPE: o << "nch"; break;
	case PFET_TYPE: o << "pch"; break;
	// TODO: honor different vt types and flavors
	default:
		o << "<type?>";
	}
	o << ' ' << source << ' ' << gate << ' ' << drain << ' ' << body;
	o << " # W=" << width << " L=" << length;
	// unitless
	emit_attribute_suffixes(o << " ", netlist_options::default_value);
	return o;
}

//-----------------------------------------------------------------------------
// class transistor::parasitics method definitions

transistor::parasitics::parasitics(
		const real_type wd, const real_type ln,
		const bool s_ext, const bool d_ext,
		const netlist_options& nopt) {
	__update(wd, ln, s_ext, d_ext, nopt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param wd device width
	\param ln device length (FIXME: is this used???)
	\param s_ext true if source is at end of device stack (external)
	\param d_ext true if drain is at end of device stack (external)
	\param nopt global netlist parameters and constants
 */
void
transistor::parasitics::__update(
		const real_type wd, const real_type ln,
		const bool s_ext, const bool d_ext,
		const netlist_options& nopt) {
	STACKTRACE_VERBOSE;
	// compute and emit parasitic area/perimeter values
	const bool pge = nopt.fet_perimeter_gate_edge;
	const real_type lsq = nopt.lambda * nopt.lambda;
	const real_type l2 = nopt.lambda * 2.0;
	const real_type half_spacing = nopt.fet_spacing_diffonly / 2.0;
	const real_type& sl(s_ext ?
		nopt.fet_diff_overhang : half_spacing);
	const real_type& dl(d_ext ?
		nopt.fet_diff_overhang : half_spacing);
	// areas of internal stack nodes are halved to 
	// assume internal sharing, split to each sharer.
	source_area = wd * sl * lsq;
	source_perimeter = pge ? 
		(s_ext ? (wd + sl) *l2		// width +side
			: sl*l2) :			// sides only
		(s_ext ? (wd*nopt.lambda + sl*l2)	// 3 sides
			: (sl * l2));			// 2 sides
	drain_area = wd * dl * lsq;
	drain_perimeter = pge ?
		(d_ext ? (wd + dl) *l2		// width +side
			: dl*l2) :			// sides only
		(d_ext ? (wd*nopt.lambda + dl*l2)	// 3 sides
			: (dl * l2));			// 2 sides
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
transistor::parasitics::update(const transistor& t, 
		const bool s_ext, const bool d_ext,
		const netlist_options& nopt) {
	__update(t.width, t.length, s_ext, d_ext, nopt);
}

//=============================================================================
// class netlist method definitions

// tag objects for convenience
const node::__logical_node_tag	node::logical_node_tag = __logical_node_tag();
const node::__internal_node_tag	node::internal_node_tag = __internal_node_tag();
const node::__auxiliary_node_tag	node::auxiliary_node_tag = __auxiliary_node_tag();

// case sensitive?
const node
netlist::void_node("__VOID__", node::auxiliary_node_tag);

#if NETLIST_VERILOG
const proc
netlist::void_proc(0, NULL);
#endif

// universal node indices to every subcircuit
// these should correspond with the order of insertion in netlist's ctor
const	index_type
netlist::void_index = 0,
netlist::first_node_index = 1;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
netlist::netlist() : netlist_common(), name(), 
		named_node_map(),
#if NETLIST_VERILOG
		named_proc_map(),
#endif
		instance_pool(), internal_node_map(), 
		internal_expr_map(), 
#if NETLIST_CHECK_NAME_COLLISIONS
		name_collision_map(), 
		struct_name_collision_map(), 
		instance_name_collision_map(), 
#endif
		local_subcircuits(), 
		node_port_list(), 
#if NETLIST_VERILOG
		proc_port_list(),
#endif
		empty(false), 	// is cached
		aux_count(0),
		subs_count(0),
		warning_count(0) {
	// copy supply nodes
#if NETLIST_VERILOG
	proc_pool.push_back(void_proc);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
netlist::~netlist() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common initialization routines based on footprint.  
 */
void
netlist::__bind_footprint(const footprint& f, const netlist_options& nopt) {
	STACKTRACE_VERBOSE;
	fp = &f;
	// pre-allocate, using same indexing and mapping as original pool
	// null index initially, and default owner is not subcircuit
if (f.has_prs_footprint()) {
	const prs_footprint& pfp(f.get_prs_footprint());
	internal_node_map.resize(pfp.get_internal_node_pool().size(),
		internal_node_entry_type(0, 0));
	const prs_footprint::subcircuit_map_type&
		subs(pfp.get_subcircuit_map());
	STACKTRACE_INDENT_PRINT(subs.size() << " local subcircuits" << endl);
	local_subcircuits.resize(subs.size());
	// import/generate subcircuit names
	prs_footprint::subcircuit_map_type::const_iterator
		si(subs.begin()), se(subs.end());
	local_subcircuit_list_type::iterator
		li(local_subcircuits.begin());
	for (; si!=se; ++si, ++li) {
		const string& nn(si->get_name());
		if (nn.length()) {
			li->set_name(nn);
			// mangle here?
		} else {
			ostringstream oss;
			oss << "INTSUB" << nopt.emit_colon() << subs_count;
			li->set_name(oss.str());
			++subs_count;
		}
	}
}
	const size_t bs = f.get_instance_pool<bool_tag>().local_entries();
	const size_t ps = f.get_instance_pool<process_tag>().local_entries();
	named_node_map.resize(bs);
	instance_pool.reserve(ps);
#if NETLIST_VERILOG
	named_proc_map.resize(ps);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Like constructor, initializes footprint and related members.
 */
void
netlist::bind_footprint(const footprint& f, const netlist_options& nopt) {
	__bind_footprint(f, nopt);
	ostringstream oss;
	f.dump_type(oss);
	name = oss.str();
	strgsub(name, " ", "");		// remove spaces (template params)
	nopt.mangle_type(name);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overload with overridden name, e.g. for top-level.
 */
void
netlist::bind_footprint(const footprint& f, const string& n) {
	__bind_footprint(f, netlist_options::default_value);
	// NOTE: this circumvents mangling options
	name = n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
netlist::is_channel_or_struct(void) const {
	return fp->get_meta_type() != entity::META_TYPE_PROCESS;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
netlist::get_unmangled_name(void) const {
	ostringstream oss;
	fp->dump_type(oss);
	return oss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
const node&
netlist::get_node_port(const size_t i) const {
	return node_pool[node_port_list[i]];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#if NETLIST_VERILOG
const proc&
netlist::get_proc_port(const size_t i) const {
	return proc_pool[proc_port_list[i]];
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
template <class Tag>
static
inline
void
__resolve_port_pre_invariant(const instance_alias_info<Tag>& a) { }

static
inline
void
__resolve_port_pre_invariant(const instance_alias_info<bool_tag>& a) {
	INVARIANT(a.is_aliased_to_port());
}

/**
	Re-factored out.
	This subroutine could be pushed to footprint as a member function.
 */
template <class Tag>
static
const instance_alias_info<Tag>*
__resolve_port_actual_alias(const index_type fid,
		const footprint& subfp) {
	const instance_alias_info<Tag>&
		fb(*subfp.get_instance_pool<Tag>()[fid -1].get_back_ref());
	__resolve_port_pre_invariant(fb);
if (!fb.is_port_alias()) {
	// HACK ALERT!
	// well, damn it, FIND me a suitable alias!
	typedef typename port_alias_tracker_base<Tag>::map_type
				alias_map_type;
	const alias_map_type&
		pat(subfp.get_scope_alias_tracker().template get_id_map<Tag>());
	typename alias_map_type::const_iterator
		f(pat.find(fid)), e(pat.end());
	INVARIANT(f != e);
	const alias_reference_set<Tag>& rs(f->second);
	typename alias_reference_set<Tag>::const_iterator
		ai(rs.begin()), ae(rs.end());
	for ( ; ai!=ae; ++ai) {
	if ((*ai)->is_port_alias()) {
		return &**ai;
	}
	}
	INVARIANT(ai != ae);	// should never be reached!
}
// else
	return &fb;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param subp global process entry containing footprint frame
		Should we use local footprint's instance instead?
		Technically, we shouldn't need global allocation information.
	\param subnet the netlist type that corresponds with instance.
	\param lpid local process id from footprint.
	\param pfp parent instance's footprint (for hierarchical name).
	\pre instance_pool is already pre-allocated to avoid 
		invalidating references due to re-allocation.
 */
void
netlist::append_instance(const state_instance<process_tag>& subp,
		const netlist& subnet, const index_type lpid,
#if NETLIST_VERILOG
		const netlist_map_type& netmap,
#endif
		const netlist_options& opt) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("lpid = " << lpid << endl);
	const footprint* subfp = subp._frame._footprint;
	// cannot use global allocated footprint_frame
//	const netlist& subnet(netmap.find(subfp)->second);
	INVARIANT(subfp == subnet.fp);
	// subnet's port list may be shorter than formals list, due to aliases
	// traverse formals list and lookup each actual to be passed
	// recall: only used nodes will be in this port list
	const size_t upid = instance_pool.size();
		// unique instance_pool index
	instance_pool.push_back(instance(subnet, lpid));
	instance& np(instance_pool.back());
	// compute mangled name once
	np.name = np.raw_identifier(*fp, opt);
	opt.mangle_instance(np.name);
	// local process instance needed to find local port actual id
	const state_instance<process_tag>::pool_type&
		appool(fp->get_instance_pool<process_tag>());
	const instance_alias_info<process_tag>&
		lp(*appool[lpid -1].get_back_ref());
	// ALERT: translates to global index, not what we want!
	netlist::node_port_list_type::const_iterator
		fi(subnet.node_port_list.begin()),
		fe(subnet.node_port_list.end());
	for ( ; fi!=fe; ++fi) {
		STACKTRACE_INDENT_PRINT("formal node = " << *fi << endl);
		const node& fn(subnet.node_pool[*fi]);	// formal node
#if NETLIST_CHECK_CONNECTIVITY
		INVARIANT(fn.used || fn.driven);
#else
		INVARIANT(fn.used);
#endif
		if (fn.is_logical_node()) {
			const index_type fid = fn.index;
			STACKTRACE_INDENT_PRINT("formal id = " << fid << endl);
			const instance_alias_info<bool_tag>* ab =
				__resolve_port_actual_alias<bool_tag>(fid, *subfp);
			NEVER_NULL(ab);
			const index_type actual_id =
				ab->trace_alias(lp).instance_index;
			INVARIANT(actual_id);
			STACKTRACE_INDENT_PRINT("LOCAL actual id = " << actual_id << endl);
			const index_type actual_node =
				register_named_node(actual_id, opt);
			STACKTRACE_INDENT_PRINT("actual node = " << actual_node << endl);
			np.node_actuals.push_back(actual_node);
			node& an(node_pool[actual_node]);
#if NETLIST_CHECK_CONNECTIVITY
			// inherit used/drive properties from formals to actuals
			if (fn.used)
				an.used = true;
			if (fn.driven)
				an.driven = true;
#endif
		} else {
			cerr << "ERROR: unhandled instance port node type."
				<< endl;
			THROW_EXIT;
		}
		// else skip for now
	}	// end for each node port
#if NETLIST_VERILOG
	// for each structure/process/channel port
	// after all traversals, this covers all used channels
	STACKTRACE_INDENT_PRINT("subnet.proc_port_list.size() = "
		<< subnet.proc_port_list.size() << endl);
	netlist::proc_port_list_type::const_iterator
		pi(subnet.proc_port_list.begin()),
		pe(subnet.proc_port_list.end());
	for ( ; pi!=pe; ++pi) {
		STACKTRACE_INDENT_PRINT("formal struct = " << *fi << endl);
		const proc& fn(subnet.proc_pool[*pi]);	// formal struct
		const index_type fid = fn.index;
		STACKTRACE_INDENT_PRINT("formal id = " << fid << endl);
		const instance_alias_info<process_tag>* ab =
			__resolve_port_actual_alias<process_tag>(fid, *subfp);
		NEVER_NULL(ab);
		const index_type actual_id =
			ab->trace_alias(lp).instance_index;
		INVARIANT(actual_id);
		STACKTRACE_INDENT_PRINT("LOCAL actual id = " << actual_id << endl);
		const footprint* afp = appool[actual_id -1]._frame._footprint;
		const netlist& anl(netmap.find(afp)->second);
		const index_type actual_proc =
			register_named_proc(actual_id, &anl, opt);
		STACKTRACE_INDENT_PRINT("actual proc = " << actual_proc << endl);
		np.proc_actuals.push_back(actual_proc);
#if NETLIST_CHECK_CONNECTIVITY
		proc_pool[actual_proc].used = true;
#endif
	}	// end for each structure port
#endif	// NETLIST_VERILOG
	// check for name collisions post-mangling
	if (subfp->get_meta_type() == entity::META_TYPE_PROCESS) {
		STACKTRACE_INDENT_PRINT("check instance name collision: " << upid << endl);
		check_instance_name_collisions(upid, opt);
	} else {
		// this looks just wrong
		STACKTRACE_INDENT_PRINT("skip chan/struct name collision: " << lpid << endl);
#if 0
		const size_t npid = named_proc_map[lpid];
		STACKTRACE_INDENT_PRINT("local named proc id: " << npid << endl);
//		check_struct_name_collisions(proc_pool[npid].name, npid, opt);
#endif
	}
#if ENABLE_STACKTRACE
	np.dump_raw(STACKTRACE_INDENT_PRINT("new instance: ")) << endl;
#endif
	INVARIANT(np.node_actuals.size() == subnet.node_port_list.size());
}	// end netgraph::append_instance

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates a new auxiliary node, incrementing counter.
	\return index of new node, 1-indexed.
 */
index_type
netlist::create_auxiliary_node(void) {
	STACKTRACE_VERBOSE;
	const node n(aux_count, node::auxiliary_node_tag);
	++aux_count;
	const index_type ret = node_pool.size();
	node_pool.push_back(n);
	return ret;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_CHECK_NAME_COLLISIONS
/**
	Registers against node name map.  
	\param n is a post-mangled name of a node (not case-slammed).
	\param ni index of this node
	\param opt case-collision error policy
	Post-mangling name collisions always result in error.
	For now, conflict with reserved names will always result in error.
	\throw general exception on error.
 */
void
netlist::check_name_collisions(const string& n, const index_type ni, 
		const netlist_options& opt) {
	STACKTRACE_VERBOSE;
	const string key((opt.case_collision_policy != OPTION_IGNORE) ?
		util::strings::string_tolower(n) : n);
	typedef name_collision_map_type::iterator	iterator;
	typedef name_collision_map_type::value_type	pair_type;
	const pair<iterator, bool>
		p(name_collision_map.insert(pair_type(key, ni)));
	if (!p.second) {
		cerr << "Error: Post-mangled node name `" << n <<
			"\' collides with another node `" <<
		node_pool[p.first->second].name << "\'." << endl;
		THROW_EXIT;
	}
	if (opt.collides_reserved_name(key)) {
		cerr << "Error: Post-mangled node name `" << n <<
			"\' collides with a reserved node name." << endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers against instance name map.  
	\param n is a post-mangled name of an instance (not case-slammed).
	\param ni index of this instance
	\param opt case-collision error policy
	Post-mangling name collisions always result in error.
 */
void
netlist::check_struct_name_collisions(const string& n, const index_type ni, 
		const netlist_options& opt) {
	STACKTRACE_VERBOSE;
	const string key((opt.case_collision_policy != OPTION_IGNORE) ?
		util::strings::string_tolower(n) : n);
	typedef name_collision_map_type::iterator	iterator;
	typedef name_collision_map_type::value_type	pair_type;
	const pair<iterator, bool>
		p(struct_name_collision_map.insert(pair_type(key, ni)));
	if (!p.second) {
		string pn(proc_pool[p.first->second].name);
		opt.mangle_instance(pn);
		cerr << "Error: Post-mangled struct/chan name `" << n <<
			"\' collides with another struct/chan `" << pn <<
			"\'." << endl;
		THROW_EXIT;
	}
	// don't check against reserved names, those are for nodes
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers against instance name map.  
	\param n is a post-mangled name of an instance (not case-slammed).
	\param ni index of this instance, index into instance_pool
	\param opt case-collision error policy
	Post-mangling name collisions always result in error.
 */
void
netlist::check_instance_name_collisions(const index_type ni, 
		const netlist_options& opt) {
	STACKTRACE_VERBOSE;
	INVARIANT(ni < instance_pool.size());
	string n(instance_pool[ni].name);	// copy
	const string key((opt.case_collision_policy != OPTION_IGNORE) ?
		util::strings::string_tolower(n) : n);
	typedef name_collision_map_type::iterator	iterator;
	typedef name_collision_map_type::value_type	pair_type;
	const pair<iterator, bool>
		p(instance_name_collision_map.insert(pair_type(key, ni)));
	if (!p.second) {
		const string& pn(instance_pool[p.first->second].name);
		cerr << "Error: Post-mangled instance name `" << n <<
			"\' collides with another instance `" << pn <<
			"\'." << endl;
		THROW_EXIT;
	}
	// don't check against reserved names, those are for nodes
}
#endif	// NETLIST_CHECK_NAME_COLLISIONS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates a new named internal node, incrementing counter.
	\param ni is the footprint-local index of the internal node (0-indexed).
	\return index of new node, 1-indexed into this netlist.
 */
index_type
netlist::create_internal_node(const index_type ni, const index_type ei,
		const netlist_options& opt) {
	STACKTRACE_VERBOSE;
	node n(ni, node::internal_node_tag);
	INVARIANT(fp->has_prs_footprint());
	n.name = fp->get_prs_footprint().get_internal_node(ni).name;
	opt.mangle_instance(n.name);
	// @node names are simple identifiers, but may contain underscores
	const index_type ret = node_pool.size();
	node_pool.push_back(n);
	INVARIANT(ni < internal_node_map.size());
	internal_node_map[ni].first = ret;
	internal_expr_map[ei] = ni;	// invariant: no duplicates
	// set owner later!
#if NETLIST_CHECK_NAME_COLLISIONS
	check_name_collisions(n.name, ret, opt);
#endif
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lookup a previously defined internal node, keyed by
	PRS footprint's expr index.
	\param ei local footprint's expr index corresponding to
		an internal node.
	\return footprint's internal node index, which can be 0.  
 */
index_type
netlist::lookup_internal_node(const index_type ei) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("expr-id: " << ei << endl);
	const internal_expr_map_type::const_iterator
		f(internal_expr_map.find(ei));
	INVARIANT(f != internal_expr_map.end());
	// ALERT: only true if we've processed all internal nodes in a priori
	return f->second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Honors (in order of precedence):
	preferred_names, 
	prefer_port_aliases, 
	[default] canonical shortest-name
	\param _i is the footprint's local node index, 1-based, never 0.
	\return designated (un-mangled) alias of the indexed local node.
 */
template <class Tag>
string
netlist::get_original_node_name(const size_t _i, 
		const netlist_options& opt) const {
	STACKTRACE_VERBOSE;
//	typedef	bool_tag		Tag;
	typedef typename port_alias_tracker_base<Tag>::map_type map_type;
	typedef typename alias_reference_set<Tag>::const_iterator
					const_iterator;
// stupid multi-pass implementation
if (!opt.preferred_names.empty()) {
	const map_type&
		sa(fp->get_scope_alias_tracker().template get_id_map<Tag>());
	// then scan *all* aliases for the match (slow)
	const typename map_type::const_iterator
		asi(sa.find(_i)), ase(sa.end());
if (asi != ase) {
	const alias_reference_set<Tag>& s(asi->second);
	const_iterator ai(s.begin()), ae(s.end());
	for ( ; ai != ae; ++ai) {
		// check for match against preferred name set
		NEVER_NULL(*ai);
		ostringstream oss;
		(*ai)->dump_hierarchical_name(oss, opt.__dump_flags);
		const string& n(oss.str());
//		STACKTRACE_INDENT_PRINT("is preferred? " << n << endl);
		if (opt.matches_preferred_name(n)) {
			// return if matched (exact), accounting for case
//			STACKTRACE_INDENT_PRINT("using preferred name" << endl);
			return n;
		}
	}
}	// else has no other aliases
}
// another pass to check for preferred port aliases
if (opt.prefer_port_aliases) {
	const map_type&
		pa(fp->get_scope_alias_tracker().template get_id_map<Tag>());
//		pa(fp->get_port_alias_tracker().template get_id_map<Tag>());
	const typename map_type::const_iterator
		asi(pa.find(_i)), ase(pa.end());
if (asi != ase) {
	const alias_reference_set<Tag>& s(asi->second);
	// use the shortest port alias
	// can't just use union-find() b/c shortest could be local non-port
	INVARIANT(s.size());
	const const_iterator
//		ai(s.find_any_port_alias());	// instead of shortest
		ai(s.find_shortest_alias(true));	// ports only
if (ai != s.end()) {
	NEVER_NULL(*ai);
	ostringstream oss;
	(*ai)->dump_hierarchical_name(oss, opt.__dump_flags);
	return oss.str();
}
}	// else has no other port aliases
}	// end if prefer_port_aliases
// fallback to using shortest canonical name (first position in set)
	const typename state_instance<Tag>::pool_type&
		bp(fp->get_instance_pool<Tag>());
	const size_t i = _i -1;		// pool is 0-based
	STACKTRACE_INDENT_PRINT("bp.size = " << bp.local_entries() << endl);
	INVARIANT(i < bp.local_entries());
	const never_ptr<const instance_alias_info<Tag> >
		a(bp[i].get_back_ref());
	// from port_alias_tracker, 
	// back_ref points to shortest canonical scope alias
	NEVER_NULL(a);
	ostringstream oss;
	a->dump_hierarchical_name(oss, opt.__dump_flags);
	return oss.str();
}	// end netlist::get_original_node_name()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates general netlist node index into footprint
	bool index, 1-based.
	\param i must refer to a logical node for this to make sense.
 */
index_type
netlist::lookup_named_node(const index_type i) const {
	const node& n(node_pool[i]);
	INVARIANT(n.is_logical_node());
	return n.index;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	As each named logical node is visited for the first time, 
	register it with the map of named nodes, where the value
	is the corresponding index of the *subcircuit* node.
	This way, unused bools are not emitted.  
	The named_node_map will be 0-indexed, even though footprint_frames
		are 1-indexed (for compactness).  
	\param _i is the footprint's local node index, never 0.
	\return 1-indexed local netlist's generic node index, never 0.
 */
index_type
netlist::register_named_node(const index_type _i, const netlist_options& opt) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("local id (+1) = " << _i << endl);
	INVARIANT(_i);
	const index_type i = _i -1;	// locally 0-indexed, no gap
	INVARIANT(i < named_node_map.size());
	index_type& ret(named_node_map[i]);
	if (!ret) {
		// reserve a new slot and update it for subsequent visits
		node new_named_node(_i, node::logical_node_tag);
		new_named_node.name = get_original_node_name<bool_tag>(_i, opt);
		STACKTRACE_INDENT_PRINT("registering: " << new_named_node.name << endl);
		opt.mangle_instance(new_named_node.name);
		ret = node_pool.size();
		INVARIANT(ret);
#if NETLIST_CHECK_NAME_COLLISIONS
		check_name_collisions(new_named_node.name, ret, opt);
#endif
		node_pool.push_back(new_named_node);
#if 0 && ENABLE_STACKTRACE
node_pool.back().dump_raw(STACKTRACE_INDENT_PRINT("new node: ")) << endl;
#endif
		// mark new node as used here?
	}	// else already mapped
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_VERILOG
/**
	For structures and processes that are passed as ports.  
 */
index_type
netlist::register_named_proc(const index_type _i, 
		const netlist* type, const netlist_options& opt) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("local id (+1) = " << _i << endl);
	INVARIANT(_i);
	const index_type i = _i -1;	// locally 0-indexed, no gap
	INVARIANT(i < named_proc_map.size());
	index_type& ret(named_proc_map[i]);
	if (!ret) {
		// reserve a new slot and update it for subsequent visits
		proc new_named_proc(_i, type);
		new_named_proc.name =
			get_original_node_name<process_tag>(_i, opt);
		STACKTRACE_INDENT_PRINT("registering: " << new_named_proc.name << endl);
		opt.mangle_instance(new_named_proc.name);
		ret = proc_pool.size();
		INVARIANT(ret);
#if NETLIST_CHECK_NAME_COLLISIONS
		check_struct_name_collisions(new_named_proc.name, ret, opt);
#endif
		proc_pool.push_back(new_named_proc);
#if 0 && ENABLE_STACKTRACE
proc_pool.back().dump_raw(STACKTRACE_INDENT_PRINT("new proc: ")) << endl;
#endif
		// mark new node as used here?
	}	// else already mapped
	return ret;
}
#endif	// NETLIST_VERILOG

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const transistor&
netlist::lookup_transistor(const transistor_reference& tr) const {
	if (tr.first) {
		return local_subcircuits[tr.first -1]
			.get_transistor(tr.second);
	} else {
		return transistor_pool[tr.second];
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
bool
__local_subckt_less(const size_t i, const local_netlist& r) {
	return i < r.get_index_offset();
}

/**
	\return local subcircuit index and transistor index
 */
transistor_reference
netlist::lookup_transistor_index(const size_t ti) const {
#if 0
	cerr << "(ti=" << ti << ')';
#endif
	if (ti < transistor_count()) {
		return transistor_reference(0, ti);
	} else {
		const vector<local_netlist>::const_iterator
			b(local_subcircuits.begin()),
			e(local_subcircuits.end());
		vector<local_netlist>::const_iterator
			f(std::upper_bound(b, e, ti, &__local_subckt_less));
		INVARIANT(f != b);
		--f;
		const size_t si = std::distance(b, f);
		const size_t rem = ti -f->get_index_offset();
#if 0
		cerr << "(si,rem=" << si << ',' << rem << ')';
#endif
		INVARIANT(rem < f->transistor_count());
		return transistor_reference(si+1, rem);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates (subckt,index) into unique index.
 */
size_t
netlist::reverse_lookup_transistor_index(const transistor_reference& r) const {
	if (r.first) {
		const local_netlist& n(local_subcircuits[r.first -1]);
		INVARIANT(r.second < n.transistor_count());
		return n.get_index_offset() +r.second;
	} else {
		INVARIANT(r.second < transistor_count());
		return r.second;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const transistor&
netlist::lookup_transistor(const size_t ti) const {
	return lookup_transistor(lookup_transistor_index(ti));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Total number of transistors in this netlist, 
	including local subcircuits.  
 */
size_t
netlist::total_transistor_count(void) const {
	if (local_subcircuits.size()) {
		const local_netlist& l(local_subcircuits.back());
		return l.get_index_offset() +l.transistor_count();
	} else	return transistor_count();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note which nodes are actually used, to avoid emitting unused nodes.
 */
void
netlist::mark_used_nodes(void) {
	STACKTRACE_VERBOSE;
	netlist_common::mark_used_nodes(node_pool);
{
	// traverse subcircuit instances
	instance_pool_type::iterator
		i(instance_pool.begin()), e(instance_pool.end());
	size_t k = 0;		// index into instance_pool
	for ( ; i!=e; ++i, ++k) {
		i->mark_used_nodes(node_pool);
#if NETLIST_NODE_GRAPH
		instance::node_actuals_list_type::const_iterator
			ii(i->node_actuals.begin()),
			ie(i->node_actuals.end());
		size_t j = 0;	// index into actuals list
		for ( ; ii!=ie; ++ii, ++j) {
			const node_terminal t('x', k, j);
			node_pool[*ii].terminals.push_back(t);
		}
#endif
	}
}{
	// traverse subcircuits
	local_subcircuit_list_type::iterator
		i(local_subcircuits.begin()), e(local_subcircuits.end());
	for ( ; i!=e; ++i) {
		i->mark_used_nodes(node_pool);
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	\param ni is a footprint-local id for the referenced bool.
 */
bool
netlist::named_node_is_used(const index_type ni) const {
	// translate to netlist-local index
	const index_type lni = named_node_map[ni];
	// must be non-zero to count
	return lni && node_pool[lni].used;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: mangle_map for structs
 */
ostream&
netlist::emit_mangle_map(ostream& o, const netlist_options& nopt) const {
	o << nopt.comment_prefix << "BEGIN node name mangle map" << endl;
	typedef	node_pool_type::const_iterator	const_iterator;
	const_iterator i(node_pool.begin()), e(node_pool.end());
	for (++i ; i!=e; ++i) {
	if (i->is_logical_node()) {	// includes implicit supply nodes
		const size_t nid = i->index;
		const string oname =
			get_original_node_name<bool_tag>(nid, nopt);
		o << nopt.comment_prefix << "\t" << i->name << " : " << oname << endl;
	}
	}
	return o << nopt.comment_prefix << "END node name mangle map" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist::collect_node_ports(vector<string>& o,
		const netlist_options& nopt) const {
	typedef	node_port_list_type::const_iterator		const_iterator;
	const_iterator i(node_port_list.begin()), e(node_port_list.end());
	for ( ; i!=e; ++i) {
		ostringstream oss;		// stage for name mangling
		// TODO: for verilog, emit 'wire' and 'direction'
		const node& nd(node_pool[*i]);
		nd.emit(oss, nopt);
		// already mangled during name caching
		o.push_back(oss.str());
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
netlist::emit_subinstances(ostream& o, const netlist_options& nopt) const {
	// emit subinstances
#if ENABLE_STACKTRACE
	o << nopt.comment_prefix << "instances:" << endl;
#endif
	typedef	instance_pool_type::const_iterator	const_iterator;
	size_t j = 0;	// DEBUG
	const_iterator i(instance_pool.begin()), e(instance_pool.end());
	for ( ; i!=e; ++i, ++j) {
		STACKTRACE_INDENT_PRINT("j = " << j << endl);
	if ((nopt.empty_subcircuits || !i->is_empty())
			&& !i->type->is_channel_or_struct()) {
		i->emit(o, node_pool, 
#if NETLIST_VERILOG
			proc_pool,
#endif
			*fp, nopt) << endl;
	}
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
netlist::emit_local_subcircuits(ostream& o, const netlist_options& nopt) const {
#if ENABLE_STACKTRACE
	// nested subcircuits?
	o << nopt.comment_prefix << "local subcircuits:" << endl;
#endif
	// print definition
	// print singleton instance
	typedef	local_subcircuit_list_type::const_iterator	const_iterator;
	const_iterator i(local_subcircuits.begin()), e(local_subcircuits.end());
	for ( ; i!=e; ++i) {
	if (nopt.nested_subcircuits) {
		i->emit_definition(o, *this, nopt);	// definition
	}
		i->emit_instance(o, *this, nopt);	// instance
	}
	// alternately, emit subcircuits outside, not nested
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_VERILOG
void
netlist::collect_struct_ports(vector<string>& ports,
		const netlist_options& nopt) const {
	typedef	proc_port_list_type::const_iterator		const_iterator;
	const_iterator i(proc_port_list.begin()), e(proc_port_list.end());
	for ( ; i!=e; ++i) {
		ostringstream oss;		// stage for name mangling
		// just comma-separate list of names
		const proc& nd(proc_pool[*i]);
		nd.emit(oss);
		// already mangled during name caching
		ports.push_back(oss.str());
	}
}	// end netlist::collect_struct_ports

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Using the local instance pool instead of the footprint's instance
	pool misses out on port structs.  
 */
ostream&
netlist::emit_verilog_struct_locals(ostream& o, 
		const netlist_options& nopt) const {
	STACKTRACE_VERBOSE;
	// emit local channel declarations, including ports
	// just use footprint directly
	typedef	process_tag			tag_type;
	typedef	class_traits<tag_type>		traits_type;
	typedef	traits_type::instance_collection_parameter_type
						type_type;
	typedef	instance_alias_info<tag_type>	alias_type;
	typedef	entity::collection_interface<tag_type>
						collection_type;
	typedef	alias_type::container_type	container_type;
	typedef	state_instance<tag_type>	instance_type;
	typedef	state_instance<tag_type>::pool_type
						pool_type;
if (nopt.struct_ports) {
#if ENABLE_STACKTRACE
	o << nopt.comment_prefix << "local " << traits_type::tag_name
		<< ":" << endl;
#endif
	const pool_type&
		ppool(fp->get_instance_pool<tag_type>());
	proc_pool_type::const_iterator
		pi(++proc_pool.begin()), pe(proc_pool.end());
//	STACKTRACE_INDENT_PRINT("proc_pool.size() = " << proc_pool.size() << endl);
	for ( ; pi!=pe; ++pi) {
		const index_type lpid = pi->index;
		const instance_type& p(ppool[lpid -1]);
		const never_ptr<const alias_type> pref(p.get_back_ref());
		const alias_type& _pref(*pref);
		const netlist* type = pi->type;
		NEVER_NULL(type);
		NEVER_NULL(type->fp);
		const entity::meta_type_tag_enum
			t(type->fp->get_meta_type());
		if (t != traits_type::type_tag_enum_value) {
			// then is a channel or datastruct
			// print direction
			const bool inny = _pref.is_input_port();
			const bool outy = _pref.is_output_port();
			if (inny) {
				if (outy) {
					o << "inout ";
				} else {
					o << "input ";
				}
			} else if (outy) {
				o << "output ";
			}
			// print type -- already mangled
			o << type->get_name() << ' ';
			// print name -- already mangled
			o << pi->name;
			o << ';' << endl;
		}
		// else skip processes -- they are sub module instances
	}	// end for each local instance
}
	return o;
}	// end netlist::emit_verilog_locals

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Using the local instance pool instead of the footprint's instance
	pool misses out on port wires.  
 */
ostream&
netlist::emit_verilog_wire_locals(ostream& o, 
		const netlist_options& nopt) const {
	STACKTRACE_VERBOSE;
	// emit local channel declarations, including ports
	// just use footprint directly
	typedef	bool_tag			tag_type;
	typedef	class_traits<tag_type>		traits_type;
	typedef	traits_type::instance_collection_parameter_type
						type_type;
	typedef	instance_alias_info<tag_type>	alias_type;
	typedef	entity::collection_interface<tag_type>
						collection_type;
	typedef	alias_type::container_type	container_type;
	typedef	state_instance<tag_type>	instance_type;
	typedef	state_instance<tag_type>::pool_type
						pool_type;
#if ENABLE_STACKTRACE
	o << nopt.comment_prefix << "local " << traits_type::tag_name
		<< ":" << endl;
#endif
	const pool_type&
		ppool(fp->get_instance_pool<tag_type>());
	node_pool_type::const_iterator
		pi(++node_pool.begin()), pe(node_pool.end());
		// skip the void node
	for ( ; pi!=pe; ++pi) {
		bool inny = false;
		bool outy = false;
		if (pi->is_logical_node()) {
		const index_type lpid = pi->index;
		const instance_type& p(ppool[lpid -1]);
		const never_ptr<const alias_type> pref(p.get_back_ref());
		const alias_type& _pref(*pref);
		inny = _pref.is_input_port();
		outy = _pref.is_output_port();
		}
		if (inny) {
			if (outy) {
				o << "inout ";
			} else {
				o << "input ";
			}
		} else if (outy) {
			o << "output ";
		} else {
			o << "wire ";
		}
		// print type -- already mangled
		// o << type->get_name() << ' ';
		// print name -- already mangled
		o << pi->name;
		o << ';' << endl;
		// else skip processes -- they are sub module instances
	}	// end for each local instance
	return o;
}	// end netlist::emit_verilog_locals

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#endif	// NETLIST_VERILOG

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should also track line length and auto-wrap.
 */
ostream&
netlist::emit_header(ostream& o, const netlist_options& nopt) const {
	size_t line_length = 0;
switch (nopt.subckt_def_style) {
case netlist_options::STYLE_SPECTRE: 
	o << "subckt ";
	line_length += 7;
	break;
case netlist_options::STYLE_SPICE: 
	o << ".subckt ";
	line_length += 8;
	break;
case netlist_options::STYLE_VERILOG:
	o << "module ";
	line_length += 7;
	break;
default:
	o << "define-subcircuit ";
	line_length += 18;
	break;
}	// end switch
	o << name;
	line_length += name.length();

	vector<string> ports;
if (nopt.node_ports) {
	collect_node_ports(ports, nopt);
}
#if NETLIST_VERILOG
if (nopt.struct_ports) {
	// name every port, K&R-style
	collect_struct_ports(ports, nopt);
}
#endif
	vector<string>::const_iterator i(ports.begin()), e(ports.end());
switch (nopt.subckt_def_style) {
case netlist_options::STYLE_SPICE:
case netlist_options::STYLE_SPECTRE:
	for (; i!=e; ++i) {
		const size_t wl = i->length() +1;
		line_length += wl;
		const bool wrap = (nopt.auto_wrap_length &&
			(line_length > nopt.auto_wrap_length));
		if (wrap) {
			nopt.line_continue(o);
			line_length = nopt.post_line_continue.length() +wl;
		}
		o << ' ' << *i;
	}
	break;
case netlist_options::STYLE_VERILOG:
	o << " (";
	line_length += 2;
if (ports.size()) {
	o << *i;
	for ( ++i; i!=e; ++i) {
		o << ", ";
		const size_t wl = i->length() +2;
		line_length += wl;
		const bool wrap = (nopt.auto_wrap_length &&
			(line_length > nopt.auto_wrap_length));
		if (wrap) {
			nopt.line_continue(o);
			line_length = nopt.post_line_continue.length() +wl;
		}
		o << *i;
	}
}
	o << ");";
}	// end switch
	return o << endl;
	// TODO: emit port-info comments
}	// end declaration with ports

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_NODE_CAPS
ostream&
netlist::emit_node_caps(ostream& o, const netlist_options& nopt) const {
	return node::emit_node_caps(o, node_pool, nopt);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
netlist::node_port_name(const size_t ni) const {
	return node_pool[node_port_list[ni]].name;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre all dependent subinstances have been processed.
		This should really only be called from netlist_generator.
	\param sub is true if this is a subckt and should be wrapped
	in .subckt/.ends with ports declared.
	TODO: option to emit dangling unused internal nodes?
 */
ostream&
netlist::emit(ostream& o, const bool sub, const netlist_options& nopt) const {
if (!nopt.nested_subcircuits) {
	// print definition
	// print singleton instance
	typedef	local_subcircuit_list_type::const_iterator	const_iterator;
	const_iterator i(local_subcircuits.begin()), e(local_subcircuits.end());
	for ( ; i!=e; ++i) {
		i->emit_definition(o, *this, nopt);	// definition
	}
}
if (sub) {
if (nopt.emit_mangle_map) {
	o << nopt.comment_prefix << "typename_mangle(\"";
	fp->dump_type(o) << "\") = " << name << endl;
}
	emit_header(o, nopt);
}	// end if sub
if (sub || nopt.emit_top) {
	// option to suppress top-level instances and rules
if (nopt.emit_mangle_map) {
	emit_mangle_map(o, nopt);
}
if (nopt.emit_port_summary) {
	emit_node_port_info(o, nopt);	// in comment block
}
if (nopt.emit_node_aliases) {
	const util::indent _temp_(o, nopt.comment_prefix + "\t");
	o << nopt.comment_prefix << "BEGIN node aliases" << endl;
	fp->get_scope_alias_tracker().dump_local_bool_aliases(o);
	o << nopt.comment_prefix << "END node aliases" << endl;
}
	// TODO: optional .connect alias statements
#if NETLIST_NODE_CAPS
if (nopt.emit_node_caps) {
	emit_node_caps(o, nopt);
}
#endif
#if NETLIST_NODE_GRAPH
if (nopt.emit_node_terminals) {
	emit_node_terminal_graph(o, nopt);
}
#endif
#if NETLIST_VERILOG
if (nopt.subckt_def_style == netlist_options::STYLE_VERILOG) {
	emit_verilog_struct_locals(o, nopt);
	emit_verilog_wire_locals(o, nopt);
}	// end if verilog mode
#endif	// NETLIST_VERILOG
	emit_subinstances(o, nopt);
	emit_local_subcircuits(o, nopt);
	emit_devices(o, nopt);
}
if (sub) {
switch (nopt.subckt_def_style) {
case netlist_options::STYLE_SPECTRE: 
	o << "ends " << name << endl;
	break;
case netlist_options::STYLE_SPICE: 
	o << ".ends" << endl;
	break;
case netlist_options::STYLE_VERILOG: 
	o << "endmodule" << endl;	// name?
	break;
default:
	o << "end-subcircuit " << name << endl;
	break;
}	// end switch
}	// end if sub
	return o;
}	// end netlist::emit()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Debug-only mode for printing raw structure of netlist.
 */
ostream&
netlist::dump_raw(ostream& o) const {
	ostream_iterator<index_type> osi(o, ",");
	o << "netlist {" << endl;
	o << "type: " << name << endl;
	o << "footprint @ " << fp << endl;
	fp->dump(o) << endl;
{
	o << "node pool:" << endl;
	size_t j = 0;
	for ( ; j<node_pool.size(); ++j) {
		o << "  [" << j << "]: ";
		const node& n(node_pool[j]);
		n.dump_raw(o) << " = ";
#if 1
		n.emit(o, netlist_options::default_value);
		// may have to comment out for debug
#else
		o << "...";
#endif
		o << endl;
	}
}{
#if NETLIST_VERILOG
	o << "struct pool:" << endl;
	size_t j = 0;
	for ( ; j<proc_pool.size(); ++j) {
		o << "  [" << j << "]: ";
		const proc& n(proc_pool[j]);
		n.dump_raw(o) << " = ";
#if 1
		n.emit(o);
		// may have to comment out for debug
#else
		o << "...";
#endif
		o << endl;
	}
#endif
}{
	o << "ports (node indices): ";
	copy(node_port_list.begin(), node_port_list.end(), osi);
	o << endl;
#if NETLIST_VERILOG
	o << "ports (struct indices): ";
	copy(proc_port_list.begin(), proc_port_list.end(), osi);
	o << endl;
#endif
}{
	o << "named node map (footprint-index -> netlist-node-index):" << endl;
	size_t j = 0;
	for ( ; j<named_node_map.size(); ++j) {
		o << "  :" << j+1 << " -> [" << named_node_map[j] << ']' << endl;
	}
}{
#if NETLIST_VERILOG
	o << "struct map (footprint-index -> netlist-node-index):" << endl;
	size_t j = 0;
	for ( ; j<named_proc_map.size(); ++j) {
		o << "  :" << j+1 << " -> [" << named_proc_map[j] << ']' << endl;
	}
#endif
}{
	o << "internal node map (footprint-index -> netlist-node-index):" << endl;
	typedef	internal_node_map_type::const_iterator	const_iterator;
	const_iterator i(internal_node_map.begin()), e(internal_node_map.end());
	const entity::PRS::footprint* fpr =
		(fp->has_prs_footprint() ? &fp->get_prs_footprint() : NULL);
	if (fpr) {
	size_t j = 0;
	for ( ; i!=e; ++i) {
		const prs_footprint::node_expr_type&
			n(fpr->get_internal_node(j));
		o << "  @" << j << " -> [" << i->first << ']' <<
			(n.second ? '+' : '-');
		if (i->second) {
			o << " in subckt{" << i->second -1 << "}";
		}
		o << endl;
	}
	}
}{
	o << "instances:" << endl;
	typedef	instance_pool_type::const_iterator	const_iterator;
	const_iterator i(instance_pool.begin()), e(instance_pool.end());
	for ( ; i!=e; ++i) {
		i->dump_raw(o << "  ") << endl;
	}
}{
#if ENABLE_STACKTRACE
	o << "subcircuits: {" << endl;
#endif
	// represents both definition and instance
	typedef	local_subcircuit_list_type::const_iterator	const_iterator;
	const_iterator i(local_subcircuits.begin()), e(local_subcircuits.end());
	for ( ; i!=e; ++i) {
		i->dump_raw(o);
	}
	o << '}' << endl;
}{
	o << "transistors:" << endl;
	dump_raw_devices(o);
}
	return o << '}' << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	unique_list<index_type>	port_index_list_type;

/** 
	Unfortunately, need this to preserve port formal decl ordering.
 */
template <class Tag>
class port_alias_collector :
		public meta_type_port_collector<Tag, port_index_list_type> {
	typedef	meta_type_port_collector<Tag, port_index_list_type>
						parent_type;
	const footprint&			fp;
	const size_t				n_ports;
public:
	explicit
	port_alias_collector(const footprint& f) :
		parent_type(), fp(f),
		n_ports(fp.get_instance_pool<Tag>().port_entries()) { }

protected:
	VISIT_INSTANCE_ALIAS_INFO_PROTO(Tag);
	using parent_type::visit;

public:
	void
	operator () (void) {
		const port_formals_manager&
			fm(fp.get_owner_def()
				.template is_a<const process_definition>()
				->get_port_formals());
		port_formals_manager::const_list_iterator
			pi(fm.begin()), pe(fm.end());
		for ( ; pi!=pe; ++pi) {
			fp[(*pi)->get_name()]
				.template is_a<const physical_instance_collection>()
					->accept(*this);
		}
	}
};	// end class

template <class Tag>
void
port_alias_collector<Tag>::visit(const instance_alias_info<Tag>& a) {
	// since ports are sorted before locals, we can compare indices
	if (a.instance_index <= n_ports) {	// 1-based index
		parent_type::visit(a);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	port_alias_collector<bool_tag>		bool_port_alias_collector;
typedef	port_alias_collector<process_tag>	process_port_alias_collector;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create a port summary so that other processes may correctly
	instantiate this.  
	This also summarizes the 'empty' flag for this netlist.  
	\param opt for netlist generation configuration
 */
void
netlist::summarize_ports(
#if NETLIST_VERILOG
		const netlist_map_type& netmap,
#endif
		const netlist_options& opt) {
	STACKTRACE_VERBOSE;
	// could mark_used_nodes here instead?
	// empty is initially false
	bool MT = true;
{
	// this ordering is based on port_formal_manager, declaration order
	bool_port_alias_collector V(*fp);
	V();
	STACKTRACE_INDENT_PRINT("named_node_map.size() = " << named_node_map.size() << endl);
	const entity::state_instance<bool_tag>::pool_type&
		bp(fp->get_instance_pool<bool_tag>());
	port_index_list_type::const_iterator
		i(V.indices.begin()), e(V.indices.end());
	node_port_list.reserve(V.indices.size() +2);	// for supplies
	for ( ; i!=e; ++i) {
		// 1-indexed local id to 0-indexed named_node_map
		INVARIANT(*i);
		const index_type& j(*i);
		const index_type local_ind = j -1;
		STACKTRACE_INDENT_PRINT("(bool) local_ind = " << local_ind << endl);
		INVARIANT(local_ind < named_node_map.size());
		index_type ni = named_node_map[local_ind];
		const instance_alias_info<bool_tag>&
			bref(*(bp[local_ind].get_back_ref()));
		// nodes explicitly marked as used/driven should be honored
		const bool b_driven = bref.has_any_fanin();
		const bool b_used = bref.has_any_fanout();
	if (!ni && (opt.unused_ports || b_driven || b_used)) {
		// the consider all ports used, even if unconnected
		ni = register_named_node(j, opt);
		node& n(node_pool[ni]);
		// even driven nodes are considered 'used'
		n.used = true;
		if (b_driven) {
			n.driven = true;
		}
	}
		const node& n(node_pool[ni]);
#if NETLIST_CHECK_CONNECTIVITY
	if (ni && (n.used || n.driven))
#else
	if (ni && n.used)
#endif
	{
		INVARIANT(n.is_logical_node());
		INVARIANT(n.index == j);	// self-reference
		node_port_list.push_back(ni);
		// sorted_ports[local_ind] = ni;
		MT = false;
	}
	}	// end for
}{
#if NETLIST_VERILOG
	STACKTRACE_INDENT_PRINT("process ports..." << endl);
	// this ordering is based on port_formal_manager, declaration order
	process_port_alias_collector V(*fp);
	V();
	STACKTRACE_INDENT_PRINT("collected." << endl);
	STACKTRACE_INDENT_PRINT("named_proc_map.size() = " << named_proc_map.size() << endl);
	const state_instance<process_tag>::pool_type& 
		lppool(fp->get_instance_pool<process_tag>());
	port_index_list_type::const_iterator
		i(V.indices.begin()), e(V.indices.end());
	proc_port_list.reserve(V.indices.size() +2);	// for supplies
	for ( ; i!=e; ++i) {
		// 1-indexed local id to 0-indexed named_proc_map
		INVARIANT(*i);
		const index_type& j(*i);
		const index_type local_ind = j -1;
		STACKTRACE_INDENT_PRINT("(proc) local_ind = " << local_ind << endl);
		INVARIANT(local_ind < named_proc_map.size());
		index_type ni = named_proc_map[local_ind];
	if (!ni) {
		// the consider all ports used, even if unconnected
		const state_instance<process_tag>& p(lppool[local_ind]);
		const footprint* pf = p._frame._footprint;
		const netlist& nl(netmap.find(pf)->second);
		ni = register_named_proc(j, &nl, opt);
		proc_pool[ni].used = true;
	}
		const proc& p(proc_pool[ni]);
	if (ni && p.used) {
		INVARIANT(p.index == j);	// self-reference
		proc_port_list.push_back(ni);
	}
	}	// end for
#endif	// NETLIST_VERILOG
}
if (MT) {
	// pretty much same as netlist_common::is_empty
	MT = netlist_common::is_empty();
}
if (MT) {
	// check subcircuits
	local_subcircuit_list_type::const_iterator
		i(local_subcircuits.begin()), e(local_subcircuits.end());
	for ( ; MT && i!=e; ++i) {
	if (!i->is_empty()) {
		MT = false;	// stop on non-empty subcircuit
	}
	}
}
if (MT) {
	// check instances
	instance_pool_type::const_iterator
		i(instance_pool.begin()), e(instance_pool.end());
	for ( ; MT && i!=e; ++i) {
	if (!i->is_empty()) {
		MT = false;	// stop on non-empty instance
	}
	}
}
	empty = MT;
}	// end netlist::summarize_ports

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_CACHE_PARASITICS
/**
	Only after netlist subgraph is complete, can parasitics be
	accurately evaluated.  Need to know which nodes are end nodes.
 */
void
netlist::summarize_parasitics(const netlist_options& nopt) {
	device_group::summarize_parasitics(node_pool, nopt);
{
	// instances already accounted for when created?
	typedef	instance_pool_type::const_iterator	const_iterator;
	const_iterator i(instance_pool.begin()), e(instance_pool.end());
	for ( ; i!=e; ++i) {
		const netlist& type(*i->type);
		instance::node_actuals_list_type::const_iterator
			ai(i->node_actuals.begin()),
			ae(i->node_actuals.end());
		node_port_list_type::const_iterator
			fi(type.node_port_list.begin()),
			fe(type.node_port_list.end());
		for ( ; fi!=fe; ++fi, ++ai) {
			node_pool[*ai].cap += type.node_pool[*fi].cap;
		}
		INVARIANT(ai == ae);
	}
}
	local_subcircuit_list_type::iterator
		i(local_subcircuits.begin()), e(local_subcircuits.end());
	for ( ; i!=e; ++i) {
		// subcircuit formals
		i->summarize_parasitics(nopt);
		// then apply to actuals
		const local_netlist::node_index_map_type& nm(i->node_index_map);
		local_netlist::node_index_map_type::const_iterator
			li(nm.begin()), le(nm.end());
		for ( ; li!=le; ++li) {
			node_pool[li->first].cap += i->node_pool[li->second].cap;
		}
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_CHECK_CONNECTIVITY
/**
	\return true if we are to error out due to connectivity
 */
error_status
netlist::check_node_connectivity(const netlist_options& opt) const {
	error_status ret = STATUS_NORMAL;
if (opt.undriven_node_policy != OPTION_IGNORE) {
	// set to test for port membership
	typedef	set<index_type>	port_set_type;
	port_set_type port_set;
	copy(node_port_list.begin(), node_port_list.end(),
		util::set_inserter(port_set));
	const port_set_type::const_iterator pe(port_set.end());
	typedef	node_pool_type::const_iterator	const_iterator;
	const_iterator i(node_pool.begin()), e(node_pool.end());
	index_type j = first_node_index;
	for (++i; i!=e; ++i, ++j) {
	if (port_set.find(j) == pe) {
		const error_status r = i->check_connectivity(opt);
		if (r > ret)
			ret = r;
	}
	}
	if (ret != STATUS_NORMAL) {
		// .name is mangled, so we print the unmangled type name
		cerr << "... in subcircuit ";
		fp->dump_type(cerr) << endl;
	}
}
	return ret;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_NODE_GRAPH
/**
	Prints the node view of connections to devices terminals.
 */
ostream&
netlist::emit_node_terminal_graph(ostream& o,
		const netlist_options& nopt) const {
	o << nopt.comment_prefix << "BEGIN node terminals" << endl;
	typedef	node_pool_type::const_iterator	const_iterator;
	const_iterator i(node_pool.begin()), e(node_pool.end());
for (++i; i!=e; ++i) {
	// include all nodes, internal, stack, supply, etc...
	i->emit(o << nopt.comment_prefix << "\t", nopt) << " :";
	vector<node_terminal>::const_iterator
		ti(i->terminals.begin()), te(i->terminals.end());
	for ( ; ti!=te; ++ti) {
		// just space-delimited
		o << ' ';
	switch (ti->device_type) {
	case 'M': {
		// transistor may be in a local subcircuit
		const node_pool_type* np = &node_pool;
		const transistor_reference
			tr(lookup_transistor_index(ti->index));
		if (tr.first) {
			const local_netlist& ln(local_subcircuits[tr.first-1]);
			ln.emit_instance_name(o, nopt) << '/';
			np = &ln.node_pool;
		}
		lookup_transistor(tr)
			.emit_identifier(o, ti->index, *np, nopt) <<
				nopt.__dump_flags.process_member_separator <<
				char(ti->port);
		break;
	}
	case 'x': {
		const instance& inst(instance_pool[ti->index]);
		const string& pname(inst.name);
		o << nopt.subckt_instance_prefix << pname <<
			nopt.__dump_flags.process_member_separator <<
		inst.type->node_port_name(ti->port);
		break;
	}
	// unhandled case
	default: o << ti->device_type << ti->index <<
			nopt.__dump_flags.process_member_separator <<
			ti->port;
		break;
	}	// end switch
	}	// end for
	o << endl;
}
	return o << nopt.comment_prefix << "END node terminals" << endl;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if we are to error out due to connectivity
 */
ostream&
netlist::emit_node_port_info(ostream& o, const netlist_options& opt) const {
	typedef	node_port_list_type::const_iterator	const_iterator;
	const_iterator i(node_port_list.begin()), e(node_port_list.end());
	o << opt.comment_prefix << "BEGIN node port info" << endl;
	for (; i!=e; ++i) {
		const node& n(node_pool[*i]);
		n.emit_port_summary(
			o << opt.comment_prefix << '\t', opt) << endl;
	}
	o << opt.comment_prefix << "END node port info" << endl;
	return o;
}

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

