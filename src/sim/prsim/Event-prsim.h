/**
	\file "sim/prsim/Event.h"
	A firing event, and the queue associated therewith.  
	NOTE: EventQueue and EventPlaceholder have moved to "sim/event.h"
	$Id: Event-prsim.h,v 1.1.40.4 2008/01/15 01:16:18 fang Exp $

	NOTE: file was renamed from:
	Id: Event.h,v 1.8 2007/01/21 06:00:59 fang Exp
	to avoid base-name conflict on dyld with chpsim's.  
 */

#ifndef	__HAC_SIM_PRSIM_EVENT_H__
#define	__HAC_SIM_PRSIM_EVENT_H__

#include <iosfwd>
#include <vector>
#include "sim/common.h"
#include "util/likely.h"
#include "util/attributes.h"
#include "util/macros.h"
#include "util/utypes.h"
#include "util/memory/index_pool.h"
#include "util/memory/free_list.h"
#include "sim/prsim/devel_switches.h"
#include "sim/prsim/Cause.h"

#define	DEBUG_EVENT_POOL_ALLOC				0

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::istream;
using std::ostream;
using std::priority_queue;
using std::vector;
using util::memory::index_pool;
using util::memory::free_list_acquire;
using util::memory::free_list_release;

//=============================================================================
/**
	Based on struct prs_event (PrsEvent).  
	NOTE: sizeof(event) should be 16B, aligned/padded.  
 */
struct Event {
private:
	typedef	Event			this_type;
public:
	/**
		Event attribute encoding.  
	 */
	typedef	enum {
		EVENT_VACUOUS = 0x01,		// no visible outcome
		EVENT_UNSTABLE = 0x02,		// pending event incomplete
		EVENT_INTERFERENCE = 0x04,	// contention with pending event
		EVENT_WEAK = 0x08,		// w.r.t. LOGIC_OTHER
		EVENT_WEAK_UNSTABLE = EVENT_UNSTABLE | EVENT_WEAK,
		EVENT_WEAK_INTERFERENCE = EVENT_INTERFERENCE | EVENT_WEAK
	} event_flags_enum;

	typedef	EventCause		cause_type;
public:
	/**
		Event classification table of 
		pull-up-state vs. event pending value.  
	 */
	static const uchar		upguard[3][3];
	/**
		Event classification table of 
		pull-dn-state vs. event pending value.  
	 */
	static const uchar		dnguard[3][3];
public:
	/**
		The index of the node to switch.
	 */
	node_index_type			node;
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
	/**
		The (node, value) pair that spawned this event.
	 */
	cause_type			cause;
#else
	/**
		The index of the node that caused this event to 
		become enqueued.  
		This is often the last arriving input to some guard
		expression, but can also be attributed to exclusive rings.  
		This index may be null/invalid.  
	 */
	node_index_type			cause_node;
#endif
	/**
		The index of the rule expression that caused this to fire, 
		also the source of the delay value.  
	 */
	rule_index_type			cause_rule;
	/**
		The node's new value: 0, 1, 2 (X).
		See Node::value enumeration.
	 */
	uchar				val;
protected:
	enum {
		/**
			Signals that event is pending interference.  
		 */
		EVENT_FLAG_PENDING_INTERFERENCE = 0x01,
		/**
			Signals that event cancelled.  
		 */
		EVENT_FLAG_KILLED = 0x02,
		/**
			Whether or not event is coerced, say, by the user.  
			Coerced events may require special handling.  
		 */
		EVENT_FLAG_FORCED = 0x04,
#if PRSIM_WEAK_RULES
		/**
			True if event was attributed to a weak rule.
		 */
		EVENT_WEAK_RULE = 0x08,
#endif
		// add more as seen fit
		EVENT_FLAGS_DEFAULT_VALUE = 0x00
	};
	/**
		Additional event flags.  
		Protected visibility to use method interface.  
	 */
        uchar				flags;
	// note: room for one more short int
public:
	Event() : node(INVALID_NODE_INDEX),
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		cause(), 
#else
		cause_node(INVALID_NODE_INDEX), 
#endif
		cause_rule(INVALID_RULE_INDEX), 
		val(0), 
		flags(EVENT_FLAGS_DEFAULT_VALUE) { }

