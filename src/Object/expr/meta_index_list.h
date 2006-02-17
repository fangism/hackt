/**
	\file "Object/expr/meta_index_list.h"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	$Id: meta_index_list.h,v 1.6.18.1 2006/02/17 05:07:32 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_META_INDEX_LIST_H__
#define __HAC_OBJECT_EXPR_META_INDEX_LIST_H__

#include "util/persistent.h"
#include "util/memory/pointer_classes_fwd.h"
#include "Object/devel_switches.h"

//=============================================================================
namespace HAC {
namespace entity {
class const_index_list;
class unroll_context;
struct expr_dump_context;
using std::ostream;
using util::persistent;
using util::memory::count_ptr;

//=============================================================================
/**
	May contained mixed pint_expr and ranges!
	Don't forget their interpretation differs!
	pint_expr is interpreted as a dimension collapse, 
	whereas meta_range_expr preserves dimension, even if range is one.  

	Doesn't make sense to ask how many dimensions are in an index_list
	because it depends on the meta_instance_reference to which it is 
	attached.  
	Instead the index list can tell one how may dimensions
	are *collapsed* by the element types.  
 */
class meta_index_list : public persistent {
	typedef	meta_index_list			this_type;
public:
	meta_index_list() : persistent() { }

virtual	~meta_index_list() { }

// copy over most param_expr interface functions...
virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const = 0;

/** NOT THE SAME **/
virtual	size_t
	size(void) const = 0;

virtual	size_t
	dimensions_collapsed(void) const = 0;

virtual	bool
	may_be_initialized(void) const = 0;

virtual	bool
	must_be_initialized(void) const = 0;

virtual	bool
	is_static_constant(void) const = 0;

virtual	bool
	is_relaxed_formal_dependent(void) const = 0;

#if WANT_IS_TEMPLATE_DEPENDENT
virtual	bool
	is_template_dependent(void) const = 0;

virtual	bool
	is_loop_independent(void) const = 0;

virtual	bool
	is_unconditional(void) const = 0;
#endif

virtual	const_index_list
	resolve_index_list(void) const = 0;

	static
	count_ptr<const const_index_list>
	unroll_resolve(const count_ptr<const this_type>&,
		const unroll_context&);

virtual	const_index_list
	unroll_resolve(const unroll_context&) const = 0;

#if 0
virtual	bool
	resolve_multikey(excl_ptr<multikey_index_type>& k) const = 0;
#endif

virtual	bool
	must_be_equivalent_indices(const meta_index_list& ) const = 0;
};	// end class meta_index_list

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_META_INDEX_LIST_H__

