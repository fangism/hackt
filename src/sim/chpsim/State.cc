/**
	\file "sim/chpsim/State.cc"
	Implementation of CHPSIM's state and general operation.  
	$Id: State.cc,v 1.2.2.4 2007/01/29 04:44:11 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_STEP			(1 && ENABLE_STACKTRACE)

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
#if CHPSIM_TRACING
#include "sim/chpsim/Trace.h"
#endif
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
	// other option is to pass in a pre-made nonmeta_context
	nonmeta_context		context;

	explicit
	recheck_transformer(this_type& s) : 
		state(s), 
		context(state.mod.get_state_manager(), 
			state.mod.get_footprint(),
			state) { }

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
		e.recheck(context, ei);
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

	/**
		TODO: different timing modes
	 */
	void
	operator () (const event_index_type ei) {
		const event_type& e(state.event_pool[ei]);
		time_type new_delay;
	switch (state.timing_mode) {
	case TIMING_UNIFORM:
		new_delay = (e.is_trivial() ?
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
		const event_placeholder_type new_event(new_time, ei);
		if (state.watching_event_queue()) {
			// is this a performance hit, rechecking inside loop?
			// if so, factor this into two versioned loops.
			state.dump_event(cout << "enqueue: ", ei, new_time);
		}
		state.event_queue.push(new_event);
	}

};	// end class event_enqueuer

//=============================================================================
// class State method definitions

const char
State::event_table_header[] = "\ttime\teid\tpid\tevent";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Will throw exception upon error.  
 */
State::State(const module& m) : 
		state_base(m, "chpsim> "), 
		instances(m.get_state_manager()), 
		event_pool(), 
		event_queue(), 
		timing_mode(TIMING_DEFAULT), 
		current_time(0), 
		uniform_delay(event_type::default_delay),
		null_event_delay(event_type::default_delay),	// or 0
		interrupted(false),
		flags(FLAGS_DEFAULT), 
		__updated_list(), 
		__enqueue_list(), 
		__rechecks()
#if CHPSIM_TRACING
		, trace_manager()
#endif
		{
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
	event_queue.clear();
	// seed events that are ready to go, like active initializations
	//	note: we use event[0] as the launching event (concurrent)
	event_queue.push(event_placeholder_type(current_time, 0));
	__updated_list.clear();
	__enqueue_list.clear();
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
	Q: since multiple instances referenced can be altered, 
		should we check against the watch-list here (set-intersection)?
	\return reference to the variable that was just modified, if any.
 */
State::step_return_type
State::step(void) {
	typedef	step_return_type	return_type;
	STACKTRACE_VERBOSE;
	// pseudocode:
	// 1) grab event off of pending event queue, dequeue it
	if (event_queue.empty()) {
		return return_type(META_TYPE_NONE, INVALID_NODE_INDEX);
	}

	const event_placeholder_type ep(dequeue_event());
	current_time = ep.time;
	DEBUG_STEP_PRINT("time = " << current_time << endl);
	const event_index_type& ei(ep.event_index);
	// first NULL event has index 0, but nothing else should enqueue it
	DEBUG_STEP_PRINT("event_index = " << ei << endl);
	// no need to deallocate event, they are all statically pre-allocated

	// 2) execute the event (alter state, variables, channel, etc.)
	//	expect references to the channel/variable(s) affected
	__enqueue_list.clear();
	__updated_list.clear();
	__rechecks.clear();
	event_type& ev(event_pool[ei]);
	// TODO: re-use this nonmeta-context in the recheck_transformer
	const nonmeta_context
		c(mod.get_state_manager(), mod.get_footprint(), ev, *this);
try {
	ev.execute(c, __updated_list);
} catch (...) {
	cerr << "Run-time error executing event " << ei << "." << endl;
	throw;		// rethrow
}
	if (watching_all_events()) {
		dump_event(cout, ei, current_time);
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
{
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
#define	CASE_META_TYPE_TAG(Tag)						\
		case class_traits<Tag>::type_tag_enum_value: {		\
			const event_subscribers_type&			\
				es(instances.get_pool<Tag>()[j]		\
					.get_subscribers());		\
			copy(es.begin(), es.end(),			\
				set_inserter(__rechecks));		\
			break;						\
		}
	CASE_META_TYPE_TAG(bool_tag)
	CASE_META_TYPE_TAG(int_tag)
	CASE_META_TYPE_TAG(enum_tag)
	CASE_META_TYPE_TAG(channel_tag)
#undef	CASE_META_TYPE_TAG
		// case INSTANCE_TYPE_NULL:	// should not have been added
		default:
			ISE(cerr, cerr << "Unexpected type." << endl;)
		}	// end switch
	}	// end for
}
	// 4) immediately include this event's successors in list
	//	to evaluate if ready to enqueue.
	// selection of successors will depend on event_type, of course
	//	This has been folded into the execution of this event.
{
#if DEBUG_STEP
	// debug: print list of events to recheck
	dump_recheck_events(cout);
#endif
try {
	for_each(__rechecks.begin(), __rechecks.end(), 
		recheck_transformer(*this));
} catch (...) {
	cerr << "Run-time error while rechecking events." << endl;
	throw;
}
	// enqueue any events that are ready to fire
	//	NOTE: check the guard expressions of events before enqueuing
	// temporarily disabled for regression testing
#if DEBUG_STEP
	// debug: print list of event to enqueue
	dump_enqueue_events(cout);
#endif
	// transfer events from staging queue to event queue, 
	// and schedule them with some delay
	for_each(__enqueue_list.begin(), __enqueue_list.end(),
		event_enqueuer(*this));
}
}
	// TODO: finish me: watch list
	return return_type(INVALID_NODE_INDEX, INVALID_NODE_INDEX);
}	// end step() method

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
#if CHPSIM_TRACING
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints event in brief form for queue, with time prefixed.  
 */
