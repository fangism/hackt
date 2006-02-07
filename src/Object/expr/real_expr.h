/**
	\file "Object/expr/real_expr.h"
	Base class for boolean data expressions.  
	TODO: future rename this file to nonmeta_expr_base.h
	$Id: real_expr.h,v 1.1.2.1 2006/02/07 02:57:56 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_REAL_EXPR_H__
#define	__HAC_OBJECT_EXPR_REAL_EXPR_H__

#include "Object/expr/data_expr.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Abstract boolean data type expression.  
 */
class real_expr : public data_expr {
	typedef	data_expr			parent_type;
protected:
	real_expr() : parent_type() { }
public:
virtual	~real_expr() { }

};	// end class real_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_REAL_EXPR_H__

