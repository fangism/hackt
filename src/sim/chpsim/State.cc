/**
	\file "sim/chpsim/State.cc"
	Implementation of CHPSIM's state and general operation.  
	$Id: State.cc,v 1.8.2.20 2007/05/01 03:07:38 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_STEP			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <iterator>
#include <functional>

#include "sim/chpsim/State.h"
#include "sim/chpsim/StateConstructor.h"
#include "sim/chpsim/graph_options.h"
#include "sim/event.tcc"
#include "sim/signal_handler.tcc"
#include "sim/chpsim/nonmeta_context.h"
#include "sim/random_time.h"
#include "sim/chpsim/Trace.h"
#include "Object/module.h"
#include "Object/state_manager.h"
#include "Object/global_channel_entry.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/type/canonical_fundamental_chan_type.h"

#include "common/TODO.h"
#include "sim/ISE.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"	// for explicit instantiation
#include "util/iterator_more.h"
#include "util/copy_if.h"
#include "util/IO_utils.h"
#include "util/binders.h"

#if	DEBUG_STEP
#define	DEBUG_STEP_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_STEP		STACKTRACE_VERBOSE
#else
#define	DEBUG_STEP_PRINT(x)
#define	STACKTRACE_VERBOSE_STEP
#endif

// functor specializations
namespace util {
using HAC::SIM::CHPSIM::State;

template <>
struct value_writer<State::event_placeholder_type> {
	ostream&		os;

	explicit
	value_writer(ostream& o) : os(o) { }

	void
	operator () (const State::event_placeholder_type& p) const {
		write_value(os, p.time);
		write_value(os, p.event_index);
		write_value(os, p.cause_event_id);
		write_value(os, p.cause_trace_id);
	}
};	// end struct value_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct value_reader<State::event_placeholder_type> {
	istream&		is;

	explicit
	value_reader(istream& i) : is(i) { }

	void
	operator () (State::event_placeholder_type& p) const {
		read_value(is, p.time);
		read_value(is, p.event_index);
		read_value(is, p.cause_event_id);
		read_value(is, p.cause_trace_id);
	}
};	// end struct value_reader

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace memory {
using HAC::SIM::CHPSIM::State;
// explicit template specialization needed for guile-module
template class count_ptr<State>;
}	// end namespace memory

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace util

//=============================================================================
namespace HAC {
namespace SIM {
namespace CHPSIM {
#include "util/using_ostream.h"
using entity::variable_type;		// from "nonmeta_variable.h"
using entity::bool_tag;
using entity::int_tag;
using entity::enum_tag;
using entity::channel_tag;
using entity::process_tag;
using entity::global_entry_pool;
using entity::global_entry;
using entity::ChannelState;
using entity::event_subscribers_type;
using entity::class_traits;
using entity::meta_type_map;
using entity::META_TYPE_NONE;
using entity::META_TYPE_BOOL;
using entity::META_TYPE_INT;
using entity::META_TYPE_ENUM;
using entity::META_TYPE_CHANNEL;
using entity::META_TYPE_PROCESS;
using entity::canonical_fundamental_chan_type_base;
using std::copy;
using std::back_inserter;
using std::mem_fun_ref;
using std::ostream_iterator;
using std::ptr_fun;
using util::set_inserter;
using util::copy_if;
using util::write_value;
using util::read_value;
using util::value_writer;
using util::value_reader;

//=============================================================================
// class State::recheck_transformer definition

/**
	Functor for re-evaluating events for the event-queue.
	Should be allowed to access private members of State.
 */
struct State::recheck_transformer {
	this_type&		state;
	// other option is to pass in a pre-made nonmeta_context
	nonmeta_context		context;
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	const event_index_type	cause_event_id;
	const size_t		cause_trace_id;
#endif

	explicit
	recheck_transformer(this_type& s
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
		, const event_index_type cei, const size_t cti
#endif
		) : 
		state(s), 
		context(state.mod.get_state_manager(), 
			state.mod.get_footprint(),
			state)
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
			, cause_event_id(cei), cause_trace_id(cti)
#endif
		{ }

	/**
		This should be the ONLY interface for
		events entering the event-queue.  
		(they are sifted into the staging queue, __enqueue_list)
		\param ei event index to re-evaluate depending on type.
		Appends event to __enqueue_list if ready to fire.
		Also manages event-to-variable subscription 
		for this event index.  
	 */
	void
	operator () (const event_index_type ei) {
		STACKTRACE_INDENT_PRINT("rechecking event " << ei << endl);
		event_type& e(state.event_pool[ei]);
		context.set_event(e);
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
		// To accomodate blocking-sends that wake-up probes
		// we must prevent self-wake up in this corner case.
		if (cause_event_id != ei) {
		// Now, the only events that should call this are ones
		// that have been woken up by an update, not first-time.
		// now that delays have been paid up-front, events that
		// pass recheck can be scheduled into the immediate event fifo
		if (e.recheck(context, ei)) {
			// note: time is irrelevant to immediate-event-fifo
			// it's just a NOW fifo
			state.immediate_event_fifo.push_back(
				event_placeholder_type(state.current_time, ei, 
					cause_event_id, cause_trace_id));
			// this is an unfortunate overloaded re-use
			// of event_placeholder_type.
		} // end if recheck
		} // end if self-wake-up
#else	// CHPSIM_DELAYED_SUCCESSOR_CHECKS
		e.recheck(context, ei);
#endif	// CHPSIM_DELAYED_SUCCESSOR_CHECKS
	}

};	// end class recheck_transformer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for enqueuing ready events.  
	Should be allowed to access private members of State.
 */
struct State::event_enqueuer {
	this_type&		state;
	event_index_type	cause_event_id;
	size_t			cause_trace_id;

	event_enqueuer(this_type& s, const event_index_type c, 
			const size_t t) : 
			state(s), cause_event_id(c), cause_trace_id(t)
			{ }

