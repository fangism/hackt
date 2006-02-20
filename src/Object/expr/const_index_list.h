/**
	\file "Object/expr/const_index_list.h"
	Classes related to constant expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: const_index_list.h,v 1.8.2.2 2006/02/20 06:52:05 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_CONST_INDEX_LIST_H__
#define __HAC_OBJECT_EXPR_CONST_INDEX_LIST_H__

#include "util/STL/pair_fwd.h"
#include <list>
#include <vector>
#include "Object/expr/meta_index_list.h"
#include "Object/common/multikey_index.h"
#include "util/memory/pointer_classes_fwd.h"

//=============================================================================
namespace HAC {
namespace entity {
class const_index;
class const_range_list;
using std::list;
using std::vector;
using std::pair;
using util::multikey_generic;
using util::multikey_generator;
using util::memory::count_ptr;

//=============================================================================
/**
	Index list whose indices are all constant.
	This means we need a const_index interface to objects.  
	Because of arbitrary pointer copying,
	members must be reference counted.  
 */
class const_index_list : public meta_index_list, 
		private vector<count_ptr<const_index> > {
	typedef	const_index_list		this_type;
public:
	typedef	count_ptr<const_index>		const_index_ptr_type;
	typedef	const_index_ptr_type		value_type;
protected:
	/** need list of pointers b/c const_index is abstract */
	typedef	vector<const_index_ptr_type>		parent_type;
public:
	typedef parent_type::iterator			iterator;
	typedef parent_type::const_iterator		const_iterator;
	typedef parent_type::reverse_iterator		reverse_iterator;
	typedef parent_type::const_reverse_iterator	const_reverse_iterator;
public:
	const_index_list();

	explicit
	const_index_list(const size_t);

	// consider templating this...
	const_index_list(const const_index_list& l, 
		const pair<list<pint_value_type>, list<pint_value_type> >& f);

	~const_index_list();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump(ostream&) const;

/** NOT THE SAME **/
	size_t
	size(void) const;

	size_t
	dimensions_collapsed(void) const;

	const_range_list
	collapsed_dimension_ranges(void) const;

	using parent_type::empty;
	using parent_type::clear;
	using parent_type::begin;
	using parent_type::end;
	using parent_type::rbegin;
	using parent_type::rend;

	void
	push_back(const const_index_ptr_type& i);

	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const { return false; }

	const_index_list
	resolve_index_list(void) const;

	const_index_list
	unroll_resolve_indices(const unroll_context&) const;

#if 0
	bool
	resolve_multikey(excl_ptr<multikey_index_type>& k) const;
#endif

	multikey_index_type
	upper_multikey(void) const;

	multikey_index_type
	lower_multikey(void) const;

	bool
	equal_dimensions(const const_index_list& ) const;

	bool
	must_be_equivalent_indices(const meta_index_list& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class const_index_list

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_CONST_INDEX_LIST_H__

