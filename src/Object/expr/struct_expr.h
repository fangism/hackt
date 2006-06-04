/**
	\file "Object/expr/struct_expr.h"
	Base class for structured data expressions.  
	NOTE: for revision history purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	$Id: struct_expr.h,v 1.4.40.1 2006/06/04 05:59:22 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_STRUCT_EXPR_H__
#define	__HAC_OBJECT_EXPR_STRUCT_EXPR_H__

#include "Object/expr/data_expr.h"

namespace HAC {
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

protected:
	/**
		Dummy function, meant to satisfy a name interface only.  
	 */
	void
	unroll_resolve_copy(void) const;

};	// end class struct_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_STRUCT_EXPR_H__