	/**
		Enqueues the event with future time depending on the 
		timing mode.
	 */
	void
	operator () (const event_index_type ei) {
		const event_type& e(state.event_pool[ei]);
		time_type new_delay;
	switch (state.timing_mode) {
	case TIMING_UNIFORM:
		new_delay = (e.has_trivial_delay() ?
			state.get_null_event_delay() :
			state.get_uniform_delay());
		break;
	case TIMING_PER_EVENT:
		new_delay = e.get_delay();
		break;
	case TIMING_RANDOM:
		new_delay = random_time<time_type>()();
		break;
	default: // huh?
		new_delay = event_type::default_delay;
		ISE(cerr, cerr << "unknown timing mode.";)
	}
		const time_type new_time = state.current_time +new_delay;
		const event_placeholder_type
			new_event(new_time, ei, cause_event_id, cause_trace_id);
		if (state.watching_event_queue()) {
			// is this a performance hit, rechecking inside loop?
			// if so, factor this into two versioned loops.
			state.dump_event(cout << "enqueue: ", ei, new_time);
		}
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
		state.check_event_queue.insert(new_event);
#else
		state.event_queue.insert(new_event);
#endif
	}

};	// end class event_enqueuer

//=============================================================================
// class State method definitions

const char
State::event_table_header[] = "\ttime\teid\tpid\tevent";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the event table header, always includeing 'cause'
 */
