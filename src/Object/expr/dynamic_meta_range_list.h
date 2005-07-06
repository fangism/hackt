/**
	\file "Object/expr/dynamic_meta_range_list.h"
	Dynamic meta parameter range list class.  
	NOTE: this file was spawned from the old
		"Object/art_object_expr.h" for revision history tracking.  
	$Id: dynamic_meta_range_list.h,v 1.1.2.3 2005/07/06 04:44:41 fang Exp $
 */

#ifndef __OBJECT_EXPR_DYNAMIC_META_RANGE_LIST_H__
#define __OBJECT_EXPR_DYNAMIC_META_RANGE_LIST_H__

#include <vector>
#include "Object/expr/meta_range_list.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class pint_range;
using std::vector;
using util::memory::count_ptr;

//=============================================================================
/**
	Base class.  
	List of range expressions, not necessarily constant.  
	May contain constant ranges however.  
	Interface methods for unrolling.  

	Replace the dynamic subclasses of instance_collection_stack_item
	with this class by adding attributes...
		is_unconditional
		is_loop_independent
	also cache these results...?
 */
class dynamic_meta_range_list : public meta_range_list,
		public vector<count_ptr<pint_range> > {
	typedef	dynamic_meta_range_list			this_type;
protected:
	// list of pointers to pint_ranges?  or just copy construct?
	// can't copy construct, is abstract
	typedef	vector<count_ptr<pint_range> >		list_type;
public:
	typedef	list_type::value_type			value_type;
	typedef	list_type::iterator			iterator;
	typedef	list_type::const_iterator		const_iterator;
	typedef	list_type::reverse_iterator		reverse_iterator;
	typedef	list_type::const_reverse_iterator	const_reverse_iterator;
public:
	dynamic_meta_range_list();

	explicit
	dynamic_meta_range_list(const size_t);

	~dynamic_meta_range_list();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	size(void) const;

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	const_range_list
	static_overlap(const meta_range_list& r) const;
		// false, will be empty
	good_bool
	resolve_ranges(const_range_list& r) const;

	good_bool
	unroll_resolve(const_range_list&, const unroll_context&) const;

	bool
	must_be_formal_size_equivalent(const meta_range_list& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class dynamic_meta_range_list

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_EXPR_DYNAMIC_META_RANGE_LIST_H__

