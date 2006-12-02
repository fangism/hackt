/**
	\file "sim/chpsim/State.cc"
	Implementation of CHPSIM's state and general operation.  
	$Id: State.cc,v 1.1.2.1 2006/12/02 22:10:12 fang Exp $
 */

#include "sim/chpsim/State.h"

#include <iostream>

namespace HAC {
namespace SIM {
namespace CHPSIM {

//=============================================================================
// class State method definitions

/**
	The main execution engine of chpsim.  
 */
void
State::step(void) {
	// pseudocode:
	// 1) grab event off of pending event queue, dequeue it
	// 2) execute the event (alter state, variables, channel, etc.)
	// 3) check if the alteration of state/variable triggers new events
	//	how do we do this? subscribe-publish/observer approach?
	//	updated variable will have a list of observers to notify
	//		should this list be a static fanout list
	//		(constructible during allocation)
	//		or a dynamic subscriber list?
	//	schedule new pending events that are ready.
	//	if using dynamic subscribers, then create 'blocked' events
	//		and subscribe them to the variables they depend on, 
	//		a form of chaining.  
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC


