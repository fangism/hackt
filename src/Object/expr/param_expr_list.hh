/**
	\file "Object/expr/param_expr_list.hh"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	TODO: rename to meta_expr_list_base.hh
	$Id: param_expr_list.hh,v 1.18 2007/07/31 23:23:24 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PARAM_EXPR_LIST_H__
#define __HAC_OBJECT_EXPR_PARAM_EXPR_LIST_H__

#include <vector>
#include "util/persistent.hh"
#include "util/memory/pointer_classes_fwd.hh"

//=============================================================================
namespace HAC {
namespace entity {
class param_expr;
class const_param_expr_list;
class param_value_placeholder;
class unroll_context;
class template_actuals;
struct expr_dump_context;
class nonmeta_expr_visitor;
using std::vector;
using std::ostream;
using util::memory::count_ptr;
using util::memory::never_ptr;
using util::persistent;

//=============================================================================
/**
	A list of meta (compile-time) parameter expressions.  
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
	at(const size_t) const = 0;

virtual	bool
	may_be_equivalent(const param_expr_list& p) const = 0;

virtual	bool
	must_be_equivalent(const param_expr_list& p) const = 0;

virtual	bool
	is_static_constant(void) const = 0;

virtual	bool
	is_relaxed_formal_dependent(void) const = 0;

virtual	void
	accept(nonmeta_expr_visitor&) const = 0;

// coordinate with COUNT_TEMPLATE_ARGS in "Object/art_object_definition_base.h"
	typedef	count_ptr<const const_param_expr_list>
					unroll_resolve_rvalues_return_type;

// coordinate with template_formals_manager::template_formals_list_type
protected:
	typedef	vector<placeholder_ptr_type>
					template_formals_list_type;

};	// end class param_expr_list

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PARAM_EXPR_LIST_H__

