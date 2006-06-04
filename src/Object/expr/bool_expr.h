/**
	\file "Object/expr/bool_expr.h"
	Base class for boolean data expressions.  
	NOTE: for revision histry purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	$Id: bool_expr.h,v 1.5.40.2 2006/06/04 22:26:10 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_BOOL_EXPR_H__
#define	__HAC_OBJECT_EXPR_BOOL_EXPR_H__

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
class bool_expr : public data_expr {
	typedef	data_expr			parent_type;
protected:
	bool_expr() : parent_type() { }
public:
virtual	~bool_expr() { }

#if	COW_UNROLL_DATA_EXPR
#define	UNROLL_RESOLVE_COPY_BOOL_PROTO					\
	count_ptr<const bool_expr>					\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const bool_expr>&) const

virtual	UNROLL_RESOLVE_COPY_BOOL_PROTO = 0;
#endif

};	// end class bool_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_BOOL_EXPR_H__

