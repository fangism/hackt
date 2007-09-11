/**
	\file "sim/chpsim/Event.h"
	Various classes of chpsim events.  
	$Id: Event.h,v 1.11 2007/09/11 06:53:08 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_EVENT_H__
#define	__HAC_SIM_CHPSIM_EVENT_H__

#include "util/size_t.h"
#include "util/attributes.h"
#include "util/string_fwd.h"
#include "sim/chpsim/devel_switches.h"
#include <iosfwd>
#include "sim/time.h"
#include "sim/chpsim/Dependence.h"
#include "util/macros.h"

namespace HAC {
namespace entity {
struct expr_dump_context;
namespace CHP {
	class action;
	class local_event;
}
}
namespace SIM {
namespace CHPSIM {
class DependenceSetCollector;
class nonmeta_context;
class graph_options;
class State;		// arg... event shouldn't depend on State?
using std::ostream;
using std::string;
using entity::CHP::action;
using entity::nonmeta_state_manager;

//=============================================================================
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
public:
	typedef	entity::CHP::local_event	local_event_type;
	/**
		Hard-coded time type for now.
	 */
	typedef	real_time		time_type;
	typedef	size_t			event_index_type;
	static const real_time		default_delay;
private:
	/**
		Pointer to instance-invariant event information.
	 */
	const local_event_type*		__local_event;
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

	EventNode(const local_event_type*, const time_type);

	~EventNode();

	/**
		\return true if this event type is considered trivial, 
			such as concurrent forks and joins, end-select, 
			but NOT condition waits.  
			This determines whether or not meta-events may
			be combined in event graph optimization.  
	 */
	bool
	is_trivial(void) const;

	/**
		Condition-waits are classified as having "trivial-delay", 
		even though they take up an action event node.
		They shouldn't incur additional delay unless set otherwise.
	 */
	bool
	has_trivial_delay(void) const;

	const local_event_type&
	get_local_event(void) const { return *__local_event; }

	/**
		this 'leaks' out the pointer, it is not meant to be misused, 
		only short-lived temporary references.  
	 */
	const action*
	get_chp_action(void) const;

	unsigned short
	get_event_type(void) const;

	unsigned short
	get_predecessors(void) const;

	size_t
	num_successors(void) const;

	const event_index_type*		// const_iterator
	local_successors_begin(void) const;

	const event_index_type*		// const_iterator
	local_successors_end(void) const;

	void
	make_global_root(const local_event_type*);

	void
	setup(const local_event_type*, const State&);

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
	dump_brief(ostream&, 
		const entity::expr_dump_context&) const;

	// overloaded, I know...
	ostream&
	dump_source(ostream&, 
		const entity::expr_dump_context&) const;

	ostream&
	dump_pending(ostream&) const;

	ostream&
	dump_struct(ostream&, const entity::expr_dump_context&,
		const size_t, const event_index_type) const;

	ostream&
	dump_dot_node(ostream&, const event_index_type, 
		const graph_options&, const entity::expr_dump_context&,
		const size_t, const event_index_type) const;

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

