/**
	\file "Object/expr/const_param_expr_list.h"
	Classes related to constant expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: const_param_expr_list.h,v 1.2.10.1 2005/08/10 20:30:55 fang Exp $
 */

#ifndef __OBJECT_EXPR_CONST_PARAM_EXPR_LIST_H__
#define __OBJECT_EXPR_CONST_PARAM_EXPR_LIST_H__

#include <vector>
#include "Object/expr/param_expr_list.h"

//=============================================================================
namespace ART {
namespace entity {
class const_param;
class dynamic_param_expr_list;
using std::vector;
// using util::persistent_object_manager;	// forward declared

//=============================================================================
/**
	List of strictly constant param expressions.  
	Only scalar expressions allowed, no array indirections or collections.  
 */
class const_param_expr_list : public param_expr_list, 
		protected vector<count_ptr<const const_param> > {
friend class dynamic_param_expr_list;
	typedef	const_param_expr_list			this_type;
	typedef	param_expr_list				interface_type;
protected:
	typedef	vector<count_ptr<const const_param> >	parent_type;
	typedef	parent_type::value_type			value_type;
public:
	typedef parent_type::iterator			iterator;
	typedef parent_type::const_iterator		const_iterator;
	typedef parent_type::reverse_iterator		reverse_iterator;
	typedef parent_type::const_reverse_iterator	const_reverse_iterator;
public:
	const_param_expr_list();

	explicit
	const_param_expr_list(const parent_type::value_type&);
// lazy: use default copy constructor
//	const_param_expr_list(const const_param_expr_list& pl);
	~const_param_expr_list();

	size_t
	size(void) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	excl_ptr<param_expr_list>
	make_copy(void) const;

	using parent_type::front;

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
	is_static_constant(void) const { return true; }

	bool
	is_relaxed_formal_dependent(void) const { return false; }

	bool
	is_loop_independent(void) const { return true; }

	unroll_resolve_return_type
	unroll_resolve(const unroll_context&) const;

	good_bool
	certify_template_arguments(const template_formals_list_type&);

	good_bool
	certify_template_arguments_without_defaults(
		const template_formals_list_type&) const;

	good_bool
	must_validate_template_arguments(
		const template_formals_list_type&) const;

	bool
	operator < (const this_type&) const;

	/**
		Dereference and compare (less-than) functor.
	 */
	struct less_ptr {
		bool
		operator () (const value_type&, const value_type&) const;
	};	// end struct less

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class const_param_expr_list

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_EXPR_CONST_PARAM_EXPR_LIST_H__

