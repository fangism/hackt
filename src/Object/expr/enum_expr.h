/**
	\file "Object/expr/enum_expr.h"
	Base class for enumerated data expressions.  
	NOTE: for revision history purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	$Id: enum_expr.h,v 1.5 2006/06/26 01:45:59 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_ENUM_EXPR_H__
#define	__HAC_OBJECT_EXPR_ENUM_EXPR_H__

#include "Object/expr/data_expr.h"
#include "util/memory/pointer_classes_fwd.h"

namespace HAC {
namespace entity {
class unroll_context;
using util::memory::count_ptr;

//=============================================================================
/**
	Expressions that represent enumerated values.  
	Pretty much limited to enum_nonmeta_instance_references.  
	This may become obsolete once we implement
	enum comparison expressions.  
 */
class enum_expr : public data_expr {
	typedef	enum_expr			this_type;
	typedef	data_expr			parent_type;
protected:
	enum_expr() : parent_type() { }
public:
virtual	~enum_expr() { }

#define	UNROLL_RESOLVE_COPY_ENUM_PROTO					\
	count_ptr<const enum_expr>					\
	unroll_resolve_copy(const unroll_context&,			\
		const count_ptr<const enum_expr>&) const

virtual	UNROLL_RESOLVE_COPY_ENUM_PROTO = 0;
protected:
	UNROLL_RESOLVE_COPY_DATA_PROTO;

};	// end class enum_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_ENUM_EXPR_H__

