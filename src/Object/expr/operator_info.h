/**
	\file "Object/expr/operator_info.h"
	Some expression specific implementation details.  
	$Id: operator_info.h,v 1.4.82.1 2007/02/07 04:51:57 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_OPERATOR_INFO_H__
#define __HAC_OBJECT_EXPR_OPERATOR_INFO_H__

namespace HAC {
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

	/**
		Implicit conversion to char for convenience with
		using different operator reverse_maps.  
	 */
	operator char () const { return op; }
};	// end struct op_info

//=============================================================================
}	// end namespace expr_detail
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_OPERATOR_INFO_H__

