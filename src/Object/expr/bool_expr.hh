/**
	\file "Object/expr/bool_expr.hh"
	Base class for boolean data expressions.  
	NOTE: for revision histry purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	$Id: bool_expr.hh,v 1.8 2007/02/08 02:11:01 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_BOOL_EXPR_H__
#define	__HAC_OBJECT_EXPR_BOOL_EXPR_H__

#include "Object/expr/data_expr.hh"
#include "util/memory/pointer_classes_fwd.hh"

namespace HAC {
namespace entity {
class pbool_const;
class const_param;

//=============================================================================
/**
	Abstract boolean data type expression.  
 */
class bool_expr : public data_expr {
	typedef	bool_expr			this_type;
	typedef	data_expr			parent_type;
public:
	typedef	pbool_const			const_expr_type;
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

#define	NONMETA_RESOLVE_COPY_BOOL_PROTO					\
	count_ptr<const const_param>					\
	nonmeta_resolve_copy(const nonmeta_context_base&,		\
		const count_ptr<const bool_expr>&) const

#define	NONMETA_RESOLVE_RVALUE_BOOL_PROTO				\
	count_ptr<const pbool_const>					\
	__nonmeta_resolve_rvalue(const nonmeta_context_base&,		\
		const count_ptr<const bool_expr>&) const

virtual	NONMETA_RESOLVE_RVALUE_BOOL_PROTO = 0;
virtual	NONMETA_RESOLVE_COPY_BOOL_PROTO = 0;

	NONMETA_RESOLVE_COPY_DATA_PROTO;
	EVALUATE_WRITE_PROTO;

virtual	EXPR_ACCEPT_VISITOR_PROTO = 0;

};	// end class bool_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_BOOL_EXPR_H__

