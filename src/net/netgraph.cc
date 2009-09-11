/**
	\file "net/netgraph.cc"
	$Id: netgraph.cc,v 1.2.2.7 2009/09/11 18:19:19 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <sstream>		// for ostringstream
#include <iterator>		// for ostream_iterator
#include "net/netgraph.h"
#include "net/netlist_options.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#include "Object/global_entry_context.h"
#include "Object/common/dump_flags.h"
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
#include "util/unique_list.tcc"
#include "util/string.h"		// for strgsub
#include "util/stacktrace.h"

namespace HAC {
namespace NET {
#include "util/using_ostream.h"
using entity::footprint_frame_map_type;
using entity::dump_flags;
using entity::bool_port_collector;
using entity::instance_alias_info;
using entity::bool_tag;
using entity::port_formals_manager;
using entity::process_definition;
using entity::physical_instance_collection;
using entity::port_alias_tracker;
using entity::port_alias_tracker_base;
using entity::alias_reference_set;
using std::ostringstream;
using std::ostream_iterator;
using entity::directive_base_params_type;
using entity::preal_value_type;
using util::unique_list;
using util::strings::strgsub;

//=============================================================================
// class netlist_common method definitions

template <class NP>
void
netlist_common::mark_used_nodes(NP& node_pool) const {
	transistor_pool_type::const_iterator
		i(transistor_pool.begin()), e(transistor_pool.end());
	for ( ; i!=e; ++i) {
		i->mark_used_nodes(node_pool);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
netlist_common::is_empty(void) const {
	return transistor_pool.empty() && passive_device_pool.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class NP>
ostream&
netlist_common::emit_devices(ostream& o, const NP& node_pool,
		const footprint& fp, const netlist_options& nopt) const {
	// emit devices
#if ENABLE_STACKTRACE
	o << "* devices:" << endl;
#endif
	typedef	transistor_pool_type::const_iterator	const_iterator;
	const_iterator i(transistor_pool.begin()), e(transistor_pool.end());
	// TODO: print originating rule in comments
	// TODO: use optional label designations
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		o << 'M' << j << '_';
		i->emit_attribute_suffixes(o, nopt) << ' ';
		i->emit(o, node_pool, fp, nopt) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
netlist_common::dump_raw_devices(ostream& o) const {
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
template <class NP>
void
local_netlist::mark_used_nodes(NP& node_pool) {
	transistor_pool_type::const_iterator
		i(transistor_pool.begin()), e(transistor_pool.end());
	for ( ; i!=e; ++i) {
		node_index_map.insert(i->gate);
		node_pool[i->gate].used = true;
		node_index_map.insert(i->source);
		node_pool[i->source].used = true;
		node_index_map.insert(i->drain);
		node_pool[i->drain].used = true;
		node_index_map.insert(i->body);
		node_pool[i->body].used = true;
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
	o << ".subckt ";
if (!nopt.nested_subcircuits) {
	// then use fully qualified type name
	o << n.name << nopt.emit_scope();;
}
	o << name;
{
	// ports, formals
	ostringstream oss;
	typedef	node_index_map_type::const_iterator	const_iterator;
	const_iterator i(node_index_map.begin()), e(node_index_map.end());
	for ( ; i!=e; ++i) {
		n.node_pool[*i].emit(oss << ' ', *n.fp, nopt);
	}
	string formals(oss.str());
#if CACHE_LOGICAL_NODE_NAMES
	// already mangled
#else
	nopt.mangle_instance(formals);
#endif
	o << formals << endl;
}
	// TODO: emit port-info comments
	emit_devices(o, n.node_pool, *n.fp, nopt);
	o << ".ends" << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiation uses the same names as the original ports.
 */
