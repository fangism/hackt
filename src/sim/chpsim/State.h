/**
	\file "sim/chpsim/State.h"
	$Id: State.h,v 1.2.2.7 2007/02/04 22:11:47 fang Exp $
	Structure that contains the state information of chpsim.  
 */

#ifndef	__HAC_SIM_CHPSIM_STATE_H__
#define	__HAC_SIM_CHPSIM_STATE_H__

#include <iosfwd>
#include <vector>
#include "sim/chpsim/devel_switches.h"
#if CHPSIM_MULTISET_EVENT_QUEUE
#include <set>
#endif
#include "sim/time.h"
#include "sim/event.h"
#include "sim/state_base.h"
#include "sim/signal_handler.h"
#include "sim/chpsim/Event.h"
#include "Object/nonmeta_state.h"
#include "util/macros.h"
#include "util/tokenize_fwd.h"
#if CHPSIM_TRACING
#include "util/memory/excl_ptr.h"
#endif

namespace HAC {
namespace SIM {
namespace CHPSIM {
using std::vector;
using entity::nonmeta_state_manager;
using entity::event_subscribers_type;
using entity::global_indexed_reference;
using util::string_list;
using std::string;
class StateConstructor;
class nonmeta_context;
class graph_options;
#if CHPSIM_TRACING
class TraceManager;
using util::memory::excl_ptr;
using util::memory::never_ptr;
#endif

//=============================================================================
/**
	The complete state of the CHPSIM simulator.  
	TODO: consider how to partition and distribute statically
	for parallel simulation, with min-cut locking.  
	TODO: think about hooks for profiling.
	TODO: how to estimate energy and delay.
 */
class State : public state_base {
friend class StateConstructor;
friend class nonmeta_context;
	typedef	State				this_type;
	typedef	EventNode			event_type;
public:
	/**
		Numeric type for time, currently hard-coded to real (double).
	 */
	typedef	event_type::time_type		time_type;
	typedef	signal_handler<this_type>	signal_handler;
#if CHPSIM_CAUSE_TRACKING
	struct event_placeholder_type : public EventPlaceholder<time_type> {
		typedef	event_placeholder_type		this_type;
		typedef	EventPlaceholder<time_type>	parent_type;
		/**
			This is the index corresponding to the
			statically allocated event.  
		 */
		event_index_type			cause_event_id;
#if CHPSIM_TRACING
		/**
			This is the index into the history of all events, 
			as counted and maintained by the tracing engine.  
		 */
		size_t					cause_trace_id;
#endif
		/**
			default constructor: don't care what values, 
			going to be overwritten immedately thereafter.  
		 */
		event_placeholder_type() : parent_type(0, 0), 
			cause_event_id(0), cause_trace_id(0) { }

		event_placeholder_type(const time_type& t, 
			const event_index_type e, 
			const event_index_type c = SIM::INVALID_EVENT_INDEX
#if CHPSIM_TRACING
			, const size_t i = 0
#endif
			) : 
			parent_type(t, e), 
			cause_event_id(c) 
#if CHPSIM_TRACING
			, cause_trace_id(i)
#endif
			{ }

#if CHPSIM_MULTISET_EVENT_QUEUE
		bool
		operator < (const this_type& t) const {
			return time < t.time;
		}
#else
		using parent_type::operator<;
#endif

	};	// end struct event_placeholder_type
#else	// CHPSIM_CAUSE_TRACING
	typedef	EventPlaceholder<time_type>	event_placeholder_type;
#endif	// CHPSIM_CAUSE_TRACING
private:
#if CHPSIM_MULTISET_EVENT_QUEUE
	typedef	std::multiset<event_placeholder_type>
#else
	typedef	EventQueue<event_placeholder_type>
#endif
						event_queue_type;
	typedef	vector<event_type>		event_pool_type;
	typedef	vector<event_queue_type::value_type>
						temp_queue_type;

	typedef	unsigned int			flags_type;

