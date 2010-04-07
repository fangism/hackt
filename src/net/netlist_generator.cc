/**
	\file "net/netlist_generator.cc"
	Implementation of hierarchical netlist generation.
	$Id: netlist_generator.cc,v 1.17 2010/04/07 00:13:04 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <sstream>		// for ostringstream
#include <iterator>		// for ostream_iterator
#include <algorithm>
#include "net/netlist_generator.h"
#include "net/netlist_options.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#include "Object/global_entry_context.h"
#include "Object/def/footprint.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/string_expr.h"
#include "Object/inst/instance_pool.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/traits/instance_traits.h"
#include "Object/lang/PRS_footprint.h"
#include "common/TODO.h"
#include "util/stacktrace.h"

namespace HAC {
namespace NET {
#include "util/using_ostream.h"
using entity::footprint_frame_map_type;
using entity::bool_tag;
using entity::state_instance;
using entity::pint_value_type;
using entity::pint_const;
using std::ostringstream;
using std::ostream_iterator;
using std::upper_bound;
using util::value_saver;
using entity::string_expr;
using entity::PRS::PRS_LITERAL_TYPE_ENUM;
using entity::PRS::PRS_NOT_EXPR_TYPE_ENUM;
using entity::PRS::PRS_AND_EXPR_TYPE_ENUM;
using entity::PRS::PRS_OR_EXPR_TYPE_ENUM;
using entity::PRS::PRS_NODE_TYPE_ENUM;
using entity::directive_base_params_type;
using entity::resolved_attribute;
using entity::resolved_attribute_list_type;

//=============================================================================
// helper globals


// static const string	dummy_type_name("<name>");	// temporary
//=============================================================================
// class netlist_generator method definitions

netlist_generator::netlist_generator(
		const footprint_frame& ff, 
		const global_offset& go, 
		ostream& o, 
		const netlist_options& p) :
		global_entry_context(ff, go), 
		cflat_visitor(), 
		os(o), opt(p), netmap(),
		prs(NULL), 
		current_netlist(NULL), 
		current_local_netlist(NULL),
		foot_node(netlist::void_index),
		output_node(netlist::void_index),
#if NETLIST_GROUPED_TRANSISTORS
		current_assoc_node(netlist::void_index), 
		current_assoc_dir(false),		// don't care
#endif
		current_width(0.0),
		current_length(0.0),
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
	NEVER_NULL(topfp);
	topfp->accept(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't know if anything needs to be done here.  
	Perhaps apply attributes to corresponding mapped local netlist nodes.  
 */
void
netlist_generator::visit(const state_instance<bool_tag>& p) {
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
netlist_generator::visit(const state_instance<process_tag>& p) {
	STACKTRACE_VERBOSE;
	// don't need to temporarily set the footprint_frame
	// because algorithm is completely hierarchical, no flattening
	// will need p._frame when emitting subinstances
		const state_instance<process_tag>& subp(p);
		const size_t lpid = subp.get_back_ref()->instance_index;
		// guarantee that dependent type is processed with netlist
		// find out how local nodes are passed to *local* instance
		const footprint* subfp = subp._frame._footprint;
		const netlist& subnet(netmap.find(subfp)->second);
		current_netlist->append_instance(subp, subnet, lpid, opt);
	// if this is not top-level, wrap emit in .subckt/.ends
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist_generator::visit(const footprint& f) {
	INVARIANT(f.is_created());	// don't need is_allocated()!!!
	netlist_map_type::iterator mi(netmap.find(&f));
	const bool first_time = (mi == netmap.end());
	const bool top_level = at_top();
if (first_time) {
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("processing unique type: ");
	f.dump_type(STACKTRACE_STREAM) << endl;
#endif
	netlist* nl = &netmap[&f];	// insert default constructed
if (&f == topfp) {	// at_top()
	nl->bind_footprint(f, "<top-level>");
} else {
	nl->bind_footprint(f, opt);
}
try {
	// set current netlist (duplicate for local):
	// should not invalidate existing iterators
	const value_saver<netlist*> __tmp(current_netlist, nl);
	const value_saver<netlist_common*> __tmp2(current_local_netlist, nl);
	const size_t bs = f.get_instance_pool<bool_tag>().local_entries();
	nl->named_node_map.resize(bs, netlist::void_index);
	// 0-fill
	// resize(f.get_instance_pool<bool_tag>().size()) ???
	STACKTRACE_INDENT_PRINT("|bools| = " << bs << endl);
	const size_t ps = f.get_instance_pool<process_tag>().local_entries();
	// really should be local_private_entries(), excluding process ports
	// process ports belong to parents
	STACKTRACE_INDENT_PRINT("|procs| = " << ps << endl);
	nl->instance_pool.reserve(ps);	// prevent reallocation!!!

	visit_recursive(f);	// process dependent types depth-first
	visit_local<process_tag>(f, top_level);
//	visit_local<bool_tag>(f, top_level);
	f.get_prs_footprint().accept(*this);
//	f.get_spec_footprint().accept(*this);	// ?

	if (!top_level || opt.top_type_ports) {
		nl->summarize_ports(opt);
#if NETLIST_CHECK_CONNECTIVITY
		// don't bother checking top-level
		if (nl->check_node_connectivity(opt) == STATUS_ERROR) {
			THROW_EXIT;
		}
#endif
	}
} catch (...) {
	cerr << "ERROR producing netlist for " << nl->name << endl;
	throw;
}
#if ENABLE_STACKTRACE
	nl->dump_raw(cerr);	// DEBUG point
#endif
if (opt.empty_subcircuits || !nl->is_empty()) {
	nl->emit(os, !top_level || opt.top_type_ports, opt) << endl;
} else {
	os << opt.comment_prefix << "subcircuit "
		<< nl->name << " is empty.\n" << endl;
}
}
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
#if !PRS_SUPPLY_OVERRIDES
	// for now, default supplies
	const value_saver<index_type>
		__s1(low_supply, netlist::GND_index),
		__s2(high_supply, netlist::Vdd_index);
#endif
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
	const prs_footprint::internal_node_pool_type&
		inode_pool(prs->get_internal_node_pool());
	prs_footprint::internal_node_pool_type::const_iterator
		i(inode_pool.begin()), e(inode_pool.end());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		// each entry is a node_expr_type
		// where pair:first is expr-index and second is direction
		// using footprint's internal_node_pool
		// as basis for subcircuits internal_node_map
		const index_type& expr = i->first;
		// direction and name can be looked up later
		const index_type new_int =
			current_netlist->create_internal_node(j, expr, opt);
		INVARIANT(new_int);
		// asserts map entry exists:
		current_netlist->lookup_internal_node(expr);
	}
	// now walk subcircuit map to assign owner subcircuits
	const subckt_map_type& subc_map(prs->get_subcircuit_map());
	const_iterator si(subc_map.begin()), se(subc_map.end());
	j = 1;	// owner 1-indexed, default upon ctor was 0
	for ( ; si!=se; ++si, ++j) {
		size_t k = si->int_nodes.first;
		for ( ; k < si->int_nodes.second; ++k) {
			current_netlist->internal_node_map[k].second = j;
		}
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
	STACKTRACE_INDENT_PRINT("processing subcircuits (rules) ..." << endl);
	const_iterator si(subc_map.begin()), se(subc_map.end());
	netlist::local_subcircuit_list_type::iterator
		mi(current_netlist->local_subcircuits.begin());
	while (si!=se && si->rules_empty()) { ++si; ++mi; }	// skip empty
	const prs_footprint::rule_pool_type& rpool(prs->get_rule_pool());
	const size_t s = rpool.size();
	size_t i = 0;
	for ( ; i<s; ++i) {
	if (si!=se && (i >= si->rules.first)) {
		INVARIANT(mi != current_netlist->local_subcircuits.end());
		local_netlist& n(*mi);
		const value_saver<netlist_common*>
			__tmp(current_local_netlist, &n);
		for ( ; i < si->rules.second; ++i) {
			visit_rule(rpool, i);
		}
			--i;	// back-adjust before continue
		// advance to next non-empty subcircuit
		do { ++si; ++mi; } while (si!=se && si->rules_empty());
	} else {
		// rule is outside of subcircuits
		visit_rule(rpool, i);
	}
	}	// end for
}{
	STACKTRACE_INDENT_PRINT("processing macros..." << endl);
	const_iterator si(subc_map.begin()), se(subc_map.end());
	netlist::local_subcircuit_list_type::iterator
		mi(current_netlist->local_subcircuits.begin());
	while (si!=se && si->macros_empty()) { ++si; ++mi; }	// skip empty
	const prs_footprint::macro_pool_type& mpool(prs->get_macro_pool());
	const size_t s = mpool.size();
	size_t i = 0;
	for ( ; i<s; ++i) {
	if (si!=se && (i >= si->macros.first)) {
		// start of a subcircuit range, can be empty
		INVARIANT(mi != current_netlist->local_subcircuits.end());
		local_netlist& n(*mi);
			const value_saver<netlist_common*>
				__tmp(current_local_netlist, &n);
		for ( ; i < si->macros.second; ++i) {
			visit_macro(mpool, i);
		}
			--i;	// back-adjust before continue
		// advance to next non-empty subcircuit
		do { ++si; ++mi; } while (si!=se && si->macros_empty());
	} else {
		// macro is outside of subcircuits
		visit_macro(mpool, i);
	}
	}	// end for
}
	// process all subcircuits first, then remaining local rules/macros
	current_netlist->mark_used_nodes();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRS_SUPPLY_OVERRIDES
// for use with std::upper_bound
static
bool
rule_supply_map_compare(const index_type v, 
		const entity::PRS::footprint::supply_map_type::value_type& i) {
	return v < i.rules.first;
}

static
bool
macro_supply_map_compare(const index_type v,
		const entity::PRS::footprint::supply_map_type::value_type& i) {
	return v < i.macros.first;
}

static
bool
internal_node_supply_map_compare(const index_type v,
		const entity::PRS::footprint::supply_map_type::value_type& i) {
	return v < i.int_nodes.first;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class RP>
void
netlist_generator::visit_rule(const RP& rpool, const index_type i) {
#if PRS_SUPPLY_OVERRIDES
	const prs_footprint::supply_map_type& m(prs->get_supply_map());
	typedef	prs_footprint::supply_map_type::const_iterator	const_iterator;
	const_iterator f(upper_bound(m.begin(), m.end(), i, 
		&rule_supply_map_compare));
	INVARIANT(f != m.begin());
	--f;
	// lookup supply in map
	const value_saver<index_type>
		__s1(low_supply, register_named_node(f->GND)),
		__s2(high_supply, register_named_node(f->Vdd));
#endif
	rpool[i].accept(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class MP>
void
netlist_generator::visit_macro(const MP& mpool, const index_type i) {
#if PRS_SUPPLY_OVERRIDES
	const prs_footprint::supply_map_type& m(prs->get_supply_map());
	typedef	prs_footprint::supply_map_type::const_iterator	const_iterator;
	const_iterator f(upper_bound(m.begin(), m.end(), i, 
		&macro_supply_map_compare));
	INVARIANT(f != m.begin());
	--f;
	// lookup supply in map
	const value_saver<index_type>
		__s1(low_supply, register_named_node(f->GND)),
		__s2(high_supply, register_named_node(f->Vdd));
#endif
	mpool[i].accept(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// PRS rule attribute action map
typedef	entity::PRS::footprint_rule		PRS_rule;
typedef	PRS_rule::attributes_list_type::value_type	rule_attribute;
typedef	rule_attribute::values_ptr_type		attr_values_ptr_type;
typedef	rule_attribute::values_type		attr_values_type;
typedef	void rule_attribute_fun(netlist_generator&, const attr_values_ptr_type&);
typedef	rule_attribute_fun*	rule_attribute_fun_ptr;
typedef	map<string, rule_attribute_fun_ptr>	rule_attribute_map_type;

// initializable
static
rule_attribute_map_type		__rule_attribute_map;

// non-modifiable
static
const rule_attribute_map_type&	rule_attribute_map(__rule_attribute_map);

struct netlist_generator::rule_attribute_functions {
static
void
__attr_iskeeper(netlist_generator& g, const attr_values_ptr_type& v) {
	pint_value_type b = 1;
	if (v && v->size()) {
		const pint_const&
			pi(*(*v)[0].is_a<const pint_const>());
		b = pi.static_constant_value();
	}
	if (b) {
		g.fet_attr |= transistor::IS_STANDARD_KEEPER;
	}
}

static
void
__attr_isckeeper(netlist_generator& g, const attr_values_ptr_type& v) {
	pint_value_type b = 1;
	if (v && v->size()) {
		const pint_const&
			pi(*(*v)[0].is_a<const pint_const>());
		b = pi.static_constant_value();
	}
	if (b) {
		g.fet_attr |= transistor::IS_COMB_FEEDBACK;
	}
}

static
void
__attr_hvt(netlist_generator& g, const attr_values_ptr_type& v) {
	pint_value_type b = 1;
	if (v && v->size()) {
		const pint_const&
			pi(*(*v)[0].is_a<const pint_const>());
		b = pi.static_constant_value();
	}
	if (b) {
		g.fet_attr |= transistor::IS_HIGH_VT;
		g.fet_attr &= ~transistor::IS_LOW_VT;
	} else {
		g.fet_attr &= ~transistor::IS_HIGH_VT;
	}
}

static
void
__attr_lvt(netlist_generator& g, const attr_values_ptr_type& v) {
	pint_value_type b = 1;
	if (v && v->size()) {
		const pint_const&
			pi(*(*v)[0].is_a<const pint_const>());
		b = pi.static_constant_value();
	}
	if (b) {
		g.fet_attr |= transistor::IS_LOW_VT;
		g.fet_attr &= ~transistor::IS_HIGH_VT;
	} else {
		g.fet_attr &= ~transistor::IS_LOW_VT;
	}
}

static
void
__attr_svt(netlist_generator& g, const attr_values_ptr_type& v) {
	pint_value_type b = 1;
	if (v && v->size()) {
		const pint_const&
			pi(*(*v)[0].is_a<const pint_const>());
		b = pi.static_constant_value();
	}
	if (b) {
		// clear low/high-Vt flags
		g.fet_attr &= ~(transistor::IS_HIGH_VT | transistor::IS_LOW_VT);
	}
}

static
int
init_rule_attribute_map(void) {
	__rule_attribute_map["iskeeper"] = &__attr_iskeeper;
	__rule_attribute_map["isckeeper"] = &__attr_isckeeper;
	__rule_attribute_map["hvt"] = &__attr_hvt;
	__rule_attribute_map["svt"] = &__attr_svt;
	__rule_attribute_map["lvt"] = &__attr_lvt;
	return 0;
}

static const int __init__;
};	// end struct rule_attribute_functions

// initialize global function map
const int netlist_generator::rule_attribute_functions::__init__ =
	netlist_generator::rule_attribute_functions::init_rule_attribute_map();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Construct subgraph from LHS expression, and connect final
	output(s) to output node.
	\pre current_netlist is set
 */
void
netlist_generator::visit(const entity::PRS::footprint_rule& r) {
	STACKTRACE_VERBOSE;
	typedef	entity::PRS::footprint_rule		rule;
	// set foot_node and output_node and fet_type
	const value_saver<index_type>
		__t1(foot_node, (r.dir ? high_supply : low_supply)),
		__t2(output_node, register_named_node(r.output_index));
	const value_saver<transistor::fet_type>
		__t3(fet_type, (r.dir ? transistor::PFET_TYPE : transistor::NFET_TYPE));
	const value_saver<char> __t4(fet_attr);
#if NETLIST_GROUPED_TRANSISTORS
	const value_saver<index_type> __ta1(current_assoc_node, output_node);
	const value_saver<bool> __ta2(current_assoc_dir, r.dir);
#endif
	// apply rule attributes: iskeeper, etc...
	const rule::attributes_list_type& rats(r.attributes);
	rule::attributes_list_type::const_iterator
		i(rats.begin()), e(rats.end());
for ( ; i!=e; ++i) {
	rule_attribute_map_type::const_iterator
		f(rule_attribute_map.find(i->key));
	if (f != rule_attribute_map.end()) {
		(*f->second)(*this, i->values);
	}
	// ignore unknown attributes silently
}
	const value_saver<real_type> __t5(current_width), __t6(current_length);
	const bool is_keeper = fet_attr & transistor::IS_STANDARD_KEEPER;
	set_current_width(opt.get_default_width(r.dir, is_keeper));
	set_current_length(opt.get_default_length(r.dir, is_keeper));
try {
	const prs_footprint::expr_pool_type& ep(prs->get_expr_pool());
	ep[r.expr_index].accept(*this);
} catch (...) {
	cerr << "ERROR in production rule." << endl;
	current_netlist->fp->dump_rule(cerr << '\t', r) << endl;
	throw;	// re-throw
}
}	// end method visit(rule)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Uses fet_type and fet_attr to determine whether this
	is NFET or PFET and whether is part of a keeper or not.
 */
void
netlist_generator::set_current_width(const real_type w) {
	const bool dir = (fet_type == transistor::PFET_TYPE);
//	const bool is_keeper = fet_attr & transistor::IS_STANDARD_KEEPER;
	real_type max_width = (dir ? opt.max_p_width : opt.max_n_width);
	real_type new_width = std::max(opt.min_width, w);
	if (max_width > 0.0)	// ignore max when 0.0
		new_width = std::min(max_width, new_width);
	current_width = new_width;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist_generator::set_current_length(const real_type l) {
	current_length = std::max(opt.min_length, l);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRS_SUPPLY_OVERRIDES
/**
	Errors out by throwing exception.
 */
static
void
__diagnose_supply_mismatch(ostream& o, const netlist_options& opt, 
		const string& subc_name, const char* supply_name, 
		const string& node_name) {
if (opt.internal_node_supply_mismatch_policy != OPTION_IGNORE) {
const bool err = opt.internal_node_supply_mismatch_policy == OPTION_ERROR;
	if (err)
		o << "Error:";
	else	o <<  opt.comment_prefix << "Warning:";
	o << " in subcircuit " << subc_name << ':' << endl <<
		opt.comment_prefix << supply_name <<
		" supply of internal node differs between definition and use: @"
		<< node_name << endl;
	if (err) {
		THROW_EXIT;
	}
}
}
#endif
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive, generate on-demand internal nodes as they are visited.
	\pre all internal nodes have been allocated/mapped a priori, 
		just not necessarily defined (node::used).
	\param nid is the footprint-local internal node index (0-indexed).
	\return netlist-local node index representing the internal node.
	\invariant no cyclic definitions of internal nodes possible.
 */
index_type
netlist_generator::register_internal_node(const index_type nid) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("@id = " << nid << endl);
	const prs_footprint::expr_pool_type& ep(prs->get_expr_pool());
	const prs_footprint::node_expr_type& ret(prs->get_internal_node(nid));
	const index_type& defexpr = ret.first;
	STACKTRACE_INDENT_PRINT("@expr = " << defexpr << endl);
	const netlist::internal_node_entry_type&
		r(current_netlist->internal_node_map[nid]);
	const index_type& node_ind = r.first;
	const index_type& node_own = r.second;	// home
	node& n(current_netlist->node_pool[node_ind]);
#if NETLIST_CHECK_CONNECTIVITY
if (!n.driven)	// define-once
#else
if (!n.used)
#endif
{
	STACKTRACE_INDENT_PRINT("defining internal node..." << endl);
	const bool dir = ret.second;
	// else need to define internal node once only
#if PRS_SUPPLY_OVERRIDES
	// lookup supply associated with internal node's definition
	const prs_footprint::supply_map_type& m(prs->get_supply_map());
	typedef	prs_footprint::supply_map_type::const_iterator	const_iterator;
	const_iterator f(upper_bound(m.begin(), m.end(), nid, 
		&internal_node_supply_map_compare));
	INVARIANT(f != m.begin());
	--f;
	const index_type gi = register_named_node(f->GND);
	const index_type vi = register_named_node(f->Vdd);
	// diagnostic: if supply differs from definition and use domains
	if (!dir && (low_supply != gi)) {
		__diagnose_supply_mismatch(cerr, opt,
			current_netlist->get_name(), "GND", n.name);
	}
	if (dir && (high_supply != vi)) {
		__diagnose_supply_mismatch(cerr, opt,
			current_netlist->get_name(), "Vdd", n.name);
	}
	const value_saver<index_type>
		__s1(low_supply, gi), __s2(high_supply, vi);
#endif
	const value_saver<index_type>
		__t1(foot_node, (dir ? high_supply : low_supply)),
		__t2(output_node, node_ind);
	const value_saver<transistor::fet_type>
		__t3(fet_type,
			(dir ? transistor::PFET_TYPE : transistor::NFET_TYPE));
	const value_saver<real_type> __t4(current_width), __t5(current_length);
#if NETLIST_GROUPED_TRANSISTORS
	const value_saver<index_type> __ta1(current_assoc_node, output_node);
	const value_saver<bool> __ta2(current_assoc_dir, dir);
#endif
	// Q: can internal nodes definitions be applied to keepers?
	// if so, then we need attributes for internal node definitions (rules)
	set_current_width(opt.get_default_width(dir, false));
	set_current_length(opt.get_default_length(dir, false));
#if NETLIST_CHECK_CONNECTIVITY
	n.driven = true;
#endif
	n.used = true;
	// mark before recursion, not after!
	// to prevent shared roots from being duplicated
	STACKTRACE_INDENT_PRINT("owner = " << node_own << endl);
	if (node_own) {
	// generate in correct owner subcircuit
	// internal nodes' partial rules can belong to local subcircuits
	// but are accessible to all sibling subcircuits
	// within a process definition.
		STACKTRACE("pointing to subcircuit");
		const value_saver<netlist_common*>
			__t6(current_local_netlist, 
				&current_netlist->local_subcircuits[node_own -1]);
		ep[defexpr].accept(*this);
	} else {
		// point back to main scope
		const value_saver<netlist_common*>
			__t6(current_local_netlist, current_netlist);
		ep[defexpr].accept(*this);
	}
}	// end if !n.driven
	return node_ind;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers node for first time or just returns previously registered
	index.  Also marks node as used.  
	Also applicable to implicit supply nodes !GND and !Vdd.
 */
index_type
netlist_generator::register_named_node(const index_type n) {
	NEVER_NULL(current_netlist);
	const index_type ret = current_netlist->register_named_node(n, opt);
	return ret;
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
	const value_saver<char>
		_t5(fet_attr, fet_attr | transistor::IS_PRECHARGE);
	const value_saver<real_type> __t6(current_width), __t7(current_length);
	set_current_width(opt.get_default_width(dir, false));
	set_current_length(opt.get_default_length(dir, false));
	// use the same output node
	ep[pchgex].accept(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
process_transistor_attributes(transistor& t, 
		const resolved_attribute_list_type& a) {
	resolved_attribute_list_type::const_iterator
		ai(a.begin()), ae(a.end());
	// TODO: write an actual attribute function map for altering transistor
	for ( ; ai!=ae; ++ai) {
		// this is just a quick hack for now
		if (ai->key == "label")
			t.name = ai->values->front().is_a<const string_expr>()
				->static_constant_value();
		else if (ai->key == "lvt")
			t.set_lvt();
		else if (ai->key == "svt")
			t.set_svt();
		else if (ai->key == "hvt")
			t.set_hvt();
		else {
			cerr << "Warning: unknown literal attribute \'" <<
				ai->key << "\' ignored." << endl;
		}
	}
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
	STACKTRACE_INDENT_PRINT("expr is leaf node" << endl);
	bool override_fet_type = false;
	if (negated ^ (fet_type == transistor::PFET_TYPE)) {
	if (fet_attr & transistor::IS_PRECHARGE) {
		// special handling for precharges based on policy
		static const char msg[] = "precharge is not fully-restoring.";
		override_fet_type = true;
	switch (opt.non_CMOS_precharge_policy) {
	case OPTION_WARN:
		cerr << opt.comment_prefix << "Warning : " << msg << endl;
		break;
	case OPTION_ERROR:
		cerr << "ERROR: " << msg << endl;
		THROW_EXIT;
	default:	// OPTION_IGNORE
		break;
	}
	} else {
		// is standard rule, expected to be CMOS and restoring
		cerr << "ERROR: rule-literal is not CMOS-implementable." << endl;
		THROW_EXIT;
	}
	}
	transistor t;
	t.type = override_fet_type ?
		(negated ? transistor::PFET_TYPE : transistor::NFET_TYPE)
		: fet_type;
	t.gate = register_named_node(e.only());
	t.source = foot_node;
	t.drain = output_node;
	t.body = (t.type == transistor::NFET_TYPE ? low_supply : high_supply);
		// Vdd or GND
#if NETLIST_GROUPED_TRANSISTORS
	t.assoc_node = current_assoc_node;
	t.assoc_dir = current_assoc_dir;
#endif
	const directive_base_params_type& p(e.params);
//	const bool is_n = (t.type == transistor::NFET_TYPE);
//	const bool is_k = (fet_attr & transistor::IS_STANDARD_KEEPER);
		// excludes combinational feedback keepers
	if (p.size() > 0) {
		set_current_width(p[0]->to_real_const());
	}
	t.width = current_width;
	// TODO: constrain width
	if (p.size() > 1) {
		set_current_length(p[1]->to_real_const());
	}
	t.length = current_length;
	// TODO: constrain length
	t.attributes = fet_attr;
	// transistor attributes
	process_transistor_attributes(t, e.attributes);
	NEVER_NULL(current_local_netlist);
	current_local_netlist->transistor_pool.push_back(t);
	break;
}
case PRS_NOT_EXPR_TYPE_ENUM: {
	STACKTRACE_INDENT_PRINT("expr is negation" << endl);
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
		STACKTRACE_INDENT_PRINT("expr is conjunctive" << endl);
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
			// left.only() is still an expression index, 
			// but we need internal-node-index.
			prev = register_internal_node(
				current_netlist->lookup_internal_node(
					left.only()));
			// confirm direction and sense of internal node
			// already checked at create-phase.
			// handle precharge
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
		STACKTRACE_INDENT_PRINT("expr is disjunctive" << endl);
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
	t.gate = register_named_node(*e.nodes[0].begin());
	t.source = register_named_node(*e.nodes[1].begin());
	t.drain = register_named_node(*e.nodes[2].begin());
	t.body = passp ? high_supply : low_supply;
#if NETLIST_GROUPED_TRANSISTORS
	t.assoc_node = t.drain;
	t.assoc_dir = passp;
#endif
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
	// transistor attributes
	process_transistor_attributes(t, e.attributes);
	t.set_pass();		// indicate is pass gate
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

