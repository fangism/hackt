/**
	\file "sim/chpsim/State.cc"
	Implementation of CHPSIM's state and general operation.  
	$Id: State.cc,v 1.1.2.4 2006/12/07 07:48:39 fang Exp $
 */

#include "sim/chpsim/State.h"
#include "sim/chpsim/StateConstructor.h"
#include "Object/module.h"
#include "Object/state_manager.h"

#include <iostream>

namespace HAC {
namespace SIM {
namespace CHPSIM {

//=============================================================================
// class State method definitions

/**
	Will throw exception upon error.  
 */
State::State(const module& m) : 
		mod(m), 
		event_pool(), 
		current_time(0) {
	const state_manager& sm(mod.get_state_manager());

	// perform initializations here
	event_pool.reserve(256);
	event_pool.resize(1);		// 0th entry is a dummy

	StateConstructor v(*this);	// + option flags
	sm.accept(v);	// may throw
	// also top-level footprint
	mod.get_footprint().get_chp_footprint().accept(v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
State::~State() {
	// clean-up
	// optional, but recommended: run some diagnostics
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::initialize(void) {
	current_time = 0;
	// initialize state of all channels and variables
	// seed events that are ready to go, like active initializations
	// register blocked events, pending value/condition changes
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main event-driven execution engine of chpsim.  
	Processes one event at a time.  
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
	// Q: what are successor events blocked on? only guard expressions
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC


