/**
	\file "Object/expr/const_param.h"
	Classes related to constant parameter expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: const_param.h,v 1.1.2.1 2005/07/05 01:16:26 fang Exp $
 */

#ifndef __OBJECT_EXPR_CONST_PARAM_H__
#define __OBJECT_EXPR_CONST_PARAM_H__

#include "Object/expr/param_expr.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Silly as it may seem...
	Interface for pure constants.  
	Used for unroll time, when everything must be bound.  
	Need this to distinguish between dynamic and const param_expr_lists
 */
class const_param : virtual public param_expr {
protected:
	// no members
public:
	const_param() : param_expr() { }
virtual	~const_param() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	size_t
	dimensions(void) const = 0;

// don't know if these are applicable... 
// depends on whether or not we use this for collective constants...
virtual	bool
	has_static_constant_dimensions(void) const = 0;

// only call this if dimensions are non-zero and sizes are static constant.  
virtual	const_range_list
	static_constant_dimensions(void) const = 0;

	bool
	may_be_initialized(void) const { return true; }

	bool
	must_be_initialized(void) const { return true; }

#if 0
virtual bool
	may_be_equivalent(const param_expr& p) const = 0;

virtual bool
	must_be_equivalent(const param_expr& p) const = 0;
#endif

	bool
	is_static_constant(void) const { return true; }

virtual	count_ptr<const const_param>
	static_constant_param(void) const = 0;

	bool
	is_loop_independent(void) const { return true; }

	bool
	is_unconditional(void) const { return true; }

virtual	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const = 0;
};	// end class const_param

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_EXPR_CONST_PARAM_H__

