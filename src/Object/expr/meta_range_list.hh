/**
	\file "Object/expr/meta_range_list.hh"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	$Id: meta_range_list.hh,v 1.10 2007/01/21 05:58:55 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_META_RANGE_LIST_H__
#define __HAC_OBJECT_EXPR_META_RANGE_LIST_H__

#include "util/persistent.hh"
#include "util/boolean_types.hh"

//=============================================================================
namespace HAC {
namespace entity {
class const_range_list;
class unroll_context;
struct expr_dump_context;
class nonmeta_expr_visitor;
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
	dump(ostream& o, const expr_dump_context&) const = 0;

	size_t
	dimensions(void) const { return size(); }

virtual	bool
	is_static_constant(void) const = 0;

virtual	bool
	is_relaxed_formal_dependent(void) const = 0;

virtual	const_range_list
	static_overlap(const meta_range_list& r) const = 0;

virtual	good_bool
	unroll_resolve_rvalues(const_range_list&, const unroll_context&) const = 0;

virtual	bool
	must_be_formal_size_equivalent(const meta_range_list& ) const = 0;

virtual	void
	accept(nonmeta_expr_visitor&) const = 0;

};	// end class meta_range_list

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_META_RANGE_LIST_H__

