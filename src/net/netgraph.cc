/**
	\file "net/netgraph.cc"
	$Id: netgraph.cc,v 1.29 2011/03/24 15:20:52 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <sstream>		// for ostringstream
#include <iterator>		// for ostream_iterator
#include "net/netgraph.h"
#include "net/netlist_options.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#include "Object/global_entry_context.h"
#include "Object/def/footprint.h"
#include "Object/def/process_definition.h"
#include "Object/traits/instance_traits.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/bool_port_collector.tcc"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_pool.h"
#include "util/iterator_more.h"		// for set_inserter
#include "util/unique_list.tcc"
#include "util/string.h"		// for strgsub
#include "util/stacktrace.h"
#include "util/indent.h"

namespace HAC {
namespace NET {
#include "util/using_ostream.h"
using entity::footprint_frame_map_type;
using entity::bool_port_collector;
using entity::instance_alias_info;
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
void
device_group::mark_used_nodes(node_pool_type& node_pool) const {
	transistor_pool_type::const_iterator
		i(transistor_pool.begin()), e(transistor_pool.end());
	for ( ; i!=e; ++i) {
		i->mark_used_nodes(node_pool);
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

//=============================================================================
// class netlist_common method definitions
bool
netlist_common::is_empty(void) const {
	return device_group::is_empty() && passive_device_pool.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Since these devices don't belong to a particular group, 
	don't use device_group::emit_devices.  
 */
