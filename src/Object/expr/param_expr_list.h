/**
	\file "Object/expr/param_expr_list.h"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	TODO: rename to meta_expr_list_base.h
	$Id: param_expr_list.h,v 1.11.6.4 2006/10/08 21:52:04 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PARAM_EXPR_LIST_H__
#define __HAC_OBJECT_EXPR_PARAM_EXPR_LIST_H__

#include "util/persistent.h"
#include "util/STL/vector_fwd.h"
#include "util/boolean_types.h"
#include "util/memory/pointer_classes_fwd.h"
#include "util/memory/excl_ptr.h"
#include "Object/devel_switches.h"

//=============================================================================
namespace HAC {
namespace entity {
class param_expr;
class const_param_expr_list;
#if USE_INSTANCE_PLACEHOLDERS
class param_value_placeholder;
#else
class param_value_collection;
#endif
class unroll_context;
class template_actuals;
struct expr_dump_context;
using util::good_bool;
using util::bad_bool;
using std::vector;
using std::default_vector;
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
#if USE_INSTANCE_PLACEHOLDERS
	typedef	param_value_placeholder		placeholder_type;
#else
	typedef	param_value_collection		placeholder_type;
#endif
	typedef	never_ptr<const placeholder_type>	placeholder_ptr_type;

	param_expr_list() : persistent() { }

virtual	~param_expr_list() { }

#if !ALWAYS_USE_DYNAMIC_PARAM_EXPR_LIST
virtual	count_ptr<param_expr_list>
	copy(void) const = 0;
#endif

virtual	size_t
	size(void) const = 0;

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const = 0;

#if !ALWAYS_USE_DYNAMIC_PARAM_EXPR_LIST
virtual	excl_ptr<param_expr_list>
	make_copy(void) const = 0;
#endif

virtual	count_ptr<const param_expr>
	operator [] (const size_t) const = 0;

#if ENABLE_STATIC_ANALYSIS
virtual	bool
	may_be_initialized(void) const = 0;

virtual	bool
	must_be_initialized(void) const = 0;
#endif

virtual	bool
	may_be_equivalent(const param_expr_list& p) const = 0;

virtual	bool
	must_be_equivalent(const param_expr_list& p) const = 0;

virtual	bool
	is_static_constant(void) const = 0;

virtual	bool
	is_relaxed_formal_dependent(void) const = 0;

// coordinate with COUNT_TEMPLATE_ARGS in "Object/art_object_definition_base.h"
	typedef	count_ptr<const const_param_expr_list>
					unroll_resolve_rvalues_return_type;

#if !ALWAYS_USE_DYNAMIC_PARAM_EXPR_LIST
virtual	unroll_resolve_rvalues_return_type
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const param_expr_list>&) const = 0;
#endif

// coordinate with template_formals_manager::template_formals_list_type
protected:
	typedef	default_vector<placeholder_ptr_type>::type
					template_formals_list_type;

#if !ALWAYS_USE_DYNAMIC_PARAM_EXPR_LIST
public:
#if RESOLVE_VALUES_WITH_FOOTPRINT
virtual	good_bool
	unroll_assign_formal_parameters(const unroll_context&, 
		const template_formals_list_type&) const = 0;
#endif

virtual	good_bool
	certify_template_arguments(const template_formals_list_type&) = 0;

virtual	good_bool
	certify_template_arguments_without_defaults(
		const template_formals_list_type&) const = 0;
#endif

};	// end class param_expr_list

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PARAM_EXPR_LIST_H__

