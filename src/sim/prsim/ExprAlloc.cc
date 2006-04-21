/**
	\file "sim/prsim/ExprAlloc.cc"
	$Id: ExprAlloc.cc,v 1.6.2.5 2006/04/21 20:10:13 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include "sim/prsim/ExprAlloc.h"
#include "sim/prsim/Expr.h"
#include "sim/prsim/State.h"
#include "Object/lang/PRS_enum.h"
#include "Object/lang/PRS_footprint_rule.h"
#include "Object/lang/PRS_footprint_expr.h"
#include "Object/lang/PRS_footprint_macro.h"
#include "Object/lang/PRS_attribute_common.h"
#include "Object/lang/PRS_attribute_registry.h"
#include "Object/lang/PRS_macro_common.h"
#include "Object/lang/PRS_macro_registry.tcc"
#include "Object/lang/SPEC_common.h"
#include "Object/lang/SPEC_registry.tcc"
#include "Object/lang/SPEC_footprint.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/pint_const.h"
#include "Object/traits/classification_tags.h"
#include "Object/global_entry.h"
#include "util/offset_array.h"
#include "util/stacktrace.h"
#include "util/qmap.tcc"
#include "common/TODO.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using entity::bool_tag;
using entity::pint_const;
using util::good_bool;
#include "util/using_ostream.h"

//=============================================================================
typedef	entity::PRS::attribute_visitor_entry<ExprAlloc>
					ExprAlloc_attribute_definition_entry;

typedef	util::qmap<string, ExprAlloc_attribute_definition_entry>
					ExprAlloc_attribute_registry_type;

static
const ExprAlloc_attribute_registry_type
ExprAlloc_attribute_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Utility function for registering an attribute class.  
 */
