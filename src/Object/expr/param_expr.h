/**
	\file "Object/expr/param_expr.h"
	Base class related to program expressions, symbolic and parameters.  
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	TODO: rename to meta_expr_base.h
	$Id: param_expr.h,v 1.1.2.2 2005/07/06 00:59:31 fang Exp $
 */

#ifndef __OBJECT_EXPR_PARAM_EXPR_H__
#define __OBJECT_EXPR_PARAM_EXPR_H__

#include "util/persistent.h"
#include "util/memory/pointer_classes_fwd.h"

//=============================================================================
namespace ART {
namespace entity {
class param_expression_assignment;
class const_param;
class const_range_list;
class unroll_context;
using util::persistent;
using std::ostream;
using util::memory::count_ptr;
using util::memory::excl_ptr;

//=============================================================================
/**
	Abstract base class for symbolic expressions to be written 
	to an object file.  
	Expression graphs must be directed-acyclic.  
	Deriving from object, so that it may be cached in 
	scopespaces' used_id_map.  
	Should statically sub-type into pints and pbools and pranges...
 */
class param_expr : virtual public persistent {
public:
	param_expr() : persistent() { }

virtual	~param_expr() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	size_t
	dimensions(void) const = 0;

virtual	bool
	has_static_constant_dimensions(void) const = 0;

// only call this if dimensions are non-zero and sizes are static constant.  
virtual	const_range_list
	static_constant_dimensions(void) const = 0;

/** is initialized if is resolved to constant or some other formal */
virtual bool
	may_be_initialized(void) const = 0;

virtual bool
	must_be_initialized(void) const = 0;

virtual bool
	may_be_equivalent_generic(const param_expr& p) const = 0;

virtual bool
	must_be_equivalent_generic(const param_expr& p) const = 0;

/** can be resolved to static constant value */
virtual bool
	is_static_constant(void) const = 0;

virtual bool
	is_relaxed_formal_dependent(void) const = 0;

virtual	count_ptr<const const_param>
	static_constant_param(void) const = 0;

/** doesn't depend on loop variables */
virtual bool
	is_loop_independent(void) const = 0;

/** doesn't depend on conditional variables */
virtual bool
	is_unconditional(void) const = 0;

	static
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment(const count_ptr<const param_expr>& p);

virtual	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const = 0;

private:
virtual	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const = 0;
};	// end class param_expr

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_EXPR_PARAM_EXPR_H__

