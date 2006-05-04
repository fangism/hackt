/**
	\file "sim/prsim/ExprAlloc.h"
	$Id: ExprAlloc.h,v 1.6.6.1 2006/05/04 02:51:40 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXPRALLOC_H__
#define	__HAC_SIM_PRSIM_EXPRALLOC_H__

#include "Object/lang/cflat_context_visitor.h"
#include "sim/prsim/ExprAllocFlags.h"
#include "sim/prsim/State.h"		// for nested typedefs
#include "sim/common.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
class State;
using entity::PRS::footprint_rule;
using entity::PRS::footprint_expr_node;
using entity::PRS::footprint_macro;
using entity::SPEC::footprint_directive;
using entity::cflat_context_visitor;

//=============================================================================
/**
	Visits all PRS expressions and allocates them for use with 
	the prsim simulator.  
 */
class ExprAlloc : public cflat_context_visitor {
public:
	typedef	State					state_type;
	typedef	State::node_type			node_type;
	typedef	State::node_pool_type			node_pool_type;
	typedef	State::expr_type			expr_type;
	typedef	State::expr_pool_type			expr_pool_type;
	typedef	State::graph_node_type			graph_node_type;
	typedef	State::expr_graph_node_pool_type	graph_node_pool_type;
	typedef	State::rule_map_type			rule_map_type;
	typedef	State::rule_type			rule_type;
public:
	state_type&				state;
	node_pool_type&				st_node_pool;
	expr_pool_type&				st_expr_pool;
	graph_node_pool_type&			st_graph_node_pool;
	rule_map_type&				st_rule_map;
protected:
	/// the expression index last returned
	expr_index_type				ret_ex_index;
	ExprAllocFlags				flags;
public:
	explicit
	ExprAlloc(state_type&);

	ExprAlloc(state_type&, const ExprAllocFlags&);

	// default empty destructor

	expr_index_type
	last_expr_index(void) const { return ret_ex_index; }

	State&
	get_state(void) { return state; }

	const State&
	get_state(void) const { return state; }

protected:
	using cflat_visitor::visit;

	void
	visit(const footprint_rule&);

	void
	visit(const footprint_expr_node&);

	void
	visit(const footprint_macro&);

	// what do we do?
	void
	visit(const footprint_directive&);

public:
	// these public functions are really only intended for
	// macro/directive/attribute visitor classes...

	void
	link_node_to_root_expr(const node_index_type, 
		const expr_index_type, const bool dir);

	expr_index_type
	allocate_new_literal_expr(const node_index_type);

	expr_index_type
	allocate_new_not_expr(const expr_index_type);

	expr_index_type
	allocate_new_Nary_expr(const char, const size_t);

	void
	link_child_expr(const expr_index_type p, const expr_index_type c, 
		const size_t o);

private:
	/// private, undefined copy-ctor.
	explicit
	ExprAlloc(const ExprAlloc&);

};	// end class ExprAlloc

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EXPRALLOC_H__

