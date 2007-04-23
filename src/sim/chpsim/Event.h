/**
	\file "sim/chpsim/Event.h"
	Various classes of chpsim events.  
	$Id: Event.h,v 1.6.2.7 2007/04/23 19:30:15 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_EVENT_H__
#define	__HAC_SIM_CHPSIM_EVENT_H__

#include "util/size_t.h"
#include "util/attributes.h"
#include "util/string_fwd.h"
#include <iosfwd>
#include <valarray>
#include <vector>
#include "sim/time.h"
#include "sim/chpsim/Dependence.h"
#include "sim/chpsim/devel_switches.h"
#include "util/macros.h"

namespace HAC {
namespace entity {
namespace CHP {
	class action;
}
}
namespace SIM {
namespace CHPSIM {
class DependenceSetCollector;
class nonmeta_context;
class graph_options;
using std::ostream;
using std::string;
using std::vector;
using std::valarray;
using entity::CHP::action;
using entity::nonmeta_state_manager;

//=============================================================================
/**
	Event type numeration codes.
	Can also introduce 'special' types of events like I/O.  
 */
enum {
	EVENT_NULL = 0,		///< can be used to mean 'skip' or 'no-op'
	EVENT_ASSIGN = 1,
	EVENT_SEND = 2,
	EVENT_RECEIVE = 3,
	EVENT_CONCURRENT_FORK = 4,	///< divergence of concurrent events
	EVENT_CONCURRENT_JOIN = EVENT_NULL,	///< convergence event (no-op)
	/**
		the start of any selection: 
		deterministic, non-deterministic, and do-while loops
	 */
	EVENT_SELECTION_BEGIN = 5,
	EVENT_SELECTION_END = EVENT_NULL,	///< end of any selection (no-op)
	EVENT_CONDITION_WAIT = 6
};

/**
	Enumerated results for recheck functions.  
 */
enum recheck_result {
	// flag to enqueue this event
	__RECHECK_ENQUEUE_THIS = 0x01,
	// flag to subscribe this event to its deps (exclusive with unsub.)
	__RECHECK_SUBSCRIBE_THIS = 0x02,
	// flag to unsubscribe this event from deps (exclusive with subscribe)
	__RECHECK_UNSUBSCRIBE_THIS = 0x04,

	/**
		The invoking event should be blocked, 
		and its dependencies subscribed.  
	 */
	RECHECK_BLOCKED_THIS = __RECHECK_SUBSCRIBE_THIS,
	/**
		The invoking event should be unblocked, 
		and its dependencies unsubscribed from.  
	 */
	RECHECK_UNBLOCKED_THIS = 
		__RECHECK_ENQUEUE_THIS | __RECHECK_UNSUBSCRIBE_THIS,
	/**
		Event is only blocked because predecessors haven't
		all arrived.  
	 */
	RECHECK_COUNT_BLOCK = 0x00,
	/**
		Special case of UNBLOCKED_THIS that never requires 
		subscription management because the event never blocks, 
		e.g. assignment.  
		Flagging this saves a call to unsubscribe.  
	 */
	RECHECK_NEVER_BLOCKED = __RECHECK_ENQUEUE_THIS,
	/**
		Selections only.
		The selection itself was unblocked, but its immediate
		successor blocked (or unblocked).  Callee will take 
		responsibility for successor's un/subscription, 
		while caller should just unsubscribe this event's deps.  
	 */
	RECHECK_DEFERRED_TO_SUCCESSOR = __RECHECK_UNSUBSCRIBE_THIS
};

//=============================================================================
/**
	This represents a node in the event graph, which can be 
	thought of as a marked graph.  
	We 'allocate' graph nodes with back-references to their
	corresponding events in the CHP footprint.  
	The 'edges' between graph nodes form predecessor-successor relations.
	This will be used as a base class for other events.  
	TODO: align to natural boundary.
 */
