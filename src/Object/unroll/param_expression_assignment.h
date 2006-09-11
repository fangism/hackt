/**
	\file "Object/unroll/param_expression_assignment.h"
	Declarations for classes related to connection of 
	assignments of parameters.
	This file came from "Object/art_object_assign.h" in a previous life.  
	$Id: param_expression_assignment.h,v 1.6.36.1 2006/09/11 02:39:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_PARAM_EXPRESSION_ASSIGNMENT_H__
#define	__HAC_OBJECT_UNROLL_PARAM_EXPRESSION_ASSIGNMENT_H__

#include "util/boolean_types.h"
#include "Object/unroll/instance_management_base.h"
#include "Object/devel_switches.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class meta_value_reference_base;
using util::bad_bool;
using util::good_bool;
using util::memory::count_ptr;
class param_expr;

//=============================================================================
/**
	Class for saving and managing expression assignments.  
	Includes both static and dynamic expressions.  
	Consider separating, rhs from the rest?
		rhs is any param_expr, while the rest are 
		meta_instance_references, may eventually be complex-aggregate.
	TODO: Consider sub-typing into pint and pbool assignments, 
		since types are static.  
 */
class param_expression_assignment : public instance_management_base {
public:
	typedef	count_ptr<param_expr>			src_ptr_type;
	typedef	count_ptr<const param_expr>		src_const_ptr_type;
	typedef	count_ptr<meta_value_reference_base>	dest_ptr_type;
	typedef	count_ptr<const meta_value_reference_base>
							dest_const_ptr_type;

// protected:
//	/** cached value for dimensions, computed on construction */
//	size_t			dimension;

public:
	param_expression_assignment();
virtual	~param_expression_assignment();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

virtual	size_t
	size(void) const = 0;

	// TODO: rename me! append_meta_value_reference
virtual	bad_bool
	append_simple_param_meta_value_reference(const dest_ptr_type& e) = 0;

	/**
		Helper class for appending instance references to
		a parameter assignment list.  
		Written as a binary operator to accumulate error conditions.  
		Used by object_list::make_param_expression_assignment.
	 */
	class meta_instance_reference_appender {
		// used to be object_list::value_type
		typedef	count_ptr<param_expr>	arg_type;
	protected:
		size_t				index;
		param_expression_assignment&	ex_ass;
	public:
		explicit
		meta_instance_reference_appender(param_expression_assignment& p) :
			index(0), ex_ass(p) { }

		bad_bool
		operator () (const bad_bool b, const arg_type& i);
	};	// end class meta_instance_reference_appender

protected:
	good_bool
	validate_dimensions_match(const dest_const_ptr_type&, 
		const size_t ) const;

#if ENABLE_STATIC_ANALYSIS
	good_bool
	validate_reference_is_uninitialized(const dest_const_ptr_type&) const;
#endif

};	// end class param_expression_assignment

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_PARAM_EXPRESSION_ASSIGNMENT_H__

