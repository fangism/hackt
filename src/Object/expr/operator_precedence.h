/**
	\file "Object/expr/operator_precedence.h"
	Enumerations for operator precedence.  
	$Id: operator_precedence.h,v 1.2.12.1 2005/12/11 00:45:29 fang Exp $
 */

#ifndef	__OBJECT_EXPR_OPERATOR_PRECEDENCE_H__
#define	__OBJECT_EXPR_OPERATOR_PRECEDENCE_H__

namespace HAC {
namespace entity {

/**
	NOTE: the values are used to distinguish when parenthesization
	is needed in expression tree dumping.
	If the parent has greater precedence than the child, 
	then the child needs to be printed parenthesized.  
 */
typedef	enum {
	OP_PREC_DEFAULT = 0,			// default value, top-level
	OP_PREC_LOGICAL_OR = 1, 
	OP_PREC_LOGICAL_AND = OP_PREC_LOGICAL_OR+1, 
	OP_PREC_EQUAL = OP_PREC_LOGICAL_AND+1,	// also for NOT_EQUAL
	OP_PREC_COMPARE = OP_PREC_EQUAL+1,	// for <,<=,>,>=
	OP_PREC_SHIFT = OP_PREC_COMPARE+1,	// for <<, >>
	OP_PREC_PLUS = OP_PREC_SHIFT+1,		// also used for -
	OP_PREC_TIMES = OP_PREC_PLUS+1,		// also for *,/,%
	OP_PREC_UNARY = OP_PREC_TIMES+1,	// for -, ~, !
	OP_PREC_LITERAL = OP_PREC_UNARY+1
}	operator_precedence;

}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_EXPR_OPERATOR_PRECEDENCE_H__

