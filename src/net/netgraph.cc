/**
	\file "net/netgraph.cc"
	$Id: netgraph.cc,v 1.1.2.5 2009/08/12 00:29:34 fang Exp $
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
#include "Object/traits/instance_traits.h"
#include "Object/lang/PRS_footprint.h"
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
using entity::footprint_frame_transformer;
using entity::dump_flags;
using entity::bool_port_collector;
using std::pair;
using std::ostringstream;
using std::ostream_iterator;
using util::value_saver;
using HAC::entity::PRS::PRS_LITERAL_TYPE_ENUM;
using HAC::entity::PRS::PRS_NOT_EXPR_TYPE_ENUM;
using HAC::entity::PRS::PRS_AND_EXPR_TYPE_ENUM;
using HAC::entity::PRS::PRS_OR_EXPR_TYPE_ENUM;
using HAC::entity::PRS::PRS_NODE_TYPE_ENUM;
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

//=============================================================================
// class local_netlist method definitions

//=============================================================================
// class node method definitions

/**
	How to format print each node's identity.  
	TODO: make special designators configurable.
 */
ostream&
node::emit(ostream& o, const footprint& fp) const {
switch (type) {
case NODE_TYPE_LOGICAL:
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
	o << '#' << index;
	break;
case NODE_TYPE_SUPPLY:
	o << name;	// prefix with any designator? '$' or '!' ?
	break;
default:
	DIE;
}
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
	// TODO: restrict lengths and widths
	o << " W=" << width *nopt.lambda << nopt.length_unit <<
		" L=" << length *nopt.lambda << nopt.length_unit;
	// TODO: scale factor?
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
	// TODO: format or mangle type name
	ostringstream oss;
	f.dump_type(oss);
	name = oss.str();
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
	const footprint_frame_map_type&
		abfm(subp._frame.get_frame_map<bool_tag>());
	const footprint_frame_transformer fft(abfm);
	netlist::port_list_type::const_iterator
		fi(subnet.port_list.begin()), fe(subnet.port_list.end());
	for ( ; fi!=fe; ++fi) {
		const node& fn(subnet.node_pool[*fi]);
		INVARIANT(fn.used);
		// TODO: handle supply nodes
		if (fn.is_logical_node()) {
			const index_type actual_id = fft(fn.index);
			const index_type actual_node =
				register_named_node(actual_id);
			np.actuals.push_back(actual_node);
		}
		// else skip for now
	}
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
	\return index of new node, 1-indexed.
 */
index_type
netlist::create_internal_node(const index_type ei, const string& int_name) {
	STACKTRACE_VERBOSE;
	const node n(int_name, node::internal_node_tag);
	const index_type ret = node_pool.size();
	// TODO: [opt] rewrite using single insertion with pair<iter, bool>
	internal_node_map_type::const_iterator f(internal_node_map.find(ei));
	INVARIANT(f == internal_node_map.end());	// no duplicates
	node_pool.push_back(n);
	internal_node_map[ei] = ret;		// insert
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lookup a previously defined internal node, keyed by
	PRS footprint's expr index.
	\param ei local footprint's expr index
	\return netlist's node pool index to internal node
 */
index_type
netlist::lookup_internal_node(const index_type ei) const {
	const internal_node_map_type::const_iterator
		f(internal_node_map.find(ei));
	INVARIANT(f != internal_node_map.end());	// no duplicates
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
netlist::register_named_node(const index_type _i) {
	STACKTRACE_VERBOSE;
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
 */
ostream&
netlist::emit(ostream& o, const bool sub, const netlist_options& nopt) const {
if (sub) {
	// FINISH_ME
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
	STACKTRACE_INDENT_PRINT("* instances:");
	// emit subinstances
	typedef	instance_pool_type::const_iterator	const_iterator;
	const_iterator i(instance_pool.begin()), e(instance_pool.end());
	for ( ; i!=e; ++i) {
		i->emit(o, node_pool, *fp) << endl;	// options?
	}
}
	// emit devices
{
	typedef	transistor_pool_type::const_iterator	const_iterator;
	const_iterator i(transistor_pool.begin()), e(transistor_pool.end());
	// TODO: print originating rule in comments
	// TODO: use optional label designations
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		o << 'M' << j << "_ ";
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
	Create a port summary so that other processes may correctly
	instantiate this.  
	What to do about supplies?
 */
void
netlist::summarize_ports(void) {
	STACKTRACE_VERBOSE;
	// could mark_used_nodes here instead...
// const port_alias_tracker& pa(f->get_port_alias_tracker());
	typedef	unique_list<index_type>	port_index_list_type;
	bool_port_collector<port_index_list_type> V;
	fp->accept(V);
	// handle Vdd and GND separately
	port_index_list_type::const_iterator
		i(V.bool_indices.begin()), e(V.bool_indices.end());
#if 0 && ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("named_node_map = ");
	copy(named_node_map.begin(), named_node_map.end(),
		ostream_iterator<index_type>(cerr, ","));
	cerr << endl;
	STACKTRACE_INDENT_PRINT("V.bool_indices = ");
	copy(i, e, ostream_iterator<index_type>(cerr, ","));
	cerr << endl;
#endif
	port_list.reserve(V.bool_indices.size());
	for ( ; i!=e; ++i) {
		// 1-indexed local id to 0-indexed named_node_map
		INVARIANT(*i);
		const index_type local_ind = *i -1;
		const index_type ni = named_node_map[local_ind];
		const node& n(node_pool[ni]);
#if 0 && ENABLE_STACKTRACE
		cerr << "local ind = " << local_ind << endl;
		cerr << "ni = " << ni << endl;
		n.emit(cerr << "node=", *fp) << endl;
#endif
	if (ni && n.used) {
		INVARIANT(n.is_logical_node());
		INVARIANT(n.index == *i);	// self-reference
		port_list.push_back(ni);
	}
	}
#if 0 && ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("port_list = ");
	copy(port_list.begin(), port_list.end(),
		ostream_iterator<index_type>(cerr, ","));
	cerr << endl;
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
 */
void
netlist_generator::visit(const global_entry<process_tag>& p) {
	STACKTRACE_VERBOSE;
	// don't need to temporarily set the footprint_frame
	// because algorithm is completely hierarchical, no flattening
	// will need p._frame when emitting subinstances
	const footprint* f(p._frame._footprint);
	NEVER_NULL(f);
	INVARIANT(f->is_created());
	netlist_map_type::iterator mi(netmap.find(f));
	const bool first_time = (mi == netmap.end());
	const bool top_level = !current_netlist;
if (first_time) {
	netlist* nl = &netmap[f];	// insert default constructed
	nl->bind_footprint(*f, opt);
	// initialize netlist:
	const footprint_frame_map_type&
		bfm(p._frame.get_frame_map<bool_tag>());
		// ALERT: top-footprint frame's size will be +1!
	nl->named_node_map.resize(bfm.size(), void_index);	// 0-fill
	// set current netlist (duplicate for local):
	const value_saver<netlist*> __tmp(current_netlist, nl);
	const value_saver<netlist_common*> __tmp2(current_local_netlist, nl);
	// should not invalidate existing iterators
	const footprint_frame_map_type&
		pfm(p._frame.get_frame_map<process_tag>());
	STACKTRACE_INDENT_PRINT("pfm.size = " << pfm.size() << endl);
	nl->instance_pool.reserve(pfm.size());	// prevent reallocation!!!
	typedef	footprint_frame_map_type::const_iterator	const_iterator;
	const_iterator i(pfm.begin()), e(pfm.end());
	// skip first NULL slot?
	// ALERT: top-level's process frame starts at 1, not 0!
	index_type lpid = top_level ? 0 : 1;
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
		// create_port_summary from footprint
		nl->summarize_ports();
	}
	// finally, emit this process
	nl->emit(os, !top_level, opt) << endl;
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
	const value_saver<index_type> __s1(low_supply, GND_index);
	const value_saver<index_type> __s2(high_supply, Vdd_index);
{
	// TODO: internal nodes (in progress)
	// TODO: alternative, only visit internal nodes on-demand
	//	to solve problem of unused dangling internal nodes
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
		const index_type expr = i->second.first;
		const bool dir = i->second.second;
		const value_saver<index_type>
			__t1(foot_node, (dir ? Vdd_index : GND_index)),
			__t2(output_node, current_netlist->create_internal_node(
				expr, i->first));
		const value_saver<char>
			__t3(fet_type,
				(dir ? transistor::PFET_TYPE : transistor::NFET_TYPE));
		// TODO: honor prs supply override directives
		// internal nodes partial rules can belong to local subcircuits
		// but are accessible to all sibling subcircuits
		// within a process definition.
		// set output node?
		prs->get_expr_pool()[expr].accept(*this);
	}
}
	// traverse subcircuits
	// ALERT: if we ever want to perform other operations before emitting
	// these local subcircuit structures need to be kept around longer
	// because the instances of them would be dangling pointers...
	const subckt_map_type& subc_map(prs->get_subcircuit_map());
{
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
	const value_saver<char>
		__t3(fet_type, (r.dir ? transistor::PFET_TYPE : transistor::NFET_TYPE));
	// TODO: honor prs supply override directives
	const prs_footprint::expr_pool_type& ep(prs->get_expr_pool());
	ep[r.expr_index].accept(*this);
	// TODO: process rule attributes, labels, names...
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TODO: handle internal node definitions and precharges!
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Walk expressions from left to right, emitting transistors.
	\pre current_netlist is set
	\pre foot_node and output_node are already set to existing nodes, 
		e.g. Vdd, GND, or auxiliary node.
 */
void
netlist_generator::visit(const entity::PRS::footprint_expr_node& e) {
	STACKTRACE_VERBOSE;
	const prs_footprint::expr_pool_type& ep(prs->get_expr_pool());
	// exception: if encountering an internal node on left-most 
	// expression, then set foot_node instead of emitting a device.
const char type = e.get_type();
switch (type) {
case PRS_LITERAL_TYPE_ENUM: {
	// TODO: check for negation normalization
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
	t.width = (fet_type == transistor::NFET_TYPE ?
		opt.std_n_width : opt.std_p_width);
	t.length = (fet_type == transistor::NFET_TYPE ?
		opt.std_n_length : opt.std_p_length);
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
#if 0
		// TODO: handle precharges
		const prs_footprint::precharge_map_type&
			pc(prs->get_precharges());
#endif
		index_type prev = foot_node;
		// create intermediate nodes
		for ( ; i<s; ++i) {		// all but last node
			// setup foot and output, then recurse!
			const value_saver<index_type>
				_t1(foot_node, prev),
				_t2(output_node, 
					current_netlist->create_auxiliary_node());
			ep[e[i]].accept(*this);
			prev = output_node;
			// TODO: precharge
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
	// lookup internal node
	foot_node = current_netlist->lookup_internal_node(e.only());
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
 */
void
netlist_generator::visit(const entity::PRS::footprint_macro& e) {
	STACKTRACE_VERBOSE;
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

