/**
	\file "sim/prsim/ExprAlloc.cc"
	$Id: ExprAlloc.cc,v 1.3 2006/01/25 20:26:05 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "sim/prsim/ExprAlloc.h"
#include "sim/prsim/Expr.h"
#include "sim/prsim/State.h"
#include "Object/lang/PRS_enum.h"
#include "Object/lang/PRS_footprint_rule.h"
#include "Object/lang/PRS_footprint_expr.h"
#include "Object/traits/classification_tags.h"
#include "Object/global_entry.h"
#include "util/offset_array.h"
#include "util/stacktrace.h"
#include "common/TODO.h"

#if	ENABLE_STACKTRACE
#include <iostream>
#endif

namespace HAC {
namespace SIM {
namespace PRSIM {
using entity::bool_tag;
#if	ENABLE_STACKTRACE
#include "util/using_ostream.h"
#endif
//=============================================================================
// class ExprAlloc method definitions

/**
	NOTE: 0 is an invalid index to the State's expr_pool.  
 */
ExprAlloc::ExprAlloc(State& _s) : state(_s), ret_ex_index(INVALID_EXPR_INDEX) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Update node fanin, and keep it consistent.  
	\pre state's node_array is already allocated.  
 */
void
ExprAlloc::visit(const footprint_rule& r) {
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
	Expr& e(st_expr_pool[top_ex_index]);
	ExprGraphNode& g(st_graph_node_pool[top_ex_index]);

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
#if 0
			STACKTRACE_INDENT << "FOO" << endl;
			st_node_pool[ni].dump_struct(STACKTRACE_INDENT) << endl;
#endif
			st_node_pool[ni].push_back_fanout(st_expr_pool.size());
			st_expr_pool.push_back(Expr(Expr::EXPR_NODE,1));
			st_graph_node_pool.push_back(ExprGraphNode());
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
			st_expr_pool.push_back(Expr(Expr::EXPR_NOT,1));
			st_graph_node_pool.push_back(ExprGraphNode());
			// now link parent to only-child
			const expr_index_type last = st_expr_pool.size() -1;
			st_expr_pool[sub_ex_index].parent = last;
			ExprGraphNode& child(st_graph_node_pool[sub_ex_index]);
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
			st_expr_pool.push_back(Expr(
				(type == entity::PRS::PRS_AND_EXPR_TYPE_ENUM ?
				Expr::EXPR_AND : Expr::EXPR_OR), sz));
			st_graph_node_pool.push_back(ExprGraphNode());
			const expr_index_type last = st_expr_pool.size() -1;
			size_t i = 1;
			for ( ; i<=sz; i++) {
				// reminder: e is 1-indexed while 
				// ExprGraphNode::children is 0-indexed
				(*expr_pool)[e[i]].accept(*this);
				const size_t sub_ex_index = ret_ex_index;
				// now link parent to each child
				st_expr_pool[sub_ex_index].parent = last;
				ExprGraphNode&
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
ExprAlloc::visit(const footprint_macro&) {
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
		Expr& pe(st_expr_pool[dir_index]);
		ExprGraphNode& pg(st_graph_node_pool[dir_index]);
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
			st_expr_pool.push_back(Expr(Expr::EXPR_OR, 2));
			st_graph_node_pool.push_back(ExprGraphNode());
			Expr& new_ex(st_expr_pool.back());
			ExprGraphNode& new_g(st_graph_node_pool.back());
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
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