ostream&
netlist_common::emit_devices(ostream& o, const node_pool_type& node_pool,
		const footprint& fp, const netlist_options& nopt) const {
	// emit devices
#if ENABLE_STACKTRACE
	o << nopt.comment_prefix << "devices:" << endl;
#endif
#if NETLIST_GROUPED_TRANSISTORS
	// preserve per-node device counters, just save the whole node pool
	typedef util::ptr_value_saver<index_type>	save_type;
	typedef	vector<pair<save_type, save_type> >	saves_type;
	saves_type __s;		// will auto-restore at end-of-scope
{
	__s.resize(node_pool.size());
	node_pool_type::const_iterator ni(node_pool.begin());
	saves_type::iterator i(__s.begin()), e(__s.end());
	for ( ; i!=e; ++i, ++ni) {
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
		i->emit(o, j, node_pool, fp, nopt) << endl;
	}
	emit_passive_devices(o, node_pool, fp, nopt);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this code is *untested*, until we have some language hook
	for adding capacitors and passive components.  
 */
ostream&
netlist_common::emit_passive_devices(ostream& o,
		const node_pool_type& node_pool,
		const footprint& fp, const netlist_options& nopt) const {
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
}	// end emit_passive_devices

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

/**
	Summarizes subcircuit ports by looking at all used nodes.  
	TODO: passive devices
 */
void
local_netlist::mark_used_nodes(node_pool_type& node_pool) {
	transistor_pool_type::const_iterator
		i(transistor_pool.begin()), e(transistor_pool.end());
	for ( ; i!=e; ++i) {
		node_index_map.insert(i->gate);
		node_index_map.insert(i->source);
		node_index_map.insert(i->drain);
		node_index_map.insert(i->body);
		i->mark_used_nodes(node_pool);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
local_netlist::dump_raw(ostream& o, const netlist& n) const {
	o << name;
	typedef	node_index_map_type::const_iterator	const_iterator;
	const_iterator i(node_index_map.begin()), e(node_index_map.end());
	for ( ; i!=e; ++i) {
		n.node_pool[*i].dump_raw(o << ' ');
	}
	o << endl;
	o << "transistors:" << endl;
	dump_raw_devices(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
	typedef	node_index_map_type::const_iterator	const_iterator;
	const_iterator i(node_index_map.begin()), e(node_index_map.end());
	for ( ; i!=e; ++i) {
		const node& nd(n.node_pool[*i]);
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
	// TODO: emit port-info comments
	emit_devices(o, n.node_pool, *n.fp, nopt);
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiation uses the same names as the original ports.
 */
ostream&
local_netlist::emit_instance(ostream& o, const netlist& n,
		const netlist_options& nopt) const {
	o << nopt.subckt_instance_prefix;
	o << name << nopt.emit_colon() << "inst";
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
	typedef	node_index_map_type::const_iterator	const_iterator;
	const_iterator i(node_index_map.begin()), e(node_index_map.end());
	for ( ; i!=e; ++i) {
		const node& nd(n.node_pool[*i]);
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
}

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
#if !PRS_SUPPLY_OVERRIDES
case NODE_TYPE_SUPPLY:
	o << name;	// prefix with any designator? '$' or '!' ?
	break;
#endif
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
#if !PRS_SUPPLY_OVERRIDES
case NODE_TYPE_SUPPLY:
	o << '!' << name;
	break;
#endif
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

//=============================================================================
// class instance method definitions

/**
	Mark all instance actuals as used automatically?
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
ostream&
instance::emit(ostream& o, const node_pool_type& node_pool, 
		const footprint& fp, const netlist_options& nopt) const {
	string pname;
{
	// process instance name
	ostringstream oss;
	fp.get_instance_pool<process_tag>()[pid -1].get_back_ref()
		->dump_hierarchical_name(oss, nopt.__dump_flags);
	pname = oss.str();
}
if (nopt.emit_mangle_map) {
	o << nopt.comment_prefix << "instance: " << type->get_unmangled_name()
		<< ' ' << pname << endl;
}
	o << nopt.subckt_instance_prefix;
	nopt.mangle_instance(pname);
	o << pname;
{
	// actuals
	const bool paren =
		(nopt.instance_port_style == netlist_options::STYLE_SPECTRE);
	if (paren) o << " (";
	ostringstream oss;
	node_actuals_list_type::const_iterator
		i(node_actuals.begin()), e(node_actuals.end());
	for ( ; i!=e; ++i) {
		node_pool[*i].emit(oss << ' ', nopt);
	}
	string line(oss.str());
	// already mangled during name caching
	o << line;
	if (paren) o << " )";
}
	// type name is already mangled
	o << ' ' << type->get_name();	// endl
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance::dump_raw(ostream& o) const {
	o << '[' << pid << "]: " << type->get_name() << ": (";
	copy(node_actuals.begin(), node_actuals.end(),
		ostream_iterator<index_type>(o, ","));
	return o << ')';
}


//=============================================================================
// class transistor method definitions

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
/**
	\param di the device index, incrementing.
 */
ostream&
transistor::emit(ostream& o, const index_type di, 
		const node_pool_type& node_pool,
		const footprint& fp, const netlist_options& nopt) const {
#if NETLIST_GROUPED_TRANSISTORS
	// don't use ordinal index di
	const node& n(node_pool[assoc_node]);
	INVARIANT(!n.is_auxiliary_node());
	index_type& c(n.device_count[size_t(assoc_dir)]);
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
			(assoc_dir ? "up" : "dn") << nopt.emit_colon() << c;
	}
	++c;	// unconditionally
#else
	o << nopt.transistor_prefix << di << '_';
#endif
	// attribute suffixes cannot be overridden
	emit_attribute_suffixes(o, nopt) << ' ';

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

	// TODO: restrict lengths and widths, from tech/conf file
	o << " W=" << width *nopt.lambda << nopt.length_unit <<
		" L=" << length *nopt.lambda << nopt.length_unit;
	if (nopt.emit_parasitics) {
		// compute and emit parasitic area/perimeter values
		const bool pge = nopt.fet_perimeter_gate_edge;
		const real_type lsq = nopt.lambda * nopt.lambda;
		const real_type l2 = nopt.lambda * 2.0;
		const bool s_ext = s.is_stack_end_node();
		const bool d_ext = d.is_stack_end_node();
		const real_type half_spacing = nopt.fet_spacing_diffonly / 2.0;
		const real_type& sl(s_ext ?
			nopt.fet_diff_overhang : half_spacing);
		const real_type& dl(d_ext ?
			nopt.fet_diff_overhang : half_spacing);
		// areas of internal stack nodes are halved to 
		// assume internal sharing, split to each sharer.
		const real_type asv = width * sl * lsq;
		const real_type psv = pge ? 
			(s_ext ? (width + sl) *l2		// width +side
				: sl*l2) :			// sides only
			(s_ext ? (width*nopt.lambda + sl*l2)	// 3 sides
				: (sl * l2));			// 2 sides
		const real_type adv = width * dl * lsq;
		const real_type pdv = pge ?
			(d_ext ? (width + dl) *l2		// width +side
				: dl*l2) :			// sides only
			(d_ext ? (width*nopt.lambda + dl*l2)	// 3 sides
				: (dl * l2));			// 2 sides
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
transistor::emit_attribute_suffixes(ostream& o, 
		const netlist_options& nopt) const {
	if (attributes & IS_PASS)
		o << nopt.emit_colon() << "pass";
	if (attributes & IS_PRECHARGE)
		o << nopt.emit_colon() << "pchg";
	if (attributes & IS_STANDARD_KEEPER)
		o << nopt.emit_colon() << "keeper";
	if (attributes & IS_COMB_FEEDBACK)
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

//=============================================================================
// class netlist method definitions

// tag objects for convenience
const node::__logical_node_tag	node::logical_node_tag = __logical_node_tag();
#if !PRS_SUPPLY_OVERRIDES
const node::__supply_node_tag	node::supply_node_tag = __supply_node_tag();
#endif
const node::__internal_node_tag	node::internal_node_tag = __internal_node_tag();
const node::__auxiliary_node_tag	node::auxiliary_node_tag = __auxiliary_node_tag();

// case sensitive?
const node
netlist::void_node("__VOID__", node::auxiliary_node_tag)
#if !PRS_SUPPLY_OVERRIDES
, netlist::GND_node("GND", node::supply_node_tag)
, netlist::Vdd_node("Vdd", node::supply_node_tag)
#endif
;

// universal node indices to every subcircuit
// these should correspond with the order of insertion in netlist's ctor
const	index_type
netlist::void_index = 0
#if !PRS_SUPPLY_OVERRIDES
// this must be kept consistent with ordering in "AST/globals.cc"
// and auto-connections in "Object/def/footprint.cc"
, netlist::GND_index = 1
, netlist::Vdd_index = 2
#endif
;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
netlist::netlist() : netlist_common(), name(), 
		named_node_map(), node_pool(),
		instance_pool(), internal_node_map(), 
		internal_expr_map(), 
#if NETLIST_CHECK_NAME_COLLISIONS
		name_collision_map(), 
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
	node_pool.reserve(8);	// reasonable pre-allocation
	// following order should match above universal node indices
	node_pool.push_back(void_node);
#if !PRS_SUPPLY_OVERRIDES
	node_pool.push_back(GND_node);
	node_pool.push_back(Vdd_node);
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
	fp = &f;
	// pre-allocate, using same indexing and mapping as original pool
	// null index initially, and default owner is not subcircuit
	const prs_footprint& pfp(f.get_prs_footprint());
	internal_node_map.resize(pfp.get_internal_node_pool().size(),
		internal_node_entry_type(0, 0));
	const prs_footprint::subcircuit_map_type&
		subs(pfp.get_subcircuit_map());
	local_subcircuits.resize(subs.size());
	// import/generate subcircuit names
	prs_footprint::subcircuit_map_type::const_iterator
		si(subs.begin()), se(subs.end());
	local_subcircuit_list_type::iterator
		li(local_subcircuits.begin());
	for (; si!=se; ++si, ++li) {
		const string& nn(si->get_name());
		if (nn.length()) {
			li->name = nn;
			// mangle here?
		} else {
			ostringstream oss;
			// TODO: mangle colon
			oss << "INTSUB" << nopt.emit_colon() << subs_count;
			li->name = oss.str();
			++subs_count;
		}
	}
	// TODO: pre-size instance_pool based on proceses
	// TODO: pre-size named_node_map based on bools
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
string
netlist::get_unmangled_name(void) const {
	ostringstream oss;
	fp->dump_type(oss);
	return oss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param subp global process entry containing footprint frame
		Should we use local footprint's instance instead?
		Technically, we shouldn't need global allocation information.
	\param subnet the netlist type that corresponds with instance.
	\param lpid local process id from footprint.
	\pre instance_pool is already pre-allocated to avoid 
		invalidating references due to re-allocation.
	TODO: possible check for name collisions?
 */
void
netlist::append_instance(const state_instance<process_tag>& subp,
		const netlist& subnet, const index_type lpid,
		const netlist_options& opt) {
	STACKTRACE_VERBOSE;
	const footprint* subfp = subp._frame._footprint;
	// cannot use global allocated footprint_frame
//	const netlist& subnet(netmap.find(subfp)->second);
	INVARIANT(subfp == subnet.fp);
	// subnet's port list may be shorter than formals list, due to aliases
	// traverse formals list and lookup each actual to be passed
	// recall: only used nodes will be in this port list
	instance_pool.push_back(instance(subnet, lpid));
	instance& np(instance_pool.back());
	// local process instance needed to find local port actual id
	const instance_alias_info<process_tag>&
		lp(*fp->get_instance_pool<process_tag>()[lpid -1]
			.get_back_ref());
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
#if !PRS_SUPPLY_OVERRIDES
		if (fn.is_supply_node()) {
			if (*fi == GND_index) {
				np.node_actuals.push_back(GND_index);
			} else if (*fi == Vdd_index) {
				np.node_actuals.push_back(Vdd_index);
			} else {
				cerr << "ERROR: unknown supply port." << endl;
				THROW_EXIT;
			}
		} else
#endif
		if (fn.is_logical_node()) {
			const index_type fid = fn.index;
			STACKTRACE_INDENT_PRINT("formal id = " << fid << endl);

			const instance_alias_info<bool_tag>&
				fb(*subnet.fp->get_instance_pool<bool_tag>()
					[fid -1].get_back_ref());
			INVARIANT(fb.is_aliased_to_port());
			index_type actual_id = 0;
			// ALERT: fb might pick a non-port alias!!!
			// picking *any* port-alias should suffice because
			// formal port-aliases have been replayed.
		if (!fb.is_port_alias()) {
			// HACK ALERT!
			// well, damn it, FIND me a suitable alias!
			typedef port_alias_tracker_base<bool_tag>::map_type
						alias_map_type;
			const alias_map_type&
				pat(subfp->get_scope_alias_tracker()
					.get_id_map<bool_tag>());
			alias_map_type::const_iterator
				f(pat.find(fid)), e(pat.end());
			INVARIANT(f != e);
			const alias_reference_set<bool_tag>& rs(f->second);
			alias_reference_set<bool_tag>::const_iterator
				ai(rs.begin()), ae(rs.end());
			for ( ; ai!=ae; ++ai) {
			if ((*ai)->is_port_alias()) {
				actual_id = (*ai)->trace_alias(lp).instance_index;
				break;
			}
			}
			INVARIANT(ai != ae);
		} else {
			const instance_alias_info<bool_tag>&
				ab(fb.trace_alias(lp));
			actual_id = ab.instance_index;
		}
			INVARIANT(actual_id);
			STACKTRACE_INDENT_PRINT("LOCAL actual id = " << actual_id << endl);
			const index_type actual_node =
				register_named_node(actual_id, opt);
			STACKTRACE_INDENT_PRINT("actual node = " << actual_node << endl);
			np.node_actuals.push_back(actual_node);
#if NETLIST_CHECK_CONNECTIVITY
			// inherit used/drive properties from formals to actuals
			if (fn.used)
				node_pool[actual_node].used = true;
			if (fn.driven)
				node_pool[actual_node].driven = true;
#endif
		} else {
			cerr << "ERROR: unhandled instance port node type."
				<< endl;
			THROW_EXIT;
		}
		// else skip for now
	}	// end for each node port
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
 */
void
netlist::check_name_collisions(const string& n, const index_type ni, 
		const netlist_options& opt) {
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
#endif

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
string
netlist::get_original_node_name(const size_t _i, 
		const netlist_options& opt) const {
	STACKTRACE_VERBOSE;
	typedef port_alias_tracker_base<bool_tag>::map_type map_type;
	typedef alias_reference_set<bool_tag>::const_iterator
					const_iterator;
// stupid multi-pass implementation
if (!opt.preferred_names.empty()) {
	const map_type&
		sa(fp->get_scope_alias_tracker().get_id_map<bool_tag>());
	// then scan *all* aliases for the match (slow)
	const map_type::const_iterator
		asi(sa.find(_i)), ase(sa.end());
if (asi != ase) {
	const alias_reference_set<bool_tag>& s(asi->second);
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
		pa(fp->get_scope_alias_tracker().get_id_map<bool_tag>());
//		pa(fp->get_port_alias_tracker().get_id_map<bool_tag>());
	const map_type::const_iterator
		asi(pa.find(_i)), ase(pa.end());
if (asi != ase) {
	const alias_reference_set<bool_tag>& s(asi->second);
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
	const state_instance<bool_tag>::pool_type&
		bp(fp->get_instance_pool<bool_tag>());
	const size_t i = _i -1;		// pool is 0-based
	STACKTRACE_INDENT_PRINT("bp.size = " << bp.local_entries() << endl);
	INVARIANT(i < bp.local_entries());
	const never_ptr<const instance_alias_info<bool_tag> >
		a(bp[i].get_back_ref());
	// from port_alias_tracker, 
	// back_ref points to shortest canonical scope alias
	NEVER_NULL(a);
	ostringstream oss;
	a->dump_hierarchical_name(oss, opt.__dump_flags);
	return oss.str();
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
		new_named_node.name = get_original_node_name(_i, opt);
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
/**
	Note which nodes are actually used, to avoid emitting unused nodes.
 */
void
netlist::mark_used_nodes(void) {
	STACKTRACE_VERBOSE;
	netlist_common::mark_used_nodes(node_pool);
{
	instance_pool_type::iterator
		i(instance_pool.begin()), e(instance_pool.end());
	for ( ; i!=e; ++i) {
		i->mark_used_nodes(node_pool);
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
ostream&
netlist::emit_mangle_map(ostream& o, const netlist_options& nopt) const {
	o << nopt.comment_prefix << "BEGIN node name mangle map" << endl;
	typedef	node_pool_type::const_iterator	const_iterator;
	const_iterator i(node_pool.begin()), e(node_pool.end());
	for ( ; i!=e; ++i) {
	if (i->is_logical_node()) {	// includes implicit supply nodes
		const size_t nid = i->index;
		const string oname = get_original_node_name(nid, nopt);
		o << nopt.comment_prefix << "\t" << i->name << " : " << oname << endl;
	}
	}
	return o << nopt.comment_prefix << "END node name mangle map" << endl;
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
{
switch (nopt.subckt_def_style) {
case netlist_options::STYLE_SPECTRE: 
	o << "subckt ";
	break;
default:
// case netlist_options::STYLE_SPICE: 
	o << ".subckt ";
}	// end switch
	o << name;
}
	typedef	node_port_list_type::const_iterator		const_iterator;
	const_iterator i(node_port_list.begin()), e(node_port_list.end());
	ostringstream oss;		// stage for name mangling
	for ( ; i!=e; ++i) {
		node_pool[*i].emit(oss << ' ', nopt);
	}
	string formals(oss.str());
	// already mangled during name caching
	o << formals << endl;
	// TODO: emit port-info comments
}	// end if sub
if (sub || nopt.emit_top) {
	// option to suppress top-level instances and rules
if (nopt.emit_mangle_map)
	emit_mangle_map(o, nopt);
if (nopt.emit_node_aliases) {
	const util::indent _temp_(o, nopt.comment_prefix + "\t");
	o << nopt.comment_prefix << "BEGIN node aliases" << endl;
	fp->get_scope_alias_tracker().dump_local_bool_aliases(o);
	o << nopt.comment_prefix << "END node aliases" << endl;
}
{
	// emit subinstances
#if ENABLE_STACKTRACE
	o << nopt.comment_prefix << "instances:" << endl;
#endif
	typedef	instance_pool_type::const_iterator	const_iterator;
	size_t j = 0;	// DEBUG
	const_iterator i(instance_pool.begin()), e(instance_pool.end());
	for ( ; i!=e; ++i, ++j) {
		STACKTRACE_INDENT_PRINT("j = " << j << endl);
	if (nopt.empty_subcircuits || !i->is_empty()) {
		i->emit(o, node_pool, *fp, nopt) << endl;
	}
	}
}{
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
}
	emit_devices(o, node_pool, *fp, nopt);
}
if (sub) {
switch (nopt.subckt_def_style) {
case netlist_options::STYLE_SPECTRE: 
	o << "ends " << name << endl;
	break;
default:
// case netlist_options::STYLE_SPICE: 
	o << ".ends" << endl;
}	// end switch
}	// end if sub
	return o;
}

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
	o << "node pool:" << endl;
{
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
	o << "ports (node indices): ";
	copy(node_port_list.begin(), node_port_list.end(), osi);
	o << endl;
}{
	o << "named node map (footprint-index -> netlist-node-index):" << endl;
	size_t j = 0;
	for ( ; j<named_node_map.size(); ++j) {
		o << "  :" << j+1 << " -> [" << named_node_map[j] << ']' << endl;
	}
}{
	o << "internal node map (footprint-index -> netlist-node-index):" << endl;
	typedef	internal_node_map_type::const_iterator	const_iterator;
	const_iterator i(internal_node_map.begin()), e(internal_node_map.end());
	size_t j = 0;
	for ( ; i!=e; ++i) {
		const prs_footprint::node_expr_type&
			n(fp->get_prs_footprint().get_internal_node(j));
		o << "  @" << j << " -> [" << i->first << ']' <<
			(n.second ? '+' : '-');
		if (i->second) {
			o << " in subckt{" << i->second -1 << "}";
		}
		o << endl;
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
		i->dump_raw(o, *this);
	}
	o << '}' << endl;
}{
	o << "transistors:" << endl;
	dump_raw_devices(o);
}
	return o << '}' << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct bool_port_alias_collector :
		public bool_port_collector<unique_list<index_type> > {
	typedef	bool_port_collector<unique_list<index_type> >
						parent_type;
	VISIT_INSTANCE_ALIAS_INFO_PROTO(bool_tag);
	using parent_type::visit;
};

void
bool_port_alias_collector::visit(const instance_alias_info<bool_tag>& a) {
	if (a.is_aliased_to_port()) {
		parent_type::visit(a);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create a port summary so that other processes may correctly
	instantiate this.  
	This also summarizes the 'empty' flag for this netlist.  
	TODO: power supply ports
	\param opt for netlist generation configuration
 */
void
netlist::summarize_ports(const netlist_options& opt) {
	STACKTRACE_VERBOSE;
	// could mark_used_nodes here instead?
#if !PRS_SUPPLY_OVERRIDES
	if (node_pool[GND_index].used) {
		node_port_list.push_back(GND_index);
	}
	if (node_pool[Vdd_index].used) {
		node_port_list.push_back(Vdd_index);
	}
#endif
{
	typedef	unique_list<index_type>	port_index_list_type;
	bool_port_alias_collector V;
	// this ordering is based on port_formal_manager, declaration order
	const port_formals_manager&
		fm(fp->get_owner_def().is_a<const process_definition>()
			->get_port_formals());
	port_formals_manager::const_list_iterator pi(fm.begin()), pe(fm.end());
	for ( ; pi!=pe; ++pi) {
		(*fp)[(*pi)->get_name()]
			.is_a<const physical_instance_collection>()->accept(V);
	}
	port_index_list_type::const_iterator
		i(V.bool_indices.begin()), e(V.bool_indices.end());
	node_port_list.reserve(V.bool_indices.size() +2);	// for supplies
	for ( ; i!=e; ++i) {
		// 1-indexed local id to 0-indexed named_node_map
		INVARIANT(*i);
		const index_type local_ind = *i -1;
		index_type ni = named_node_map[local_ind];
	if (!ni && opt.unused_ports) {
		// the consider all ports used, even if unconnected
		ni = register_named_node(*i, opt);
		node_pool[ni].used = true;
	}
		const node& n(node_pool[ni]);
#if NETLIST_CHECK_CONNECTIVITY
	if (ni && (n.used || n.driven))
#else
	if (ni && n.used)
#endif
	{
		INVARIANT(n.is_logical_node());
		INVARIANT(n.index == *i);	// self-reference
		node_port_list.push_back(ni);
		// sorted_ports[local_ind] = ni;
	}
	}	// end for
}
	// empty is initially false
	bool MT = true;
	MT = transistor_pool.empty() && passive_device_pool.empty();
{
	// check subcircuits
	local_subcircuit_list_type::const_iterator
		i(local_subcircuits.begin()), e(local_subcircuits.end());
	for ( ; MT && i!=e; ++i) {
	if (!i->is_empty()) {
		MT = false;	// stop on non-empty subcircuit
	}
	}
}{
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
}

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
	index_type j = 1;
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

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

