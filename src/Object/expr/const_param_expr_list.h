/**
	\file "Object/expr/const_param_expr_list.h"
	Classes related to constant expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: const_param_expr_list.h,v 1.7 2006/02/02 06:30:03 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_CONST_PARAM_EXPR_LIST_H__
#define __HAC_OBJECT_EXPR_CONST_PARAM_EXPR_LIST_H__

#include <vector>
#include "Object/expr/param_expr_list.h"

//=============================================================================
namespace HAC {
namespace entity {
class const_param;
class dynamic_param_expr_list;
using std::vector;
using util::persistent_object_manager;

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
public:
	typedef	parent_type::value_type			value_type;
	typedef	parent_type::const_reference		const_reference;
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

	/**
		uses std::copy to initialize a sequence.  
	 */
	template <class InIter>
	const_param_expr_list(InIter b, InIter e) :
			interface_type(), parent_type(b, e) {
	}

	~const_param_expr_list();

	size_t
	size(void) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	ostream&
	dump_range(ostream&, const expr_dump_context&, 
		const size_t, const size_t) const;

	excl_ptr<param_expr_list>
	make_copy(void) const;

	using parent_type::front;

	count_ptr<const param_expr>
	operator [] (const size_t) const;

	using parent_type::at;
	using parent_type::begin;
	using parent_type::end;
	using parent_type::empty;
	using parent_type::push_back;

	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

	bool
	may_be_equivalent(const param_expr_list& p) const;

	bool
	must_be_equivalent(const param_expr_list& p) const;

	/// checks equivalence up to the end of the strict argument list
	bool
	must_be_equivalent(const this_type&, const size_t s) const;

	/// only checks the relaxed parameters of this list against the argument
	bool
	is_tail_equivalent(const this_type&) const;

	/// checks entire list
	bool
	must_be_equivalent(const this_type&) const;

	bool
	is_static_constant(void) const { return true; }

	bool
	is_relaxed_formal_dependent(void) const { return false; }

	bool
	is_template_dependent(void) const { return false; }

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
	void
	collect_transient_info_base(persistent_object_manager&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class const_param_expr_list

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_CONST_PARAM_EXPR_LIST_H__