template <class T>
static
size_t
register_ExprAlloc_attribute_class(void) {
	typedef	ExprAlloc_attribute_registry_type	registry_type;
	typedef	registry_type::iterator			iterator;
	typedef	registry_type::mapped_type		mapped_type;
	ExprAlloc_attribute_registry_type&
		__init_registry(const_cast<ExprAlloc_attribute_registry_type&>(
				ExprAlloc_attribute_registry));
	const string k(T::name);
	mapped_type& m(__init_registry[k]);
	if (m) {
		cerr << "Error: PRS attribute by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = ExprAlloc_attribute_definition_entry(k, &T::main, &T::check_vals);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n __ATTRIBUTE_UNUSED_CTOR__((__init_registry[k]));
	INVARIANT(n);
	return ExprAlloc_attribute_registry.size();
}

//=============================================================================
typedef	entity::PRS::macro_visitor_entry<ExprAlloc>
					ExprAlloc_macro_definition_entry;

typedef	util::qmap<string, ExprAlloc_macro_definition_entry>
					ExprAlloc_macro_registry_type;

static
const ExprAlloc_macro_registry_type
ExprAlloc_macro_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Utility function for registering an attribute class.  
 */
template <class T>
static
size_t
register_ExprAlloc_macro_class(void) {
	typedef	ExprAlloc_macro_registry_type		registry_type;
	typedef	registry_type::iterator			iterator;
	typedef	registry_type::mapped_type		mapped_type;
	ExprAlloc_macro_registry_type&
		__init_registry(const_cast<ExprAlloc_macro_registry_type&>(
				ExprAlloc_macro_registry));
	const string k(T::name);
	mapped_type& m(__init_registry[k]);
	if (m) {
		cerr << "Error: PRS macro by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = ExprAlloc_macro_definition_entry(k, &T::main,
		&T::check_num_params, &T::check_num_nodes, 
		&T::check_param_args, &T::check_node_args);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n __ATTRIBUTE_UNUSED_CTOR__((__init_registry[k]));
	INVARIANT(n);
	return ExprAlloc_macro_registry.size();
}

//=============================================================================
typedef	entity::SPEC::spec_visitor_entry<ExprAlloc>
					ExprAlloc_spec_definition_entry;

typedef	util::qmap<string, ExprAlloc_spec_definition_entry>
					ExprAlloc_spec_registry_type;

static
const ExprAlloc_spec_registry_type
ExprAlloc_spec_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Utility function for registering an attribute class.  
 */
template <class T>
static
size_t
register_ExprAlloc_spec_class(void) {
	typedef	ExprAlloc_spec_registry_type		registry_type;
	typedef	registry_type::iterator			iterator;
	typedef	registry_type::mapped_type		mapped_type;
	ExprAlloc_spec_registry_type&
		__init_registry(const_cast<ExprAlloc_spec_registry_type&>(
				ExprAlloc_spec_registry));
	const string k(T::name);
	mapped_type& m(__init_registry[k]);
	if (m) {
		cerr << "Error: SPEC directive by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = ExprAlloc_spec_definition_entry(k, &T::main,
		&T::check_num_params, &T::check_num_nodes, 
		&T::check_param_args, &T::check_node_args);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n __ATTRIBUTE_UNUSED_CTOR__((__init_registry[k]));
	INVARIANT(n);
	return ExprAlloc_spec_registry.size();
}

//=============================================================================
// class ExprAlloc method definitions

/**
	NOTE: 0 is an invalid index to the state's expr_pool.  
 */
ExprAlloc::ExprAlloc(state_type& _s) :
		cflat_context_visitor(), 
		state(_s),
		st_node_pool(state.node_pool), 
		st_expr_pool(state.expr_pool), 
		st_graph_node_pool(state.expr_graph_node_pool), 
		st_rule_map(state.get_rule_map()),
		ret_ex_index(INVALID_EXPR_INDEX) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Update node fanin, and keep it consistent.  
	\pre state's node_array is already allocated.  
 */
void
ExprAlloc::visit(const footprint_rule& r) {
{
	STACKTRACE("ExprAlloc::visit(footprint_rule&)");
	(*expr_pool)[r.expr_index].accept(*this);
	const size_t top_ex_index = ret_ex_index;
	// r.output_index gives the local unique ID,
	// which needs to be translated to global ID.
	// bfm[...] refers to a global_entry<bool_tag> (1-indexed)
	// const size_t j = bfm[r.output_index-1];
	const size_t ni = __lookup_global_bool_id(r.output_index);

	// ignored: state.expr_graph_node_pool[top_ex_index].offset
	// not computing node fanin?  this would be the place to do it...
	// can always compute it (cacheable) offline
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "expr " << top_ex_index << " pulls node " <<
		ni << (r.dir ? " up" : " down") << endl;
#endif
	link_node_to_root_expr(ni, top_ex_index, r.dir);
#if ENABLE_STACKTRACE
	state.dump_struct(cerr) << endl;
#endif
	INVARIANT(top_ex_index == ret_ex_index);	// sanity check
}
{
	// first, unconditionally create a rule entry in the state's rule
	// map for every top-level (root) expression that affects a node.  
	rule_type& rule
		__ATTRIBUTE_UNUSED_CTOR__((st_rule_map[ret_ex_index]));
	// now iterate over attributes to apply changes
	typedef footprint_rule::attributes_list_type	attr_list_type;
	typedef	attr_list_type::const_iterator		const_iterator;
	const_iterator i(r.attributes.begin()), e(r.attributes.end());
	for ( ; i!=e; ++i) {
		ExprAlloc_attribute_registry[i->key].main(*this, *i->values);
	}
}
}	// end method visit(const footprint_rule&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates a new leaf literal expression in the sim state object.  
	\param ni resolved global ID of unique node.
	\return index of new literal expression, allocated in the state. 
		NOTE: this does not set ret_ex_index on return, it is the
		caller's responsibility to use it.  
 */
expr_index_type
ExprAlloc::allocate_new_literal_expr(const node_index_type ni) {
	// NOTE: Expr's parent and ExprGraphNode's offset
	// fields are not set until returned to caller!
	st_node_pool[ni].push_back_fanout(st_expr_pool.size());
	st_expr_pool.push_back(
		expr_type(expr_type::EXPR_NODE,1));
	st_graph_node_pool.push_back(graph_node_type());
	st_graph_node_pool.back().push_back_node(ni);
	// literal graph node has no children
	return st_expr_pool.size() -1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates a new NOT expression in the sim state object.  
	\param ei the index of the input expression.
	\return newly allocated ID of the negated expression.  
 */
expr_index_type
ExprAlloc::allocate_new_not_expr(const expr_index_type ei) {
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "sub_ex_index = " << ei << endl;
#endif
	st_expr_pool.push_back(
		expr_type(expr_type::EXPR_NOT,1));
	st_graph_node_pool.push_back(graph_node_type());
	// now link parent to only-child
	const expr_index_type last = st_expr_pool.size() -1;
	st_expr_pool[ei].parent = last;
	graph_node_type& child(st_graph_node_pool[ei]);
	child.offset = 0;
	st_graph_node_pool.back().push_back_expr(ei);
	return st_expr_pool.size() -1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param parent index of the parent expression
	\param child index of the child subexpression
	\param (0-indexed) offset of the child relative to parent, 
		the ordinal number of child.  
 */
void
ExprAlloc::link_child_expr(const expr_index_type parent,
		const expr_index_type child, const size_t offset) {
	// st_expr_pool[child].parent = parent;
	st_expr_pool[child].set_parent_expr(parent);
	graph_node_type& child_node(st_graph_node_pool[child]);
	child_node.offset = offset;
	st_graph_node_pool[parent].push_back_expr(child);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param type one of the entity::PRS::PRS_* enums defined in
		"Object/lan/PRS_enum.h", must be either AND or OR.  
	\param sz the number of children of this expression.  
	\return index of newly allocated N-ary expression (and/or).
 */
expr_index_type
ExprAlloc::allocate_new_Nary_expr(const char type, const size_t sz) {
	INVARIANT(sz);
	st_expr_pool.push_back(expr_type(
		(type == entity::PRS::PRS_AND_EXPR_TYPE_ENUM ?
		expr_type::EXPR_AND : expr_type::EXPR_OR), sz));
	st_graph_node_pool.push_back(graph_node_type());
	return st_expr_pool.size() -1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create expressions bottom-up.  
	\pre state's node_array is already allocated.  
	TODO: optimize expression hierarchy using NAND and NOR expr types
		for nodes whose children are all negated.  Ooooo.
 */
void
ExprAlloc::visit(const footprint_expr_node& e) {
	STACKTRACE("ExprAlloc::visit(footprint_expr_node&)");
	const size_t sz = e.size();
	const char type = e.get_type();
	// NOTE: 1-indexed
	switch (type) {
		// enumerations from "Object/lang/PRS_enum.h"
		case entity::PRS::PRS_LITERAL_TYPE_ENUM: {
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT << "literal" << endl;
#endif
			// leaf node
			INVARIANT(sz == 1);
			// lookup global bool ID
			const node_index_type ni =
				__lookup_global_bool_id(e.only());
			ret_ex_index = allocate_new_literal_expr(ni);
			break;
		}
		case entity::PRS::PRS_NOT_EXPR_TYPE_ENUM: {
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT << "not" << endl;
#endif
			INVARIANT(sz == 1);
			(*expr_pool)[e.only()].accept(*this);
			const size_t sub_ex_index = ret_ex_index;
			ret_ex_index = allocate_new_not_expr(sub_ex_index);
			break;
		}
		case entity::PRS::PRS_AND_EXPR_TYPE_ENUM:
			// yes, fall-through
		case entity::PRS::PRS_OR_EXPR_TYPE_ENUM: {
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT << "or/and" << endl;
#endif
			const expr_index_type last =
				allocate_new_Nary_expr(type, sz);
			size_t i = 1;
			for ( ; i<=sz; i++) {
				// reminder: e is 1-indexed while 
				// ExprGraphNode::children is 0-indexed
				(*expr_pool)[e[i]].accept(*this);
				const size_t sub_ex_index = ret_ex_index;
				link_child_expr(last, sub_ex_index, i-1);
			}
			ret_ex_index = last;
			break;
		}
		default:
			THROW_EXIT;
			break;
	}	// end switch
	// 'return' the index of the expr just allocated
#if ENABLE_STACKTRACE
	state.dump_struct(cerr) << endl;
#endif
}	// end method visit(const footprint_expr_node&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprAlloc::visit(const footprint_macro& m) {
	const ExprAlloc_macro_definition_entry&
		d(ExprAlloc_macro_registry[m.name]);
	INVARIANT(d);		// was already checked during unroll
	if (!d.check_param_args(m.params).good
			|| !d.check_node_args(m.nodes).good) {
		cerr << "Error with PRS macro." << endl;
		THROW_EXIT;
	} else {
		d.main(*this, m.params, m.nodes);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprAlloc::visit(const footprint_directive& s) {
	const ExprAlloc_spec_definition_entry&
		d(ExprAlloc_spec_registry[s.name]);
	INVARIANT(d);		// was already checked during unroll
	if (!d.check_param_args(s.params).good
			|| !d.check_node_args(s.nodes).good) {
		cerr << "Error with SPEC directive." << endl;
		THROW_EXIT;
	} else {
		d.main(*this, s.params, s.nodes);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Utility routine for connecting an output node to
	its root pull-up or pull-down expression.  
	This automatically takes care of OR-combination by allocating
	new root expressions when necessary.  
 */
void
ExprAlloc::link_node_to_root_expr(const node_index_type ni,
		const expr_index_type top_ex_index, const bool dir) {
	STACKTRACE("ExprAlloc::link_node_to_root_expr(...)");
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "linking expr " << top_ex_index << " to node " <<
		ni << (dir ? '+' : '-') << endl;
#endif
	node_type& output(st_node_pool[ni]);
	// now link root expression to node
	expr_type& ne(st_expr_pool[top_ex_index]);
	graph_node_type& ng(st_graph_node_pool[top_ex_index]);
	// prepare to take OR-combination?
	// or can we get away with multiple pull-up/dn roots?
	// or cheat! short-cut to root during operation.  
	// i.e. bypass OR-combination super-expression to root...
	// super-expression will be referenced in fanin
	// for consistent structure...
	// be careful to keep root expr consistent
	expr_index_type& dir_index(output.get_pull_expr(dir));
	if (dir_index) {
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "pull-up/dn already set" << endl;
#endif
		// already set, need OR-combination
		expr_type& pe(st_expr_pool[dir_index]);
		graph_node_type& pg(st_graph_node_pool[dir_index]);
		// see if either previous pull-up expr is OR-type already
		// may also work with NAND! (UNTESTED)

		// we don't OR-combine if the expression being examined
		// is already a top-level root-expression.  
		if ((pe.is_or() || pe.is_nand()) &&
				!state.is_rule_expr(top_ex_index)) {
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT << "prev. root expr is OR" << endl;
#endif
			// then simply extend the previous expr's children
			INVARIANT(dir == pe.direction());
			pg.push_back_expr(top_ex_index);
			// pe.parent unchanged (same node)
			// node's dir_index unchanged
			// NOTE: or short-cut to output node?
			ne.set_parent_expr(dir_index);
			ng.offset = pe.size;
			++pe.size;
		} else if ((ne.is_or() || ne.is_nand()) &&
				!state.is_rule_expr(dir_index)) {
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT << "new expr is OR" << endl;
#endif
			ng.push_back_expr(dir_index);
			dir_index = top_ex_index;
			ne.pull(ni, dir);
			pe.set_parent_expr(top_ex_index);
			pg.offset = ne.size;
			++ne.size;
		} else {
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT << "neither expr is OR" << endl;
#endif
			// then need to allocate new root-expression
			// and old expressions are no longer root!
			const expr_index_type root_ex_id =
				allocate_new_Nary_expr(
					entity::PRS::PRS_OR_EXPR_TYPE_ENUM, 2);
			expr_type& new_ex(st_expr_pool.back());
			new_ex.pull(ni, dir);
			link_child_expr(root_ex_id, dir_index, 0);
			link_child_expr(root_ex_id, top_ex_index, 1);
			// update the pull-up/dn root expression for the node
			dir_index = root_ex_id;
		}
	} else {
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "pull-up/dn not yet set" << endl;
#endif
		// easy: not already set, just link the new root expr.  
		dir_index = top_ex_index;
		ne.pull(ni, dir);
	}
}	// end ExprAlloc::link_node_to_root_expr(...)

//=============================================================================
/**
	Local namespace for prsim rule attributes.  
 */
namespace prsim_rule_attributes {

/**
	Macro for declaring attribute classes.  
	Here, the vistor_type is prsim's ExprAlloc.
	NOTE: these classes should have hidden visibility.  
 */
#define DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(class_name, att_name) \
	DECLARE_PRS_RULE_ATTRIBUTE_CLASS(class_name, ExprAlloc)		\
	DEFINE_PRS_RULE_ATTRIBUTE_CLASS(class_name, att_name, 		\
		register_ExprAlloc_attribute_class)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(After, "after")

/**
	Delay values real or floating or both?
 */
void
After::main(visitor_type& v, const values_type& a) {
	typedef	visitor_type::rule_type	rule_type;
	rule_type& r(v.st_rule_map[v.last_expr_index()]);
	const values_type::value_type& d(a.front());
	// assert type cast, b/c already checked
	r.set_delay(d.is_a<const pint_const>()->static_constant_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(Weak, "weak")

/**
	Sets or clears weak flag on a rule.  
 */
void
Weak::main(visitor_type& v, const values_type& a) {
	typedef	visitor_type::rule_type	rule_type;
	rule_type& r(v.st_rule_map[v.last_expr_index()]);
	const values_type::value_type& w(a.front());
	if (w.is_a<const pint_const>()->static_constant_value())
		r.set_weak();
	else	r.clear_weak();
}

#undef	DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS
}	// end namespace prsim_rule_attributes

//=============================================================================
/**
	Namespace for macro implementations specific to prsim.  
 */
namespace prsim_macros {
// DECLARE_AND_DEFINE_PRSIM_MACRO_CLASS(Echo, "echo")	// ignored

/**
	Convenient macro for declaring prsim PRS macros.  
 */
#define DECLARE_AND_DEFINE_PRSIM_MACRO_CLASS(class_name, att_name)	\
	DECLARE_PRS_MACRO_CLASS(class_name, ExprAlloc)			\
	DEFINE_PRS_MACRO_CLASS(class_name, att_name, 			\
		register_ExprAlloc_macro_class)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_MACRO_CLASS(PassN, "passn")

/**
	Produce the rule:
		after 0	g & ~s -> d-
	\param params unused.  
	\pre each node group argument contains exactly one node.  
 */
void
PassN::main(visitor_type& v, const param_args_type& params,
		const node_args_type& nodes) {
	const expr_index_type g =
		v.allocate_new_literal_expr(*nodes[0].begin());
	const expr_index_type s =
		v.allocate_new_literal_expr(*nodes[1].begin());
	const node_index_type d = v.__lookup_global_bool_id(*nodes[2].begin());
	INVARIANT(g && s && d);
	// construct and allocate rule
	const expr_index_type ns = v.allocate_new_not_expr(s);
	const expr_index_type pe =
		v.allocate_new_Nary_expr(entity::PRS::PRS_AND_EXPR_TYPE_ENUM,2);
	v.link_child_expr(pe, g, 0);
	v.link_child_expr(pe, ns, 1);
	v.link_node_to_root_expr(d, pe, false);	// pull-down

	typedef	visitor_type::rule_type	rule_type;
	rule_type& r(v.st_rule_map[pe]);
	r.set_delay(delay_policy<visitor_type::state_type::time_type>::zero);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_MACRO_CLASS(PassP, "passp")

/**
	Produce the rule:
		after 0	~g & s -> d+
	\param params unused.  
	\pre each node group argument contains exactly one node.  
 */
void
PassP::main(visitor_type& v, const param_args_type& params,
		const node_args_type& nodes) {
	const expr_index_type g =
		v.allocate_new_literal_expr(*nodes[0].begin());
	const expr_index_type s =
		v.allocate_new_literal_expr(*nodes[1].begin());
	const node_index_type d = v.__lookup_global_bool_id(*nodes[2].begin());
	INVARIANT(g && s && d);
	// construct and allocate rule
	const expr_index_type ng = v.allocate_new_not_expr(g);
	const expr_index_type pe =
		v.allocate_new_Nary_expr(entity::PRS::PRS_AND_EXPR_TYPE_ENUM,2);
	v.link_child_expr(pe, ng, 0);
	v.link_child_expr(pe, s, 1);
	v.link_node_to_root_expr(d, pe, true);	// pull-up

	typedef	visitor_type::rule_type	rule_type;
	rule_type& r(v.st_rule_map[pe]);
	r.set_delay(delay_policy<visitor_type::state_type::time_type>::zero);
}

#undef	DECLARE_AND_DEFINE_PRSIM_MACRO_CLASS
}	// end namespace prsim_macros

//=============================================================================
/**
	Namespace for prsim-specific implementations of spec directives.  
 */
namespace prsim_spec_directives {

/**
	Convenient macro for declaring prsim SPEC directives.  
 */
#define	DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(class_name, spec_name) \
	DECLARE_SPEC_DIRECTIVE_CLASS(class_name, ExprAlloc)		\
	DEFINE_SPEC_DIRECTIVE_CLASS(class_name, spec_name, 		\
		register_ExprAlloc_spec_class)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(UnAliased, "unaliased")

void
UnAliased::main(visitor_type& v, const param_args_type& params, 
		const node_args_type& nodes) {
	if (!__main(v, nodes).good) {
		cerr << "Error: detected aliased nodes during "
			"processing of \'unaliased\' directive."
			<< endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(Assert, "assert")

void
Assert::main(visitor_type& v, const param_args_type& params, 
		const node_args_type& nodes) {
	// does absolutely nothing
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(SIM_force_exclhi, "mk_exclhi")

/**
	Implementation ripped off of original prsim's parse_excl function.  
	Just appends each node into excl ring list and sets exclhi flag.  
	\param params unused.  
	TODO: is there any way of checking uniqueness of nodes?
		only if State's excl_ring_map_type is a map/set.  
 */
void
SIM_force_exclhi::main(visitor_type& v, const param_args_type& params, 
		const node_args_type& nodes) {
	typedef	node_args_type::const_iterator	const_iterator;
	const_iterator i(nodes.begin()), e(nodes.end());
	for ( ; i!=e; ++i) {
		INVARIANT(i->size() == 1);
		const node_index_type ni =
			v.__lookup_global_bool_id(*i->begin());
		v.state.append_exclhi_ring(ni);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(SIM_force_excllo, "mk_excllo")

/**
	Implementation ripped off of original prsim's parse_excl function.  
	Just appends each node into excl ring list and sets excllo flag.  
	\param params unused.  
	TODO: is there any way of checking uniqueness of nodes?
		only if State's excl_ring_map_type is a map/set.  
 */
void
SIM_force_excllo::main(visitor_type& v, const param_args_type& params, 
		const node_args_type& nodes) {
	typedef	node_args_type::const_iterator	const_iterator;
	const_iterator i(nodes.begin()), e(nodes.end());
	for ( ; i!=e; ++i) {
		INVARIANT(i->size() == 1);
		const node_index_type ni =
			v.__lookup_global_bool_id(*i->begin());
		v.state.append_excllo_ring(ni);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// layout_min_sep -- reserved for SEU prsim... later

#undef	DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS
}	// end namespace prsim_spec_directives

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

