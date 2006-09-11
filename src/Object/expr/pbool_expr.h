/**
	\file "Object/expr/pbool_expr.h"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	$Id: pbool_expr.h,v 1.11.6.3 2006/09/11 22:30:45 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PBOOL_EXPR_H__
#define __HAC_OBJECT_EXPR_PBOOL_EXPR_H__

#include "Object/expr/param_expr.h"
#include "Object/expr/bool_expr.h"
#include "Object/expr/types.h"
#include "util/boolean_types.h"
#include "Object/devel_switches.h"

//=============================================================================
namespace HAC {
namespace entity {
struct pbool_tag;
class pbool_const;
class const_param;
class const_index_list;
class unroll_context;
class template_actuals;
using util::good_bool;
using util::bad_bool;
using util::memory::excl_ptr;
using util::memory::never_ptr;

//=============================================================================
/**
	Abstract expression checked to be a single boolean.  
 */
class pbool_expr : virtual public param_expr, public bool_expr {
	typedef	pbool_expr			this_type;
public:
	typedef	pbool_tag			tag_type;
	/**
		The global boolean value type, set in "Object/expr/types.h".
		We bother with this typedef for the future potential of 
		using templates to extend to other parameter types.  
	 */
	typedef	pbool_value_type		value_type;
public:
	pbool_expr() : param_expr(), bool_expr() { }

	// temporary de-inline for debugging
virtual	~pbool_expr() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const = 0;

virtual	size_t
	dimensions(void) const = 0;

	GET_UNRESOLVED_DATA_TYPE_REF_PROTO;
#if USE_RESOLVED_DATA_TYPES
	GET_RESOLVED_DATA_TYPE_REF_PROTO;
#endif

#if ENABLE_STATIC_ANALYSIS
virtual bool
	may_be_initialized(void) const = 0;

virtual bool
	must_be_initialized(void) const = 0;
#endif

	bool
	may_be_equivalent_generic(const param_expr& p) const;

	bool
	must_be_equivalent_generic(const param_expr& p) const;

virtual	bool
	must_be_equivalent(const pbool_expr& ) const = 0;

virtual bool
	is_static_constant(void) const = 0;

virtual	count_ptr<const const_param>
	static_constant_param(void) const;

virtual value_type
	static_constant_value(void) const = 0;

#if !USE_INSTANCE_PLACEHOLDERS
virtual	good_bool
	resolve_value(value_type& i) const = 0;
#endif

virtual	good_bool
	unroll_resolve_value(const unroll_context&, value_type&) const = 0;

#if !USE_INSTANCE_PLACEHOLDERS
virtual	const_index_list
	resolve_dimensions(void) const = 0;
#endif

virtual	count_ptr<const pbool_const>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const pbool_expr>&) const = 0;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const param_expr>&) const;

virtual	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const pbool_expr>&) const = 0;

#define	UNROLL_RESOLVE_COPY_PBOOL_PROTO					\
	count_ptr<const pbool_expr>					\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const pbool_expr>&) const

	UNROLL_RESOLVE_COPY_BOOL_PROTO;

virtual	UNROLL_RESOLVE_COPY_PBOOL_PROTO = 0;
protected:
	using bool_expr::unroll_resolve_copy;

protected:
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const param_expr>&) const;

	count_ptr<aggregate_meta_value_reference_base>
	make_aggregate_meta_value_reference_private(
		const count_ptr<const param_expr>&) const;

};	// end class pbool_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PBOOL_EXPR_H__

