/**
	\file "Object/expr/meta_range_list.h"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	$Id: meta_range_list.h,v 1.1.2.1 2005/07/05 01:16:30 fang Exp $
 */

#ifndef __OBJECT_EXPR_META_RANGE_LIST_H__
#define __OBJECT_EXPR_META_RANGE_LIST_H__

#include "util/persistent.h"
#include "util/boolean_types.h"

//=============================================================================
namespace ART {
namespace entity {
class const_range_list;
class unroll_context;
using std::ostream;
using util::persistent;
using util::good_bool;
using util::persistent;

//=============================================================================
/**
	General interface to multidimensional indices.  
	Make interface like std::list.  
	Instance collection stack item?
	Replace instance_collection_stack_item with this!
	Elements of meta_range_list must be meta_range_expr, 
	i.e. fully expanded, no shorthand ranges.  
 */
class meta_range_list : public persistent {
protected:
public:
	meta_range_list() : persistent() { }

virtual	~meta_range_list() { }

virtual	size_t
	size(void) const = 0;

virtual	ostream&
	dump(ostream&) const = 0;

	size_t
	dimensions(void) const { return size(); }

virtual	bool
	is_static_constant(void) const = 0;

virtual	const_range_list
	static_overlap(const meta_range_list& r) const = 0;

// is this obsolete?
virtual	good_bool
	resolve_ranges(const_range_list& r) const = 0;

virtual	good_bool
	unroll_resolve(const_range_list&, const unroll_context&) const = 0;

virtual	bool
	must_be_formal_size_equivalent(const meta_range_list& ) const = 0;

};	// end class meta_range_list

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_EXPR_META_RANGE_LIST_H__

