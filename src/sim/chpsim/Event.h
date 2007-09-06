/**
	\file "sim/chpsim/Event.h"
	Various classes of chpsim events.  
	$Id: Event.h,v 1.10.8.3 2007/09/06 01:12:19 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_EVENT_H__
#define	__HAC_SIM_CHPSIM_EVENT_H__

#include "util/size_t.h"
#include "util/attributes.h"
#include "util/string_fwd.h"
#include "sim/chpsim/devel_switches.h"
#include <iosfwd>
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
#include <valarray>
#endif
#include "sim/time.h"
#include "sim/chpsim/Dependence.h"
#include "util/macros.h"

namespace HAC {
namespace entity {
#if CHPSIM_DUMP_PARENT_CONTEXT
struct expr_dump_context;
#endif
namespace CHP {
	class action;
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	class local_event;
#endif
}
}
namespace SIM {
namespace CHPSIM {
class DependenceSetCollector;
class nonmeta_context;
class graph_options;
using std::ostream;
using std::string;
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
using std::valarray;
#endif
using entity::CHP::action;
using entity::nonmeta_state_manager;

//=============================================================================
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
/**
	Event type numeration codes.
	Can also introduce 'special' types of events like I/O.  
 */
enum {
	EVENT_NULL = 0,		///< can be used to mean 'skip' or 'no-op'
	EVENT_ASSIGN = 1,
	EVENT_SEND = 2,
	EVENT_RECEIVE = 3,
	EVENT_PEEK = 4,		///< read values from channel w/o acknowledge
	EVENT_CONCURRENT_FORK = 5,	///< divergence of concurrent events
	EVENT_CONCURRENT_JOIN = EVENT_NULL,	///< convergence event (no-op)
	/**
		the start of any selection: 
		deterministic, non-deterministic, and do-while loops
	 */
	EVENT_SELECTION_BEGIN = 6,
	EVENT_SELECTION_END = EVENT_NULL,	///< end of any selection (no-op)
	EVENT_CONDITION_WAIT = 7,	///< predicate wait
	EVENT_FUNCTION_CALL = 8		///< external function call
};
#endif	// CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
		Represents no change in status, e.g. still blocked.  
		Don't enqueue, and no need to re-subscribe.
	 */
	RECHECK_NO_OP = 0x00,
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
	This represents a node in the global, whole-program event graph, 
	which can be thought of as a marked graph.  
	We 'allocate' graph nodes with back-references to their
	corresponding events in the CHP footprint.  
	The 'edges' between graph nodes form predecessor-successor relations.
	This will be used as a base class for other events.  
	TODO: align to natural boundary.
 */
class EventNode {
	typedef	EventNode		this_type;
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	typedef	entity::local_event	local_event_type;
#endif
public:
	/**
		Hard-coded time type for now.
	 */
	typedef	real_time		time_type;
	typedef	size_t			event_index_type;
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	typedef	valarray<event_index_type>	successor_list_type;
#endif
	static const real_time		default_delay;
	static const char		node_prefix[];
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	/**
		Pointer to instance-invariant event information.
	 */
	const local_event_type*		__local_event;
#else
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
#endif	// CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
private:
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	/**
		enumeration for this event, 
		semi-redundant with the action pointer.  
	 */
	unsigned short			event_type;
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
		footprint, footprint frame / global_entry<process>
		or just 0-based index to process entries.  (0 => top)
	 */
	size_t				process_index;
#endif	// CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
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
		General purpose flags (space-filler).  
	 */
	unsigned short			flags;
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

#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	EventNode(const local_event_type*, const time_type);
#else
	EventNode(const action*, const unsigned short, const size_t pid);

	EventNode(const action*, const unsigned short, const size_t pid, 
		const time_type);
#endif

	~EventNode();

#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	// need to override because valarray doesn't implement 
	// operator = using default container behavior
	this_type&
	operator = (const this_type&);

	void
	orphan(void);
#endif

	/**
		\return true if this event type is considered trivial, 
			such as concurrent forks and joins, end-select, 
			but NOT condition waits.  
			This determines whether or not meta-events may
			be combined in event graph optimization.  
	 */
	bool
	is_trivial(void) const
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	;
#else
	{
		return (event_type == EVENT_NULL) ||
			(event_type == EVENT_CONCURRENT_FORK);
	}
#endif

	/**
		Condition-waits are classified as having "trivial-delay", 
		even though they take up an action event node.
		They shouldn't incur additional delay unless set otherwise.
	 */
	bool
	has_trivial_delay(void) const
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	;
#else
	{
		return is_trivial() || (event_type == EVENT_CONDITION_WAIT);
	}
#endif

#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	/**
		NOTE: need not be trivial, just copiable and 
		successor-substitutable.  
		NOTE: No selection has only one successor.  
	 */
	bool
	is_movable(void) const {
		return (successor_events.size() == 1);
	}
#endif

	/**
		this 'leaks' out the pointer, it is not meant to be misused, 
		only short-lived temporary references.  
	 */
	const action*
	get_chp_action(void) const
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		;
#else
		{ return action_ptr; }
#endif

#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	const size_t
	get_process_index(void) const { return process_index; }

	void
	set_event_type(const unsigned short t) { event_type = t; }
#endif

	unsigned short
	get_event_type(void) const
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		;
#else
		{ return event_type; }
#endif

#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	void
	set_predecessors(const event_index_type n) { predecessors = n; }
#endif

	unsigned short
	get_predecessors(void) const
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		;
#else
		{ return predecessors; }
#endif

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
	reset_countdown(void) { countdown = get_predecessors(); }

	void
	execute(nonmeta_context&);

	/// \return true if enqueued.
	bool
	recheck(const nonmeta_context&, const event_index_type) const;

	bool
	first_check(const nonmeta_context&, const event_index_type);

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

#if CHPSIM_DUMP_PARENT_CONTEXT
	ostream&
	dump_brief(ostream&, 
		const entity::expr_dump_context&) const;

	// overloaded, I know...
	ostream&
	dump_source(ostream&, 
		const entity::expr_dump_context&) const;
#endif

	ostream&
	dump_pending(ostream&) const;

	ostream&
	dump_struct(ostream&
#if CHPSIM_DUMP_PARENT_CONTEXT
		, const entity::expr_dump_context&
#endif
		) const;

	ostream&
	dump_dot_node(ostream&, const event_index_type, 
		const graph_options&
#if CHPSIM_DUMP_PARENT_CONTEXT
		, const entity::expr_dump_context&
#endif
		) const;

	ostream&
	dump_successor_edges_default(ostream&, const event_index_type) const;

	/// forwarded call
	ostream&
	dump_subscribed_status(ostream& o, const nonmeta_state_manager& s, 
			const event_index_type ei) const {
		return block_deps.dump_subscribed_status(o, s, ei);
	}

};	// end class EventNode

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_EVENT_H__