ostream&
State::dump_event_table_header(ostream& o) {
	return o << event_table_header << "\tcause" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Will throw exception upon error.  
 */
State::State(const module& m) : 
		state_base(m, "chpsim> "), 
		instances(m.get_state_manager()), 
		event_pool(), 
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
		check_event_queue(), 
#else
		event_queue(), 
#endif
		timing_mode(TIMING_DEFAULT), 
		current_time(0), 
		uniform_delay(event_type::default_delay),
		null_event_delay(event_type::default_delay),	// or 0
		interrupted(false),
		flags(FLAGS_DEFAULT), 
		__updated_list(), 
#if !CHPSIM_DELAYED_SUCCESSOR_CHECKS
		__enqueue_list(), 
#endif
		__rechecks(), 
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
		immediate_event_fifo(),
#endif
		event_watches(), 
		event_breaks(),
		value_watches(), 
		value_breaks(), 
		trace_manager(), 
		trace_flush_interval(1L<<16)
		{
	// perform initializations here
	event_pool.reserve(256);
	event_pool.resize(1);		// 0th entry is a dummy
#if !CHPSIM_DELAYED_SUCCESSOR_CHECKS
	__enqueue_list.reserve(16);	// optional pre-allocation
#endif
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
	// anything to do with trace_manager?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializing the simulator resets all simulator state and
	enqueues the 0th spawning event into the event-queue.  
 */
void
State::initialize(void) {
	current_time = 0;
	// initialize state of all channels and variables
	instances.reset();
	// empty the event_queue
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	immediate_event_fifo.clear();
	check_event_queue.clear();
#else
	event_queue.clear();
#endif
	// seed events that are ready to go, like active initializations
	//	note: we use event[0] as the launching event (concurrent)
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	check_event_queue.insert(event_placeholder_type(current_time, 0));
#else
	event_queue.insert(event_placeholder_type(current_time, 0));
#endif
	__updated_list.clear();
#if !CHPSIM_DELAYED_SUCCESSOR_CHECKS
	__enqueue_list.clear();
#endif
	__rechecks.clear();
	for_each(event_pool.begin(), event_pool.end(),
		mem_fun_ref(&event_type::reset)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In addition to initializing the simulator, this also resets
	any modes that may have changed, like a fresh boot.  
	See constructor member initializers for reference.  
 */
void
State::reset(void) {
	initialize();
	uniform_delay = event_type::default_delay;
	null_event_delay = event_type::default_delay;
	timing_mode = TIMING_DEFAULT;
	interrupted = false;
	flags = FLAGS_DEFAULT;
	event_watches.clear();
	event_breaks.clear();
	value_watches.clear();
	value_breaks.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Fetches next event from the priority queue.  
	Automatically skips and deallocates killed events.  
	NOTE: possible that last event in queue is killed, 
		in which case, need to return a NULL placeholder.  
	\pre before calling this, event_queue must not be empty.  
 */
State::event_placeholder_type
State::dequeue_event(void) {
	STACKTRACE_VERBOSE_STEP;
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	const event_placeholder_type ret(*check_event_queue.begin());
	check_event_queue.erase(check_event_queue.begin());
#else
	const event_placeholder_type ret(*event_queue.begin());
	event_queue.erase(event_queue.begin());
#endif
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Peek at the time of the next event.  
	\pre event queue must not be empty (either immedate or checking)
 */
State::time_type
State::next_event_time(void) const {
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	if (!immediate_event_fifo.empty()) {
		return immediate_event_fifo.begin()->time;
	} else {
		INVARIANT(!check_event_queue.empty());
		return check_event_queue.begin()->time;
	}
#else
	INVARIANT(!event_queue.empty());
	return event_queue.begin()->time;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main event-driven execution engine of chpsim.  
	Processes one event at a time.  
	Q: since multiple instances referenced can be altered, 
		should we check against the watch-list here (set-intersection)?
	\return true to break if breakpoint tripped.  
 */
State::step_return_type
State::step(void) {
	STACKTRACE_VERBOSE;
	// pseudocode:
	// 1) grab event off of pending event queue, dequeue it
	nonmeta_context c(mod.get_state_manager(), mod.get_footprint(), *this);
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	std::pair<bool, bool>	status(false, false);
do {
	// TODO: check immediate event FIFO first
	// or check and load into FIFO first?
#endif
	event_placeholder_type ep;
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	bool immediate = false;
	if (immediate_event_fifo.empty()) {
	// TODO: grab and check events until one is ready to execute
	// and execute it.
	if (check_event_queue.empty())
#else
	if (event_queue.empty())
#endif
	{
		return false;
	}
	ep = dequeue_event();
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	} else {
		ep = immediate_event_fifo.front();
		immediate_event_fifo.pop_front();
		immediate = true;
	}	// end if immediate_event_fifo empty
#endif

	current_time = ep.time;
	DEBUG_STEP_PRINT("time = " << current_time << endl);
	const event_index_type ei(ep.event_index);
	// first NULL event has index 0, but nothing else should enqueue it
	DEBUG_STEP_PRINT("event_index = " << ei << endl);
	// no need to deallocate event, they are all statically pre-allocated
	const event_index_type cause_event_id = ep.cause_event_id;
	DEBUG_STEP_PRINT("caused by = " << cause_event_id << endl);
	const size_t cause_trace_id = ep.cause_trace_id;
	DEBUG_STEP_PRINT("at event # = " << cause_trace_id << endl);

	// 2) execute the event (alter state, variables, channel, etc.)
	//	expect references to the channel/variable(s) affected
#if !CHPSIM_DELAYED_SUCCESSOR_CHECKS
	__enqueue_list.clear();
#endif
	__updated_list.clear();
	__rechecks.clear();
	event_type& ev(event_pool[ei]);
	c.set_event(ev);	// not nonmeta_context::event_setter!!!
	// TODO: re-use this nonmeta-context in the recheck_transformer
	// TODO: unify check and execute into "chexecute"
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	try {
	// this is where events are checked for their first time as successor
	if (immediate || ev.first_check(c, ei)) {
		// don't recheck if event is immediate
		status.first = true;
#endif
		try {
			ev.execute(c);
		} catch (...) {
			cerr << "Run-time error executing event "
				<< ei << "." << endl;
			throw;		// rethrow
		}
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
		const size_t cti = (is_tracing() ?
			trace_manager->last_event_trace_id() : 0);
		DEBUG_STEP_PRINT("this trace index = " << cti << endl);
		status.second = __step(ei, cause_event_id, cause_trace_id);
		// enqueue these events for first-checking (after delay)
		for_each(c.first_checks.begin(), c.first_checks.end(),
			event_enqueuer(*this, ei, cti));
		// c.first_checks.clear();	// not needed, loop will exit
#else
	return __step(ei, cause_event_id, cause_trace_id);
#endif
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	} else {	// end if recheck
		// TODO: is this on critical path?
		// initial check failed predicate, block waiting, 
		// subscribe to variables
		STACKTRACE_INDENT_PRINT("event blocked waiting." << endl);
		// this is already done in Event::recheck
		// NOTE: a blocked channel send alters channel state
		// so we need to recheck waiting probes
		// CAUTION: self-modifying blocked events will effectively
		// wake themselves up! taken care of in recheck_transformer.
	if (!__updated_list.empty()) {
		// INVARIANT: only channels can appear in this list!
		__notify_updates_for_recheck_no_trace<channel_tag>(cause_trace_id);
		// newly unblocked events will appear in immedate_event_fifo.
		__perform_rechecks(ei, cause_event_id, cause_trace_id);
	}
		// DILEMMA: cause_trace_index is not exactly applicable
		// because the blocking send hasn't actually executed yet,
		// it is coupled with the receive, in the future!
		// The best we can do is associate the last arriving cause
		// with the cause of event being considered.  :-/
	}
	} catch (...) {
		// exception can now occur on first check, just like recheck
		cerr << "Run-time error while checking event." << endl;
		cerr << "event[" << ei << "]:";
		dump_event(cerr, ei, current_time);
		if (cause_event_id) {
			cerr << "\t[by:" << cause_event_id << ']';
		}
		cerr << endl;
		throw;
	}
} while (!status.first);
	return status.second;
#endif
}	// end method step()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper routine for converting the updated reference list to
	a set of events to recheck.
	This also records in the trace file the state of modified variables
		(when tracing is on, of course).
	\param ti is the current event's trace index.  
 */
void
State::__notify_updates_for_recheck(const size_t ti) {
	typedef	update_reference_array_type::ref_bin_type::const_iterator
							const_iterator;
	// TODO: hello: template method, anybody?
#define	CASE_META_TYPE_TAG(Tag)						\
	{								\
	const update_reference_array_type::ref_bin_type&		\
		ub(__updated_list.ref_bin				\
			[class_traits<Tag>::type_tag_enum_value]);	\
	const_iterator ui(ub.begin()), ue(ub.end());			\
	for ( ; ui!=ue; ++ui) {						\
		const variable_type<Tag>::type&				\
			v(instances.get_pool<Tag>()[*ui]);		\
		if (is_tracing()) {					\
			trace_manager->current_chunk			\
				.push_back<Tag>(v, ti, *ui);		\
		}							\
		const event_subscribers_type& es(v.get_subscribers());	\
		copy(es.begin(), es.end(), set_inserter(__rechecks));	\
	}								\
	}
	CASE_META_TYPE_TAG(bool_tag)
	CASE_META_TYPE_TAG(int_tag)
	CASE_META_TYPE_TAG(enum_tag)
	CASE_META_TYPE_TAG(channel_tag)
#undef	CASE_META_TYPE_TAG
}	// end __notify_updates_for_recheck()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
/**
	This variation only processes updates and rechecks without
	saving the state of modified variables to trace.  
	Checkpoints, however, will still correctly pick up the current
	value of any variable at any trace point.  
 */
template <class Tag>
void
State::__notify_updates_for_recheck_no_trace(const size_t ti) {
	typedef	update_reference_array_type::ref_bin_type::const_iterator
							const_iterator;
	const update_reference_array_type::ref_bin_type&
		ub(__updated_list.ref_bin
			[class_traits<Tag>::type_tag_enum_value]);
	const_iterator ui(ub.begin()), ue(ub.end());
	for ( ; ui!=ue; ++ui) {
		const typename variable_type<Tag>::type&
			v(instances.template get_pool<Tag>()[*ui]);
		const event_subscribers_type& es(v.get_subscribers());
		copy(es.begin(), es.end(), set_inserter(__rechecks));
	}
}	// end __notify_updates_for_recheck()
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper routine to recheck all events accumulated in the 
	__rechecks list member.  
	\throw exception on fatal errors.  
 */
void
State::__perform_rechecks(const event_index_type ei, 
		const event_index_type cause_event_id
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
		, const size_t ti
#endif
		) {
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
// NOTE: since delays have been paid for up front, rechecked events that
// now pass are immediately ready for execution, and hence should be placed
// in the immediate_event_fifo.  Change happens in recheck_transformer.
#endif
try {
	for_each(__rechecks.begin(), __rechecks.end(), 
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
		recheck_transformer(*this, ei, ti)
#else
		recheck_transformer(*this)
#endif
	);
} catch (...) {
	cerr << "Run-time error while rechecking events." << endl;
	cerr << "event[" << ei << "]:";
	dump_event(cerr, ei, current_time);
	if (cause_event_id) {
		cerr << "\t[by:" << cause_event_id << ']';
	}
	cerr << endl;
	throw;
}
}	// end __perform_rechecks()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Internal step method, factored out for re-usability.  
	This processes a step after its execution.  
 */
State::step_return_type
State::__step(const event_index_type ei, 
		const event_index_type cause_event_id,
		const size_t cause_trace_id) {
	typedef	step_return_type	return_type;
	return_type event_trig = false;
	// event tracing
	size_t ti = 0;	// because we'll want to reference it later...
	if (is_tracing()) {
		// should only be true if trace opening succeeded
		NEVER_NULL(trace_manager);
		ti = trace_manager->push_back_event(
			event_trace_point(current_time, ei, cause_trace_id));
	}
	// __updated_list lists variables updated
	// Q: should __updated_list be set-sorted to eliminate duplicates?
	//	Yes, but do this later...
	// At the same time, enlist the successors for evaluation
	// Don't forget to update countdown!
#if DEBUG_STEP
	// debugging: 
	// print the list of references affected
	dump_updated_references(cout);
	// print the list of successor events scheduled for recheck
	dump_recheck_events(cout);
#endif
	// list of events to check next
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
	__notify_updates_for_recheck(ti);
	// 4) immediately include this event's successors in list
	//	to evaluate if ready to enqueue.
	// selection of successors will depend on event_type, of course
	//	This has been folded into the execution of this event.
#if DEBUG_STEP
	// debug: print list of events to recheck
	dump_recheck_events(cout);
#endif
	__perform_rechecks(ei, cause_event_id
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
		, ti
#endif
	);
	// TODO: factor out watch/break into subroutine
	bool value_trig = false;
	bool value_break = false;
{
	typedef	global_references_set::ref_bin_type::const_iterator
							const_iterator;
	global_references_set watch_matches;
	__updated_list.set_intersection(value_watches, watch_matches);
	// the following is slow, but fortunately infrequent
#define	PRINT_WATCHED_VALUES(Tag)					\
{									\
	const global_references_set::ref_bin_type&			\
		ub(watch_matches.ref_bin				\
			[class_traits<Tag>::type_tag_enum_value]);	\
	if (ub.size()) {						\
		cout << "updated " << class_traits<Tag>::tag_name <<	\
			"(s): " << endl;				\
		const_iterator ui(ub.begin()), ue(ub.end());		\
		for ( ; ui!=ue; ++ui) {					\
			print_instance_name_value(cout, 		\
				global_indexed_reference(		\
				class_traits<Tag>::type_tag_enum_value,	\
				*ui)) << endl;				\
		}							\
		value_trig = true;					\
	}								\
}
	PRINT_WATCHED_VALUES(bool_tag)
	PRINT_WATCHED_VALUES(int_tag)
	PRINT_WATCHED_VALUES(enum_tag)
	PRINT_WATCHED_VALUES(channel_tag)
#undef	PRINT_WATCHED_VALUES
	// see if any updated values are breakpoints
	if (value_trig) {
		global_references_set break_matches;
		value_watches.set_intersection(value_breaks, break_matches);
		if (!break_matches.empty()) {
			value_break = true;
		}
	}
}
	// is event being watched? or were any watched values updated?
	event_trig = (event_watches.find(ei) != event_watches.end());
	if (watching_all_events() || event_trig || value_trig) {
		if (event_trig || value_trig) {
			cout << "watch:";
		}
		dump_event(cout, ei, current_time);
		if (showing_cause() && cause_event_id) {
			cout << "\t[by:" << cause_event_id << ']';
		}
		cout << endl;
	}
	// enqueue any events that are ready to fire
	//	NOTE: check the guard expressions of events before enqueuing
	// temporarily disabled for regression testing
#if DEBUG_STEP
#if !CHPSIM_DELAYED_SUCCESSOR_CHECKS
	// debug: print list of event to enqueue
	dump_enqueue_events(cout);
#endif
#endif
	// transfer events from staging queue to event queue, 
	// and schedule them with some delay
#if !CHPSIM_DELAYED_SUCCESSOR_CHECKS
	for_each(__enqueue_list.begin(), __enqueue_list.end(),
		event_enqueuer(*this, ei, ti)
	);
#endif
	// check for flush period
	// TODO: count events in State, shouldn't depend on TraceManager for it.
	if (is_tracing() && 
		trace_manager->current_event_count() >= trace_flush_interval) {
		trace_manager->flush();
	}
	const bool event_break =
		event_trig && (event_breaks.find(ei) != event_breaks.end());
	return value_break || event_break;
}	// end __step() method

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print the current timing mode.  
 */
ostream&
State::dump_timing(ostream& o) const {
	o << "timing: ";
switch (timing_mode) {
	case TIMING_PER_EVENT:
		o << "per-event";
		break;
	case TIMING_UNIFORM:
		o << "uniform (" << uniform_delay << ")";
		break;
	case TIMING_RANDOM:
		o << "random";
		break;
	default: o << "unknown";
}
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if there is an error.  
 */
bool
State::set_timing(const string& m, const string_list& a) {
	/// use delay assign to event upon construction
	static const string __per_event("per-event");	// use event's delay
	/// use uniform_delay, possibly overridden for null-event-delay
	static const string __uniform("uniform");	// fixed delay (default)
	static const string __random("random");
	static const string __default("default");
	if (m == __per_event) {
		timing_mode = TIMING_PER_EVENT;
	} else if (m == __uniform) {
		timing_mode = TIMING_UNIFORM;
	} else if (m == __random) {
		timing_mode = TIMING_RANDOM;
		switch (a.size()) {
		case 0: break;
		case 1: {
			FINISH_ME(Fang);
			cerr << "TODO: plant random seed." << endl;
			break;
		default: return true;	// error
		}
		}
	} else if (m == __default) {
		timing_mode = TIMING_DEFAULT;
		// ignore other arguments
	} else {
		return true;
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Keep this help consistent with the implementation of set_timing().  
 */
ostream&
State::help_timing(ostream& o) {
	o << "available timing modes:\n"
	"\tuniform -- use \'uniform-delay\' to set delay\n"
	"\tper-event -- use event-specific constant delay\n"
	"\trandom -- use random delay\n"
	"\tdefault -- (uniform)" << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: maintain breaks as subset of watches.  
	\param ei the event of the index to break on. 
 */
void
State::watch_event(const event_index_type ei) {
	if (ei < event_pool.size()) {
		event_watches.insert(ei);
	} else {
		cerr << "Invalid event index: " << ei << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: maintain breaks as subset of watches.  
	\param ei the event of the index to break on. 
 */
void
State::unwatch_event(const event_index_type ei) {
	if (ei < event_pool.size()) {
		// don't remove if it is a breakpoint
		if (event_breaks.find(ei) != event_breaks.end()) {
			cerr << "WARNING: not de-listing because event " << ei 
				<< " is also listed as a breakpoint." << endl;
			cerr << "To remove this watchpoint, use \'unbreak-event.\'"
				<< endl;
		} else {
			event_watches.erase(ei);
		}
	} else {
		cerr << "Invalid event index: " << ei << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Remove all events that are NOT break points.  
	NOTE: breaks is subset of watches
 */
void
State::unwatch_all_events(void) {
	event_watches = event_breaks;
	if (event_watches.size()) {
		cerr << 
"NOTE: break-events were not removed from the watch-event list." << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print list of watched events.  
 */
ostream&
State::dump_watch_events(ostream& o) const {
	o << "watch events: ";
	switch (event_watches.size()) {
	case 0: break;
	case 1: o << *event_watches.begin(); break;
	default: {
		const event_watch_list_type::const_iterator
			l(--event_watches.end());
		copy(event_watches.begin(), l, 
			ostream_iterator<event_index_type>(o, ", "));
		o << *l;
	}
	}
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: maintain breaks as subset of watches.  
	\param ei the event of the index to break on. 
 */
void
State::break_event(const event_index_type ei) {
	if (ei < event_pool.size()) {
		event_breaks.insert(ei);
		event_watches.insert(ei);
	} else {
		cerr << "Invalid event index: " << ei << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: maintain breaks as subset of watches.  
	\param ei the event of the index to break on. 
 */
void
State::unbreak_event(const event_index_type ei) {
	if (ei < event_pool.size()) {
		event_breaks.erase(ei);
		event_watches.erase(ei);
	} else {
		cerr << "Invalid event index: " << ei << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Removes breakpoints from the watch list and break list.
 */
void
State::unbreak_all_events(void) {
	event_watch_list_type temp;
	std::set_difference(event_watches.begin(), event_watches.end(), 
		event_breaks.begin(), event_breaks.end(), set_inserter(temp));
	event_breaks.clear();
	event_watches.swap(temp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print list of breaking events.  
 */
ostream&
State::dump_break_events(ostream& o) const {
	o << "break events: ";
	switch (event_breaks.size()) {
	case 0: break;
	case 1: o << *event_breaks.begin(); break;
	default: {
		const event_watch_list_type::const_iterator
			l(--event_breaks.end());
		copy(event_breaks.begin(), l, 
			ostream_iterator<event_index_type>(o, ", "));
		o << *l;
	}
	}
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param g pair<type,index>, must be well-formed.  
 */
void
State::watch_value(const global_indexed_reference& g) {
	value_watches.ref_bin[g.first].insert(g.second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Remove variable from watch list if it is not a break point.  
 */
void
State::unwatch_value(const global_indexed_reference& g) {
	const global_references_set::ref_bin_type&
		vb(value_breaks.ref_bin[g.first]);
	if (vb.find(g.first) != vb.end()) {
		cerr << "WARNING: not un-watching because variable (" << 
			g.first << "," << g.second <<
			") is also listed as a breakpoint." << endl;
		cerr << "To remove this watchpoint, use \'unbreak-value.\'"
			<< endl;
	} else {
		value_watches.ref_bin[g.first].erase(g.second);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reminder: breaks is subset of watches.  
 */
void
State::unwatch_all_values(void) {
	value_watches = value_breaks;
	if (!value_breaks.empty()) {
		cout <<
"NOTE: break-values were not removed from the watch-value list." << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints name and value of *all* watched variables.  
 */
ostream&
State::dump_watch_values(ostream& o) const {
	typedef	global_references_set::ref_bin_type::const_iterator
							const_iterator;
	o << "watched values:" << endl;
#define	PRINT_WATCHED_VALUES(Tag)					\
{									\
	const global_references_set::ref_bin_type&			\
		ub(value_watches.ref_bin				\
			[class_traits<Tag>::type_tag_enum_value]);	\
	if (ub.size()) {						\
		cout << class_traits<Tag>::tag_name << "(s): " << endl;	\
		const_iterator ui(ub.begin()), ue(ub.end());		\
		for ( ; ui!=ue; ++ui) {					\
			print_instance_name_value(cout, 		\
				global_indexed_reference(		\
				class_traits<Tag>::type_tag_enum_value,	\
				*ui)) << endl;				\
		}							\
	}								\
}
	PRINT_WATCHED_VALUES(bool_tag)
	PRINT_WATCHED_VALUES(int_tag)
	PRINT_WATCHED_VALUES(enum_tag)
	PRINT_WATCHED_VALUES(channel_tag)
#undef	PRINT_WATCHED_VALUES
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Variable is added as a breakpoint AND watchpoint. 
	\param g pair<type,index>, must be well-formed.  
 */
void
State::break_value(const global_indexed_reference& g) {
	value_watches.ref_bin[g.first].insert(g.second);
	value_breaks.ref_bin[g.first].insert(g.second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Remove variable from watch list if it is not a break point.  
 */
void
State::unbreak_value(const global_indexed_reference& g) {
	value_watches.ref_bin[g.first].erase(g.second);
	value_breaks.ref_bin[g.first].erase(g.second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::unbreak_all_values(void) {
	global_references_set temp;
	value_watches.set_difference(value_breaks, temp);
	value_breaks.clear();
	value_watches = temp;	// no swap member function, drat
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints name and value of *all* breakpoint variables.  
 */
ostream&
State::dump_break_values(ostream& o) const {
	typedef	global_references_set::ref_bin_type::const_iterator
							const_iterator;
	o << "break values:" << endl;
#define	PRINT_WATCHED_VALUES(Tag)					\
{									\
	const global_references_set::ref_bin_type&			\
		ub(value_breaks.ref_bin				\
			[class_traits<Tag>::type_tag_enum_value]);	\
	if (ub.size()) {						\
		cout << class_traits<Tag>::tag_name << "(s): " << endl;	\
		const_iterator ui(ub.begin()), ue(ub.end());		\
		for ( ; ui!=ue; ++ui) {					\
			print_instance_name_value(cout, 		\
				global_indexed_reference(		\
				class_traits<Tag>::type_tag_enum_value,	\
				*ui)) << endl;				\
		}							\
	}								\
}
	PRINT_WATCHED_VALUES(bool_tag)
	PRINT_WATCHED_VALUES(int_tag)
	PRINT_WATCHED_VALUES(enum_tag)
	PRINT_WATCHED_VALUES(channel_tag)
#undef	PRINT_WATCHED_VALUES
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if result is successful/good.  
 */
bool
State::open_trace(const string& tfn) {
	if (trace_manager) {
		cerr << "Error: trace stream already open." << endl;
	}
	trace_manager = excl_ptr<TraceManager>(new TraceManager(tfn));
	NEVER_NULL(trace_manager);
	if (trace_manager->good()) {
		flags |= FLAG_TRACE_ON;
		return true;
	} else {
		stop_trace();
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Invoke this if you want tracing to end early.  
 */
void
State::close_trace(void) {
if (trace_manager) {
	// destroying the trace manager should cause it to finish writing out.
	trace_manager = excl_ptr<TraceManager>(NULL);
}
	stop_trace();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints event in brief form for queue, with time prefixed.  
 */
ostream&
State::dump_event(ostream& o, const event_index_type ei,
		const time_type t) const {
	const event_type& ev(get_event(ei));
	o << '\t' << t << '\t';
	o << ei << '\t';
	return ev.dump_brief(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps structural information for a single event.  
 */
ostream&
State::dump_event(ostream& o, const event_index_type ei) const {
	INVARIANT(ei < event_pool.size());
	o << "event[" << ei << "]: ";
	const event_type& e(event_pool[ei]);
	e.dump_struct(o);	// << endl;
	e.dump_source(o << "source: ") << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reports whether or not event is currently subscribed to its
	dependencies (all or none).  
	If it lacks dependencies, then say so.  
 */
ostream&
State::dump_event_status(ostream& o, const event_index_type ei) const {
	INVARIANT(ei < event_pool.size());
	get_event(ei).dump_subscribed_status(o << "status: ", instances, ei)
		<< endl;
{
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	// NOTE: this means that an event is scheduled to be CHECKED not
	// necessarily executed.  Only events in immediate_event_fifo are
	// are guaranteed to execute.
	o << "in queue: ";
	if (find_if(immediate_event_fifo.begin(), immediate_event_fifo.end(), 
			event_placeholder_type::index_finder(ei))
			== immediate_event_fifo.end()) {
		o << "no";
	} else {
		o << "yes";
	}
	const event_queue_type& temp(check_event_queue);	// just alias
	o << "\nin check queue: ";
#else
	const event_queue_type& temp(event_queue);	// just alias
	o << "in queue: ";
#endif
	if (find_if(temp.begin(), temp.end(), 
			event_placeholder_type::index_finder(ei))
			== temp.end()) {
		o << "no";
	} else {
		o << "yes";
	}
}
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the event queue.  
	If using priority queue, beware of unstable orderings.  
 */
ostream&
State::dump_event_queue(ostream& o) const {
	o << "event queue:" << endl;
	bool empty = true;
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
// print the check-event-queue separately
{
	typedef	immediate_event_queue_type::const_iterator	const_iterator;
	const_iterator i(immediate_event_fifo.begin()),
		e(immediate_event_fifo.end());
	if (i!=e) {
		empty = false;
		o << event_table_header;
		if (showing_cause())
			o << "\tcause";
		o << endl;
		for ( ; i!=e; ++i) {
			dump_event(o, i->event_index, i->time);
			if (showing_cause() && i->cause_event_id) {
				o << "\t[by:" << i->cause_event_id << ']';
			}
			o << endl;
		}
	}
}
	o << "check queue:" << endl;
	const event_queue_type& temp(check_event_queue);	// just alias
#else
	const event_queue_type& temp(event_queue);	// just alias
#endif
{
	typedef	event_queue_type::const_iterator	const_iterator;
	const_iterator i(temp.begin()), e(temp.end());
	if (i!=e) {
		if (empty) {
			o << event_table_header;
			empty = false;
			if (showing_cause())
				o << "\tcause";
			o << endl;
		}
		for ( ; i!=e; ++i) {
			dump_event(o, i->event_index, i->time);
			if (showing_cause() && i->cause_event_id) {
				o << "\t[by:" << i->cause_event_id << ']';
			}
			o << endl;
		}
	}
}
	if (empty) {
		// deadlocked!
		o << "\t(empty)" << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
/**
	Synonym to original definition, because too lazy to
	rename with new check-event changes.  
 */
ostream&
State::dump_check_event_queue(ostream& o) const {
	return dump_event_queue(o);
}
#endif

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
State::dump_struct_dot(ostream& o, const graph_options& g) const {
	o << "digraph G {" << endl;
{
if (g.show_instances) {
	o << "# Instances: " << endl;
	mod.get_state_manager().dump_dot_instances(o, mod.get_footprint());
}

	o << "# Events: " << endl;
	const event_index_type es = event_pool.size();
	event_index_type i = 0;		// FIRST_VALID_EVENT;
	// we use the 0th event to launch initial batch of events
	for ( ; i<es; ++i) {
		const event_type& e(event_pool[i]);
		e.dump_dot_node(o, i, g) << endl;
		// includes outgoing edges
	}
}
	o << "}" << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_updated_references(ostream& o) const {
	typedef	update_reference_array_type::ref_bin_type::const_iterator
							const_iterator;
	o << "updated references:" << endl;

#define	CASE_PRINT_TYPE_TAG_NAME(Tag)					\
{									\
	const update_reference_array_type::ref_bin_type&		\
		ub(__updated_list.ref_bin				\
			[class_traits<Tag>::type_tag_enum_value]);	\
	const_iterator i(ub.begin()), e(ub.end());			\
	for ( ; i!=e; ++i) {						\
		o << class_traits<Tag>::tag_name << '[' << *i << "], ";	\
	}								\
}
	CASE_PRINT_TYPE_TAG_NAME(bool_tag)
	CASE_PRINT_TYPE_TAG_NAME(int_tag)
	CASE_PRINT_TYPE_TAG_NAME(enum_tag)
	CASE_PRINT_TYPE_TAG_NAME(channel_tag)
#undef	CASE_PRINT_TYPE_TAG_NAME
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_recheck_events(ostream& o) const {
	o << "recheck events: ";
	ostream_iterator<size_t> osi(o, ", ");
	copy(__rechecks.begin(), __rechecks.end(), osi);
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_DELAYED_SUCCESSOR_CHECKS
ostream&
State::dump_enqueue_events(ostream& o) const {
	o << "to be enqueued: ";
	ostream_iterator<size_t> osi(o, ", ");
	copy(__enqueue_list.begin(), __enqueue_list.end(), osi);
	return o << endl;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation for the `get' command.  
	Prints canonical name and current value.  
 */
ostream&
State::print_instance_name_value(ostream& o,
		const global_indexed_reference& g) const {
	const state_manager& sm(mod.get_state_manager());
	const entity::footprint& topfp(mod.get_footprint());
	switch (g.first) {
	case META_TYPE_BOOL: {
		o << "bool ";
		sm.get_pool<bool_tag>()[g.second]
			.dump_canonical_name(o, topfp, sm);
		o << " = ";
		o << size_t(instances.get_pool<bool_tag>()[g.second].value);
		break;
	}
	case META_TYPE_INT: {
		o << "int ";
		sm.get_pool<int_tag>()[g.second]
			.dump_canonical_name(o, topfp, sm);
		o << " = ";
		o << instances.get_pool<int_tag>()[g.second].value;
		break;
	}
	case META_TYPE_ENUM: {
		o << "enum ";
		sm.get_pool<enum_tag>()[g.second]
			.dump_canonical_name(o, topfp, sm);
		o << " = ";
		o << instances.get_pool<enum_tag>()[g.second].value;
		break;
	}
	case META_TYPE_CHANNEL: {
		const global_entry<channel_tag>&
			c(sm.get_pool<channel_tag>()[g.second]);
		const ChannelState&
			nc(instances.get_pool<channel_tag>()[g.second]);
		const canonical_fundamental_chan_type_base& t(*c.channel_type);
		t.dump(o) << ' ';
		c.dump_canonical_name(o, topfp, sm);
		o << " = ";
		nc.dump(o, t);	// print the channel data using type info
		break;
	}
	case META_TYPE_PROCESS:
		// print canonical name?
		o << "(process)";	// has no 'value'
		break;
	default:
		o << "(unsupported)";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 */
ostream&
State::print_instance_name_subscribers(ostream& o,
		const global_indexed_reference& g) const {
	const state_manager& sm(mod.get_state_manager());
	const entity::footprint& topfp(mod.get_footprint());
	switch (g.first) {
	case META_TYPE_BOOL: {
		o << "bool ";
		sm.get_pool<bool_tag>()[g.second]
			.dump_canonical_name(o, topfp, sm);
		o << " : ";
		instances.get_pool<bool_tag>()[g.second].dump_subscribers(o);
		break;
	}
	case META_TYPE_INT: {
		o << "int ";
		sm.get_pool<int_tag>()[g.second]
			.dump_canonical_name(o, topfp, sm);
		o << " : ";
		instances.get_pool<int_tag>()[g.second].dump_subscribers(o);
		break;
	}
	case META_TYPE_ENUM: {
		o << "enum ";
		sm.get_pool<enum_tag>()[g.second]
			.dump_canonical_name(o, topfp, sm);
		o << " : ";
		instances.get_pool<enum_tag>()[g.second].dump_subscribers(o);
		break;
	}
	case META_TYPE_CHANNEL: {
		const global_entry<channel_tag>&
			c(sm.get_pool<channel_tag>()[g.second]);
		const ChannelState&
			nc(instances.get_pool<channel_tag>()[g.second]);
		c.dump_canonical_name(o, topfp, sm);
		o << " : ";
		nc.dump_subscribers(o);	// print the channel subscribers
		break;
	}
	case META_TYPE_PROCESS:
		// print canonical name?
		o << "(process)";	// has no 'value'
		break;
	default:
		o << "(unsupported)";
	}
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints all variables with subscribers, and their precise
	subscriber sets.  
	Omits unsubscribed variables.  
 */
ostream&
State::print_all_subscriptions(ostream& o) const {
	return instances.dump_all_subscriptions(o, 
		mod.get_state_manager(), mod.get_footprint());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checkpoint things that are not reconstructible from the object file.
	checklist:
	dynamic subscription state, which encodes which events are
		outstanding and blocked.  
	TODO: save and re-confirm chpsim options, because it affects
		the event graph allocationa and enumeration.  
	TODO: running event count, even when not tracing...
	\return true to signal an error
 */
bool
State::save_checkpoint(ostream& o) const {
	// save some flags?
	// save the state of all instances
	if (instances.save_checkpoint(o)) {
		return true;
	}
	// and which events are subscribed
{
	vector<event_index_type> tmp;
	size_t i=1;
	for ( ; i<event_pool.size(); ++i) {
		if (event_pool[i].is_subscribed(instances, i))
			tmp.push_back(i);
	}
	size_t s = tmp.size();
	write_value(o, s);
	for_each(tmp.begin(), tmp.end(), value_writer<size_t>(o));
}{
	// save the event queue
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	size_t s = immediate_event_fifo.size();
	write_value(o, s);
	for_each(immediate_event_fifo.begin(), immediate_event_fifo.end(), 
		value_writer<event_placeholder_type>(o));
}{
	const event_queue_type& temp(check_event_queue);	// just alias
#else
	const event_queue_type& temp(event_queue);	// just alias
#endif
	size_t s = temp.size();
	write_value(o, s);
	for_each(temp.begin(), temp.end(), 
		value_writer<event_placeholder_type>(o));
}
	// save current time
	write_value(o, current_time);
	// delay settings?
#if 0
	write_value(o, uniform_delay);
	write_value(o, null_event_delay);
#endif
//	write_value(o, flags);
	// skip trace manager
	// save checkpoint interval?
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores simulation state from checkpoint.  
	This simulation checkpoint must use the same object file and 
	invocation options.  
	\return true to signal an error
 */
bool
State::load_checkpoint(istream& i) {
	// restore data/channel/variable state
	if (instances.load_checkpoint(i)) {
		return true;
	}
	// restore event subscription state
{
	size_t j = 0;
	size_t s;
	read_value(i, s);
	const nonmeta_context
		c(mod.get_state_manager(), mod.get_footprint(), *this);
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	immediate_event_fifo.clear();
	check_event_queue.clear();
#else
	event_queue.clear();
#endif
	for ( ; j<s; ++j) {
		event_index_type ei;
		read_value(i, ei);
		INVARIANT(ei);
		if (ei >= event_pool.size()) {
			cerr << "FATAL: event index out-of-bounds!\n"
				"Simulation is in an incoherent state!  "
				"Recommend re-initializing." << endl;
			// THROW_EXIT;
			return true;
		}
		event_pool[ei].subscribe_deps(c, ei);
	}
}
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
{
	// restore the immediate event queue
	size_t s;
	read_value(i, s);
	size_t j = 0;
	value_reader<event_placeholder_type> read(i);
	for ( ; j<s; ++j) {
		event_placeholder_type ep;
		read(ep);
		immediate_event_fifo.push_back(ep);
	}
}
#endif
{
	// restore the event queue
	size_t s;
	read_value(i, s);
	size_t j = 0;
	value_reader<event_placeholder_type> read(i);
	for ( ; j<s; ++j) {
		event_placeholder_type ep;
		read(ep);
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
		check_event_queue.insert(ep);
#else
		event_queue.insert(ep);
#endif
	}
}
	read_value(i, current_time);
#if 0
	read_value(i, uniform_delay);
	read_value(i, null_event_delay);
#endif
//	read_value(i, flags);		// but not stopped?
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This prints the contents of a checkpoint, unattached to 
	any object file.
	This should follow load_checkpoint exactly.  
	The limitation of this is that no event-type information is present.
 */
ostream&
State::dump_raw_checkpoint(ostream& o, istream& i) {
	nonmeta_state_manager instances;
	instances.load_checkpoint(i);
	instances.dump_state(o);
{
	size_t s;
	read_value(i, s);
	size_t j = 0;
	o << "events subscribed to their dependencies:" << endl;
	for ( ; j<s; ++j) {
		event_index_type ei;
		read_value(i, ei);
		o << ei << " ";
	}
	o << endl;
}{
	// restore the event queue
	size_t s;
	read_value(i, s);
	size_t j = 0;
	value_reader<event_placeholder_type> read(i);
	o << "event queue:" << endl;
	if (s) {
		o << event_table_header << "\tcause" << endl;
		for ( ; j<s; ++j) {
			event_placeholder_type ep;
			read(ep);
			o << '\t' << ep.time << '\t' << ep.event_index << '\t';
			// can't dump_brief b/c not attached to object file
			if (ep.cause_event_id) {	// always show
				o << "\t[by:" << ep.cause_event_id << ']';
			}
			o << endl;
		}
	}
}
	time_type current_time;
	read_value(i, current_time);
	o << "current time: " << current_time << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create a local temporary copy and work with it.  
	Needs in invoking (this) object to allocate/construct state
	and events based on the same module.
 */
ostream&
State::dump_checkpoint(ostream& o, istream& i) const {
	o << "chpsim checkpoint dump:" << endl;
	State state(this->get_module());
if (state.load_checkpoint(i)) {
	return o << "Error loading checkpoint." << endl;
} else {
	return state.dump_state(o);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Textual dump of state information saved in checkpoint. 
	Keep consistent with save/load_checkpoint.  
 */
ostream&
State::dump_state(ostream& o) const {
	// dump all the information in the checkpoint
	o << "variable states:" << endl;
	instances.dump_state(o);
{
	o << "events subscribed to their dependencies:" << endl;
	size_t j=1;
	for ( ; j<event_pool.size(); ++j) {
		if (event_pool[j].is_subscribed(instances, j))
			o << j << ' ';
	}
	o << endl;
}
	dump_event_queue(o);
	o << "current time: " << current_time << endl;
	// other stuff are simulation settings
	return o;
}

//=============================================================================
// class state_dump_options method definitions

/**
	Default configuration for dumping.  
 */
graph_options::graph_options() :
		show_event_index(true), 
		show_instances(false), 
#if CHPSIM_READ_WRITE_DEPENDENCIES
		with_antidependencies(false),
#endif
		process_event_clusters(false), 
		show_delays(false) {
}

//=============================================================================
}	// end namespace CHPSIM

// explicit class instantiation
template class signal_handler<CHPSIM::State>;
// template class EventQueue<EventPlaceholder<real_time> >;
	// NOTE: already instantiated for PRSIM, factor into common lib?

}	// end namespace SIM
}	// end namespace HAC


