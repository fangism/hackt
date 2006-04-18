/**
	\file "sim/prsim/ExprAlloc.cc"
	$Id: ExprAlloc.cc,v 1.6.2.2 2006/04/18 05:57:23 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include "sim/prsim/ExprAlloc.h"
#include "sim/prsim/Expr.h"
#include "sim/prsim/State.h"
#include "Object/lang/PRS_enum.h"
#include "Object/lang/PRS_footprint_rule.h"
#include "Object/lang/PRS_footprint_expr.h"
#include "Object/lang/PRS_attribute_common.h"
#include "Object/lang/PRS_attribute_registry.h"
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
	const mapped_type& n
		__ATTRIBUTE_UNUSED_CTOR__((__init_registry[k]));
	INVARIANT(n);
	return ExprAlloc_attribute_registry.size();
}

//=============================================================================
// class ExprAlloc method definitions

/**
	NOTE: 0 is an invalid index to the State's expr_pool.  
 */
ExprAlloc::ExprAlloc(State& _s) :
		cflat_context_visitor(), 
		state(_s),
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
	const size_t ni = fpf->get_frame_map<bool_tag>()[r.output_index-1];

	// local convenience references
	State::node_pool_type& st_node_pool(state.node_pool);
	State::expr_pool_type& st_expr_pool(state.expr_pool);
	State::expr_graph_node_pool_type&
		st_graph_node_pool(state.expr_graph_node_pool);
	Node& output_node(st_node_pool[ni]);
	// now link root expression to node
	expr_type& e(st_expr_pool[top_ex_index]);
	graph_node_type& g(st_graph_node_pool[top_ex_index]);

	// ignored: state.expr_graph_node_pool[top_ex_index].offset
	// not computing node fanin?  this would be the place to do it...
	// can always compute it (cacheable) offline
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "expr " << top_ex_index << " pulls node " <<
		ni << (r.dir ? " up" : " down") << endl;
#endif
	link_node_to_root_expr(output_node, ni, e, g, top_ex_index, r.dir);
#if ENABLE_STACKTRACE
	state.dump_struct(cerr) << endl;
