/**
	\file "Object/global_entry_context.tcc"
	$Id: global_entry_context.tcc,v 1.4.20.1 2010/01/09 03:29:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_TCC__
#define	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_TCC__

#include "Object/global_entry_context.h"
#include "Object/global_entry.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/unroll/unroll_context.h"
#include "common/TODO.h"

namespace HAC {
namespace entity {
//=============================================================================
// class global_entry_context method definitions

/**
	Looks up globally allocated index using footprint frame
	if this is a local index, else returns the same id as a global id. 
	\param lni the index of the instance local to the footprint (type).
 */
template <class Tag>
size_t
global_entry_context::lookup_global_id(const size_t lni) const {
	INVARIANT(lni);
	// ALERT: some back ends now populate the footprint_frame_pointer
	// for the top-level process, which invalidates this conditional :S
	if (fpf) {
		return footprint_frame_transformer(*fpf, Tag())(lni);
	} else {
		// is top-level footprint
		return lni;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates a meta-instance-reference to globally allocated index.
	This already folds the operation of member-function lookup_global_id.
	\param r the meta-instance-reference to lookup.  
	\return globally allocated index, or 0 if lookup failed.  
 */
template <class Tag>
size_t
global_entry_context::lookup_meta_reference_global_index(
		const simple_meta_instance_reference<Tag>& r) const {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	FINISH_ME_EXIT(Fang);
	return 0;
#else
	NEVER_NULL(sm);
	NEVER_NULL(topfp);
	if (fpf) {
		const unroll_context c(fpf->_footprint, topfp);
		const size_t local_index =
			r.lookup_locally_allocated_index(*sm, c);
		if (!local_index)
			return 0;
		return footprint_frame_transformer(*fpf, Tag())(local_index);
	} else {
		return r.lookup_globally_allocated_index(*sm, *topfp);
	}
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_TCC__


