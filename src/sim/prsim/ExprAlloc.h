/**
	\file "sim/prsim/ExprAlloc.h"
	$Id: ExprAlloc.h,v 1.5.2.2 2006/04/18 05:57:23 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXPRALLOC_H__
#define	__HAC_SIM_PRSIM_EXPRALLOC_H__

#include <stack>
#include "Object/lang/cflat_context_visitor.h"
#include "sim/prsim/State.h"		// for nested typedefs
#include "sim/common.h"
#include "util/member_saver.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
class State;
class Node;
class Expr;
class ExprGraphNode;
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
protected:
	typedef	std::stack<expr_index_type>	build_stack_type;
	typedef	State				state_type;
	typedef	State::expr_type		expr_type;
	typedef	State::graph_node_type		graph_node_type;
	typedef	State::rule_map_type		rule_map_type;
	typedef	State::rule_type		rule_type;
protected:
	state_type&				state;
	/// the expression index last returned
	expr_index_type				ret_ex_index;
public:
	explicit
	ExprAlloc(State&);

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

protected:
	void
	link_node_to_root_expr(Node&, const node_index_type, 
		Expr&, ExprGraphNode&, const expr_index_type, const bool dir);

};	// end class ExprAlloc

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EXPRALLOC_H__