	// NOTE: duplicate definition in InstancePools
	enum {
		/// index of the first valid node
		FIRST_VALID_NODE = SIM::INVALID_NODE_INDEX +1,
		/// index of the first valid event
		FIRST_VALID_EVENT = SIM::INVALID_EVENT_INDEX +1
	};
	typedef	global_indexed_reference	step_return_type;
	typedef	vector<global_indexed_reference>
						update_reference_array_type;
	/**
		Various mode flags, settable by user.  
	 */
	enum {
		/**
			Whether or not the simulation was halted for any 
			reason, self-stopped on error, or interrupted.  
		 */
		FLAG_STOP_SIMULATION = 0x0001,
		/**
			When true report event-queue activity, i.e.
			when events are enqueued.  
			Initially off.  
		 */
		FLAG_WATCH_QUEUE = 0x0002,
		/**
			When true, print each event as it is dequeued and
			executed.  
			Initially off.  
		 */
		FLAG_WATCH_ALL_EVENTS = 0x0004,
#if CHPSIM_CAUSE_TRACKING
		/**
			Set to true to show last-arrival event-causality.  
			Initially off.  
		 */
		FLAG_SHOW_CAUSE = 0x0008,
#endif
#if CHPSIM_TRACING
		/**
			Set true if named trace file is opened successfully.  
			Initially off.  
		 */
		FLAG_TRACE_ON = 0x8000,
#endif
		/**
			TODO: timing mode flags
		 */
		FLAGS_DEFAULT = 0x0000
	};
	/**
		Timing mode enumerations.
	 */
	enum {
		/**
			Give every event a constant delay.
			Exception may be granted for "trivial" events
			by modifying null_event_delay.  
		 */
		TIMING_UNIFORM,
		/**
			Use the delay value associated with each event.
		 */
		TIMING_PER_EVENT,
		/**
			Use random delays for all events, 
			including "trivial" events.  
			If we introduced 'bullet' semantics, would that be
			an exception?
		 */
		TIMING_RANDOM,
		/**
			Default timing mode.  
		 */
		TIMING_DEFAULT = TIMING_UNIFORM
	};
	struct recheck_transformer;
	struct event_enqueuer;
private:
	/**
		Collection of variable values and channel state.
	 */
	nonmeta_state_manager			instances;

	// event pools: for each type of event?
	// to give CHP action classes access ...
	event_pool_type				event_pool;
	// event queue: unified priority queue of event_placeholders
	event_queue_type			event_queue;
	// do we need a successor graph representing allocated
	//	CHP dataflow constructs?  
	//	predecessors? (if we want them, construct separately)
private:
	static const char			event_table_header[];
private:
	char					timing_mode;
	time_type				current_time;
	time_type				uniform_delay;
	time_type				null_event_delay;

	// mode flags
	/// is this redundant?
	bool					interrupted;
	flags_type				flags;

	/**
		Auxiliary list of references updated as the result
		of an event execution.  Need an array because
		more than one variable may be affected 
		(consider channel receive).  
		Eventually, aggregate references.  
		We keep this around between step() invocations
		to avoid repeated initial allocations.  
	 */
	update_reference_array_type		__updated_list;
	/**
		Why not a set, to guarantee uniqueness?
	 */
	typedef	vector<event_index_type>	enqueue_list_type;
	/**
		List of events to enqueue for certain, accumulated
		in step() method.  
		We keep this around between step() invocations
		to avoid repeated initial allocations.  
	 */
	enqueue_list_type			__enqueue_list;
	/**
		Set of events to recheck for unblocking.  
	 */
	event_subscribers_type			__rechecks;
#if CHPSIM_TRACING
	/**
		Private pointer to the event trace manager.  
		Data checkpointed persistently.  
	 */
	excl_ptr<TraceManager>			trace_manager;
	/**
		The interval after which the trace_manager should
		flush out a chunk of trace.  
		Smaller: more I/O overhead, slower.  
		Larger: faster, more memory used during execution.
		Counted by the number of events that have executed.
		Default: some big number
	 */
	size_t					trace_flush_interval;
#endif
#if CHPSIM_CHECKPOINTING
	/**
		Name of checkpoint file.  
		The same file is overridden periodically.  
		Suggestion: write to a temp file first, then mv it over.  
	 */
	string					checkpoint_name;
	/**
		(inverse) frequency of checkpointing.  
	 */
	time_t					checkpoint_interval;
#endif
public:
	explicit
	State(const module&);

	~State();

	void
	initialize(void);

	void
	reset(void);

	/**
		\return true if event_queue is not empty.
	 */
	bool
	pending_events(void) const { return !event_queue.empty(); }