class EventNode {
	typedef	EventNode		this_type;
public:
	/**
		Hard-coded time type for now.
	 */
	typedef	real_time		time_type;
	typedef	size_t			event_index_type;
	typedef	valarray<event_index_type>	successor_list_type;
	static const real_time		default_delay;
	static const char		node_prefix[];
private:
	/**
		the (atomic) event to occur corresponding to this node
		Would be nice if some of theses actions were 
		resolved to static references... (don't optimize now)
		Can be lightweight pointer instead of reference count?
	 */
	// count_ptr<const action>		action_ptr;
	const action*			action_ptr;
public:
	/**
		events that follow this event.  
		The interpretation of these events (concurrent, 
		deterministic, nondeterministic) is depending on the type.  
	 */
	successor_list_type		successor_events;
private:
#if 1
	/**
		enumeration for this event, 
		semi-redundant with the action pointer.  
	 */
	unsigned short			event_type;
#endif
#if 1
	/**
		General purpose flags (space-filler).  
	 */
	unsigned short			flags;
#endif
	/**
		footprint, footprint frame / global_entry<process>
		or just 0-based index to process entries.  (0 => top)
	 */
	size_t				process_index;
	/**
		The number of concurrent event predecessors.
		Wait for this number of events to precede before executing, 
		like a barrier count.  
		Sequential actions (in chain) only have one predecessor.  
		Selection statements re-join with only one predecessor.  
		Do we keep predecessor edge information anywhere else?
		Should be const, incidentally...
	 */
	unsigned short			predecessors;
	/**
		barrier count: from number of predecessors (join operation)
		Event fires when countdown reaches zero, post-decrement.  
		This is dynamic stateful information that needs 
		to be checkpointed.  
		This countdown should be decremented when an event is
		'rechecked' for the first time, as a result of a predecessor
		executing.  Other rechecks induced by dependent variable
		status changes should not cause a decrement.  
		The countdown is reset immediately upon event execution.  
	 */
	unsigned short			countdown;
	/**
		The constant value of delay for this event, 
		set by the event constructor.  
		Default: 10 units.  
	 */
	time_type			delay;
	/**
		Set of variables and channels whose update may affect the
		*blocking* state of this event.  
		Consider using an excl_ptr to this (sparser) if 
		a significant number of events don't need this.  
		TODO: yes, use pointer for private-implementation.
	 */
	DependenceSet			block_deps;
#if CHPSIM_READ_WRITE_DEPENDENCIES
	/**
		The set of variables that this event depends on (read)
		that are NOT already in the block_deps set.  
		TODO: combine into a pair of dependence sets, so we can
		use a single pointer for both sets.  
	 */
	DependenceSet			read_deps;
	/**
		Set of variables and channels that this event MAY affect.
		Only needed for dot-graph construction.
		Consider making this an excl_ptr instead, 
		and its construction optional to save memory.  
	 */
	DependenceSet			anti_deps;
#endif
public:
	EventNode();

	EventNode(const action*, const unsigned short, const size_t pid);

	EventNode(const action*, const unsigned short, const size_t pid, 
		const time_type);

	~EventNode();

	// need to override because valarray doesn't implement 
	// operator = using default container behavior
	this_type&
	operator = (const this_type&);

	void
	orphan(void);

	/**
		\return true if this event type is considered trivial, 
			such as concurrent forks and joins, end-select, 
			but NOT condition waits.  
			This determines whether or not meta-events may
			be combined in event graph optimization.  
	 */
	bool
	is_trivial(void) const {
		return (event_type == EVENT_NULL) ||
			(event_type == EVENT_CONCURRENT_FORK);
	}

	/**
		Condition-waits are classified as having "trivial-delay", 
		even though they take up an action event node.
		They shouldn't incur additional delay unless set otherwise.
	 */
	bool
	has_trivial_delay(void) const {
		return is_trivial() || (event_type == EVENT_CONDITION_WAIT);
	}

	/**
		NOTE: need not be trivial, just copiable and 
		successor-substitutable.  
		NOTE: No selection has only one successor.  
	 */
	bool
	is_movable(void) const {
		return (successor_events.size() == 1);
	}

	/**
		this 'leaks' out the pointer, it is not meant to be misused, 
		only short-lived temporary references.  
	 */
	const action*
	get_chp_action(void) const { return action_ptr; }

	const size_t
	get_process_index(void) const { return process_index; }

	void
	set_event_type(const unsigned short t) { event_type = t; }

	unsigned short
	get_event_type(void) const { return event_type; }

	void
	set_predecessors(const event_index_type n) { predecessors = n; }

	unsigned short
	get_predecessors(void) const { return predecessors; }

	void
	import_block_dependencies(const DependenceSetCollector& d) {
		block_deps.import(d);
	}

	const DependenceSet&
	get_block_dependencies(void) const { return block_deps; }

#if CHPSIM_READ_WRITE_DEPENDENCIES
	void
	import_antidependencies(const DependenceSetCollector& d) {
		anti_deps.import(d);
	}
#endif

	time_type
	get_delay(void) const { return delay; }

	void
	set_delay(const time_type t) { delay = t; }

	void
	reset(void);

	void
	reset_countdown(void) { countdown = predecessors; }

	void
	execute(const nonmeta_context&);

	/// \return true if enqueued.
	bool
	recheck(const nonmeta_context&, const event_index_type) const;

	void
	subscribe_deps(const nonmeta_context&, const event_index_type) const;

	bool
	is_subscribed(const nonmeta_state_manager& s, 
			const event_index_type ei) const {
		return block_deps.is_subscribed(s, ei);
	}

	ostream&
	dump_brief(ostream&) const;

	ostream&
	dump_source(ostream&) const;

	ostream&
	dump_pending(ostream&) const;

	ostream&
	dump_struct(ostream&) const;

	ostream&
	dump_dot_node(ostream&, const event_index_type, 
		const graph_options&) const;

	ostream&
	dump_successor_edges_default(ostream&, const event_index_type) const;

	/// forwarded call
	ostream&
	dump_subscribed_status(ostream& o, const nonmeta_state_manager& s, 
			const event_index_type ei) const {
		return block_deps.dump_subscribed_status(o, s, ei);
	}

public:
	// helper classes
	class countdown_decrementer {
		typedef	vector<this_type>	event_pool_type;
		event_pool_type&		pool;
	public:
		explicit
		countdown_decrementer(event_pool_type& p) : pool(p) { }

		void
		operator () (const event_index_type ei) const {
			// INVARIANT checks and assertions?
			INVARIANT(pool[ei].countdown);
			--pool[ei].countdown;
		}
	};	// end struct count_decrementer

};	// end class EventNode

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_EVENT_H__

