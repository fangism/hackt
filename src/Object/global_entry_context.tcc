/**
	\file "Object/global_entry_context.tcc"
	$Id: global_entry_context.tcc,v 1.4.20.5 2010/03/26 01:31:24 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_TCC__
#define	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_TCC__

#include "Object/global_entry_context.h"
#include "Object/global_entry.h"
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/def/footprint.h"
#endif
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/unroll/unroll_context.h"
#include "common/TODO.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
//=============================================================================
// class global_entry_context method definitions

/**
	Looks up globally allocated index using footprint frame
	if this is a local index, else returns the same id as a global id. 
	\param lni the index of the instance local to the footprint (type), 
		1-based (alias::instance_index).
	\return 1-based global index.
 */
template <class Tag>
size_t
global_entry_context::lookup_global_id(const size_t lni) const {
	INVARIANT(lni);
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	NEVER_NULL(fpf);
	NEVER_NULL(parent_offset);
	const size_t ports = at_top() ? 0 :
		fpf->_footprint->get_instance_pool<Tag>().port_entries();
if (lni <= ports) {
	// lookup up from the context passed into the current process
	return footprint_frame_transformer(*fpf, Tag())(lni);
} else {
	// TODO: range check lni
	// compute global index based on global offset
	const size_t toffset = parent_offset->global_offset_base<Tag>::offset;
	const size_t global_index = toffset +lni -ports;
	return global_index;
}
#else
	// ALERT: some back ends now populate the footprint_frame_pointer
	// for the top-level process, which invalidates this conditional :S
	if (fpf) {
		return footprint_frame_transformer(*fpf, Tag())(lni);
	} else {
		// is top-level footprint
		return lni;
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates a meta-instance-reference to globally allocated index.
	This already folds the operation of member-function lookup_global_id.
	\param r the meta-instance-reference to lookup.  
	\return globally allocated index (1-based), or 0 if lookup failed.  
 */
template <class Tag>
size_t
global_entry_context::lookup_meta_reference_global_index(
		const simple_meta_instance_reference<Tag>& r) const {
	STACKTRACE_VERBOSE;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	return r.lookup_globally_allocated_index(*this);
#else
	NEVER_NULL(topfp);
	NEVER_NULL(sm);
	if (fpf) {
		const unroll_context c(fpf->_footprint, topfp);
		const size_t local_index =
			r.lookup_locally_allocated_index(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
				*sm,
#endif
				c);
		if (!local_index)
			return 0;
		return footprint_frame_transformer(*fpf, Tag())(local_index);
	} else {
		return r.lookup_globally_allocated_index(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
				*sm,
#endif
				*topfp);
	}
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_TCC__


