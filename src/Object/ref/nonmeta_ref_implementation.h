/**
	\file "Object/ref/nonmeta_ref_implementation.h"
	Policy-based implementations of some nonmeta reference functions.  
 	$Id: nonmeta_ref_implementation.h,v 1.1.2.2 2006/12/26 21:26:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_NONMETA_REF_IMPLEMENTATION_H__
#define	__HAC_OBJECT_REF_NONMETA_REF_IMPLEMENTATION_H__

#include <vector>
#include "Object/traits/class_traits_fwd.h"
#include "util/boolean_types.h"
#include "util/size_t.h"

namespace HAC {
namespace entity {
template <class> class simple_nonmeta_value_reference;
#if 0
class state_manager;
class footprint;
class footprint_frame;
#else
class global_entry_context;
#endif
using util::good_bool;
using std::vector;

//=============================================================================
/**
	Private implementation to this module.
	Overloaded and specialized using tag-inheritance.  
	Code ripped from simple_nonmeta_instance_reference::lookup_may_ref...
 */
template <class reference_type>
// static
good_bool
__nonmeta_instance_lookup_may_reference_indices_impl(
	const reference_type& r, 
#if 0
	const state_manager& sm, const footprint& fp, 
	const footprint_frame* const ff, 
#else
	const global_entry_context&, 
#endif
	vector<size_t>& indices, physical_instance_tag);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Meta-values are not aliasable, value-only semantics.  
	We're done! (no-op).
 */
template <class Tag>
// static
good_bool
__nonmeta_instance_lookup_may_reference_indices_impl(
		const simple_nonmeta_value_reference<Tag>& r, 
#if 0
		const state_manager& sm, const footprint& fp, 
		const footprint_frame* const ff, 
#else
		const global_entry_context&, 
#endif
		vector<size_t>& indices, parameter_value_tag);

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

#endif	// __HAC_OBJECT_REF_NONMETA_REF_IMPLEMENTATION_H__

