/**
	\file "Object/expr/operator_info.h"
	Some expression specific implementation details.  
	$Id: operator_info.h,v 1.2 2005/10/25 20:51:53 fang Exp $
 */

#ifndef __OBJECT_EXPR_OPERATOR_INFO_H__
#define __OBJECT_EXPR_OPERATOR_INFO_H__

namespace ART {
namespace entity {
/**
	Namespace for expression-specific implementations.  
 */
namespace expr_detail {
//=============================================================================
/**
	operator information.  
 */
struct op_info {
	/// the character to print representing this operator
	char				op;
	/// the precedence, see "Object/expr/operator_precedence.h"
	char				prec;
	op_info() { }
	op_info(const char o, const char p) : op(o), prec(p) { }
};	// end struct op_info

//=============================================================================
}	// end namespace expr_detail
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_EXPR_OPERATOR_INFO_H__

