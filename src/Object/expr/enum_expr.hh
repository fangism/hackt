/**
	\file "Object/expr/enum_expr.hh"
	Base class for enumerated data expressions.  
	NOTE: for revision history purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	$Id: enum_expr.hh,v 1.6 2007/01/21 05:58:48 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_ENUM_EXPR_H__
#define	__HAC_OBJECT_EXPR_ENUM_EXPR_H__

#include "Object/expr/data_expr.hh"
#include "util/memory/pointer_classes_fwd.hh"

namespace HAC {
namespace entity {
class unroll_context;
class pint_const;
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

#define	NONMETA_RESOLVE_COPY_ENUM_PROTO					\
	count_ptr<const const_param>					\
	nonmeta_resolve_copy(const nonmeta_context_base&,		\
		const count_ptr<const enum_expr>&) const

#define	NONMETA_RESOLVE_RVALUE_ENUM_PROTO				\
	count_ptr<const pint_const>					\
	__nonmeta_resolve_rvalue(const nonmeta_context_base&,		\
		const count_ptr<const enum_expr>&) const

//	TODO: define what enums should resolve to (pints?)
// virtual	NONMETA_RESOLVE_COPY_ENUM_PROTO = 0;
virtual	NONMETA_RESOLVE_RVALUE_ENUM_PROTO = 0;

	EVALUATE_WRITE_PROTO;

protected:
	NONMETA_RESOLVE_COPY_DATA_PROTO;
	UNROLL_RESOLVE_COPY_DATA_PROTO;

};	// end class enum_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_ENUM_EXPR_H__

