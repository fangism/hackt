/**
	\file "Object/expr/real_expr.h"
	Base class for boolean data expressions.  
	TODO: future rename this file to nonmeta_expr_base.h
	$Id: real_expr.h,v 1.3.32.1 2006/12/25 03:27:47 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_REAL_EXPR_H__
#define	__HAC_OBJECT_EXPR_REAL_EXPR_H__

#include "Object/expr/data_expr.h"
#include "util/memory/pointer_classes_fwd.h"

namespace HAC {
namespace entity {
class preal_const;

//=============================================================================
/**
	Abstract boolean data type expression.  
 */
class real_expr : public data_expr {
	typedef	real_expr			this_type;
	typedef	data_expr			parent_type;
protected:
	real_expr() : parent_type() { }
public:
virtual	~real_expr() { }

#define	UNROLL_RESOLVE_COPY_REAL_PROTO					\
	count_ptr<const real_expr>					\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const real_expr>&) const

virtual	UNROLL_RESOLVE_COPY_REAL_PROTO = 0;

#if USE_NONMETA_RESOLVE
#define	NONMETA_RESOLVE_COPY_REAL_PROTO					\
	count_ptr<const const_param>					\
	nonmeta_resolve_copy(const nonmeta_context_base&,		\
		const count_ptr<const real_expr>&) const

#define	NONMETA_RESOLVE_RVALUE_REAL_PROTO				\
	count_ptr<const preal_const>					\
	__nonmeta_resolve_rvalue(const nonmeta_context_base&,		\
		const count_ptr<const real_expr>&) const

virtual	NONMETA_RESOLVE_RVALUE_REAL_PROTO = 0;
virtual	NONMETA_RESOLVE_COPY_REAL_PROTO = 0;

	NONMETA_RESOLVE_COPY_DATA_PROTO;
#endif

protected:
	UNROLL_RESOLVE_COPY_DATA_PROTO;

};	// end class real_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_REAL_EXPR_H__

