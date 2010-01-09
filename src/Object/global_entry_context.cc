/**
	\file "Object/global_entry_context.cc"
	$Id: global_entry_context.cc,v 1.4.46.1 2010/01/09 03:29:57 fang Exp $
 */

#include "Object/global_entry_context.h"
#include "Object/module.h"
#include "Object/global_entry.h"
#include "Object/traits/proc_traits.h"	// for process_tag

namespace HAC {
namespace entity {
//=============================================================================
// class global_entry_context_base method definitions

//-----------------------------------------------------------------------------
// class global_entry_context_base::module_setter method definitions

global_entry_context_base::module_setter::module_setter(
		global_entry_context_base& _ccb, 
		const module& m) :
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		manager_saver_type(_ccb, &m.get_state_manager()),
#endif
		footprint_saver_type(_ccb, &m.get_footprint()) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_entry_context_base::module_setter::~module_setter() { }

//=============================================================================
// class global_entry_context method definitions

//=============================================================================
// class global_entry_context::footprint_frame_setter method definitions

global_entry_context::footprint_frame_setter::footprint_frame_setter(
		global_entry_context& _cc, const footprint_frame& _ff) :
		frame_saver_type(_cc, &_ff) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	\param pid a global process index, corresponding to the 
		processes, whose footprint-frame should be referenced.
 */
global_entry_context::footprint_frame_setter::footprint_frame_setter(
		global_entry_context& _cc, const size_t pid) :
		frame_saver_type(_cc, 
			pid ? &_cc.get_state_manager()->
				get_pool<process_tag>()[pid]._frame
			: NULL) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_entry_context::footprint_frame_setter::~footprint_frame_setter() { }

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