	/**
		The rule index is allowed to be NULL (invalid), 
		to indicate an external (perhaps user) cause.  
		\param w true for weak rules
	 */
	Event(const node_index_type n,
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		const cause_type& c, 
#else
		const node_index_type c, 
#endif
		const rule_index_type r, 
		const uchar v
#if PRSIM_WEAK_RULES
		, const bool w
#endif
		) :
		node(n),
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		cause(c), 
#else
		cause_node(c), 
#endif
		cause_rule(r),
		val(v),
		flags(
#if PRSIM_WEAK_RULES
			w ? EVENT_WEAK_RULE : 
#endif
			EVENT_FLAGS_DEFAULT_VALUE
			) { }

	void
	kill(void) { flags |= EVENT_FLAG_KILLED; }

	bool
	killed(void) const { return flags & EVENT_FLAG_KILLED; }

	void
	force(void) { flags |= EVENT_FLAG_FORCED; }

	void
	unforce(void) { flags &= ~EVENT_FLAG_FORCED; }

	bool
	forced(void) const { return flags & EVENT_FLAG_FORCED; }

#if PRSIM_WEAK_RULES
	bool
	is_weak(void) const { return flags & EVENT_WEAK_RULE; }

	void
	set_weak(const bool w) {
		if (w)	flags |= EVENT_WEAK_RULE;
		else	flags &= ~EVENT_WEAK_RULE;
	}
#endif

	void
	set_cause_node(const node_index_type ni) {
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		cause.node = ni;
#else
		cause_node = ni;
#endif
	}

	void
	save_state(ostream&) const;

	void
	load_state(istream&);

	bool
	pending_interference(void) const {
		return flags & EVENT_FLAGS_DEFAULT_VALUE;
	}

	void
	pending_interference(const bool i) {
		if (i)	flags |= EVENT_FLAGS_DEFAULT_VALUE;
		else	flags &= ~EVENT_FLAGS_DEFAULT_VALUE;
	}

	static
	ostream&
	dump_checkpoint_state_header(ostream&);

	static
	ostream&
	dump_checkpoint_state(ostream&, istream&);

} __ATTRIBUTE_ALIGNED__ ;	// end struct Event

//=============================================================================
/**
	Allocator for events.  
	Returns indices instead of pointers.  
	Could template this: combining pool_type with free_list type!
	Can't you tell I love templates?
 */
class EventPool {
public:
	typedef	Event				event_type;
	typedef	index_pool<vector<Event> >	event_allocator_type;
	/**
		TODO: use a more compact discrete_interval_set
	 */
	typedef	vector<event_index_type>	free_list_type;
private:
	event_allocator_type			event_pool;
	free_list_type				free_indices;
public:
	EventPool();
	~EventPool();

	const Event&
	operator [] (const event_index_type i) const {
		INVARIANT(i);
		return event_pool[i];
	}

	Event&
	operator [] (const event_index_type i) {
		INVARIANT(i);
		return event_pool[i];
	}

	bool
	check_valid_empty(void) const;

#if DEBUG_EVENT_POOL_ALLOC
	event_index_type
	allocate(const event_type& e);
#else
	event_index_type
	allocate(const event_type& e) {
		if (UNLIKELY(free_indices.empty())) {	// UNLIKELY
			const event_index_type ret = event_pool.size();
			event_pool.allocate(e);	// will realloc
			INVARIANT(ret);
			return ret;
		} else {			// LIKELY
			const event_index_type ret =
				free_list_acquire(free_indices);
			event_pool[ret] = e;
			return ret;
		}
	}
#endif

#if DEBUG_EVENT_POOL_ALLOC
	void
	deallocate(const event_index_type i);
#else
	void
	deallocate(const event_index_type i) {
		INVARIANT(i);
		free_list_release(free_indices, i);
	}
#endif

	void
	clear(void);

};	// end class EventPool

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EVENT_H__

