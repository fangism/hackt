/**
	\file "Object/expr/preal_expr.h"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	$Id: preal_expr.h,v 1.3.6.2 2006/03/19 22:47:05 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PREAL_EXPR_H__
#define __HAC_OBJECT_EXPR_PREAL_EXPR_H__

#include "Object/expr/param_expr.h"
#include "Object/expr/real_expr.h"
#include "Object/expr/types.h"
#include "util/boolean_types.h"

//=============================================================================
namespace HAC {
namespace entity {
struct preal_tag;
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
	must_be_equivalent(const preal_expr& ) const = 0;

virtual bool
	is_static_constant(void) const = 0;

virtual	count_ptr<const const_param>
	static_constant_param(void) const;

virtual value_type
	static_constant_value(void) const = 0;

virtual	good_bool
	resolve_value(value_type& i) const = 0;

virtual	good_bool
	unroll_resolve_value(const unroll_context&, value_type&) const = 0;

virtual	const_index_list
	resolve_dimensions(void) const = 0;

protected:
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const param_expr>&) const;

	count_ptr<aggregate_meta_value_reference_base>
	make_aggregate_meta_value_reference_private(
		const count_ptr<const param_expr>&) const;

};	// end class preal_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PREAL_EXPR_H__

