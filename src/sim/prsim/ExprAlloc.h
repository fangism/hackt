/**
	\file "sim/prsim/ExprAlloc.h"
	$Id: ExprAlloc.h,v 1.3 2006/01/25 20:26:05 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXPRALLOC_H__
#define	__HAC_SIM_PRSIM_EXPRALLOC_H__

#include <stack>
#include "Object/lang/cflat_visitor.h"
#include "Object/cflat_context.h"
#include "sim/common.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
class State;
class Node;
class Expr;
class ExprGraphNode;
using entity::PRS::cflat_visitor;
using entity::PRS::footprint_rule;
using entity::PRS::footprint_expr_node;
using entity::PRS::footprint_macro;
using entity::cflat_context;
//=============================================================================

/**
	Visits all PRS expressions and allocates them.  
 */
class ExprAlloc : public cflat_visitor, public cflat_context {
protected:
	typedef	std::stack<expr_index_type>	build_stack_type;
	State&					state;
	/// the expression index last returned
	expr_index_type				ret_ex_index;
public:
	explicit
	ExprAlloc(State&);

	// default empty destructor

	using cflat_visitor::visit;

	void
	visit(const footprint_rule&);

	void
	visit(const footprint_expr_node&);

	void
	visit(const footprint_macro&);

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

