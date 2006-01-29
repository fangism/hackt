/**
	\file "Object/global_entry_context.cc"
	$Id: global_entry_context.cc,v 1.1.2.1 2006/01/29 04:42:27 fang Exp $
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
		const module& m) : ccb(_ccb) {
	ccb.sm = &m.get_state_manager();
	ccb.fp = &m.get_footprint();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_entry_context_base::module_setter::~module_setter() {
	ccb.sm = NULL;
	ccb.fp = NULL;
}

//=============================================================================
// class global_entry_context method definitions

//=============================================================================
// class global_entry_context::footprint_frame_setter method definitions

global_entry_context::footprint_frame_setter::footprint_frame_setter(
		global_entry_context& _cc, const footprint_frame& _ff) :
		cc(_cc) {
	cc.fpf = &_ff;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_entry_context::footprint_frame_setter::~footprint_frame_setter() {
	cc.fpf = NULL;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

