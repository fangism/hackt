// "art_object_control.cc"

#include "art_object_control.h"

namespace ART {
namespace entity {
using namespace std;
//=============================================================================
// class loop_scope method definitions

loop_scope::loop_scope(never_const_ptr<sequential_scope> p) :
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

