/**
	\file "Object/ref/nonmeta_ref_implementation.h"
	Policy-based implementations of some nonmeta reference functions.  
 	$Id: nonmeta_ref_implementation.h,v 1.1.2.3 2006/12/27 06:01:38 fang Exp $
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
class global_entry_context;
class nonmeta_context_base;
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
	const global_entry_context&, 
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
	const global_entry_context&, 
	vector<size_t>& indices, parameter_value_tag);

//-----------------------------------------------------------------------------
template <class reference_type>
// static
size_t
__nonmeta_instance_global_lookup_impl(
	const reference_type& r, 
	const nonmeta_context_base&, 
	physical_instance_tag);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
// static
size_t
__nonmeta_instance_global_lookup_impl(
	const simple_nonmeta_value_reference<Tag>& r, 
	const nonmeta_context_base&, 
	parameter_value_tag);

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

#endif	// __HAC_OBJECT_REF_NONMETA_REF_IMPLEMENTATION_H__

