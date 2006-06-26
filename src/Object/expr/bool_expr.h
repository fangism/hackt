/**
	\file "Object/expr/bool_expr.h"
	Base class for boolean data expressions.  
	NOTE: for revision histry purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	$Id: bool_expr.h,v 1.6 2006/06/26 01:45:55 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_BOOL_EXPR_H__
#define	__HAC_OBJECT_EXPR_BOOL_EXPR_H__

#include "Object/expr/data_expr.h"
#include "util/memory/pointer_classes_fwd.h"

namespace HAC {
namespace entity {
class unroll_context;
using util::memory::count_ptr;

//=============================================================================
/**
	Abstract boolean data type expression.  
 */
class bool_expr : public data_expr {
	typedef	bool_expr			this_type;
	typedef	data_expr			parent_type;
protected:
	bool_expr() : parent_type() { }
public:
virtual	~bool_expr() { }

#define	UNROLL_RESOLVE_COPY_BOOL_PROTO					\
	count_ptr<const bool_expr>					\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const bool_expr>&) const

virtual	UNROLL_RESOLVE_COPY_BOOL_PROTO = 0;

	UNROLL_RESOLVE_COPY_DATA_PROTO;

};	// end class bool_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_BOOL_EXPR_H__

