/**
	\file "art_object_control.cc"
	Control-flow related class method definitions.  
 	$Id: art_object_control.cc,v 1.5 2004/11/30 01:25:09 fang Exp $
 */

#include "art_object_control.h"
#include "persistent_object_manager.tcc"

namespace ART {
namespace entity {
using namespace std;
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

