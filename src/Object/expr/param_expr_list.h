/**
	\file "Object/expr/param_expr_list.h"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	TODO: rename to meta_expr_list_base.h
	$Id: param_expr_list.h,v 1.15.2.1 2006/10/24 04:24:28 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PARAM_EXPR_LIST_H__
#define __HAC_OBJECT_EXPR_PARAM_EXPR_LIST_H__

#include "util/persistent.h"
#include "util/STL/vector_fwd.h"
#include "util/boolean_types.h"
#include "util/memory/pointer_classes_fwd.h"
#include "util/memory/excl_ptr.h"

//=============================================================================
namespace HAC {
namespace entity {
class param_expr;
class const_param_expr_list;
class param_value_placeholder;
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
	typedef	param_value_placeholder		placeholder_type;
	typedef	never_ptr<const placeholder_type>	placeholder_ptr_type;

	param_expr_list() : persistent() { }

virtual	~param_expr_list() { }

virtual	size_t
	size(void) const = 0;

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const = 0;

virtual	count_ptr<const param_expr>
	operator [] (const size_t) const = 0;

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

// coordinate with template_formals_manager::template_formals_list_type
protected:
	typedef	default_vector<placeholder_ptr_type>::type
					template_formals_list_type;

};	// end class param_expr_list

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PARAM_EXPR_LIST_H__