#endif
	INVARIANT(top_ex_index == ret_ex_index);	// sanity check
}
{
	// first, unconditionally create a rule entry in the state's rule
	// map for every top-level (root) expression that affects a node.  
	rule_map_type& rmap(state.get_rule_map());
	rule_type& rule __ATTRIBUTE_UNUSED_CTOR__((rmap[ret_ex_index]));
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
	// local abbreviations and aliases
	State::node_pool_type& st_node_pool(state.node_pool);
	State::expr_pool_type& st_expr_pool(state.expr_pool);
	State::expr_graph_node_pool_type&
		st_graph_node_pool(state.expr_graph_node_pool);
	// NOTE: 1-indexed
	switch (type) {
		// enumerations from "Object/lang/PRS_enum.h"
		case entity::PRS::PRS_LITERAL_TYPE_ENUM: {
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT << "literal" << endl;
#endif
			// leaf node
			INVARIANT(sz == 1);
			const node_index_type ni =
				fpf->get_frame_map<bool_tag>()[e.only()-1];
			// NOTE: Expr's parent and ExprGraphNode's offset
			// fields are not set until returned to caller!
			st_node_pool[ni].push_back_fanout(st_expr_pool.size());
			st_expr_pool.push_back(
				expr_type(expr_type::EXPR_NODE,1));
			st_graph_node_pool.push_back(graph_node_type());
			st_graph_node_pool.back().push_back_node(ni);
			// literal graph node has no children
			ret_ex_index = st_expr_pool.size() -1;
			break;
		}
		case entity::PRS::PRS_NOT_EXPR_TYPE_ENUM: {
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT << "not" << endl;
#endif
			INVARIANT(sz == 1);
			(*expr_pool)[e.only()].accept(*this);
			const size_t sub_ex_index = ret_ex_index;
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT << "sub_ex_index = " <<
				sub_ex_index << endl;
#endif
			st_expr_pool.push_back(
				expr_type(expr_type::EXPR_NOT,1));
			st_graph_node_pool.push_back(graph_node_type());
			// now link parent to only-child
			const expr_index_type last = st_expr_pool.size() -1;
			st_expr_pool[sub_ex_index].parent = last;
			graph_node_type&
				child(st_graph_node_pool[sub_ex_index]);
			child.offset = 0;
			st_graph_node_pool.back().push_back_expr(sub_ex_index);
			ret_ex_index = st_expr_pool.size() -1;
			break;
		}
		case entity::PRS::PRS_AND_EXPR_TYPE_ENUM:
			// yes, fall-through
		case entity::PRS::PRS_OR_EXPR_TYPE_ENUM: {
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT << "or/and" << endl;
#endif
			INVARIANT(sz);
			st_expr_pool.push_back(expr_type(
				(type == entity::PRS::PRS_AND_EXPR_TYPE_ENUM ?
				expr_type::EXPR_AND : expr_type::EXPR_OR), sz));
			st_graph_node_pool.push_back(graph_node_type());
			const expr_index_type last = st_expr_pool.size() -1;
			size_t i = 1;
			for ( ; i<=sz; i++) {
				// reminder: e is 1-indexed while 
				// ExprGraphNode::children is 0-indexed
				(*expr_pool)[e[i]].accept(*this);
				const size_t sub_ex_index = ret_ex_index;
				// now link parent to each child
				st_expr_pool[sub_ex_index].parent = last;
				graph_node_type&
					child(st_graph_node_pool[sub_ex_index]);
				child.offset = i-1;
				st_graph_node_pool[last]
					.push_back_expr(sub_ex_index);
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
	FINISH_ME(Fang);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprAlloc::visit(const footprint_directive& d) {
	FINISH_ME(Fang);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Utility routine for connecting an output node to
	its root pull-up or pull-down expression.  
	This automatically takes care of OR-combination by allocating
	new root expressions when necessary.  
 */
void
ExprAlloc::link_node_to_root_expr(Node& output, const node_index_type ni,
		Expr& ne, ExprGraphNode& ng, 
		const expr_index_type top_ex_index, const bool dir) {
	STACKTRACE("ExprAlloc::link_node_to_root_expr(...)");
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
		State::expr_pool_type& st_expr_pool(state.expr_pool);
		State::expr_graph_node_pool_type&
			st_graph_node_pool(state.expr_graph_node_pool);
		expr_type& pe(st_expr_pool[dir_index]);
		graph_node_type& pg(st_graph_node_pool[dir_index]);
		// see if either previous pull-up expr is OR-type already
		// may also work with NAND!
		if (pe.is_or()) {
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
		} else if (ne.is_or()) {
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
			const expr_index_type root_ex_id = st_expr_pool.size();
			// TODO: check for NAND, which is OR-like
			st_expr_pool.push_back(
				expr_type(expr_type::EXPR_OR, 2));
			st_graph_node_pool.push_back(graph_node_type());
			expr_type& new_ex(st_expr_pool.back());
			graph_node_type& new_g(st_graph_node_pool.back());
			new_ex.pull(ni, dir);
			// link sub-expressions to new root expression
			new_g.push_back_expr(dir_index);
			new_g.push_back_expr(top_ex_index);
			pg.offset = 0;
			ng.offset = 1;
			pe.set_parent_expr(root_ex_id);
			ne.set_parent_expr(root_ex_id);
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
	Macro for declaring attribute classes.  
	Here, the vistor_type is prsim's ExprAlloc.
	NOTE: these classes should have hidden visibility.  
 */
#define DECLARE_PRSIM_RULE_ATTRIBUTE_CLASS(class_name, att_name)	\
struct class_name : public entity::PRS::rule_attributes::class_name {	\
	typedef	entity::PRS::rule_attributes::class_name parent_type;	\
	typedef	ExprAlloc_attribute_definition_entry	entry_type;	\
	typedef	entry_type::visitor_type		visitor_type;	\
	typedef entry_type::values_type			values_type;	\
	typedef values_type::value_type			value_type;	\
public:									\
	static const char				name[];		\
	static void main(visitor_type&, const values_type&);		\
	static good_bool check_vals(const values_type&);		\
private:								\
	static const size_t				id;		\
};									\
const char class_name::name[] = att_name;				\
good_bool								\
class_name::check_vals(const values_type& v) {				\
	return parent_type::__check_vals(name, v);			\
}									\
const size_t class_name::id = register_ExprAlloc_attribute_class<class_name>();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Local namespace for prsim rule attributes.  
 */
namespace prsim_rule_attributes {

DECLARE_PRSIM_RULE_ATTRIBUTE_CLASS(After, "after")

/**
	Delay values real or floating or both?
 */
void
After::main(visitor_type& v, const values_type& a) {
	State& s(v.get_state());
	typedef	State::rule_type	rule_type;
	typedef	State::rule_map_type	rule_map_type;
	rule_map_type& rmap(s.get_rule_map());
	rule_type& r(rmap[v.last_expr_index()]);
	const values_type::value_type& d(a.front());
	// assert type cast, b/c already checked
	r.set_delay(d.is_a<const pint_const>()->static_constant_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_PRSIM_RULE_ATTRIBUTE_CLASS(Weak, "weak")

/**
	Sets or clears weak flag on a rule.  
 */
void
Weak::main(visitor_type& v, const values_type& a) {
	State& s(v.get_state());
	typedef	State::rule_type	rule_type;
	typedef	State::rule_map_type	rule_map_type;
	rule_map_type& rmap(s.get_rule_map());
	rule_type& r(rmap[v.last_expr_index()]);
	const values_type::value_type& w(a.front());
	if (w.is_a<const pint_const>()->static_constant_value())
		r.set_weak();
	else	r.clear_weak();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace prsim_rule_attributes

#undef	DECLARE_PRSIM_RULE_ATTRIBUTE_CLASS

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

