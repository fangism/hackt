/**
	\file "Object/expr/enum_expr.h"
	Base class for enumerated data expressions.  
	NOTE: for revision histry purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the ARTXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	$Id: enum_expr.h,v 1.1.2.1 2005/07/05 01:16:28 fang Exp $
 */

#ifndef	__OBJECT_EXPR_ENUM_EXPR_H__
#define	__OBJECT_EXPR_ENUM_EXPR_H__

#include "Object/expr/data_expr.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Expressions that represent enumerated values.  
	Pretty much limited to enum_nonmeta_instance_references.  
	This may become obsolete once we implement
	enum comparison expressions.  
 */
class enum_expr : public data_expr {
	typedef	data_expr			parent_type;
protected:
	enum_expr() : parent_type() { }
public:
virtual	~enum_expr() { }

};	// end class enum_expr

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_EXPR_ENUM_EXPR_H__

