/**
	\file "sim/chpsim/State.cc"
	Implementation of CHPSIM's state and general operation.  
	$Id: State.cc,v 1.1.2.10 2006/12/14 23:43:26 fang Exp $
 */

#include "sim/chpsim/State.h"
#include "sim/chpsim/StateConstructor.h"
#include "sim/signal_handler.tcc"
#include "Object/module.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/chan_traits.h"

#include <iostream>

#include "common/TODO.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
#include "util/using_ostream.h"
using entity::bool_tag;
using entity::int_tag;
using entity::channel_tag;
using entity::process_tag;
using entity::global_entry_pool;

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
	const global_entry_pool<bool_tag>& bp(sm.get_pool<bool_tag>());
	const global_entry_pool<int_tag>& ip(sm.get_pool<int_tag>());
	const global_entry_pool<channel_tag>& cp(sm.get_pool<channel_tag>());

	// perform initializations here
	event_pool.reserve(256);
	event_pool.resize(1);		// 0th entry is a dummy

	const size_t bs = bp.size();
	const size_t is = ip.size();
	const size_t cs = cp.size();
	bool_pool.resize(bs);
	int_pool.resize(is);
	channel_pool.resize(cs);

	StateConstructor v(*this);	// + option flags
	// visit top-level footprint
	// v.current_process_index = 0;	// already initialized
	mod.get_footprint().get_chp_footprint().accept(v);
	if (v.last_event_index) {
		v.initial_events.push_back(v.last_event_index);
		// first top-level event
	}
	// visit hierarchical footprints
	sm.accept(v);	// may throw

	// now we have a list of initial_events, use event slot 0 to
	// launch them all concurrently upon startup.
	event_type& init(event_pool[0]);
	init.set_predecessors(0);		// first event, no predecessors
	// init.process_index = 0;		// associate with top-level
	init.set_event_type(EVENT_CONCURRENT_FORK);
	init.successor_events.resize(v.initial_events.size());
	copy(v.initial_events.begin(), v.initial_events.end(),
		&init.successor_events[0]);
	v.count_predecessors(init);	// careful: event optimizations!
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
	//	note: we use event[0] as the launching event
	// register blocked events, pending value/condition changes
	// TODO: for-all events: reset countdown
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
/**
	Prints non-hierarchical structure of the entire allocated state.
	See also: dump_struct_dot() for a graphical view.  
 */
ostream&
State::dump_struct(ostream& o) const {
{
	o << "Variables: " << endl;
	const state_manager& sm(mod.get_state_manager());
	const entity::footprint& topfp(mod.get_footprint());
	{
		const global_entry_pool<bool_tag>& bp(sm.get_pool<bool_tag>());
		const node_index_type bools = bool_pool.size();
		node_index_type i = FIRST_VALID_NODE;
		for ( ; i<bools; ++i) {
			o << "bool[" << i << "]: \"";
			bp[i].dump_canonical_name(o, topfp, sm);
			o << "\" ";
			// no static structural information
			// bool_pool[i].dump_struct(o);
			o << endl;
		}
	}{
		const global_entry_pool<int_tag>& ip(sm.get_pool<int_tag>());
		const node_index_type ints = int_pool.size();
		node_index_type i = FIRST_VALID_NODE;
		for ( ; i<ints; ++i) {
			o << "int[" << i << "]: \"";
			ip[i].dump_canonical_name(o, topfp, sm);
			o << "\" ";
			// no static structural information
			// int_pool[i].dump_struct(o);
			o << endl;
		}
	}{
		const global_entry_pool<channel_tag>&
			cp(sm.get_pool<channel_tag>());
		const node_index_type chans = channel_pool.size();
		node_index_type i = FIRST_VALID_NODE;
		for ( ; i<chans; ++i) {
			o << "chan[" << i << "]: \"";
			cp[i].dump_canonical_name(o, topfp, sm);
			o << "\" ";
			// no static structural information
			// channel_pool[i].dump_struct(o);
			o << endl;
		}
	}
	// repeat for channels
}
	o << endl;
{
// CHP graph structures (non-hierarchical)
	o << "Event graph: " << endl;
	const event_index_type es = event_pool.size();
	event_index_type i = 0;		// FIRST_VALID_EVENT;
	// we use the 0th event to launch initial batch of events
	for ( ; i<es; ++i) {
		o << "event[" << i << "]: ";
		event_pool[i].dump_struct(o);	// << endl;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checkpoint things that are not reconstructible from the object file.
	checklist:
	dynamic subscription state, which encodes which events are
		outstanding and blocked.  
 */
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


