/**
	\file "Object/expr/preal_expr.h"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	$Id: preal_expr.h,v 1.10 2006/10/18 19:08:00 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PREAL_EXPR_H__
#define __HAC_OBJECT_EXPR_PREAL_EXPR_H__

#include "Object/expr/param_expr.h"
#include "Object/expr/real_expr.h"
#include "Object/expr/types.h"
#include "util/boolean_types.h"
#include "Object/devel_switches.h"

//=============================================================================
namespace HAC {
namespace entity {
struct preal_tag;
class preal_const;
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
class preal_expr : virtual public param_expr, public real_expr {
	typedef	preal_expr			this_type;
public:
	typedef	preal_tag			tag_type;
	/**
		The global boolean value type, set in "Object/expr/types.h".
		We bother with this typedef for the future potential of 
		using templates to extend to other parameter types.  
	 */
	typedef	preal_value_type		value_type;
public:
	preal_expr() : param_expr(), real_expr() { }

	// temporary de-inline for debugging
virtual	~preal_expr() { }

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
	must_be_equivalent(const preal_expr& ) const = 0;

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

virtual	count_ptr<const preal_const>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const preal_expr>&) const = 0;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const param_expr>&) const;

virtual	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const preal_expr>&) const = 0;

#define	UNROLL_RESOLVE_COPY_PREAL_PROTO					\
	count_ptr<const preal_expr>					\
	unroll_resolve_copy(const unroll_context&,			\
		const count_ptr<const preal_expr>&) const

	UNROLL_RESOLVE_COPY_REAL_PROTO;

virtual UNROLL_RESOLVE_COPY_PREAL_PROTO = 0;

#define	SUBSTITUTE_DEFAULT_PARAMETERS_PREAL_PROTO			\
	count_ptr<const preal_expr>					\
	substitute_default_positional_parameters(			\
		const template_formals_manager&,			\
		const dynamic_param_expr_list&,				\
		const count_ptr<const preal_expr>&) const

	SUBSTITUTE_DEFAULT_PARAMETERS_PROTO;

virtual SUBSTITUTE_DEFAULT_PARAMETERS_PREAL_PROTO = 0;

protected:
	using real_expr::unroll_resolve_copy;

protected:
	MAKE_PARAM_EXPRESSION_ASSIGNMENT_PROTO;
	MAKE_AGGREGATE_META_VALUE_REFERENCE_PROTO;

};	// end class preal_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PREAL_EXPR_H__