ostream&
local_netlist::emit_instance(ostream& o, const netlist& n,
		const netlist_options& nopt) const {
	o << 'x';
	o << name << nopt.emit_colon() << "inst";
{
	// actuals
	ostringstream oss;
	typedef	node_index_map_type::const_iterator	const_iterator;
	const_iterator i(node_index_map.begin()), e(node_index_map.end());
	for ( ; i!=e; ++i) {
		n.node_pool[*i].emit(oss << ' ', *n.fp, nopt);
	}
	oss << ' ';
	string line(oss.str());
#if CACHE_LOGICAL_NODE_NAMES
	// already mangled
#else
	nopt.mangle_instance(line);
#endif
	// type name is already mangled
	o << line;
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
node::emit(ostream& o, const footprint& fp, const netlist_options& n) const {
switch (type) {
case NODE_TYPE_LOGICAL:
#if CACHE_LOGICAL_NODE_NAMES
	o << name;
#else
	// NEVER_NULL(fp.get_instance_pool<bool_tag>()[index].get_back_ref());
	// does this guarantee canonical name?  seems to
	fp.get_instance_pool<bool_tag>()[index].get_back_ref()
		->dump_hierarchical_name(o, dump_flags::no_definition_owner);
#endif
	break;
case NODE_TYPE_INTERNAL:
	// Q: do internal node names ever need to be mangled?
	// A: don't think so because they are only simple identifiers
	o << n.emit_internal_at() <<
		fp.get_prs_footprint().get_internal_node(index).name;
	break;
case NODE_TYPE_AUXILIARY:
	o << n.emit_auxiliary_pound();
	if (name.length()) {
		o << name;
	} else {
		o << index;
	}
	break;
case NODE_TYPE_SUPPLY:
	o << name;	// prefix with any designator? '$' or '!' ?
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
case NODE_TYPE_SUPPLY:
	o << '!' << name;
	break;
default:
	o << "???";
}
	if (!used) o << " (unconnected)";
	return o;
}

//=============================================================================
// class instance method definitions

template <class NP>
void
instance::mark_used_nodes(NP& node_pool) const {
	actuals_list_type::const_iterator
		i(actuals.begin()), e(actuals.end());
	for ( ; i!=e; ++i) {
		node_pool[*i].used = true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
instance::is_empty(void) const {
	NEVER_NULL(type);
	return type->is_empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class NP>
ostream&
instance::emit(ostream& o, const NP& node_pool, const footprint& fp, 
		const netlist_options& nopt) const {
	o << 'x';
{
	// process instance name
	ostringstream oss;
	fp.get_instance_pool<process_tag>()[pid].get_back_ref()
		->dump_hierarchical_name(oss, dump_flags::no_definition_owner);
	string pname(oss.str());
	nopt.mangle_instance(pname);
	o << pname;
}{
	// actuals
	ostringstream oss;
	actuals_list_type::const_iterator
		i(actuals.begin()), e(actuals.end());
	for ( ; i!=e; ++i) {
		node_pool[*i].emit(oss << ' ', fp, nopt);
	}
	string line(oss.str());
#if CACHE_LOGICAL_NODE_NAMES
	// already mangled
#else
	nopt.mangle_instance(line);
#endif
	o << line;
}
	// type name is already mangled
	o << ' ' << type->get_name();	// endl
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance::dump_raw(ostream& o) const {
	o << '[' << pid << "]: " << type->get_name() << ": (";
	copy(actuals.begin(), actuals.end(),
		ostream_iterator<index_type>(o, ","));
	return o << ')';
}


//=============================================================================
// class transistor method definitions

/**
	In a separate pass mark all nodes participating on transistor.
	Why?  Is possible that a supply node is not used.  
 */
template <class NP>
void
transistor::mark_used_nodes(NP& node_pool) const {
	node_pool[source].used = true;
	node_pool[gate].used = true;
	node_pool[drain].used = true;
	node_pool[body].used = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class NP>
ostream&
transistor::emit(ostream& o, const NP& node_pool, const footprint& fp,
		const netlist_options& nopt) const {
	const node& s(node_pool[source]);
	const node& g(node_pool[gate]);
	const node& d(node_pool[drain]);
	const node& b(node_pool[body]);
{
	// perform name-mangling
	ostringstream oss;
	s.emit(oss, fp, nopt) << ' ';
	g.emit(oss, fp, nopt) << ' ';
	d.emit(oss, fp, nopt) << ' ';
	b.emit(oss, fp, nopt) << ' ';
	string nodes(oss.str());
#if CACHE_LOGICAL_NODE_NAMES
	// all nodes already mangled
#else
	nopt.mangle_instance(nodes);
#endif
	o << nodes;
}
	switch (type) {
	case NFET_TYPE: o << "nch"; break;
	case PFET_TYPE: o << "pch"; break;
	// TODO: honor different vt types and flavors
	default:
		o << "<type?>";
	}
	if (attributes & IS_LOW_VT)
		o << "_lvt";
	else if (attributes & IS_HIGH_VT)
		o << "_hvt";
	// else leave svt unmarked
	// TODO: restrict lengths and widths, from tech/conf file
	o << " W=" << width *nopt.lambda << nopt.length_unit <<
		" L=" << length *nopt.lambda << nopt.length_unit;
	if (nopt.emit_parasitics) {
		const real_type lsq = nopt.lambda * nopt.lambda;
		const real_type l2 = nopt.lambda * 2.0;
		const real_type& sl(s.is_logical_node() || s.is_supply_node() ?
			nopt.fet_diff_overhang : nopt.fet_spacing_diffonly);
		const real_type& dl(d.is_logical_node() || d.is_supply_node() ?
			nopt.fet_diff_overhang : nopt.fet_spacing_diffonly);
		nopt.line_continue(o);
		o <<	" AS=" << width * sl * lsq << nopt.area_unit <<
			" PS=" << (width + sl) *l2 << nopt.length_unit <<
			" AD=" << width * dl * lsq << nopt.area_unit <<
			" PD=" << (width + dl) *l2 << nopt.length_unit;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
transistor::emit_attribute_suffixes(ostream& o, 
		const netlist_options& nopt) const {
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
const node::__supply_node_tag	node::supply_node_tag = __supply_node_tag();
const node::__internal_node_tag	node::internal_node_tag = __internal_node_tag();
const node::__auxiliary_node_tag	node::auxiliary_node_tag = __auxiliary_node_tag();

// case sensitive?
const node
netlist::void_node("__VOID__", node::auxiliary_node_tag),
netlist::GND_node("GND", node::supply_node_tag),
netlist::Vdd_node("Vdd", node::supply_node_tag);

// universal node indices to every subcircuit
// these should correspond with the order of insertion in netlist's ctor
const	index_type
netlist::void_index = 0,
netlist::GND_index = 1,
netlist::Vdd_index = 2;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
netlist::netlist() : netlist_common(), name(), 
		named_node_map(), node_pool(),
		instance_pool(), internal_node_map(), port_list(), 
		empty(false), 
		aux_count(0),
		subs_count(0) {
	// copy supply nodes
	node_pool.reserve(8);	// reasonable pre-allocation
	// following order should match above universal node indices
	node_pool.push_back(void_node);
	node_pool.push_back(GND_node);
	node_pool.push_back(Vdd_node);
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
/**
	\param subp global process entry containing footprint frame
		Should we use local footprint's instance instead?
		Technically, we shouldn't need global allocation information.
	\param subnet the netlist type that corresponds with instance.
	\param lpid local process id from footprint.
	\pre instance_pool is already pre-allocated to avoid 
		invalidating references due to re-allocation.
 */
void
netlist::append_instance(const global_entry<process_tag>& subp,
		const netlist& subnet, const index_type lpid
#if CACHE_LOGICAL_NODE_NAMES
		, const netlist_options& opt
#endif
		) {
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
		lp(*fp->get_instance_pool<process_tag>()[lpid].get_back_ref());
	// ALERT: translates to global index, not what we want!
	netlist::port_list_type::const_iterator
		fi(subnet.port_list.begin()), fe(subnet.port_list.end());
	for ( ; fi!=fe; ++fi) {
		STACKTRACE_INDENT_PRINT("formal node = " << *fi << endl);
		const node& fn(subnet.node_pool[*fi]);	// formal node
		INVARIANT(fn.used);
		if (fn.is_supply_node()) {
			if (*fi == GND_index) {
				np.actuals.push_back(GND_index);
			} else if (*fi == Vdd_index) {
				np.actuals.push_back(Vdd_index);
			} else {
				cerr << "ERROR: unknown supply port." << endl;
				THROW_EXIT;
			}
		} else if (fn.is_logical_node()) {
			const index_type fid = fn.index;
			STACKTRACE_INDENT_PRINT("formal id = " << fid << endl);

			const instance_alias_info<bool_tag>&
				fb(*subnet.fp->get_instance_pool<bool_tag>()[fid].get_back_ref());
			INVARIANT(fb.is_aliased_to_port());
			index_type actual_id = 0;
			// ALERT: fb might pick a non-port alias!!!
			// picking *any* port-alias should suffice because
			// formal port-aliases have been replayed.
		if (!fb.is_port_alias()) {
			// HACK ALERT!
			// well, damn it, FIND me a suitable alias!
//			fb.dump_hierarchical_name(cerr << "ALIAS: ") << endl;
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
				register_named_node(actual_id
#if CACHE_LOGICAL_NODE_NAMES
				, opt
#endif
				);
			STACKTRACE_INDENT_PRINT("actual node = " << actual_node << endl);
			np.actuals.push_back(actual_node);
		} else {
			cerr << "ERROR: unhandled instance port node type."
				<< endl;
			THROW_EXIT;
		}
		// else skip for now
	}
#if ENABLE_STACKTRACE
	np.dump_raw(STACKTRACE_INDENT_PRINT("new instance: ")) << endl;
#endif
	INVARIANT(np.actuals.size() == subnet.port_list.size());
}

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
/**
	Allocates a new named internal node, incrementing counter.
	\param ni is the footprint-local index of the internal node (0-indexed).
	\return index of new node, 1-indexed into this netlist.
 */
index_type
netlist::create_internal_node(const index_type ni, const index_type ei) {
	STACKTRACE_VERBOSE;
	const node n(ni, node::internal_node_tag);
	const index_type ret = node_pool.size();
	node_pool.push_back(n);
	INVARIANT(ni < internal_node_map.size());
	internal_node_map[ni].first = ret;
	internal_expr_map[ei] = ni;	// invariant: no duplicates
	// set owner later!
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
netlist::register_named_node(const index_type _i
#if CACHE_LOGICAL_NODE_NAMES
		, const netlist_options& opt
#endif
		) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("local id (+1) = " << _i << endl);
	INVARIANT(_i);
	const index_type i = _i -1;	// locally 0-indexed, no gap
	INVARIANT(i < named_node_map.size());
	index_type& ret(named_node_map[i]);
	if (!ret) {
		// reserve a new slot and update it for subsequent visits
		node new_named_node(_i, node::logical_node_tag);
#if CACHE_LOGICAL_NODE_NAMES
		ostringstream oss;
		fp->get_instance_pool<bool_tag>()[_i].get_back_ref()
			->dump_hierarchical_name(oss,
				dump_flags::no_definition_owner);
		new_named_node.name = oss.str();
		opt.mangle_instance(new_named_node.name);
#endif
		ret = node_pool.size();
		INVARIANT(ret);
		node_pool.push_back(new_named_node);
#if ENABLE_STACKTRACE
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
	o << ".subckt " << name;
	typedef	port_list_type::const_iterator		const_iterator;
	const_iterator i(port_list.begin()), e(port_list.end());
	ostringstream oss;		// stage for name mangling
	for ( ; i!=e; ++i) {
		node_pool[*i].emit(oss << ' ', *fp, nopt);
	}
	string formals(oss.str());
#if CACHE_LOGICAL_NODE_NAMES
	// already mangled
#else
	nopt.mangle_instance(formals);
#endif
	o << formals << endl;
	// TODO: emit port-info comments
}
if (sub || nopt.emit_top) {
	// option to suppress top-level instances and rules
{
	// emit subinstances
#if ENABLE_STACKTRACE
	o << "* instances:" << endl;
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
	o << "* local subcircuits:" << endl;
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
	o << ".ends" << endl;
}
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
		n.emit(o, *fp, netlist_options::default_value);
		// may have to comment out for debug
#else
		o << "...";
#endif
		o << endl;
	}
}{
	o << "ports (node indices): ";
	copy(port_list.begin(), port_list.end(), osi);
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
	if (node_pool[GND_index].used) {
		port_list.push_back(GND_index);
	}
	if (node_pool[Vdd_index].used) {
		port_list.push_back(Vdd_index);
	}
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
	port_list.reserve(V.bool_indices.size() +2);	// for supplies
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
	if (ni && n.used) {
		INVARIANT(n.is_logical_node());
		INVARIANT(n.index == *i);	// self-reference
		port_list.push_back(ni);
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

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

