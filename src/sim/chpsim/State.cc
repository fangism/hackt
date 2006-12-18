/**
	\file "sim/chpsim/State.cc"
	Implementation of CHPSIM's state and general operation.  
	$Id: State.cc,v 1.1.2.13 2006/12/18 21:28:05 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_STEP			(0 && ENABLE_STACKTRACE)

#include <iostream>

#include "sim/chpsim/State.h"
#include "sim/chpsim/StateConstructor.h"
#include "sim/event.tcc"
#include "sim/signal_handler.tcc"
#include "Object/module.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/chan_traits.h"

#include "common/TODO.h"
#include "util/stacktrace.h"

#if	DEBUG_STEP
#define	DEBUG_STEP_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_STEP		STACKTRACE_VERBOSE
#else
#define	DEBUG_STEP_PRINT(x)
#define	STACKTRACE_VERBOSE_STEP
#endif


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
		instances(m.get_state_manager()), 
		event_pool(), 
		event_queue(), 
		current_time(0), 
		interrupted(false),
		flags(FLAGS_DEFAULT) {
	// perform initializations here
	event_pool.reserve(256);
	event_pool.resize(1);		// 0th entry is a dummy
{
	StateConstructor v(*this);	// + option flags
	// visit top-level footprint
	// v.current_process_index = 0;	// already initialized
	mod.get_footprint().get_chp_footprint().accept(v);
	if (v.last_event_index) {
		v.initial_events.push_back(v.last_event_index);
		// first top-level event
	}
	// visit hierarchical footprints
	const state_manager& sm(mod.get_state_manager());
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
	// empty the event_queue
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
	Fetches next event from the priority queue.  
	Automatically skips and deallocates killed events.  
	NOTE: possible that last event in queue is killed, 
		in which case, need to return a NULL placeholder.  
 */
State::event_placeholder_type
State::dequeue_event(void) {
	STACKTRACE_VERBOSE_STEP;
#if 0
	event_placeholder_type ret(event_queue.pop());
	while (get_event(ret.event_index).killed()) {
		__deallocate_killed_event(ret.event_index);
		if (event_queue.empty()) {
			return event_placeholder_type(
				current_time, INVALID_EVENT_INDEX);
		} else {
			ret = event_queue.pop();
		}
	};
	return ret;
#else
	return event_queue.pop();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main event-driven execution engine of chpsim.  
	Processes one event at a time.  
	\return reference to the variable that was just modified, if any.
 */
State::step_return_type
State::step(void) {
	typedef	step_return_type	return_type;
	// pseudocode:
	// 1) grab event off of pending event queue, dequeue it
	if (event_queue.empty()) {
		return return_type(INSTANCE_TYPE_NULL, INVALID_NODE_INDEX);
	}

	const event_placeholder_type ep(dequeue_event());
	current_time = ep.time;
	DEBUG_STEP_PRINT("time = " << current_time << endl);
	const event_index_type& ei(ep.event_index);
	if (!ei) {
		// possible in the event that last events are killed
		return return_type(INVALID_NODE_INDEX, INVALID_NODE_INDEX);
	}
	DEBUG_STEP_PRINT("event_index = " << ei << endl);
	// no need to deallocate event, they are all statically pre-allocated

	// 2) execute the event (alter state, variables, channel, etc.)
	//	expect a reference to the variable(s) that were affected
	// const instance_reference ret;

	// 3) check if the alteration of state/variable triggers new events
	//	each variable affected has a dynamic set of 
	//		'subscribed' pending events (dynamic fanout)
	//	accumulate set of blocked events to recheck/re-evaluate
	//	re-evaluate list of events, enqueuing/unblocking any events
	//		that are ready to fire.

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

	// TODO: finish me
	return return_type(INVALID_NODE_INDEX, INVALID_NODE_INDEX);
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
	instances.dump_struct(o, sm, topfp);
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
	Any control over dumping options?
	For now we don't bother dumping variables and dependencies.  
	Dependencies are "may" instance sets.
	Consider evaluating sets of anti-dependence (affects) variables.  
	Would be nice to distinguish between may and must variables.  
 */
ostream&
State::dump_struct_dot(ostream& o) const {
	o << "digraph G {" << endl;
{
	o << "# Events: " << endl;
	const event_index_type es = event_pool.size();
	event_index_type i = 0;		// FIRST_VALID_EVENT;
	// we use the 0th event to launch initial batch of events
	const string prefix("EVENT_");
	for ( ; i<es; ++i) {
		// o << "EVENT_" << i << "\t[label=\"" << i << "\"];";
		o << prefix << i << "\t";
		const event_type& e(event_pool[i]);
		e.dump_dot_node(o) << endl;
		// iterate over edges
		const event_index_type* j = &e.successor_events[0];
		const event_index_type* z =
			&e.successor_events[e.successor_events.size()];
		for ( ; j!=z; ++j) {
			const event_index_type h = *j;
			o << prefix << i << " -> " << prefix << h;
			event_pool[h].dump_dot_edge(o) << ';' << endl;
		}
	}
}
	o << "}" << endl;
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
// template class EventQueue<EventPlaceholder<real_time> >;
	// NOTE: already instantiated for PRSIM, factor into common lib?

}	// end namespace SIM
}	// end namespace HAC


