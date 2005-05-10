/**
	\file "Object/art_object_control.cc"
	Control-flow related class method definitions.  
 	$Id: art_object_control.cc,v 1.8 2005/05/10 04:51:11 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_CONTROL_CC__
#define	__OBJECT_ART_OBJECT_CONTROL_CC__

#include "Object/art_object_control.h"
#include "util/persistent_object_manager.tcc"

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

#endif	// __OBJECT_ART_OBJECT_CONTROL_CC__

