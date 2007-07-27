/**
	\file "Object/expr/const_param_expr_list.h"
	Classes related to constant expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: const_param_expr_list.h,v 1.17.22.3 2007/07/27 02:39:03 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_CONST_PARAM_EXPR_LIST_H__
#define __HAC_OBJECT_EXPR_CONST_PARAM_EXPR_LIST_H__

#include <vector>
#include "Object/expr/param_expr_list.h"
#include "util/boolean_types.h"

//=============================================================================
namespace HAC {
namespace entity {
class const_param;
class dynamic_param_expr_list;
using std::vector;
using util::good_bool;
using util::persistent_object_manager;

//=============================================================================
/**
	List of strictly constant param expressions.  
	Only scalar expressions allowed, no array indirections or collections.  
	TODO: liberate from param_expr_list base class eventually.  
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
	typedef	parent_type::reference			reference;
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

	using parent_type::front;
	using parent_type::back;

	count_ptr<const param_expr>
	at(const size_t) const;

#if 0
	using parent_type::operator[];
#else
	const_reference
	operator [] (const size_t) const;
#endif

	using parent_type::begin;
	using parent_type::end;
	using parent_type::empty;
	using parent_type::push_back;

	bool
	is_all_true(void) const;

	static
	bool
	is_all_true(const parent_type&);

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

	void
	accept(nonmeta_expr_visitor&) const;

	count_ptr<dynamic_param_expr_list>
	to_dynamic_list(void) const;

	// need unroll context in case formals list depends on these actuals!
	good_bool
	must_validate_template_arguments(
		const template_formals_list_type&, 
		const unroll_context&) const;

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

