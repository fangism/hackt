/**
	\file "Object/expr/struct_expr.hh"
	Base class for structured data expressions.  
	NOTE: for revision history purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	$Id: struct_expr.hh,v 1.5 2006/06/26 01:46:10 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_STRUCT_EXPR_H__
#define	__HAC_OBJECT_EXPR_STRUCT_EXPR_H__

#include "Object/expr/data_expr.hh"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Expressions that represent structured values.  
	Pretty much limited to struct_nonmeta_instance_references.  
 */
class struct_expr : public data_expr {
	typedef	struct_expr			this_type;
	typedef	data_expr			parent_type;
protected:
	struct_expr() : parent_type() { }
public:
virtual	~struct_expr() { }

protected:
	/**
		Dummy function, meant to satisfy a name interface only.  
	 */
#define	UNROLL_RESOLVE_COPY_STRUCT_PROTO				\
	count_ptr<const struct_expr>					\
	unroll_resolve_copy(const unroll_context&,			\
		const count_ptr<const struct_expr>&) const

virtual	UNROLL_RESOLVE_COPY_STRUCT_PROTO = 0;

	UNROLL_RESOLVE_COPY_DATA_PROTO;
};	// end class struct_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_STRUCT_EXPR_H__

