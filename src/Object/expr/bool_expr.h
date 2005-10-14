/**
	\file "Object/expr/bool_expr.h"
	Base class for boolean data expressions.  
	NOTE: for revision histry purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the ARTXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	TODO: future rename this file to nonmeta_expr_base.h
	$Id: bool_expr.h,v 1.2.22.2 2005/10/14 03:30:12 fang Exp $
 */

#ifndef	__OBJECT_EXPR_BOOL_EXPR_H__
#define	__OBJECT_EXPR_BOOL_EXPR_H__

#include "Object/expr/data_expr.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Abstract boolean data type expression.  
 */
class bool_expr : public data_expr {
	typedef	data_expr			parent_type;
protected:
	bool_expr() : parent_type() { }
public:
virtual	~bool_expr() { }

};	// end class bool_expr

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_EXPR_BOOL_EXPR_H__

