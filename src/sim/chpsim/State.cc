/**
	\file "sim/chpsim/State.cc"
	Implementation of CHPSIM's state and general operation.  
	$Id: State.cc,v 1.1.2.7 2006/12/09 07:52:14 fang Exp $
 */

#include "sim/chpsim/State.h"
#include "sim/chpsim/StateConstructor.h"
#include "sim/signal_handler.tcc"
#include "Object/module.h"
#include "Object/state_manager.h"

#include <iostream>

#include "common/TODO.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
#include "util/using_ostream.h"

//=============================================================================
// class State method definitions

/**
	Will throw exception upon error.  
 */
State::State(const module& m) : 
		state_base(m, "chpsim> "), 
		event_pool(), 
		current_time(0), 
		interrupted(false),
		flags(FLAGS_DEFAULT) {
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
void
State::reset(void) {
	// TOOD: write me!
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_event_queue(ostream& o) const {
	return o << "what event queue?" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
State::save_checkpoint(ostream& o) const {
	FINISH_ME(Fang);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
State::load_checkpoint(istream& o) {
	FINISH_ME(Fang);
	return false;
}

//=============================================================================
}	// end namespace CHPSIM

// explicit class instantiation
template class signal_handler<CHPSIM::State>;
}	// end namespace SIM
}	// end namespace HAC


