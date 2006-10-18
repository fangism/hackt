/**
	\file "Object/expr/dynamic_param_expr_list.h"
	Non-constant meta parameter expression list.
	NOTE: this file was spawned from the old
		"Object/art_object_expr.h" for revision history tracking.  
	$Id: dynamic_param_expr_list.h,v 1.10 2006/10/18 05:32:39 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_DYNAMIC_PARAM_EXPR_LIST_H__
#define __HAC_OBJECT_EXPR_DYNAMIC_PARAM_EXPR_LIST_H__

#include <vector>
#include "Object/expr/param_expr_list.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class const_param_expr_list;
class template_formals_manager;
using std::vector;

//=============================================================================
/**
	Generalized list of parameter expressions, can be dynamic.  
 */
class dynamic_param_expr_list : public param_expr_list, 
		public vector<count_ptr<const param_expr> > {
friend class const_param_expr_list;
	typedef	dynamic_param_expr_list			this_type;
	typedef	param_expr_list				interface_type;
protected:
	typedef	vector<count_ptr<const param_expr> >	parent_type;
public:
	typedef parent_type::value_type			value_type;
	typedef parent_type::iterator			iterator;
	typedef parent_type::const_iterator		const_iterator;
	typedef parent_type::reverse_iterator		reverse_iterator;
	typedef parent_type::const_reverse_iterator	const_reverse_iterator;
public:
	dynamic_param_expr_list();

	explicit
	dynamic_param_expr_list(const value_type&);

	dynamic_param_expr_list(const const_param_expr_list&);

	dynamic_param_expr_list(const const_param_expr_list&, const size_t);

	explicit
	dynamic_param_expr_list(const size_t);

// lazy: use default copy constructor
//	dynamic_param_expr_list(const dynamic_param_expr_list& pl);
	~dynamic_param_expr_list();

#if !ALWAYS_USE_DYNAMIC_PARAM_EXPR_LIST
	count_ptr<param_expr_list>
	copy(void) const;
#endif

	size_t
	size(void) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

#if ALWAYS_USE_DYNAMIC_PARAM_EXPR_LIST
	count_ptr<const_param_expr_list>
	make_const_param_expr_list(void) const;
#else
	excl_ptr<param_expr_list>
	make_copy(void) const;
#endif

	count_ptr<const param_expr>
	operator [] (const size_t) const;

#if ENABLE_STATIC_ANALYSIS
	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;
#endif

	bool
	may_be_equivalent(const param_expr_list& p) const;

	bool
	must_be_equivalent(const param_expr_list& p) const;

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	unroll_resolve_rvalues_return_type
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const param_expr_list>&) const;

#if RESOLVE_VALUES_WITH_FOOTPRINT
	good_bool
	unroll_assign_formal_parameters(const unroll_context&, 
		const template_formals_list_type&) const;
#endif

	good_bool
	certify_template_arguments(const template_formals_manager&,
		const template_formals_list_type&);

	good_bool
	certify_template_arguments_without_defaults(
		const template_formals_list_type&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class dynamic_param_expr_list

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_DYNAMIC_PARAM_EXPR_LIST_H__

