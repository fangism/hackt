/**
	\file "Object/unroll/loop_scope.cc"
	Control-flow related class method definitions.  
 	$Id: loop_scope.cc,v 1.1.2.1 2005/07/22 21:34:29 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_LOOP_SCOPE_CC__
#define	__OBJECT_UNROLL_LOOP_SCOPE_CC__

#include "Object/unroll/loop_scope.h"
#include "util/persistent_object_manager.tcc"

namespace ART {
namespace entity {
//=============================================================================
// class loop_scope method definitions

loop_scope::loop_scope(const never_ptr<const sequential_scope> p) :
		instance_management_base(), sequential_scope(), parent(p) {
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
loop_scope::~loop_scope() {
}

//=============================================================================
} 	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_UNROLL_LOOP_SCOPE_CC__

