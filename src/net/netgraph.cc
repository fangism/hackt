/**
	\file "net/netgraph.cc"
	$Id: netgraph.cc,v 1.1.2.14 2009/08/19 00:11:32 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <sstream>		// for ostringstream
#include <iterator>		// for ostream_iterator
#include "net/netgraph.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#include "Object/global_entry_context.h"
#include "Object/common/dump_flags.h"
#include "Object/def/footprint.h"
#include "Object/def/process_definition.h"
#include "Object/expr/preal_const.h"
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
#include "util/stacktrace.h"

namespace HAC {
namespace NET {
#include "util/using_ostream.h"
using entity::port_alias_tracker;
using entity::footprint_frame_map_type;
using entity::dump_flags;
using entity::bool_port_collector;
using entity::instance_alias_info;
using entity::bool_tag;
using entity::state_instance;
using entity::port_formals_manager;
using entity::process_definition;
using entity::physical_instance_collection;
using std::pair;
using std::ostringstream;
using std::ostream_iterator;
using util::value_saver;
using entity::PRS::PRS_LITERAL_TYPE_ENUM;
using entity::PRS::PRS_NOT_EXPR_TYPE_ENUM;
using entity::PRS::PRS_AND_EXPR_TYPE_ENUM;
using entity::PRS::PRS_OR_EXPR_TYPE_ENUM;
using entity::PRS::PRS_NODE_TYPE_ENUM;
using entity::directive_base_params_type;
using entity::preal_value_type;
using util::unique_list;

//=============================================================================
// class netlist_options method definitions

netlist_options::netlist_options() :
		std_n_width(5.0),
		std_p_width(5.0),
		std_n_length(2.0),
		std_p_length(2.0),
		stat_n_width(3.0),
		stat_p_width(3.0),
		stat_n_length(20.0),
		stat_p_length(10.0),
		length_unit("u"),
		lambda(1.0), 
		nested_subcircuits(false),
		emit_top(true)
		{
}

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

//=============================================================================
// class local_netlist method definitions

//=============================================================================
// class node method definitions

/**
	How to format print each node's identity.  
	TODO: make special designators configurable.
	TODO: configurable optional name-mangling?
 */
