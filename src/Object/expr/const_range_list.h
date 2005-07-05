/**
	\file "Object/expr/const_range_list.h"
	Classes related to constant expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: const_range_list.h,v 1.1.2.1 2005/07/05 01:16:27 fang Exp $
 */

#ifndef __OBJECT_EXPR_CONST_RANGE_LIST_H__
#define __OBJECT_EXPR_CONST_RANGE_LIST_H__

#include <list>
// #include <vector>
#include "Object/expr/meta_range_list.h"
#include "Object/art_object_index.h"
#include "util/multikey_fwd.h"

//=============================================================================
namespace ART {
namespace entity {
class const_range;
class const_index_list;
using std::list;
// using std::vector;
// using util::multikey_generic;
using util::multikey_generator;
// using util::persistent_object_manager;	// forward declared

//=============================================================================
/**
	List of constant range expressions.  
	Would a vector be more appropriate?   consider changing later...
 */
class const_range_list : public meta_range_list, public list<const_range> {
	typedef	const_range_list			this_type;
protected:
	// no need for pointers here
	typedef	list<const_range>			list_type;
public:
	typedef	list_type::iterator			iterator;
	typedef	list_type::const_iterator		const_iterator;
	typedef	list_type::reverse_iterator		reverse_iterator;
	typedef	list_type::const_reverse_iterator	const_reverse_iterator;
public:
	const_range_list();

	explicit
	const_range_list(const list_type& l);

	explicit
	const_range_list(const const_index_list& i);

	~const_range_list();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	ostream&
	dump_force(ostream& o) const;

	size_t
	size(void) const;

	bool
	is_static_constant(void) const { return true; }

	const_range_list
	static_overlap(const meta_range_list& r) const;

	bool
	is_size_equivalent(const const_range_list& il) const;
	// see also must_be_formal_size_equivalent, declared below

	// may be obsolete by must_be_formal_size_equivalent...
	bool
	operator == (const const_range_list& c) const;

	good_bool
	resolve_ranges(const_range_list& r) const;

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
	resolve_sizes(void) const;

	good_bool
	unroll_resolve(const_range_list&, const unroll_context&) const;

	bool
	must_be_formal_size_equivalent(const meta_range_list& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class const_range_list

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_EXPR_CONST_RANGE_LIST_H__

