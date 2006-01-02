/**
	\file "sim/prsim/ExprAlloc.h"
	$Id: ExprAlloc.cc,v 1.1.2.1 2006/01/02 23:13:35 fang Exp $
 */

#include "sim/prsim/ExprAlloc.h"
#include "sim/prsim/Expr.h"
#include "sim/prsim/State.h"
#include "Object/lang/PRS_enum.h"
#include "Object/lang/PRS_footprint_rule.h"
#include "Object/lang/PRS_footprint_expr.h"
#include "Object/traits/classification_tags.h"
#include "Object/global_entry.h"
#include "util/offset_array.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using entity::bool_tag;
//=============================================================================
// class ExprAlloc method definitions

/**
	NOTE: 0 is an invalid index to the State's expr_pool.  
 */
ExprAlloc::ExprAlloc(State& _s) : state(_s), ret_ex_index(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Update node fanin, and keep it consistent.  
	\pre state's node_array is already allocated.  
 */
void
ExprAlloc::visit(const footprint_rule& r) {
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
	switch (r.dir) {
	case '+': 
		link_node_to_root_expr(output_node, ni, e, g, 
			top_ex_index, true);	// up
		break;
	case '-':
		link_node_to_root_expr(output_node, ni, e, g, 
			top_ex_index, false);	// down
		break;
	default:
		THROW_EXIT;
		break;
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
	const size_t sz = e.size();
	const char type = e.get_type();
	// local abbreviations and aliases
	State::node_pool_type& st_node_pool(state.node_pool);
	State::expr_pool_type& st_expr_pool(state.expr_pool);
	State::expr_graph_node_pool_type&
		st_graph_node_pool(state.expr_graph_node_pool);
	// NOTE: 0-indexed
	const size_t ex_id = st_expr_pool.size();
	switch (type) {
		// enumerations from "Object/lang/PRS_enum.h"
		case entity::PRS::PRS_LITERAL_TYPE_ENUM: {
			// leaf node
			INVARIANT(sz == 1);
			const node_index_type ni =
				fpf->get_frame_map<bool_tag>()[e.only()-1];
			// NOTE: Expr's parent and ExprGraphNode's offset
			// fields are not set until returned to caller!
			st_node_pool[ni].push_back_fanout(ex_id);
			st_expr_pool.push_back(Expr(Expr::EXPR_NODE,1));
			st_graph_node_pool.push_back(ExprGraphNode());
			st_graph_node_pool.back().push_back_node(ni);
			// literal graph node has no children
			break;
		}
		case entity::PRS::PRS_NOT_EXPR_TYPE_ENUM: {
			INVARIANT(sz == 1);
			(*expr_pool)[e.only()].accept(*this);
			const size_t sub_ex_index = ret_ex_index;
			st_expr_pool.push_back(Expr(Expr::EXPR_NOT,1));
			st_graph_node_pool.push_back(ExprGraphNode());
			// now link parent to only-child
			st_expr_pool[sub_ex_index].parent = ex_id;
			ExprGraphNode& child(st_graph_node_pool[sub_ex_index]);
			child.offset = 0;
			child.push_back_expr(sub_ex_index);
			break;
		}
		case entity::PRS::PRS_AND_EXPR_TYPE_ENUM:
			// yes, fall-through
		case entity::PRS::PRS_OR_EXPR_TYPE_ENUM: {
			INVARIANT(sz);
			st_expr_pool.push_back(Expr(
				(type == entity::PRS::PRS_AND_EXPR_TYPE_ENUM ?
				Expr::EXPR_AND : Expr::EXPR_OR), sz));
			st_graph_node_pool.push_back(ExprGraphNode());
			size_t i = 1;
			for ( ; i<=sz; i++) {
				// reminder: e is 1-indexed while 
				// ExprGraphNode::children is 0-indexed
				(*expr_pool)[e[i]].accept(*this);
				const size_t sub_ex_index = ret_ex_index;
				// now link parent to each child
				st_expr_pool[sub_ex_index].parent = ex_id;
				ExprGraphNode&
					child(st_graph_node_pool[sub_ex_index]);
				child.offset = i-1;
				child.push_back_expr(sub_ex_index);
			}
			break;
		}
		default:
			THROW_EXIT;
			break;
	}	// end switch
	// 'return' the index of the expr just allocated
	ret_ex_index = ex_id;
}	// end method visit(const footprint_expr_node&)

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
	// prepare to take OR-combination?
	// or can we get away with multiple pull-up/dn roots?
	// or cheat! short-cut to root during operation.  
	// i.e. bypass OR-combination super-expression to root...
	// super-expression will be referenced in fanin
	// for consistent structure...
	// be careful to keep root expr consistent
	expr_index_type&
		dir_index(dir ? output.pull_up_index : output.pull_dn_index);
	if (dir_index) {
		// already set, need OR-combination
		State::expr_pool_type& st_expr_pool(state.expr_pool);
		State::expr_graph_node_pool_type&
			st_graph_node_pool(state.expr_graph_node_pool);
		Expr& pe(st_expr_pool[dir_index]);
		ExprGraphNode& pg(st_graph_node_pool[dir_index]);
		// see if either previous pull-up expr is OR-type already
		// may also work with NAND!
		if (pe.is_or()) {
			// then simply extend the previous expr's children
			INVARIANT(dir == pe.type & Expr::EXPR_DIR);
			pg.push_back_expr(top_ex_index);
			++pe.size;
			// pe.parent unchanged (same node)
			// node's dir_index unchanged
			// NOTE: or short-cut to output node?
			ne.set_parent_expr(dir_index);
		} else if (ne.is_or()) {
			ng.push_back_expr(dir_index);
			++ne.size;
			dir_index = top_ex_index;
			ne.pull(ni, dir);
			pe.set_parent_expr(top_ex_index);
		} else {
			// then need to allocate new root-expression
			const size_t root_ex_id = st_expr_pool.size();
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
		}
	} else {
		// easy: not already set, just link the new root expr.  
		dir_index = top_ex_index;
		ne.pull(ni, dir);
	}
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

