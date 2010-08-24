/**
	\file "sim/chpsim/State.cc"
	Implementation of CHPSIM's state and general operation.  
	$Id: State.cc,v 1.25 2010/08/24 18:08:43 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_STEP			(0 && ENABLE_STACKTRACE)
#define	DEBUG_QUEUE			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <iterator>
#include <functional>
#include <sstream>

#include "sim/chpsim/State.h"
#include "sim/chpsim/StateConstructor.h"
#include "sim/chpsim/graph_options.h"
#include "sim/event.tcc"
#include "sim/signal_handler.tcc"
#include "sim/chpsim/nonmeta_context.h"
#include "sim/random_time.h"
#include "sim/chpsim/Trace.h"
#include "Object/module.h"
#include "Object/global_channel_entry.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/lang/CHP_footprint.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_pool.h"

#include "common/TODO.h"
#include "sim/ISE.h"
#include "util/likely.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"	// for explicit instantiation
#include "util/iterator_more.h"
#include "util/copy_if.h"
#include "util/IO_utils.tcc"
#include "util/binders.h"
#include "util/discrete_interval_set.tcc"
#include "util/utypes.h"

#if	DEBUG_STEP
#define	DEBUG_STEP_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_STEP		STACKTRACE_VERBOSE
#else
#define	DEBUG_STEP_PRINT(x)
#define	STACKTRACE_VERBOSE_STEP
#endif

#if	DEBUG_QUEUE
#define	DEBUG_QUEUE_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_QUEUE	STACKTRACE_VERBOSE
#else
#define	DEBUG_QUEUE_PRINT(x)
#define	STACKTRACE_VERBOSE_QUEUE
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
using entity::expr_dump_context;
using entity::state_instance;
using entity::dump_flags;
using std::copy;
using std::back_inserter;
using std::mem_fun_ref;
using std::ostream_iterator;
using std::ptr_fun;
using util::set_inserter;
USING_COPY_IF
using util::write_value;
using util::read_value;
using util::value_writer;
using util::value_reader;
using util::discrete_interval_set;
using entity::CHP::local_event;
using entity::CHP::local_event_footprint;
using entity::CHP::EVENT_SEND;
using entity::CHP::EVENT_RECEIVE;

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
	const event_index_type	cause_event_id;
	const size_t		cause_trace_id;

	explicit
	recheck_transformer(this_type& s,
		const event_index_type cei, const size_t cti) : 
			state(s), 
			context(state),
			cause_event_id(cei), cause_trace_id(cti)
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
		DEBUG_QUEUE_PRINT("rechecking event " << ei << endl);
		event_type& e(state.event_pool[ei]);
		const size_t pid = state.get_process_id(ei);
		STACKTRACE_INDENT_PRINT("in process " << pid << endl);
		context.set_global_context(state.get_global_context(pid));
		// no need for global_offset
		context.set_event(state, 
			e, pid, state.get_offset_from_pid(pid));
		// To accomodate blocking-sends that wake-up probes
		// we must prevent self-wake up in this corner case.
		if (cause_event_id != ei) {
		// Now, the only events that should call this are ones
		// that have been woken up by an update, not first-time.
		// now that delays have been paid up-front, events that
		// pass recheck can be scheduled into the immediate event fifo
#if ENABLE_STACKTRACE
		context.dump_context(STACKTRACE_INDENT_PRINT("context: "));
#endif
		if (e.recheck(context, ei)) {
			DEBUG_QUEUE_PRINT("enqueue to immediate fifo" << endl);
			// note: time is irrelevant to immediate-event-fifo
			// it's just a NOW fifo
			state.immediate_event_fifo.push_back(
				event_placeholder_type(state.current_time, ei, 
					cause_event_id, cause_trace_id));
			// this is an unfortunate overloaded re-use
			// of event_placeholder_type.
		} else {
			DEBUG_QUEUE_PRINT("still blocked" << endl);
		} // end if recheck
		} else {
			DEBUG_QUEUE_PRINT("ignored cause_event_id == ei" << endl);
		} // end if self-wake-up
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
		STACKTRACE_VERBOSE_QUEUE;
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
		if (state.watching_all_event_queue()) {
			// is this a performance hit, rechecking inside loop?
			// if so, factor this into two versioned loops.
			state.dump_event(cout << "enqueue: ", ei, new_time)
				<< endl;
		}
		state.check_event_queue.insert(new_event);
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
		instances(m.get_footprint()), 
		event_pool(), 
		global_root_event(NULL, entity::CHP::EVENT_CONCURRENT_FORK),
		global_event_to_pid(),
		pid_to_offset(),
		check_event_queue(), 
		timing_mode(TIMING_DEFAULT), 
		current_time(0), 
		uniform_delay(event_type::default_delay),
		null_event_delay(event_type::default_delay),	// or 0
		interrupted(false),
		flags(FLAGS_DEFAULT), 
		__updated_list(), 
		__rechecks(), 
		immediate_event_fifo(),
		event_watches(), 
		event_breaks(),
		value_watches(), 
		value_breaks(), 
#define	E(e)	error_policy_enum(ERROR_DEFAULT_##e)
		assert_fail_policy(E(ASSERT_FAIL)),
#undef	E
		trace_manager(), 
		trace_flush_interval(1L<<16),
		checkpoint_name("autosave.chpsimckpt"),
		checkpoint_interval(1000000), 
		_dump_flags(dump_flags::no_owners)	// fixed for now
		{
	// perform initializations here
	event_pool.reserve(256);	// pre-allocate some
	event_pool.resize(1);		// 0th entry is a global-spawn event
{
	event_type& init(event_pool[0]);
	const footprint& topfp(mod.get_footprint());
	init.make_global_root(&global_root_event);	// use the FORK, Luke...
	const state_instance<process_tag>::pool_type&
		pp(topfp.get_instance_pool<process_tag>());
	const size_t procs = pp.total_entries() +1;	// count top-level as 0
	pid_to_offset.resize(procs +1);
	// use pid_to_offset[procs] to represent the spawn event (offset 0)
	pid_to_offset[procs] = std::make_pair(0, 1);
	global_event_to_pid[0] = procs;	// map event 0 to top-process (procs)

	// top-level process reserves index 0
	initialize_process_event_chunk(topfp.get_chp_event_footprint(), 0);

	size_t i = 1;
	for ( ; i<procs; ++i) {
		const state_instance<process_tag>&
			p(topfp.get_instance<process_tag>(i-1));	// 0-based
		const footprint& fp(*p._frame._footprint);
#if ENABLE_STACKTRACE
		fp.dump_type(STACKTRACE_INDENT_PRINT("type: ")) << endl;
#endif
		initialize_process_event_chunk(fp.get_chp_event_footprint(), i);
	}
	// connect all process-root events to the spawn event
{
	typedef	global_event_to_pid_map_type::const_iterator const_iterator;
	// +1 to skip the global-root event
	const_iterator j(++global_event_to_pid.begin()),
		e(global_event_to_pid.end());
	for ( ; j!=e; ++j) {
		// the offsets also point to the root event of each process
		global_root_event.successor_events.push_back(j->first);
	}
}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
State::~State() {
	if ((flags & FLAG_AUTOSAVE) && checkpoint_name.size()) {
		// close trace before saving autocheckpoint
		close_trace();
		ofstream o(checkpoint_name.c_str());
		if (o) {
		try {
			save_checkpoint(o);
		} catch (...) {
			cerr << "Fatal error during checkpoint save." << endl;
		}
		} else {
			cerr << "Error opening \'" << checkpoint_name <<
				"\' for saving checkpoint." << endl;
		}
	}
	// clean-up
	// optional, but recommended: run some diagnostics
	// anything to do with trace_manager?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translate global event index to process id, which is possible
	because events belonging to the same process lie are contiguous.
	NOTE: pid for the global-root event does NOT correspond to a 
	real process, so caller should check valid_process_id() as needed.
 */
