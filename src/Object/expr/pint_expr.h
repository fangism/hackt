/**
	\file "Object/expr/pint_expr.h"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	$Id: pint_expr.h,v 1.9.16.3 2006/06/05 04:02:47 fang Exp $
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

	GET_DATA_TYPE_REF_PROTO;

virtual bool
	may_be_initialized(void) const = 0;

virtual bool
	must_be_initialized(void) const = 0;

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
	resolve_index(void) const;

	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;

virtual	good_bool
	resolve_value(value_type& i) const = 0;

virtual	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const = 0;

virtual	const_index_list
	resolve_dimensions(void) const = 0;

#if COW_UNROLL_DATA_EXPR
#define	UNROLL_RESOLVE_COPY_PINT_PROTO					\
	count_ptr<const pint_expr>					\
	unroll_resolve_copy(const unroll_context&,			\
		const count_ptr<const pint_expr>&) const

	UNROLL_RESOLVE_COPY_INT_PROTO;
	UNROLL_RESOLVE_COPY_META_INDEX_PROTO;
	UNROLL_RESOLVE_COPY_NONMETA_INDEX_PROTO;

virtual UNROLL_RESOLVE_COPY_PINT_PROTO = 0;
protected:
	using nonmeta_parent_type::unroll_resolve_copy;
	// using index_parent_type::unroll_resolve_copy;
#endif

protected:
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const param_expr>&) const;

	count_ptr<aggregate_meta_value_reference_base>
	make_aggregate_meta_value_reference_private(
		const count_ptr<const param_expr>&) const;

};	// end class pint_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PINT_EXPR_H__

