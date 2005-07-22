/**
	\file "Object/inst/inst_ref_implementation.h"
	Implementation details of instance references.  
 	$Id: inst_ref_implementation.h,v 1.2.2.2 2005/07/22 04:54:33 fang Exp $
 */

#ifndef	__OBJECT_REF_INST_REF_IMPLEMENTATION__
#define	__OBJECT_REF_INST_REF_IMPLEMENTATION__

#include <iostream>
#include "Object/inst/substructure_alias_base.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/ref/inst_ref_implementation_fwd.h"
#include "Object/ref/simple_meta_instance_reference_base.h"
#include "Object/inst/instance_alias.h"
#include "Object/inst/alias_actuals.h"

#include "util/packed_array.h"
#include "util/memory/excl_ptr.h"
#include "util/boolean_types.h"
#include "util/wtf.h"

namespace ART {
namespace entity {
using util::bad_bool;
using util::memory::never_ptr;
#include "util/using_ostream.h"
template <class> class simple_meta_instance_reference;

//=============================================================================
// struct simple_meta_instance_reference_implementation method definitions

/**
	Implementation specializations for meta tyes with substructure.
 */
template <>
struct simple_meta_instance_reference_implementation<true> {
/**
	Called by member_instance_reference::unroll_references.
	This implementation should be policy-determined.  
	\return a single instance alias.
 */
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_reference(
		const typename
			class_traits<Tag>::instance_collection_generic_type& inst,
		const never_ptr<const simple_meta_instance_reference_base::index_list_type> ind, 
		const unroll_context& c) {
	typedef	simple_meta_instance_reference<Tag>	inst_ref_type;
	typedef	typename inst_ref_type::alias_collection_type
						alias_collection_type;
	typedef	never_ptr<substructure_alias>		return_type;
	alias_collection_type aliases;
	const bad_bool
		bad(inst_ref_type::unroll_references_helper(
			c, inst, ind, aliases));
	if (bad.bad) {
		return return_type(NULL);
	} else if (aliases.dimensions()) {
		cerr << "ERROR: got a " << aliases.dimensions() <<
			"-dimension collection where a scalar was required."
			<< endl;
		return return_type(NULL);
	} else {
		// util::wtf_is(aliases.front());
		return aliases.front();
	}
}

};	// end struct simple_meta_instance_reference_implementation<true>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation specializations for meta tyes without substructure.
 */
template <>
struct simple_meta_instance_reference_implementation<false> {
/**
	Technically this should never be called, but is implemented
	merely for interface consistency.  
 */
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_reference(
		const typename
			class_traits<Tag>::instance_collection_generic_type& inst,
		const never_ptr<const simple_meta_instance_reference_base::index_list_type> ind, 
		const unroll_context& c) {
	return never_ptr<substructure_alias>(NULL);
}

};	// end struct simple_meta_instance_reference_implementation<false>

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_REF_INST_REF_IMPLEMENTATION__

