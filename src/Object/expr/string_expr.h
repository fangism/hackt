/**
	\file "Object/expr/string_expr.h"
	$Id: string_expr.h,v 1.3 2010/09/21 00:18:21 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_STRING_EXPR_H__
#define	__HAC_OBJECT_EXPR_STRING_EXPR_H__

#include "Object/expr/data_expr.h"
#include "util/memory/pointer_classes_fwd.h"

namespace HAC {
namespace entity {
using std::string;
using std::istream;
using util::persistent_object_manager;
class pstring_const;
class const_param;

//=============================================================================
/**
	For now the only strings supported in the nonmeta languages
	are constant literal strings.  
	Thus, they are only used as arguments to functions, 
	since string lvalues don't exist... yet.  
 */
class string_expr : public data_expr {
	typedef	string_expr			this_type;
	typedef	data_expr			parent_type;
public:
	typedef	pstring_const			const_expr_type;
protected:
	string_expr() { }
public:
virtual	~string_expr() { }

#define	UNROLL_RESOLVE_COPY_STRING_PROTO				\
	count_ptr<const string_expr>					\
	unroll_resolve_copy(const unroll_context&,			\
		const count_ptr<const string_expr>&) const

virtual	UNROLL_RESOLVE_COPY_STRING_PROTO = 0;

#define	NONMETA_RESOLVE_COPY_STRING_PROTO				\
	count_ptr<const const_param>					\
	nonmeta_resolve_copy(const nonmeta_context_base&,		\
		const count_ptr<const string_expr>&) const

#define	NONMETA_RESOLVE_RVALUE_STRING_PROTO				\
	count_ptr<const pstring_const>					\
	__nonmeta_resolve_rvalue(const nonmeta_context_base&,		\
		const count_ptr<const string_expr>&) const

virtual NONMETA_RESOLVE_RVALUE_STRING_PROTO = 0;
virtual NONMETA_RESOLVE_COPY_STRING_PROTO = 0;

	NONMETA_RESOLVE_COPY_DATA_PROTO;
	EVALUATE_WRITE_PROTO;

virtual EXPR_ACCEPT_VISITOR_PROTO = 0;

protected:
	UNROLL_RESOLVE_COPY_DATA_PROTO;

};	// end class string_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_STRING_EXPR_H__

