/**
	\file "Object/cflat_context.cc"
	$Id: cflat_context.cc,v 1.1.2.1 2005/12/30 17:41:22 fang Exp $
 */

#include "Object/cflat_context.h"
#include "Object/module.h"

namespace HAC {
namespace entity {
//=============================================================================
// class cflat_context_base method definitions

//-----------------------------------------------------------------------------
// class cflat_context_base::module_setter method definitions

cflat_context_base::module_setter::module_setter(cflat_context_base& _ccb, 
		const module& m) : ccb(_ccb) {
	ccb.sm = &m.get_state_manager();
	ccb.fp = &m.get_footprint();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cflat_context_base::module_setter::~module_setter() {
	ccb.sm = NULL;
	ccb.fp = NULL;
}

//=============================================================================
// class cflat_context method definitions

//=============================================================================
// class cflat_context::footprint_frame_setter method definitions

cflat_context::footprint_frame_setter::footprint_frame_setter(
		cflat_context& _cc, const footprint_frame& _ff) :
		cc(_cc) {
	cc.fpf = &_ff;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cflat_context::footprint_frame_setter::~footprint_frame_setter() {
	cc.fpf = NULL;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

