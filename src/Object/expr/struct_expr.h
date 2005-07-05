/**
	\file "Object/expr/struct_expr.h"
	Base class for structured data expressions.  
	NOTE: for revision histry purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the ARTXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	$Id: struct_expr.h,v 1.1.2.1 2005/07/05 01:16:33 fang Exp $
 */

#ifndef	__OBJECT_EXPR_STRUCT_EXPR_H__
#define	__OBJECT_EXPR_STRUCT_EXPR_H__

#include "Object/expr/data_expr.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Expressions that represent structured values.  
	Pretty much limited to struct_nonmeta_instance_references.  
 */
class struct_expr : public data_expr {
	typedef	data_expr			parent_type;
protected:
	struct_expr() : parent_type() { }
public:
virtual	~struct_expr() { }

};	// end class struct_expr

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_EXPR_STRUCT_EXPR_H__

