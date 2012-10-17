/**
	\file "Object/expr/real_expr.hh"
	Base class for boolean data expressions.  
	TODO: future rename this file to nonmeta_expr_base.hh
	$Id: real_expr.hh,v 1.5 2007/02/08 02:11:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_REAL_EXPR_H__
#define	__HAC_OBJECT_EXPR_REAL_EXPR_H__

#include "Object/expr/data_expr.hh"
#include "util/memory/pointer_classes_fwd.hh"

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
public:
	typedef	preal_const			const_expr_type;
protected:
	real_expr() : parent_type() { }
public:
virtual	~real_expr() { }

#define	UNROLL_RESOLVE_COPY_REAL_PROTO					\
	count_ptr<const real_expr>					\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const real_expr>&) const

virtual	UNROLL_RESOLVE_COPY_REAL_PROTO = 0;

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
	EVALUATE_WRITE_PROTO;

protected:
	UNROLL_RESOLVE_COPY_DATA_PROTO;

};	// end class real_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_REAL_EXPR_H__