ostream&
node::emit(ostream& o, const footprint& fp) const {
switch (type) {
case NODE_TYPE_LOGICAL:
	// NEVER_NULL(fp.get_instance_pool<bool_tag>()[index].get_back_ref());
	// does this guarantee canonical name?  seems to
	fp.get_instance_pool<bool_tag>()[index].get_back_ref()
		->dump_hierarchical_name(o, dump_flags::no_definition_owner);
	// TODO: take options dump_flags for changing hierarchical separator
	// TODO: possibly perform mangling
	break;
case NODE_TYPE_INTERNAL:
	o << '@' << name;
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
	o << ":" << index;
	break;
case NODE_TYPE_INTERNAL:
	o << '@' << name;
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
instance::emit(ostream& o, const NP& node_pool, const footprint& fp) const {
	o << 'x';
	fp.get_instance_pool<process_tag>()[pid].get_back_ref()
		->dump_hierarchical_name(o, dump_flags::no_definition_owner);
	actuals_list_type::const_iterator
		i(actuals.begin()), e(actuals.end());
	for ( ; i!=e; ++i) {
		node_pool[*i].emit(o << ' ', fp);
	}
	return o << ' ' << type->get_name();	// endl
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
	node_pool[source].emit(o, fp) << ' ';
	node_pool[gate].emit(o, fp) << ' ';
	node_pool[drain].emit(o, fp) << ' ';
	node_pool[body].emit(o, fp) << ' ';
	switch (type) {
	case NFET_TYPE: o << "nch"; break;
	case PFET_TYPE: o << "pch"; break;
	// TODO: honor different vt types and flavors
	default:
		o << "<type?>";
	}
	// TODO: restrict lengths and widths, from tech/conf file
	o << " W=" << width *nopt.lambda << nopt.length_unit <<
		" L=" << length *nopt.lambda << nopt.length_unit;
	// TODO: scale factor?
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
transistor::emit_attribute_suffixes(ostream& o) const {
	if (attributes & IS_PRECHARGE)
		o << ":pchg";
	if (attributes & IS_STANDARD_KEEPER)
		o << ":keeper";
	if (attributes & IS_COMB_FEEDBACK)
		o << ":ckeeper";
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
	emit_attribute_suffixes(o << " ");
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
static const node void_node("__VOID__", node::auxiliary_node_tag);	// NULL
static const node GND_node("GND", node::supply_node_tag);
static const node Vdd_node("Vdd", node::supply_node_tag);

// universal node indices to every subcircuit
// these should correspond with the order of insertion in netlist's ctor
static const	index_type	void_index = 0;
static const	index_type	GND_index = 1;
static const	index_type	Vdd_index = 2;

// static const string	dummy_type_name("<name>");	// temporary

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
netlist::netlist() : netlist_common(), name(), 
		named_node_map(), node_pool(),
		instance_pool(), internal_node_map(), port_list(), 
		aux_count(0) {
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
	Like constructor, initializes footprint and related members.
 */
void
netlist::bind_footprint(const footprint& f, const netlist_options& nopt) {
	fp = &f;
	// TODO: format or mangle type name, e.g. eliminate space
	ostringstream oss;
	f.dump_type(oss);
	name = oss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overload with overridden name, e.g. for top-level.
 */
void
netlist::bind_footprint(const footprint& f, const netlist_options& nopt, 
		const string& n) {
	fp = &f;
	name = n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only emit definitions and instances of non-empty subcircuits.  
 */
bool
netlist::is_empty(void) const {
	return netlist_common::is_empty() &&
		instance_pool.empty() && local_subcircuits.empty();
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
		const netlist& subnet, const index_type lpid) {
	STACKTRACE_VERBOSE;
	const footprint* subfp = subp._frame._footprint;
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
		// TODO: handle supply nodes
		if (fn.is_logical_node()) {
			const index_type fid = fn.index;
			STACKTRACE_INDENT_PRINT("formal id = " << fid << endl);

			const instance_alias_info<bool_tag>&
				fb(*subnet.fp->get_instance_pool<bool_tag>()[fid].get_back_ref());
			INVARIANT(fb.is_aliased_to_port());
			// ALERT: might pick a non-port alias!!!
			// picking *any* port-alias should suffice because
			// formal port-aliases have been replayed.
			const instance_alias_info<bool_tag>&
				ab(fb.trace_alias(lp));
			const index_type actual_id = ab.instance_index;
			STACKTRACE_INDENT_PRINT("LOCAL actual id = " << actual_id << endl);
			const index_type actual_node =
				register_named_node(actual_id);
			STACKTRACE_INDENT_PRINT("actual node = " << actual_node << endl);
			np.actuals.push_back(actual_node);
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
	\param ei is the prs_footprint subexpression index for named node.
	\param int_name is the name of the defined internal node.
	\return index of new node, 1-indexed into this netlist.
 */
index_type
netlist::create_internal_node(const index_type ei, const string& int_name,
		const bool dir) {
	STACKTRACE_VERBOSE;
	const node n(int_name, node::internal_node_tag);
	const index_type ret = node_pool.size();
	// TODO: [opt] rewrite using single insertion with pair<iter, bool>
	internal_node_map_type::const_iterator f(internal_node_map.find(ei));
	INVARIANT(f == internal_node_map.end());	// no duplicates
	node_pool.push_back(n);
	internal_node_map[ei] = std::make_pair(ret, dir);	// insert
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lookup a previously defined internal node, keyed by
	PRS footprint's expr index.
	\param ei local footprint's expr index
	\return netlist's node pool index to internal node, or 0 if not found.
 */
netlist::internal_node_entry_type
netlist::lookup_internal_node(const index_type ei) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("expr-id: " << ei << endl);
	const internal_node_map_type::const_iterator
		f(internal_node_map.find(ei));
	if (f != internal_node_map.end()) {
	// ALERT: only true if we've processed all internal nodes in a priori
		return f->second;
	} else	return internal_node_entry_type(0, false);	// any dir
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
netlist::register_named_node(const index_type _i) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("local id (+1) = " << _i << endl);
	INVARIANT(_i);
	const index_type i = _i -1;	// locally 0-indexed, no gap
	INVARIANT(i < named_node_map.size());
	index_type& ret(named_node_map[i]);
	if (!ret) {
		// reserve a new slot and update it for subsequent visits
		const node new_named_node(_i, node::logical_node_tag);
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
	netlist_common::mark_used_nodes(node_pool);
	instance_pool_type::iterator
		i(instance_pool.begin()), e(instance_pool.end());
	for ( ; i!=e; ++i) {
		i->mark_used_nodes(node_pool);
	}
	// traverse subcircuits?
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
if (sub) {
	o << ".subckt " << name;
	typedef	port_list_type::const_iterator		const_iterator;
	const_iterator i(port_list.begin()), e(port_list.end());
	for ( ; i!=e; ++i) {
		node_pool[*i].emit(o << ' ', *fp);	// options?
	}
	o << endl;
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
	if (!i->is_empty()) {	// TODO: netlist_option for empty_instances?
		i->emit(o, node_pool, *fp) << endl;	// options?
	}
	}
}
	// emit subcircuit instances
{
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
		i->emit_attribute_suffixes(o) << ' ';
		i->emit(o, node_pool, *fp, nopt) << endl;
	}
}
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
		n.emit(o, *fp);	// may have to comment out for debug
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
	o << "internal node map (fp-expr-index -> netlist-node-index):" << endl;
	typedef	internal_node_map_type::const_iterator	const_iterator;
	const_iterator i(internal_node_map.begin()), e(internal_node_map.end());
	for ( ; i!=e; ++i) {
		o << "  %" << i->first << " -> [" << i->second.first << ']' <<
			(i->second.second ? '+' : '-') << endl;
	}
}{
	o << "instances:" << endl;
	typedef	instance_pool_type::const_iterator	const_iterator;
	const_iterator i(instance_pool.begin()), e(instance_pool.end());
	for ( ; i!=e; ++i) {
		i->dump_raw(o << "  ") << endl;
	}
}
// TODO: subcircuits
{
	o << "transistors:" << endl;
	typedef	transistor_pool_type::const_iterator	const_iterator;
	const_iterator i(transistor_pool.begin()), e(transistor_pool.end());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		i->dump_raw(o << "  " << j << ": ") << endl;
	}
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
	What to do about supplies?
	FIXME: inadvertently adds non-ports to the port_list
	TODO: power supply ports
 */
void
netlist::summarize_ports(void) {
	STACKTRACE_VERBOSE;
	// could mark_used_nodes here instead?
#if 1
	typedef	unique_list<index_type>	port_index_list_type;
	bool_port_alias_collector V;
#if 0
	// this node ordering is based on order of being seen/used
	fp->accept(V);
#else
	// this ordering is based on port_formal_manager, declaration order
	const port_formals_manager&
		fm(fp->get_owner_def().is_a<const process_definition>()
			->get_port_formals());
	port_formals_manager::const_list_iterator pi(fm.begin()), pe(fm.end());
	for ( ; pi!=pe; ++pi) {
		(*fp)[(*pi)->get_name()]
			.is_a<const physical_instance_collection>()->accept(V);
	}
#endif
	// handle Vdd and GND separately
	port_index_list_type::const_iterator
		i(V.bool_indices.begin()), e(V.bool_indices.end());
	port_list.reserve(V.bool_indices.size());
	for ( ; i!=e; ++i) {
		// 1-indexed local id to 0-indexed named_node_map
		INVARIANT(*i);
		const index_type local_ind = *i -1;
		const index_type ni = named_node_map[local_ind];
		const node& n(node_pool[ni]);
	if (ni && n.used) {
		INVARIANT(n.is_logical_node());
		INVARIANT(n.index == *i);	// self-reference
		port_list.push_back(ni);
		// sorted_ports[local_ind] = ni;
	}
	}
#else
	// this ordering is based on locally assigned indices, 
	// not necessarily the order of declaration
	const state_instance<bool_tag>::pool_type&
		bp(fp->get_instance_pool<bool_tag>());
	named_node_map_type::const_iterator
		i(named_node_map.begin()), e(named_node_map.end());
	size_t j = 1;	// 0-indexed -> 1-indexed
	for ( ; i!=e; ++i, ++j) {
	// NOTE: this visits only logical nodes
	if (*i && bp[j].get_back_ref()->is_aliased_to_port()) {
		port_list.push_back(*i);
	}
	}
#endif
}

//=============================================================================
// class netlist_generator method definitions

netlist_generator::netlist_generator(const state_manager& _sm,
		const footprint& _topfp, ostream& o, 
		const netlist_options& p) :
		cflat_context_visitor(_sm, _topfp), os(o), opt(p), netmap(),
		prs(NULL), 
		current_netlist(NULL), 
		current_local_netlist(NULL),
		foot_node(void_index), output_node(void_index),
		fet_type(transistor::NFET_TYPE), 	// don't care
		fet_attr(transistor::DEFAULT_ATTRIBUTE),
		negated(false),
		// TODO: set these to technology defaults
		last_width(0.0), 
		last_length(0.0) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
netlist_generator::~netlist_generator() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Main function for starting hierarchy traversal for netlist generation.
	\pre top-level footprint contains top-level instances in
		reserved process slot [0] in the state_manager.
	Algorithm: starting with top-level footprint
		for all dependent subcircuit types
			recurse (pre-order)
		process local rules
		create port map and summary
 */
void
netlist_generator::operator () (void) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(sm);
	const global_entry<process_tag>& ptop(sm->get_pool<process_tag>()[0]);
	visit(ptop);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't know if anything needs to be done here.  
	Perhaps apply attributes to corresponding mapped local netlist nodes.  
 */
void
netlist_generator::visit(const global_entry<bool_tag>& p) {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-maps insert() member function into push_back().
 */
template <class Sequence>
struct set_adaptor : public Sequence {
	typedef	Sequence				sequence_type;
	typedef	typename sequence_type::const_reference	const_reference;

	// everything else inherited

	void
	insert(const_reference v) {
		sequence_type::push_back(v);
	}
};	// end struct set_adaptor

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visit all dependent subcircuits (processes) *first*.
	Each unique type is only visited once.  
	Remember that top-most level should not be wrapped in subcircuit.  
	Q: Is there a way to traverse the footprint without using the
	footprint_frame?  This should be a non-global hierarchical traversal.
 */
void
netlist_generator::visit(const global_entry<process_tag>& p) {
	STACKTRACE_VERBOSE;
	// don't need to temporarily set the footprint_frame
	// because algorithm is completely hierarchical, no flattening
	// will need p._frame when emitting subinstances
	const footprint* f(p._frame._footprint);
	NEVER_NULL(f);
	INVARIANT(f->is_created());	// don't need is_allocated()!!!
	netlist_map_type::iterator mi(netmap.find(f));
	const bool first_time = (mi == netmap.end());
	const bool top_level = !current_netlist;
if (first_time) {
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("processing unique type: ");
	f->dump_type(STACKTRACE_STREAM) << endl;
#endif
	netlist* nl = &netmap[f];	// insert default constructed
if (f == topfp) {
	nl->bind_footprint(*f, opt, "<top-level>");
} else {
	nl->bind_footprint(*f, opt);
}
	// initialize netlist:
	const footprint_frame_map_type&
		bfm(p._frame.get_frame_map<bool_tag>());
		// ALERT: top-footprint frame's size will be +1!
	nl->named_node_map.resize(bfm.size(), void_index);	// 0-fill
	STACKTRACE_INDENT_PRINT("bfm.size = " << bfm.size() << endl);
	// set current netlist (duplicate for local):
	const value_saver<netlist*> __tmp(current_netlist, nl);
	const value_saver<netlist_common*> __tmp2(current_local_netlist, nl);
	// should not invalidate existing iterators
	const footprint_frame_map_type&
		pfm(p._frame.get_frame_map<process_tag>());
	// TODO: instead of relying on globally allocated footprint frame
	// just traverse the footprint's instance_pool<process_tag>
	// this way, don't require global allocation first, only create phase!
	STACKTRACE_INDENT_PRINT("pfm.size = " << pfm.size() << endl);
	nl->instance_pool.reserve(pfm.size());	// prevent reallocation!!!
	typedef	footprint_frame_map_type::const_iterator	const_iterator;
	const_iterator i(pfm.begin()), e(pfm.end());
	// skip first NULL slot?
	// ALERT: top-level's process frame starts at 1, not 0!
	index_type lpid = top_level ? 0 : 1;
try {
	for (; i!=e; ++i, ++lpid) {
	if (*i) {
		STACKTRACE_INDENT_PRINT("examining sub-process id " << *i <<
			", local id " << lpid << endl);
		// const index_type lpid = *i +1;
		const global_entry<process_tag>&
			subp(sm->get_pool<process_tag>()[*i]);
		// no need to set footprint frames (global use only)
		visit(subp);	// recursion
		// guarantee that dependent type is processed with netlist
		// find out how local nodes are passed to *local* instance
		const footprint* subfp = subp._frame._footprint;
		const netlist& subnet(netmap.find(subfp)->second);
		nl->append_instance(subp, subnet, lpid);
	}
	}
	// process local production rules and macros
	f->get_prs_footprint().accept(*this);
	// f->get_spec_footprint().accept(*this);	// ?
	if (!top_level) {
		nl->summarize_ports();
	}
} catch (...) {
	cerr << "ERROR producing netlist for " << nl->name << endl;
	throw;
}
	// finally, emit this process
#if ENABLE_STACKTRACE
	nl->dump_raw(cerr);	// DEBUG point
#endif
if (!nl->is_empty()) {		// TODO: netlist_option show_empty_subcircuits
	nl->emit(os, !top_level, opt) << endl;
} else {
	os << "* subcircuit " << nl->name << " is empty." << endl;
}
}
	// if this is not top-level, wrap emit in .subckt/.ends
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist_generator::visit(const state_manager& s) {
	STACKTRACE_VERBOSE;
	// never called, do nothing
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Traverse footprint in *subcircuit* order first.  
	Would this traversal be useful to other classes?
 */
void
netlist_generator::visit(const entity::PRS::footprint& r) {
	STACKTRACE_VERBOSE;
	// no local name translation needed for subcircuits
	typedef	prs_footprint::subcircuit_map_type	subckt_map_type;
	typedef	subckt_map_type::const_iterator		const_iterator;
	// must be sorted ranges
	const value_saver<const prs_footprint*> __prs(prs, &r);
	// for now, default supplies
	const value_saver<index_type> __s1(low_supply, GND_index);
	const value_saver<index_type> __s2(high_supply, Vdd_index);
{
	STACKTRACE_INDENT_PRINT("reserving internal nodes..." << endl);
	// Internal node definitions may have a dependency ordering
	// that is not reflected by the container, thus, we cannot simply
	// iterate over the footprint's internal-node-map (string-keyed).
	// Instead, only visit internal nodes on-demand
	//	to solve problem of unused dangling internal nodes.
	// PRS::footprint maps from string (name of internal node) to 
	// a local internal node index, which is mapped to a subcircuit
	// node index here (reverse map).
	const prs_footprint::internal_node_expr_map_type&
		inode_map(prs->get_internal_node_map());
	prs_footprint::internal_node_expr_map_type::const_iterator
		i(inode_map.begin()), e(inode_map.end());
	for ( ; i!=e; ++i) {
		// each entry is a node_expr_type
		// where pair:first is expr-index and second is direction
		const index_type& expr = i->second.first;
		const bool dir = i->second.second;
		const index_type new_int =
			current_netlist->create_internal_node(
				expr, i->first, dir);
		INVARIANT(new_int);
		INVARIANT(current_netlist->lookup_internal_node(expr).first);
	}
#if 0 && ENABLE_STACKTRACE
	current_netlist->dump_raw(STACKTRACE_STREAM) << endl;
#endif
}
	// traverse subcircuits
	// ALERT: if we ever want to perform other operations before emitting
	// these local subcircuit structures need to be kept around longer
	// because the instances of them would be dangling pointers...
	const subckt_map_type& subc_map(prs->get_subcircuit_map());
{
	STACKTRACE_INDENT_PRINT("processing subcircuits..." << endl);
	const_iterator si(subc_map.begin()), se(subc_map.end());
	// rules
	const prs_footprint::rule_pool_type& rpool(prs->get_rule_pool());
	const size_t s = rpool.size();
	size_t i = 0;
	for ( ; i<s; ++i) {
	if (si!=se && (i >= si->rules.first)) {
		// start of a subcircuit range, can be empty
		do {
			current_netlist->local_subcircuits.push_back(local_netlist());
			local_netlist& n(current_netlist->local_subcircuits.back());
			n.name = si->get_name();
			const value_saver<netlist_common*>
				__tmp(current_local_netlist, &n);
			rpool[i].accept(*this);
			++i;
		} while (i < si->rules.second);
			--i;	// back-adjust before continue
	} else {
		// rule is outside of subcircuits
		rpool[i].accept(*this);
	}
	}	// end for
}{
	STACKTRACE_INDENT_PRINT("processing macros..." << endl);
	const_iterator si(subc_map.begin()), se(subc_map.end());
	netlist::local_subcircuit_list_type::iterator
		mi(current_netlist->local_subcircuits.begin());
	// macros
	const prs_footprint::macro_pool_type& mpool(prs->get_macro_pool());
	const size_t s = mpool.size();
	size_t i = 0;
	for ( ; i<s; ++i) {
	if (si!=se && (i >= si->macros.first)) {
		// start of a subcircuit range, can be empty
		do {
			INVARIANT(mi != current_netlist->local_subcircuits.end());
			local_netlist& n(*mi);
			const value_saver<netlist_common*>
				__tmp(current_local_netlist, &n);
			mpool[i].accept(*this);
			++mi;
			++i;
		} while (i < si->macros.second);
			--i;	// back-adjust before continue
	} else {
		// macro is outside of subcircuits
		mpool[i].accept(*this);
	}
	}	// end for
}
	// process all subcircuits first, then remaining local rules/macros
	current_netlist->mark_used_nodes();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Construct subgraph from LHS expression, and connect final
	output(s) to output node.
	\pre current_netlist is set
 */
void
netlist_generator::visit(const entity::PRS::footprint_rule& r) {
	STACKTRACE_VERBOSE;
	// set foot_node and output_node and fet_type
	const value_saver<index_type>
		__t1(foot_node, (r.dir ? Vdd_index : GND_index)),
		__t2(output_node, current_netlist->register_named_node(r.output_index));
	const value_saver<transistor::fet_type>
		__t3(fet_type, (r.dir ? transistor::PFET_TYPE : transistor::NFET_TYPE));
	// TODO: honor prs supply override directives
	const prs_footprint::expr_pool_type& ep(prs->get_expr_pool());
try {
	ep[r.expr_index].accept(*this);
} catch (...) {
	// TODO: better diagnostic tracing message
	cerr << "ERROR in production rule." << endl;
	throw;
}
	// TODO: process rule attributes, labels, names...
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TODO: handle and precharges!
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive, generate on-demand internal nodes as they are visited.
	\pre all internal nodes have been allocated/mapped a priori, 
		just not necessarily defined (node::used).
	\param nex is the footprint-local expression index.
	\return netlist-local node index representing the internal node.
	\invariant no cyclic definitions of internal nodes possible.
 */
index_type
netlist_generator::register_internal_node(const index_type nex) {
	STACKTRACE_VERBOSE;
	const prs_footprint::expr_pool_type& ep(prs->get_expr_pool());
	const index_type defexpr = ep[nex].only();
	const netlist::internal_node_entry_type
		ret(current_netlist->lookup_internal_node(defexpr));
	const index_type& node_ind(ret.first);
	INVARIANT(node_ind);
	node& n(current_netlist->node_pool[node_ind]);
if (!n.used) {
	STACKTRACE_INDENT_PRINT("defining internal node..." << endl);
	const bool dir = ret.second;
	// else need to define internal node once only
	// TODO: honor prs supply override directives
	const value_saver<index_type>
		__t1(foot_node, (dir ? Vdd_index : GND_index)),
		__t2(output_node, node_ind);
	const value_saver<transistor::fet_type>
		__t3(fet_type,
			(dir ? transistor::PFET_TYPE : transistor::NFET_TYPE));
	// internal nodes partial rules can belong to local subcircuits
	// but are accessible to all sibling subcircuits
	// within a process definition.
	n.used = true;		
	// mark before recursion, not after!
	// to prevent shared roots from being duplicated
	ep[defexpr].accept(*this);
}
	return node_ind;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function for visiting precharge expressions.
 */
void
netlist_generator::visit(const footprint_expr_node::precharge_pull_type& p) {
	// recall: precharge is 0-indexed
	const prs_footprint::expr_pool_type& ep(prs->get_expr_pool());
	const index_type& pchgex = p.first;
	const bool dir = p.second;
	const value_saver<index_type>
		_t3(foot_node, dir ? high_supply : low_supply);
	const value_saver<transistor::fet_type>
		_t4(fet_type, dir ? transistor::PFET_TYPE
			: transistor::NFET_TYPE);
	const value_saver<transistor::flags>
		_t5(fet_attr, transistor::flags(
			fet_attr | transistor::IS_PRECHARGE));
	// use the same output node
	ep[pchgex].accept(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Walk expressions from left to right, emitting transistors.
	\pre current_netlist is set
	\pre foot_node and output_node are already set to existing nodes, 
		e.g. Vdd, GND, or auxiliary node.
 */
void
netlist_generator::visit(const footprint_expr_node& e) {
	STACKTRACE_VERBOSE;
	const prs_footprint::expr_pool_type& ep(prs->get_expr_pool());
	// exception: if encountering an internal node on left-most 
	// expression, then set foot_node instead of emitting a device.
const char type = e.get_type();
switch (type) {
case PRS_LITERAL_TYPE_ENUM: {
	// TODO: check for negation normalization
	if (negated ^ (fet_type == transistor::PFET_TYPE)) {
		cerr << "ERROR: rule-literal is not CMOS-implementable." << endl;
		THROW_EXIT;
	}
	transistor t;
	t.type = fet_type;
	// TODO: handle FET type override
	t.gate = current_netlist->register_named_node(e.only());
	t.source = foot_node;
	t.drain = output_node;
	// TODO: honor supply overrides
	t.body = (fet_type == transistor::NFET_TYPE ? low_supply : high_supply);
		// Vdd or GND
	// TODO: extract length/width parameters
	const directive_base_params_type& p(e.get_params());
	if (p.size() > 0) {
		t.width = p[0]->to_real_const();
	} else {
		t.width = (fet_type == transistor::NFET_TYPE ?
			opt.std_n_width : opt.std_p_width);
	}
	if (p.size() > 1) {
		t.length = p[1]->to_real_const();
	} else {
		t.length = (fet_type == transistor::NFET_TYPE ?
			opt.std_n_length : opt.std_p_length);
	}
	t.attributes = fet_attr;
	// TODO: import attributes from rule attributes?
	NEVER_NULL(current_local_netlist);
	current_local_netlist->transistor_pool.push_back(t);
	break;
}
case PRS_NOT_EXPR_TYPE_ENUM: {
	// automatically negation normalize
	// toggle negation of subexpressions
	const value_saver<bool> __tmp(negated, !negated);
	ep[e.only()].accept(*this);
	break;
}
case PRS_AND_EXPR_TYPE_ENUM:	// fall-through
case PRS_OR_EXPR_TYPE_ENUM: {
	const bool is_conjunctive = (type == PRS_AND_EXPR_TYPE_ENUM) ^ negated;
	size_t i = 1;
	const size_t s = e.size();
	if (is_conjunctive) {
		typedef footprint_expr_node::precharge_map_type
				precharge_map_type;
		const footprint_expr_node::precharge_map_type&
			p(e.get_precharges());
		precharge_map_type::const_iterator
			pi(p.begin()), pe(p.end());
		// special case: first branch is an internal node
		index_type prev;
		const footprint_expr_node& left(ep[e[i]]);
		if (left.get_type() == PRS_NODE_TYPE_ENUM) {
			prev = register_internal_node(e[i]);
			// TODO: confirm direction and sense of internal node
			// hand precharge
			if (pi != pe && pi->first == i-1) {
				const value_saver<index_type>
					_t1(output_node, prev);
				// recall: precharge is 0-indexed
				visit(pi->second);
				++pi;
			}
			++i;
		} else {
			prev = foot_node;
		}
		// create intermediate nodes
		for ( ; i<s; ++i) {		// all but last node
			// setup foot and output, then recurse!
			const value_saver<index_type>
				_t1(foot_node, prev),
				_t2(output_node, 
					current_netlist->create_auxiliary_node());
			ep[e[i]].accept(*this);
			prev = output_node;
			// handle precharge
			if (pi != pe && pi->first == i-1) {
				// recall: precharge is 0-indexed
				visit(pi->second);
				++pi;
			}
		}
		{
		// last node connected to output
			const value_saver<index_type> _t1(foot_node, prev);
			// use current output_node, not new auxiliary node
			ep[e[i]].accept(*this);
		}
	} else {
		// take OR combination:
		// don't need to add any auxiliary nodes,
		// share the same foot and output nodes.
		for (; i<=s; ++i) {
			ep[e[i]].accept(*this);
		}
	}
	break;
}
case PRS_NODE_TYPE_ENUM: {
	// ERROR: unexpected internal node out of position
	cerr << "ERROR: found internal node in unexpected position." << endl;
	THROW_EXIT;
	break;
}
default:
	DIE;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expand a macro into netlist.
	Examples: passn and passp for pass-gates.
	\pre current_netlist is set
	TODO: eventually implement using a function table/map
 */
void
netlist_generator::visit(const entity::PRS::footprint_macro& e) {
	STACKTRACE_VERBOSE;
	const bool passn = (e.name == "passn");
	const bool passp = (e.name == "passp");
if (passn || passp) {
	transistor t;
	t.type = passp ? transistor::PFET_TYPE : transistor::NFET_TYPE;
	// TODO: override with vt types
	t.gate = current_netlist->register_named_node(*e.nodes[0].begin());
	t.source = current_netlist->register_named_node(*e.nodes[1].begin());
	t.drain = current_netlist->register_named_node(*e.nodes[2].begin());
	t.body = passp ? high_supply : low_supply;
	const directive_base_params_type& p(e.params);
	if (p.size() > 0) {
		t.width = p[0]->to_real_const();
	} else {
		t.width = (passn ? opt.std_n_width : opt.std_p_width);
	}
	if (p.size() > 1) {
		t.length = p[1]->to_real_const();
	} else {
		t.length = (passn ? opt.std_n_length : opt.std_p_length);
	}
	t.attributes = fet_attr;
	// TODO: import attributes
	NEVER_NULL(current_local_netlist);
	current_local_netlist->transistor_pool.push_back(t);
} else if (e.name == "echo") {
	// do nothing
} else {
	cerr << "WARNING: unknown PRS macro " << e.name
		<< " ignored." << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist_generator::visit(const entity::SPEC::footprint&) {
	// nothing... yet
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist_generator::visit(const entity::SPEC::footprint_directive&) {
	// nothing... yet
}

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

