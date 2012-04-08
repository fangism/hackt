/**
	\file "Object/expr/pint_expr.h"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	$Id: pint_expr.h,v 1.19 2011/01/14 01:32:57 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PINT_EXPR_H__
#define __HAC_OBJECT_EXPR_PINT_EXPR_H__

#include "Object/expr/param_expr.h"
#include "Object/expr/meta_index_expr.h"
#include "Object/expr/int_expr.h"
#include "Object/expr/types.h"
#include "util/boolean_types.h"

//=============================================================================
namespace HAC {
namespace entity {
struct pint_tag;
class pint_const;
class const_param;
class const_index_list;
class unroll_context;
using util::good_bool;
using util::bad_bool;
using util::memory::excl_ptr;
using util::memory::never_ptr;

//=============================================================================
/**
	Abstract expression checked to be a single integer.  
 */
class pint_expr : virtual public param_expr, virtual public meta_index_expr, 
		public int_expr {
	typedef	pint_expr			this_type;
	typedef	int_expr			nonmeta_parent_type;
	typedef	meta_index_expr			index_parent_type;
public:
	typedef	pint_tag			tag_type;
	/**
		The internal storage type, set in "Object/expr/types.h".
		We bother with this typedef for the future potential of 
		using templates to extend to other parameter types.  
	 */
	typedef	pint_value_type			value_type;
	typedef	pint_const			const_expr_type;
protected:
	pint_expr() : param_expr(), meta_index_expr(), int_expr() { }

public:
	// temporary de-inline for debugging purposes
virtual	~pint_expr() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const = 0;

virtual	size_t
	dimensions(void) const = 0;

	GET_UNRESOLVED_DATA_TYPE_REF_PROTO;
	GET_RESOLVED_DATA_TYPE_REF_PROTO;

	bool
	may_be_equivalent_generic(const param_expr& p) const;

	bool
	must_be_equivalent_generic(const param_expr& p) const;

virtual	bool
	must_be_equivalent(const pint_expr& ) const = 0;

	bool
	must_be_equivalent_index(const meta_index_expr& ) const;

virtual bool
	is_static_constant(void) const = 0;

virtual	bool
	is_relaxed_formal_dependent(void) const = 0;

virtual	count_ptr<const const_param>
	static_constant_param(void) const;

virtual value_type
	static_constant_value(void) const = 0;

	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;

virtual	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const = 0;

virtual	count_ptr<const const_expr_type>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const pint_expr>&) const = 0;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const param_expr>&) const;

virtual	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const pint_expr>&) const = 0;

#define	UNROLL_RESOLVE_COPY_PINT_PROTO					\
	count_ptr<const pint_expr>					\
	unroll_resolve_copy(const unroll_context&,			\
		const count_ptr<const pint_expr>&) const

	UNROLL_RESOLVE_COPY_INT_PROTO;
	UNROLL_RESOLVE_COPY_META_INDEX_PROTO;
	UNROLL_RESOLVE_COPY_NONMETA_INDEX_PROTO;
	NONMETA_RESOLVE_RVALUE_INT_PROTO;
	NONMETA_RESOLVE_COPY_INT_PROTO;

virtual UNROLL_RESOLVE_COPY_PINT_PROTO = 0;

#define	SUBSTITUTE_DEFAULT_PARAMETERS_PINT_PROTO			\
	count_ptr<const pint_expr>					\
	substitute_default_positional_parameters(			\
		const template_formals_manager&,			\
		const dynamic_param_expr_list&,				\
		const count_ptr<const pint_expr>&) const

	SUBSTITUTE_DEFAULT_PARAMETERS_PROTO;
	SUBSTITUTE_DEFAULT_PARAMETERS_META_INDEX_PROTO;

virtual	SUBSTITUTE_DEFAULT_PARAMETERS_PINT_PROTO = 0;

virtual	EXPR_ACCEPT_VISITOR_PROTO = 0;

protected:
	using nonmeta_parent_type::unroll_resolve_copy;
	using nonmeta_parent_type::nonmeta_resolve_copy;
	using index_parent_type::unroll_resolve_copy;
	using index_parent_type::nonmeta_resolve_copy;

protected:
	MAKE_PARAM_EXPRESSION_ASSIGNMENT_PROTO;
	MAKE_AGGREGATE_META_VALUE_REFERENCE_PROTO;

};	// end class pint_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PINT_EXPR_H__