size_t
State::get_process_id(const event_index_type ei) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("event = " << ei << endl);
	global_event_to_pid_map_type::const_iterator
		f(global_event_to_pid.upper_bound(ei));
	INVARIANT(f != global_event_to_pid.begin());
	--f;
	const size_t pid = f->second;
	STACKTRACE_INDENT_PRINT("pid = " << pid << endl);
	INVARIANT(pid < pid_to_offset.size());
	return pid;
// used to be embedded for fast lookup at the cost of storage:
//	return event_pool[ei].get_process_index();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
State::get_process_id(const event_type& e) const {
	return get_process_id(get_event_id(e));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only invalid pid is the last one, which is faked as the 
	owner/parent of the global-root event.  
 */
bool
State::valid_process_id(const size_t pid) const {
	return pid < (pid_to_offset.size() -1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
event_index_type
State::get_event_id(const event_type& e) const {
	const event_index_type d = std::distance(&event_pool[0], &e);
	INVARIANT(d < event_pool.size());
	return d;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the base offset of the process to which this event belongs.
 */
event_index_type
State::get_offset_from_event(const event_index_type eid) const {
	return get_offset_from_pid(get_process_id(eid));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the base offset of the process to which this event belongs.
 */
event_index_type
State::get_offset_from_event(const event_type& e) const {
	return get_offset_from_pid(get_process_id(e));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
event_index_type
State::get_offset_from_pid(const size_t pid) const {
	return pid_to_offset[pid].first;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only called during construction and state allocation.  
 */
void
State::initialize_process_event_chunk(const local_event_footprint& chpfp, 
		const size_t pid) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("pid = " << pid << endl);
	const size_t offset = event_pool.size();
	const size_t local_events = chpfp.size();
	pid_to_offset[pid] = std::make_pair(offset, local_events);
	if (local_events) {
		global_event_to_pid[offset] = pid;	// pid 0 is top-level
		const size_t M = event_pool.size() +local_events;
		event_pool.resize(M);
//		const global_entry_context::footprint_frame_setter fs(dc, pid);
		size_t i = 0;
		do {
			// setup per-event info: dependencies, delays, ...
			event_pool[i +offset].setup(&chpfp[i], *this);
			++i;
		} while (i<local_events);
	}
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
	immediate_event_fifo.clear();
	check_event_queue.clear();
	// seed events that are ready to go, like active initializations
	//	note: we use event[0] as the launching event (concurrent)
	check_event_queue.insert(event_placeholder_type(current_time, 0));
	__updated_list.clear();
	__rechecks.clear();
	for_each(event_pool.begin(), event_pool.end(),
		mem_fun_ref(&event_type::reset)
	);
	// ok to retain autosave
	close_trace();	// close trace file
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
#define	E(e)	error_policy_enum(ERROR_DEFAULT_##e)
	assert_fail_policy = E(ASSERT_FAIL);
#undef	E
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
	const event_placeholder_type ret(*check_event_queue.begin());
	check_event_queue.erase(check_event_queue.begin());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Peek at the time of the next event.  
	\pre event queue must not be empty (either immedate or checking)
 */
State::time_type
State::next_event_time(void) const {
	if (!immediate_event_fifo.empty()) {
		return immediate_event_fifo.begin()->time;
	} else {
		INVARIANT(!check_event_queue.empty());
		return check_event_queue.begin()->time;
	}
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
	nonmeta_context c(*this);
	std::pair<bool, bool>	status(false, false);
do {
	// TODO: check immediate event FIFO first
	// or check and load into FIFO first?
	event_placeholder_type ep;
	bool immediate = false;
	if (immediate_event_fifo.empty()) {
	// TODO: grab and check events until one is ready to execute
	// and execute it.
	if (check_event_queue.empty()) {
		return false;
	}
	ep = dequeue_event();
	} else {
		ep = immediate_event_fifo.front();
		immediate_event_fifo.pop_front();
		immediate = true;
	}	// end if immediate_event_fifo empty

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
	__updated_list.clear();
	__rechecks.clear();
	event_type& ev(event_pool[ei]);
	const size_t pid = get_process_id(ei);
	const event_index_type offset = get_offset_from_pid(pid);
	DEBUG_STEP_PRINT("in process " << pid << endl);
	// pid could be one-past-the-end to represent the global spawn event
	if (LIKELY(valid_process_id(pid))) {
		c.set_global_context(get_global_context(pid));
	}
	// no need for global_offset
#if ENABLE_STACKTRACE
	c.dump_context(STACKTRACE_INDENT_PRINT("context: "));
#endif
	c.set_event(*this, ev, pid, offset);
	// not nonmeta_context::event_setter!!!
	// TODO: re-use this nonmeta-context in the recheck_transformer
	// TODO: unify check and execute into "chexecute"
	try {
	// this is where events are checked for their first time as successor
	if (immediate || ev.first_check(c, ei)) {
		DEBUG_STEP_PRINT("executing..." << endl);
		// don't recheck if event is immediate
		status.first = true;
		try {
			ev.execute(c);
		} catch (...) {
			cerr << "Run-time error executing event "
				<< ei << "." << endl;
			throw;		// rethrow
		}
		const size_t cti = (is_tracing() ?
			trace_manager->last_event_trace_id() : 0);
		DEBUG_STEP_PRINT("this trace index = " << cti << endl);
		status.second = __step(ei, cause_event_id, cause_trace_id);
		// enqueue these events for first-checking (after delay)
		for_each(c.first_checks_begin(), c.first_checks_end(),
			event_enqueuer(*this, ei, cti));
		// c.first_checks.clear();	// not needed, loop will exit
	} else {	// end if recheck
		// TODO: is this on critical path?
		// initial check failed predicate, block waiting, 
		// subscribe to variables
		DEBUG_STEP_PRINT("event blocked waiting." << endl);
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
	STACKTRACE_VERBOSE_QUEUE;
	// TODO: hello: private template method, anybody?
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
			trace_manager->push_back_data<Tag>(v, ti, *ui);	\
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
	// g++-3.4 parse-bug workaround
	const typename entity::nonmeta_state_base<Tag>::pool_type&
		_pool(instances.template get_pool<Tag>());
	for ( ; ui!=ue; ++ui) {
		const typename variable_type<Tag>::type& v(_pool[*ui]);
		//	v(instances.template get_pool<Tag>()[*ui]);
		const event_subscribers_type& es(v.get_subscribers());
		copy(es.begin(), es.end(), set_inserter(__rechecks));
	}
}	// end __notify_updates_for_recheck()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper routine to recheck all events accumulated in the 
	__rechecks list member.  
	\throw exception on fatal errors.  
 */
void
State::__perform_rechecks(const event_index_type ei, 
		const event_index_type cause_event_id, const size_t ti) {
	STACKTRACE_VERBOSE_QUEUE;
// NOTE: since delays have been paid for up front, rechecked events that
// now pass are immediately ready for execution, and hence should be placed
// in the immediate_event_fifo.  Change happens in recheck_transformer.
try {
	for_each(__rechecks.begin(), __rechecks.end(), 
		recheck_transformer(*this, ei, ti)
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
	size_t ti = INVALID_TRACE_INDEX;
	// because we'll want to reference it later...
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
	__perform_rechecks(ei, cause_event_id, ti);
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
	// transfer events from staging queue to event queue, 
	// and schedule them with some delay
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
cerr << "Error: trace stream already open.  (command ignored)" << endl;
		return true;
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
	Create the parent name context from the process-index.
 */
entity::expr_dump_context
State::make_process_dump_context(const node_index_type pid) const {
	// -1 because we allocated one more pid slot for the
	// global spawn event.
	if (pid && valid_process_id(pid)) {
		std::ostringstream canonical_name;
		top_context.get_top_footprint().
			dump_canonical_name<process_tag>(
				canonical_name, pid -1, 
				_dump_flags);
		return expr_dump_context(canonical_name.str());
	} else {
		return expr_dump_context::default_value;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_event(ostream& o, const event_type& e) const {
	const size_t pid = get_process_id(e);
	return e.dump_struct(o, make_process_dump_context(pid), 
		valid_process_id(pid) ? pid : 0, 
		get_offset_from_pid(pid));
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
	const size_t pid = get_process_id(ei);
	o << pid << '\t';
	// caution: global-root event's pid
	return ev.dump_brief(o, make_process_dump_context(pid));
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
	const size_t pid = get_process_id(ei);
	if (valid_process_id(pid)) {
		// because global root event is a fake process
		e.dump_struct(o, make_process_dump_context(pid), 
			pid, get_offset_from_pid(pid));
	} else {
		e.dump_struct(o, expr_dump_context::default_value, 0, 0);
	}
	// o << endl;
	e.dump_source(o << "source: ", expr_dump_context::default_value) << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_event_source(ostream& o, const event_index_type ei) const {
if (ei) {
	INVARIANT(ei < event_pool.size());
	const event_type& e(event_pool[ei]);
	const size_t pid = get_process_id(ei);
	return e.dump_source_context(o, make_process_dump_context(pid));
} else {
	return o << "[global-root]" << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param e event MUST be a member event of this simulation state.
 */
ostream&
State::dump_event_source(ostream& o, const event_type& e) const {
	return dump_event_source(o, get_event_id(e));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print all events grouped by process.  
	Skips global-root event because it has no source.
 */
ostream&
State::dump_all_event_source(ostream& o) const {
	typedef	pid_to_offset_map_type::const_iterator	const_iterator;
	const_iterator i(pid_to_offset.begin()), e(--pid_to_offset.end());
	// skip last process because that is reserved for event[0] (fake)
	size_t pid = 0;
	for ( ; i!=e; ++i, ++pid) {
	if (i->second) {
		o << "\n# process[" << pid << "]:" << endl;
		const event_index_type offset = i->first;
		event_index_type j = 0;
		const expr_dump_context edc(make_process_dump_context(pid));
		for ( ; j < i->second; ++j) {
			const size_t eid = offset +j;
			o << "event[" << eid << "]: ";
			const event_type& ev(event_pool[eid]);
			ev.dump_source_context(o, edc);
		}
	}
	}
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
{
	typedef	event_queue_type::const_iterator	const_iterator;
	const event_queue_type& temp(check_event_queue);	// just alias
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
/**
	Synonym to original definition, because too lazy to
	rename with new check-event changes.  
 */
ostream&
State::dump_check_event_queue(ostream& o) const {
	return dump_event_queue(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints non-hierarchical structure of the entire allocated state.
	See also: dump_struct_dot() for a graphical view.  
	TODO: with bulk-allocated events, print and group by process!
 */
ostream&
State::dump_struct(ostream& o) const {
	const entity::footprint& topfp(mod.get_footprint());
{
	o << "Variables: " << endl;
	instances.dump_struct(o, topfp);
	o << endl;
}
// CHP graph structures (non-hierarchical)
	o << "Event graph: " << endl;
	// group events by process, since they now lie in contiguous ranges
	// event[0] is special, print it separately

	o << "event[0]: ";
//	const size_t pid = get_process_id(0);
	event_pool[0].dump_struct(o, 
		expr_dump_context::default_value, 0, 0
//		make_process_dump_context(pid), get_offset_from_pid(pid)
	);
{
	typedef	pid_to_offset_map_type::const_iterator	const_iterator;
	const_iterator i(pid_to_offset.begin()), e(--pid_to_offset.end());
	// skip last one because that is reserved for event[0]
	size_t pid = 0;
	for ( ; i!=e; ++i, ++pid) {
	if (i->second) {
		o << "\n# process[" << pid << "]:" << endl;
		const event_index_type offset = i->first;
		event_index_type j = 0;
		const expr_dump_context edc(make_process_dump_context(pid));
		for ( ; j < i->second; ++j) {
			const size_t eid = offset +j;
			o << "event[" << eid << "]: ";
			const event_type& ev(event_pool[eid]);
//			const size_t pid = get_process_id(eid);
			ev.dump_struct(o, edc, pid, offset);
		}
	}
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
	// should be event_type::local_event_type
	static const char* node_prefix = local_event::node_prefix;

	o << "digraph G {" << endl;
	// consider using global_entry_context_base instead...
	const footprint& topfp(mod.get_footprint());
{
if (g.show_instances) {
	o << "# Instances: " << endl;
	topfp.dump_non_process_dot_nodes(o);
	// reminder: label enumerations are 1-based
}
	o << "# Events: " << endl;
	const event_index_type es = event_pool.size();
	// group events by process (making subgraph clustering trivial)
	size_t pid = 0;
	event_pool[0].dump_dot_node(o, 0, g,
		expr_dump_context::default_value, 0, 0) << endl;
{
	// the last event was reserved for event[0], the global root event
	const size_t mp = pid_to_offset.size() -1;
for ( ; pid < mp; ++pid) {
	const size_t max = pid_to_offset[pid].second;
	if (max) {
		// events are already clustered by process
		// but don't cluster the top-level process
	if (pid && g.process_event_clusters) {
		o << "subgraph cluster" << pid << " {" << endl;
		std::ostringstream oss;
		topfp.dump_canonical_name<process_tag>(oss, pid-1, 
				_dump_flags);
		o << "label=\"pid=" << pid << ": " << oss.str() << "\";"
			<< endl;
	} else {
		o << "# process " << pid << endl;
	}
		const size_t offset = get_offset_from_pid(pid);
		const expr_dump_context edc(make_process_dump_context(pid));
		size_t i = 0;
		for ( ; i<max; ++i) {
			const event_index_type eid = i +offset;
			const event_type& e(event_pool[eid]);
			e.dump_dot_node(o, eid, g, edc, pid, offset) << endl;
		}
	if (pid && g.process_event_clusters) {
		o << "}" << endl;
	}
	}	// end if max
}	// end for-all pid's
}
if (g.show_channels) {
	static const char channel_prefix[] = "CHANNEL_";
	// TODO: can this re-used as a guile-routine?
	o << "# Channels:" << endl;
	typedef std::set<event_index_type>	event_id_set_type;
	const size_t cs =
		topfp.get_instance_pool<channel_tag>().total_entries() +1;
	vector<event_id_set_type>
		send_map(cs), recv_map(cs);
	event_index_type i = 0;		// FIRST_VALID_EVENT
	// collect communicating channels over all events
	for ( ; i<es; ++i) {
		const event_type& e(event_pool[i]);
		const DependenceSet& bdeps(e.get_block_dependencies());
		const instance_set_type&
			bchans(bdeps.get_instance_set<channel_tag>());
		// Deduce channel send/receives from the
		// respective block-dependencies.  
		// Reminder: these sets are conservative.
		size_t j = 0;
		switch (e.get_event_type()) {
		case EVENT_SEND:
			for ( ; j<bchans.size(); ++j)
				send_map[bchans[j]].insert(i);
			break;
		case EVENT_RECEIVE:
			for ( ; j<bchans.size(); ++j)
				recv_map[bchans[j]].insert(i);
			break;
		default: break;
		}
	}
	o << "node [shape=plaintext];" << endl;	// change node style
	o << "edge [style=dashed];" << endl;
	// keep arrowheads? constraint=false?
	i = 1;	// FIRST_VALID_GLOBAL_NODE
	for ( ; i<cs; ++i) {
		const event_id_set_type& ss(send_map[i]), rs(recv_map[i]);
		event_id_set_type::const_iterator
			si(ss.begin()), se(ss.end()),
			ri(rs.begin()), re(rs.end());
		// get channel name
		std::ostringstream oss;
		topfp.dump_canonical_name<channel_tag>(oss, i-1, 
				_dump_flags);
		// emit a node if there are multiple senders or receivers
		// also if sender/receiver set is empty
		if (ss.size() != 1 || rs.size() != 1) {
			// node (labeled)
			o << channel_prefix << i << "\t[label=\"" << oss.str()
				<< "\"];" << endl;
			// edges (unlabeled)
			for ( ; si!=se; ++si) {
				o << node_prefix << *si << " -> " <<
					channel_prefix << i << ';' << endl;
			}
			for ( ; ri!=re; ++ri) {
				o << channel_prefix << i << " -> " <<
					node_prefix << *ri << ';' << endl;
			}
		} else {
			// just collapse into a single labeled edge
			o << node_prefix << *si << " -> " <<
				node_prefix << *ri <<
				"\t[label=\"" << oss.str() << "\"];" << endl;
		}
	}	// end for all channels
}	// end if show_channels
}
	o << "}" << endl;
	return o;
}	// end dump_struct_dot

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_updated_references(ostream& o) const {
	typedef	update_reference_array_type::ref_bin_type::const_iterator
							const_iterator;
	o << "updated references:" << endl;
	return __updated_list.dump(o) << endl;
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
/**
	Implementation for the `get' command.  
	Prints canonical name and current value.  
 */
ostream&
State::print_instance_name_value(ostream& o,
		const global_indexed_reference& g) const {
	const entity::footprint& topfp(mod.get_footprint());
	const dump_flags& df(_dump_flags);
	INVARIANT(g.second);
	const size_t id0 = g.second -1;
	switch (g.first) {
	case META_TYPE_BOOL: {
		o << "bool ";
		topfp.dump_canonical_name<bool_tag>(o, id0, df);
		o << " = ";
		o << size_t(instances.get_pool<bool_tag>()[g.second].value);
		break;
	}
	case META_TYPE_INT: {
		o << "int ";
		topfp.dump_canonical_name<int_tag>(o, id0, df);
		o << " = ";
		o << instances.get_pool<int_tag>()[g.second].value;
		break;
	}
	case META_TYPE_ENUM: {
		o << "enum ";
		topfp.dump_canonical_name<enum_tag>(o, id0, df);
		o << " = ";
		o << instances.get_pool<enum_tag>()[g.second].value;
		break;
	}
	case META_TYPE_CHANNEL: {
		const state_instance<channel_tag>&
			c(topfp.get_instance<channel_tag>(id0));
		const ChannelState&
			nc(instances.get_pool<channel_tag>()[g.second]);
		const canonical_fundamental_chan_type_base& t(*c.channel_type);
		t.dump(o) << ' ';
		topfp.dump_canonical_name<channel_tag>(o, id0, df);
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
	const entity::footprint& topfp(mod.get_footprint());
	const dump_flags& df(_dump_flags);
	INVARIANT(g.second);
	const size_t id0 = g.second-1;
	switch (g.first) {
	case META_TYPE_BOOL: {
		o << "bool ";
		topfp.dump_canonical_name<bool_tag>(o, id0, df);
		o << " : ";
		instances.get_pool<bool_tag>()[g.second].dump_subscribers(o);
		break;
	}
	case META_TYPE_INT: {
		o << "int ";
		topfp.dump_canonical_name<int_tag>(o, id0, df);
		o << " : ";
		instances.get_pool<int_tag>()[g.second].dump_subscribers(o);
		break;
	}
	case META_TYPE_ENUM: {
		o << "enum ";
		topfp.dump_canonical_name<enum_tag>(o, id0, df);
		o << " : ";
		instances.get_pool<enum_tag>()[g.second].dump_subscribers(o);
		break;
	}
	case META_TYPE_CHANNEL: {
		const ChannelState&
			nc(instances.get_pool<channel_tag>()[g.second]);
		topfp.dump_canonical_name<channel_tag>(o, id0, df);
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
		mod.get_footprint());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::autosave(const bool b, const string& n) {
	if (b)	flags |= FLAG_AUTOSAVE;
	else	flags &= ~FLAG_AUTOSAVE;
	if (n.length()) {
		checkpoint_name = n;
	}
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
	Do not save the autosave checkpoint name.
	\return true to signal an error
 */
bool
State::save_checkpoint(ostream& o) const {
{
	// save the random seed
	ushort seed[3] = {0, 0, 0};
	const ushort* old_seed = seed48(seed);	// libc
	seed[0] = old_seed[0];
	seed[1] = old_seed[1];
	seed[2] = old_seed[2];
	// put it back
	seed48(seed);
	write_value(o, seed[0]);
	write_value(o, seed[1]);
	write_value(o, seed[2]);
}
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
	size_t s = immediate_event_fifo.size();
	write_value(o, s);
	for_each(immediate_event_fifo.begin(), immediate_event_fifo.end(), 
		value_writer<event_placeholder_type>(o));
}{
	const event_queue_type& temp(check_event_queue);	// just alias
	size_t s = temp.size();
	write_value(o, s);
	for_each(temp.begin(), temp.end(), 
		value_writer<event_placeholder_type>(o));
}
	write_value(o, assert_fail_policy);
	// save current time
	write_value(o, current_time);
	// delay settings?
#if 0
	write_value(o, uniform_delay);
	write_value(o, null_event_delay);
#endif
	write_value(o, flags & FLAGS_CHECKPOINT_MASK);
	// skip trace manager
	// save checkpoint interval?
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores simulation state from checkpoint.  
	This simulation checkpoint must use the same object file and 
	invocation options.  
	Preserve previous autosave settings.
	\return true to signal an error
 */
bool
State::load_checkpoint(istream& i) {
{
	// restore random seed
	ushort seed[3];
	read_value(i, seed[0]);
	read_value(i, seed[1]);
	read_value(i, seed[2]);
	seed48(seed);
}
	// restore data/channel/variable state
	if (instances.load_checkpoint(i)) {
		return true;
	}
	// restore event subscription state
{
	size_t j = 0;
	size_t s;
	read_value(i, s);
	const nonmeta_context c(*this);
	immediate_event_fifo.clear();
	check_event_queue.clear();
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
}{
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
}{
	// restore the event queue
	size_t s;
	read_value(i, s);
	size_t j = 0;
	value_reader<event_placeholder_type> read(i);
	for ( ; j<s; ++j) {
		event_placeholder_type ep;
		read(ep);
		check_event_queue.insert(ep);
	}
}
	read_value(i, assert_fail_policy);
	read_value(i, current_time);
#if 0
	read_value(i, uniform_delay);
	read_value(i, null_event_delay);
#endif
{
	flags_type tmp;
	read_value(i, tmp);		// but not stopped?
	flags = (flags & FLAG_AUTOSAVE) | (tmp & ~FLAG_AUTOSAVE);
}
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
	error_policy_enum p;
	read_value(i, p);
	o << "assert-fail policy: " << error_policy_string(p) << endl;
	time_type current_time;
	read_value(i, current_time);
	o << "current time: " << current_time << endl;
{
	flags_type tmp;
	read_value(i, tmp);		// but not stopped?
	o << "flags: 0x" << std::hex << size_t(tmp) << endl;
}
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
{
	// save the random seed
	ushort seed[3] = {0, 0, 0};
	const ushort* old_seed = seed48(seed);	// libc
	seed[0] = old_seed[0];
	seed[1] = old_seed[1];
	seed[2] = old_seed[2];
	// put it back
	seed48(seed);
	o << "seed48: " << seed[0] << ' ' << seed[1] << ' ' << seed[2] << endl;
}
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
		show_delays(false),
		show_channels(false) {
}

//=============================================================================
}	// end namespace CHPSIM

// explicit class instantiation
template class signal_handler<CHPSIM::State>;
// template class EventQueue<EventPlaceholder<real_time> >;
	// NOTE: already instantiated for PRSIM, factor into common lib?

}	// end namespace SIM
}	// end namespace HAC


