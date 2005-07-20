/**
	\file "Object/expr/dynamic_param_expr_list.h"
	Non-constant meta parameter expression list.
	NOTE: this file was spawned from the old
		"Object/art_object_expr.h" for revision history tracking.  
	$Id: dynamic_param_expr_list.h,v 1.2 2005/07/20 21:00:42 fang Exp $
 */

#ifndef __OBJECT_EXPR_DYNAMIC_PARAM_EXPR_LIST_H__
#define __OBJECT_EXPR_DYNAMIC_PARAM_EXPR_LIST_H__

#include <vector>
#include "Object/expr/param_expr_list.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
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
	typedef parent_type::iterator			iterator;
	typedef parent_type::const_iterator		const_iterator;
	typedef parent_type::reverse_iterator		reverse_iterator;
	typedef parent_type::const_reverse_iterator	const_reverse_iterator;
public:
	dynamic_param_expr_list();

	explicit
	dynamic_param_expr_list(const size_t);

// lazy: use default copy constructor
//	dynamic_param_expr_list(const dynamic_param_expr_list& pl);
	~dynamic_param_expr_list();

	size_t
	size(void) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	excl_ptr<param_expr_list>
	make_copy(void) const;

	count_ptr<const param_expr>
	operator [] (const size_t) const;

	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

	bool
	may_be_equivalent(const param_expr_list& p) const;

	bool
	must_be_equivalent(const param_expr_list& p) const;

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	bool
	is_loop_independent(void) const;

	unroll_resolve_return_type
	unroll_resolve(const unroll_context&) const;

	good_bool
	certify_template_arguments(const template_formals_list_type&);

	good_bool
	certify_template_arguments_without_defaults(
		const template_formals_list_type&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class dynamic_param_expr_list

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_EXPR_DYNAMIC_PARAM_EXPR_LIST_H__

