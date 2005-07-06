/**
	\file "Object/expr/param_expr_list.h"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	TODO: rename to meta_expr_list_base.h
	$Id: param_expr_list.h,v 1.1.2.3 2005/07/06 23:11:24 fang Exp $
 */

#ifndef __OBJECT_EXPR_PARAM_EXPR_LIST_H__
#define __OBJECT_EXPR_PARAM_EXPR_LIST_H__

#include "util/persistent.h"
#include "util/STL/vector_fwd.h"
#include "util/boolean_types.h"
#include "util/memory/pointer_classes_fwd.h"

//=============================================================================
namespace ART {
namespace entity {
class param_expr;
class const_param_expr_list;
class param_instance_collection;
class unroll_context;
class template_actuals;
using util::good_bool;
using util::bad_bool;
using std::vector;
using std::ostream;
using util::memory::count_ptr;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::persistent;

//=============================================================================
/**
	A list of parameter expressions.  
	Consider splitting into dynamic vs. const?
 */
class param_expr_list : public persistent {
public:
	param_expr_list() : persistent() { }

virtual	~param_expr_list() { }

virtual	size_t
	size(void) const = 0;

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	excl_ptr<param_expr_list>
	make_copy(void) const = 0;

virtual	count_ptr<const param_expr>
	operator [] (const size_t) const = 0;

virtual	bool
	may_be_initialized(void) const = 0;

virtual	bool
	must_be_initialized(void) const = 0;

virtual	bool
	may_be_equivalent(const param_expr_list& p) const = 0;

virtual	bool
	must_be_equivalent(const param_expr_list& p) const = 0;

virtual	bool
	is_static_constant(void) const = 0;

virtual	bool
	is_relaxed_formal_dependent(void) const = 0;

virtual	bool
	is_loop_independent(void) const = 0;

// coordinate with COUNT_TEMPLATE_ARGS in "Object/art_object_definition_base.h"
	typedef	count_ptr<const_param_expr_list>
					unroll_resolve_return_type;

virtual	unroll_resolve_return_type
	unroll_resolve(const unroll_context&) const = 0;

// coordinate with template_formals_manager::template_formals_list_type
protected:
	typedef	DEFAULT_VECTOR(never_ptr<const param_instance_collection>)
					template_formals_list_type;

public:
virtual	good_bool
	certify_template_arguments(const template_formals_list_type&) = 0;

virtual	good_bool
	certify_template_arguments_without_defaults(
		const template_formals_list_type&) const = 0;

};	// end class param_expr_list

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_EXPR_PARAM_EXPR_LIST_H__