	const time_type&
	time(void) const { return current_time; }

	time_type
	next_event_time(void) const;

	const time_type&
	get_uniform_delay(void) const { return uniform_delay; }

	void
	set_uniform_delay(const time_type& t) {
		if (t >= 0) {
			uniform_delay = t;
		}
		// else error message?
	}

	const time_type&
	get_null_event_delay(void) const { return null_event_delay; }

	void
	set_null_event_delay(const time_type& t) {
		if (t >= 0) {
			null_event_delay = t;
		}
		// else error message?
	}

	const event_type&
	get_event(const event_index_type i) const { return event_pool[i]; }

	size_t
	event_pool_size(void) const { return event_pool.size(); }

private:
	event_placeholder_type
	dequeue_event(void);

public:
	step_return_type
	step(void);	// THROWS_STEP_EXCEPTION

	void
	stop(void) {
		flags |= FLAG_STOP_SIMULATION;
		interrupted = true;
	}

	bool
	stopped(void) const { return flags & FLAG_STOP_SIMULATION; }

	void
	resume(void) {
		flags &= ~FLAG_STOP_SIMULATION;
		interrupted = false;
	}

	void
	watch_all_events(void) { flags |= FLAG_WATCH_ALL_EVENTS; }

	void
	nowatch_all_events(void) { flags &= ~FLAG_WATCH_ALL_EVENTS; }

	bool
	watching_all_events(void) const {
		return flags & FLAG_WATCH_ALL_EVENTS;
	}

	void
	watch_event_queue(void) { flags |= FLAG_WATCH_QUEUE; }

	void
	nowatch_event_queue(void) { flags &= ~FLAG_WATCH_QUEUE; }

	bool
	watching_event_queue(void) const { return flags & FLAG_WATCH_QUEUE; }

	void
	check_structure(void) const;

	bool
	set_timing(const string&, const string_list&);

	ostream&
	dump_timing(ostream&) const;

	static
	ostream&
	help_timing(ostream&);

#if CHPSIM_CAUSE_TRACKING
	bool
	showing_cause(void) const { return flags & FLAG_SHOW_CAUSE; }

	void
	show_cause(void) { flags |= FLAG_SHOW_CAUSE; }

	void
	no_show_cause(void) { flags &= ~FLAG_SHOW_CAUSE; }
#endif

#if CHPSIM_TRACING
	bool
	is_tracing(void) const { return flags & FLAG_TRACE_ON; }

	/// Do we ever want to do this?
	void
	stop_trace(void) { flags &= ~FLAG_TRACE_ON; }

	/// unconditionally returns trace_manager
	never_ptr<TraceManager>
	get_trace_manager(void) const {
		return trace_manager;
	}

	/// returns trace_manager only if flag is enabled
	never_ptr<TraceManager>
	get_trace_manager_if_tracing(void) const {
		return is_tracing() ? trace_manager :
			never_ptr<TraceManager>(NULL);
	}

	bool
	open_trace(const string&);

	void
	close_trace(void);

	size_t
	get_trace_flush_interval(void) const {
		return trace_flush_interval;
	}

	void
	set_trace_flush_interval(const size_t i) {
		INVARIANT(i);
		trace_flush_interval = i;
	}
#endif

	ostream&
	dump_struct(ostream&) const;

	ostream&
	dump_struct_dot(ostream&, const graph_options&) const;

	ostream&
	dump_event(ostream&, const event_index_type) const;

	ostream&
	dump_event_status(ostream&, const event_index_type) const;

	ostream&
	dump_event(ostream&, const event_index_type, const time_type) const;

	ostream&
	dump_event_queue(ostream&) const;

	ostream&
	print_instance_name_value(ostream&, 
		const global_indexed_reference&) const;

	ostream&
	print_instance_name_subscribers(ostream&, 
		const global_indexed_reference&) const;

	ostream&
	print_all_subscriptions(ostream&) const;

	ostream&
	dump_state(ostream&) const;

#if CHPSIM_CHECKPOINTING
	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

	ostream&
	dump_checkpoint(ostream&, istream&) const;
#endif

private:
	ostream&
	dump_updated_references(ostream&) const;

	ostream&
	dump_recheck_events(ostream&) const;

	ostream&
	dump_enqueue_events(ostream&) const;

};	// end class State

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_STATE_H__

