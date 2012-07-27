/**
	\file "Object/global_entry_context.tcc"
	$Id: global_entry_context.tcc,v 1.8 2010/08/24 21:05:40 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_TCC__
#define	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_TCC__

#include <vector>
#include "Object/global_entry_context.h"
#include "Object/global_entry.h"
#include "Object/def/footprint.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/unroll/unroll_context.h"
#include "common/TODO.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
//=============================================================================
// struct global_entry_context method definitions

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
	NEVER_NULL(fpf);
	NEVER_NULL(parent_offset);
	const size_t ports = at_top() ? 0 :
		fpf->_footprint->
			template get_instance_pool<Tag>().port_entries();
if (lni <= ports) {
	// lookup up from the context passed into the current process
	return footprint_frame_transformer(*fpf, Tag())(lni);
} else {
	// TODO: range check lni
	// compute global index based on global offset
	const size_t toffset = parent_offset->template get_offset<Tag>();
	const size_t global_index = toffset +lni -ports;
	return global_index;
}
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
		const simple_meta_instance_reference<Tag>& r, 
		const unroll_context* u) const {
	STACKTRACE_VERBOSE;
	return r.lookup_globally_allocated_index(*this, u);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_TCC__


