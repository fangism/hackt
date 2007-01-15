/**
	\file "sim/chpsim/State.h"
	$Id: State.h,v 1.1.2.20 2007/01/15 21:53:40 fang Exp $
	Structure that contains the state information of chpsim.  
 */

#ifndef	__HAC_SIM_CHPSIM_STATE_H__
#define	__HAC_SIM_CHPSIM_STATE_H__

#include <iosfwd>
#include <vector>
#include "sim/time.h"
#include "sim/event.h"
#include "sim/state_base.h"
#include "sim/signal_handler.h"
#include "sim/chpsim/Event.h"
#include "Object/nonmeta_state.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using std::vector;
using entity::nonmeta_state_manager;
using entity::event_subscribers_type;
class StateConstructor;
class nonmeta_context;
class graph_options;

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
public:
	typedef	real_time			time_type;
	typedef	signal_handler<this_type>	signal_handler;
private:
	typedef	EventNode			event_type;
	typedef	vector<event_type>		event_pool_type;
	typedef	EventPlaceholder<time_type>	event_placeholder_type;
	typedef	EventQueue<event_placeholder_type>
						event_queue_type;
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
	enum {
		/**
			Whether or not the simulation was halted for any 
			reason, self-stopped on error, or interrupted.  
		 */
		FLAG_STOP_SIMULATION = 0x0001,
		FLAG_WATCH_QUEUE = 0x0002,
		FLAG_WATCH_ALL_EVENTS = 0x0004,
		FLAGS_DEFAULT = 0x0000
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
	//	predecessors? (if we want them, construct separate)ly
private:
	time_type				current_time;
	// time_type				uniform_delay;

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

	const event_type&
	get_event(const event_index_type i) const { return event_pool[i]; }

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

	ostream&
	dump_struct(ostream&) const;

	ostream&
	dump_struct_dot(ostream&, const graph_options&) const;

	ostream&
	dump_event(ostream&, const event_index_type, const time_type) const;

	ostream&
	dump_event_queue(ostream&) const;

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

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

