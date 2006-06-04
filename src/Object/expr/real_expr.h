/**
	\file "Object/expr/real_expr.h"
	Base class for boolean data expressions.  
	TODO: future rename this file to nonmeta_expr_base.h
	$Id: real_expr.h,v 1.2.26.2 2006/06/04 22:26:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_REAL_EXPR_H__
#define	__HAC_OBJECT_EXPR_REAL_EXPR_H__

#include "Object/expr/data_expr.h"
#include "Object/devel_switches.h"
#if COW_UNROLL_DATA_EXPR
#include "util/memory/pointer_classes_fwd.h"
#endif

namespace HAC {
namespace entity {
#if COW_UNROLL_DATA_EXPR
class unroll_context;
using util::memory::count_ptr;
#endif

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

#if COW_UNROLL_DATA_EXPR
#define	UNROLL_RESOLVE_COPY_REAL_PROTO					\
	count_ptr<const real_expr>					\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const real_expr>&) const

virtual	UNROLL_RESOLVE_COPY_REAL_PROTO = 0;
#endif

};	// end class real_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_REAL_EXPR_H__

