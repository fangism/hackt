/**
	\file "Object/expr/const_param.h"
	Classes related to constant parameter expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: const_param.h,v 1.14.40.1 2009/08/18 18:05:56 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_CONST_PARAM_H__
#define __HAC_OBJECT_EXPR_CONST_PARAM_H__

#include "Object/expr/param_expr.h"
#include "Object/expr/types.h"		// for preal_value_type

namespace HAC {
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
	dump(ostream& o, const expr_dump_context&) const = 0;

virtual	ostream&
	dump_nonmeta(ostream&) const;

	ostream&
	dump(ostream&) const;

virtual	size_t
	dimensions(void) const = 0;

virtual	bool
	is_true(void) const = 0;

// don't know if these are applicable... 
// depends on whether or not we use this for collective constants...
virtual	bool
	has_static_constant_dimensions(void) const = 0;

// only call this if dimensions are non-zero and sizes are static constant.  
virtual	const_range_list
	static_constant_dimensions(void) const = 0;

	bool
	is_static_constant(void) const { return true; }

virtual	count_ptr<const const_param>
	static_constant_param(void) const = 0;

	bool
	is_loop_independent(void) const { return true; }

	bool
	is_unconditional(void) const { return true; }

virtual	preal_value_type
	to_real_const(void) const;

#define	LESS_OPERATOR_PROTO						\
	bool								\
	operator < (const const_param&) const

#define	EQUAL_OPERATOR_PROTO						\
	bool								\
	operator == (const const_param&) const

/**
	\pre the type being compared to must have 'same' type, 
		as defined in each implementation.  
 */
virtual	LESS_OPERATOR_PROTO = 0;

};	// end class const_param

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_CONST_PARAM_H__

