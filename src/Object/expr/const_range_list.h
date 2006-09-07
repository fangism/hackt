/**
	\file "Object/expr/const_range_list.h"
	Classes related to constant expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: const_range_list.h,v 1.11.8.1 2006/09/07 21:34:22 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_CONST_RANGE_LIST_H__
#define __HAC_OBJECT_EXPR_CONST_RANGE_LIST_H__

#include <list>
#include <vector>
#include "Object/expr/meta_range_list.h"
#include "Object/expr/const_range.h"
#include "Object/common/multikey_index.h"
#include "util/multikey_fwd.h"

//=============================================================================
namespace HAC {
namespace entity {
class const_range;
class const_index_list;
using std::list;
using std::vector;
using util::multikey_generator;

//=============================================================================
/**
	List of constant range expressions.  
	Would a vector be more appropriate?   consider changing later...
 */
class const_range_list : public meta_range_list, public vector<const_range> {
	typedef	const_range_list			this_type;
protected:
	// no need for pointers here
	typedef	vector<const_range>			list_type;
	typedef	list<const_range>			alt_list_type;
public:
	typedef	list_type::value_type			value_type;
	typedef	list_type::iterator			iterator;
	typedef	list_type::const_iterator		const_iterator;
	typedef	list_type::reverse_iterator		reverse_iterator;
	typedef	list_type::const_reverse_iterator	const_reverse_iterator;
	/**
		Exception type for resolving a bad range, such
		as negative indices, or backwards bounds. 
	 */
	typedef	const_range				bad_range;
public:
	const_range_list();

	explicit
	const_range_list(const size_t);

	explicit
	const_range_list(const list_type& l);

	explicit
	const_range_list(const alt_list_type& l);

	explicit
	const_range_list(const const_index_list& i);

	~const_range_list();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump(ostream&) const;

	ostream&
	dump_force(ostream& o) const;

	size_t
	size(void) const;

	bool
	is_valid(void) const;

	bool
	is_static_constant(void) const { return true; }

	bool
	is_relaxed_formal_dependent(void) const { return false; }

	const_range_list
	static_overlap(const meta_range_list& r) const;

	bool
	is_size_equivalent(const const_range_list& il) const;
	// see also must_be_formal_size_equivalent, declared below

	// may be obsolete by must_be_formal_size_equivalent...
	bool
	operator == (const const_range_list& c) const;

#if !USE_INSTANCE_PLACEHOLDERS
	good_bool
	resolve_ranges(const_range_list& r) const;
#endif

	multikey_index_type
	upper_multikey(void) const;

	multikey_index_type
	lower_multikey(void) const;

	template <size_t D>
	void
	make_multikey_generator(
		multikey_generator<D, pint_value_type>& k) const;

	// is a pint_const_collection::array_type::key_type
	multikey_index_type
	resolve_sizes(void) const throw (bad_range);

	good_bool
	unroll_resolve_rvalues(const_range_list&, const unroll_context&) const;

	bool
	must_be_formal_size_equivalent(const meta_range_list& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class const_range_list

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_CONST_RANGE_LIST_H__

