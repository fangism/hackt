/**
	\file "art_object_control.cc"
	Control-flow related class method definitions.  
 	$Id: art_object_control.cc,v 1.7 2005/01/13 05:28:28 fang Exp $
 */

#ifndef	__ART_OBJECT_CONTROL_CC__
#define	__ART_OBJECT_CONTROL_CC__

#include "art_object_control.h"
#include "persistent_object_manager.tcc"

namespace ART {
namespace entity {
//=============================================================================
// class loop_scope method definitions

loop_scope::loop_scope(never_ptr<const sequential_scope> p) :
		instance_management_base(), sequential_scope(), parent(p) {
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
loop_scope::~loop_scope() {
}

//=============================================================================
// class conditional_scope method definitions

//=============================================================================
} 	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_CONTROL_CC__

