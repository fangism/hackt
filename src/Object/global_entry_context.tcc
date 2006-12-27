/**
	\file "Object/global_entry_context.tcc"
	$Id: global_entry_context.tcc,v 1.1.2.1 2006/12/27 06:01:33 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_TCC__
#define	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_TCC__

#include "Object/global_entry_context.h"
#include "Object/global_entry.h"

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
	if (fpf) {
		// see also footprint_frame_transformer in global_entry.h
#if 1
		// equivalent
		return footprint_frame_transformer(
			fpf->get_frame_map<bool_tag>())(lni);
#else
		return fpf->get_frame_map<Tag>()[lni-1];
#endif
	} else {
		// is top-level footprint
		return lni;
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_TCC__


