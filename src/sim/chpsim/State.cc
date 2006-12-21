/**
	\file "sim/chpsim/State.cc"
	Implementation of CHPSIM's state and general operation.  
	$Id: State.cc,v 1.1.2.16.2.1 2006/12/21 07:09:15 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_STEP			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <iterator>

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
#include "sim/ISE.h"
#include "util/stacktrace.h"
#include "util/iterator_more.h"
#include "util/copy_if.h"

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
using entity::event_subscribers_type;
using std::copy;
using std::back_inserter;
using util::set_inserter;
using util::copy_if;

//=============================================================================
// class State::recheck_transformer definition

/**
	Functor for re-evaluating events for the event-queue.
	Should be allowed to access private members of State.
 */
struct State::recheck_transformer {
	this_type&		state;

	explicit
	recheck_transformer(this_type& s) : state(s) { }

	/**
		\param ei event index to re-evaluate depending on type.
		Appends event to __enqueue_list if ready to fire.
	 */
	void
	operator () (const event_index_type ei) {
		event_type& e(state.event_pool[ei]);
		e.recheck(state.mod.get_state_manager(), 
			state.instances, state.__enqueue_list);
	}
};	// end class recheck_transformer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for enqueuing ready events.  
	Should be allowed to access private members of State.
 */
struct State::event_enqueuer {
	this_type&		state;

	explicit
	event_enqueuer(this_type& s) : state(s) { }

	void
	operator () (const event_index_type);

};	// end class event_enqueuer

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
		flags(FLAGS_DEFAULT), 
		__updated_list(), 
		__enqueue_list() {
	// perform initializations here
	event_pool.reserve(256);
	event_pool.resize(1);		// 0th entry is a dummy
	__updated_list.reserve(16);	// optional pre-allocation
	__enqueue_list.reserve(16);	// optional pre-allocation
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
	//	expect references to the channel/variable(s) affected
	__enqueue_list.clear();
	__updated_list.clear();
	event_pool[ei].execute(mod.get_state_manager(), 
		instances, __updated_list, __enqueue_list);
	// Q: should __updated_list be set-sorted to eliminate duplicates?
	// __updated_list lists variables updated
	// At the same time, enlist the successors for evaluation
	// Don't forget to update countdown!

{
	// list of events to check next
	event_subscribers_type __rechecks;
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
{
	typedef	update_reference_array_type::const_iterator	const_iterator;
	const_iterator ui(__updated_list.begin()), ue(__updated_list.end());
	for ( ; ui!=ue; ++ui) {
		const size_t j = ui->second;
		// switch on the reference type (enum)
		switch (ui->first) {
		// we don't care about variables values, 
		// just collect the affected subscribers
		// set insertion should maintain uniqueness
		// events happen to be sorted by index
		case INSTANCE_TYPE_BOOL: {
			const event_subscribers_type&
				es(instances.get_pool<bool_tag>()[j]
					.get_subscribers());
			copy(es.begin(), es.end(), set_inserter(__rechecks));
			break;
		}
		case INSTANCE_TYPE_INT: {
			const event_subscribers_type&
				es(instances.get_pool<int_tag>()[j]
					.get_subscribers());
			copy(es.begin(), es.end(), set_inserter(__rechecks));
			break;
		}
		case INSTANCE_TYPE_CHANNEL: {
			const event_subscribers_type&
				es(instances.get_pool<channel_tag>()[j]
					.get_subscribers());
			copy(es.begin(), es.end(), set_inserter(__rechecks));
			break;
		}
		// case INSTANCE_TYPE_NULL:	// should not have been added
		default:
			ISE(cerr, cerr << "Unexpected type." << endl;)
		}	// end switch
	}	// end for
}{
	// 4) immediately include this event's successors in list
	//	to evaluate if ready to enqueue.
	// selection of successors will depend on event_type, of course
	// TODO: finish me
}{
	for_each(__rechecks.begin(), __rechecks.end(), 
		recheck_transformer(*this));
	// enqueue any events that are ready to fire
	//	NOTE: check the guard expressions of events before enqueuing
#if 0
	// temporarily disabled for regression testing
	for_each(__enqueue_list.begin(), __enqueue_list.end(),
		event_enqueuer(*this));
#endif
}
}


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


