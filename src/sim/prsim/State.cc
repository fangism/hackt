/**
	\file "sim/prsim/State.cc"
	Implementation of prsim simulator state.  
	$Id: State.cc,v 1.1.2.1 2005/12/15 04:46:06 fang Exp $
 */

#include "sim/prsim/State.h"
#include "util/list_vector.tcc"

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================
// class State method definitions

/**
	TODO: pick reasonable chunk sizes.  
 */
State::State() : node_pool(), expr_pool(), event_pool(), event_queue() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
State::~State() { }

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