ostream&
State::dump_event(ostream& o, const event_index_type ei,
		const time_type t) const {
	const event_type& ev(get_event(ei));
	o << '\t' << t << '\t';
#if 1
	o << ei << '\t';
#endif
	ev.dump_brief(o) << endl;
	return o;
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
	// search wouldn't be necessary if event was flagged in member field
	temp_queue_type temp;
	// copy wouldn't be necessary if queue was a map...
	event_queue.copy_to(temp);
	o << "in queue: ";
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
 */
ostream&
State::dump_event_queue(ostream& o) const {
	typedef	temp_queue_type::const_iterator	const_iterator;
	temp_queue_type temp;
	event_queue.copy_to(temp);
	const_iterator i(temp.begin()), e(temp.end());
	o << "event queue:" << endl;
	// print header
	if (i!=e) {
		o << event_table_header << endl;
		for ( ; i!=e; ++i) {
			dump_event(o, i->event_index, i->time);
		}
	} else {
		// deadlocked!
		o << "\t(empty)" << endl;
	}
	return o;
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
	typedef	update_reference_array_type::const_iterator	const_iterator;
	o << "updated references:" << endl;
	const_iterator i(__updated_list.begin()), e(__updated_list.end());
	for ( ; i!=e; ++i) {
	switch (i->first) {
#define	CASE_PRINT_TYPE_TAG_NAME(V)					\
	case V: o << class_traits<meta_type_map<V>::type>::tag_name; break;
	CASE_PRINT_TYPE_TAG_NAME(META_TYPE_BOOL)
	CASE_PRINT_TYPE_TAG_NAME(META_TYPE_INT)
	CASE_PRINT_TYPE_TAG_NAME(META_TYPE_ENUM)
	CASE_PRINT_TYPE_TAG_NAME(META_TYPE_CHANNEL)
	default: o << "UNKNOWN";
#undef	CASE_PRINT_TYPE_TAG_NAME
	}	// end switch
		o << '[' << i->second << "], ";
	}
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
ostream&
State::dump_enqueue_events(ostream& o) const {
	o << "to be enqueued: ";
	ostream_iterator<size_t> osi(o, ", ");
	copy(__enqueue_list.begin(), __enqueue_list.end(), osi);
	return o << endl;
}

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


