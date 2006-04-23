/**
	\file "Object/global_entry_context.cc"
	$Id: global_entry_context.cc,v 1.3 2006/04/23 07:37:18 fang Exp $
 */

#include "Object/global_entry_context.h"
#include "Object/module.h"

namespace HAC {
namespace entity {
//=============================================================================
// class global_entry_context_base method definitions

//-----------------------------------------------------------------------------
// class global_entry_context_base::module_setter method definitions

global_entry_context_base::module_setter::module_setter(
		global_entry_context_base& _ccb, 
		const module& m) :
		manager_saver_type(_ccb, &m.get_state_manager()),
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
global_entry_context::footprint_frame_setter::~footprint_frame_setter() { }

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

