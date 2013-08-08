/**
	\file "sim/prsim/Event.hh"
	A firing event, and the queue associated therewith.  
	NOTE: EventQueue and EventPlaceholder have moved to "sim/event.hh"
	$Id: Event-prsim.hh,v 1.6 2010/09/23 00:19:53 fang Exp $

	NOTE: file was renamed from:
	Id: Event.hh,v 1.8 2007/01/21 06:00:59 fang Exp
	to avoid base-name conflict on dyld with chpsim's.  
 */

#ifndef	__HAC_SIM_PRSIM_EVENT_H__
#define	__HAC_SIM_PRSIM_EVENT_H__

#include <iosfwd>
#include <vector>
#include "sim/common.hh"
#include "util/likely.h"
#include "util/attributes.h"
#include "util/macros.h"
#include "util/utypes.h"
#include "util/memory/index_pool.hh"
#include "util/memory/free_list.hh"
#include "sim/prsim/devel_switches.hh"
#include "sim/prsim/Cause.hh"
#include "sim/trace_common.hh"		// for trace_index_type

/**
	Verbosely trace each item added and removed from pool free list.
 */
#define	DEBUG_EVENT_POOL_ALLOC				0
/**
	Use a set for free-list to check for uniqueness.
	I think a set<> (unpooled) will be slower than vector<>.
	TODO: use a discrete_interval_set for memory efficiency?
 */
#define	PARANOID_EVENT_FREE_LIST			1

#if PARANOID_EVENT_FREE_LIST
#include <set>
#endif

/**
	Define to 1 to store rule_type* along with cause rule_index
	to avoid having to lookup rule pointer later.
	Rationale: performance.
 */
#define	EVENT_INCLUDE_RULE_POINTER				1
#if EVENT_INCLUDE_RULE_POINTER
#include "sim/prsim/Rule.hh"		// for rule_time_type
#endif

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
using SIM::trace_index_type;
using SIM::INVALID_TRACE_INDEX;

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

	/**
		Extension of cause that includes trace index.  
		This is a good place to keep track of the critical
		event trace index, because this is passed around, 
		and helps minimize invasiveness of change in code.  
	 */
	struct cause_type : public EventCause {
		/**
			When simulation tracing is on, this index references
			an event in the entire event history.  
			This points to the last event that corresponds
			to the cause_rule firing, hence critical path.  
		 */
		trace_index_type	critical_trace_event;

		cause_type() : EventCause(),
			critical_trace_event(INVALID_TRACE_INDEX) { }

		cause_type(const node_index_type n, const value_enum v, 
			const trace_index_type t = INVALID_TRACE_INDEX) :
			EventCause(n, v), critical_trace_event(t) { }

	};	// end struct cause_type
#if EVENT_INCLUDE_RULE_POINTER
	typedef	Rule<rule_time_type>	rule_type;
#endif
public:
	/**
		Event classification table of 
		pull-up-state vs. event pending value.  
		Q: would this be faster to access array was 4-char aligned?
	 */
	static const uchar		upguard[3][3];
	/**
		Event classification table of 
		pull-dn-state vs. event pending value.  
		Q: would this be faster to access array was 4-char aligned?
	 */
	static const uchar		dnguard[3][3];
public:
	/**
		The index of the node to switch.
	 */
	node_index_type			node;
	/**
		The (node, value) pair that spawned this event.
	 */
	cause_type			cause;
	/**
		The index of the rule expression that caused this to fire, 
		also the source of the delay value.  
	 */
	rule_index_type			cause_rule;
#if EVENT_INCLUDE_RULE_POINTER
	/**
		Pointer to the rule referenced by the cause_rule index.
		Passing this together avoids an extra lookup_rule() call.
	 */
	const rule_type*		cause_rule_ptr;
#endif
	/**
		The node's new value: 0, 1, 2 (X).
		See value_enum enumeration, "sim/prsim/enums.h"
	 */
	value_enum			val;
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
			This could be inferred from the cause_rule, 
			so it should at least be consistent.  
			NOTE: the causing rule may not always be the
			one that is actively pulling, a rule may 
			shut off and allow another to win.
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
		cause(), 
		cause_rule(INVALID_RULE_INDEX), 
#if EVENT_INCLUDE_RULE_POINTER
		cause_rule_ptr(NULL),
#endif
		val(LOGIC_LOW), 
		flags(EVENT_FLAGS_DEFAULT_VALUE) { }

	/**
		The rule index is allowed to be NULL (invalid), 
		to indicate an external (perhaps user) cause.  
		Q: is r allowed to be NULL in the case of user-set events?
		\param w true for weak rules
	 */
	Event(const node_index_type n,
		const cause_type& c, 
		const rule_index_type r, 
#if EVENT_INCLUDE_RULE_POINTER
		const rule_type* rp,
#endif
		const value_enum v
#if PRSIM_WEAK_RULES
		, const bool w
#endif
		) :
		node(n),
		cause(c), 
		cause_rule(r),
#if EVENT_INCLUDE_RULE_POINTER
		cause_rule_ptr(rp),
#endif
		val(v),
		flags(
#if PRSIM_WEAK_RULES
			w ? EVENT_WEAK_RULE : 
#endif
			EVENT_FLAGS_DEFAULT_VALUE
			) { }

#if 1
// for debugging: tracing through copy-constructor
// kludge: clang++-3.1 screws up the synthesized default copy-ctor (crash)
#if 0
	// default copy-ctor
	Event(const Event&);
#else
	// inline
	Event(const Event& e) : node(e.node), cause(e.cause), 
		cause_rule(e.cause_rule), 
#if EVENT_INCLUDE_RULE_POINTER
		cause_rule_ptr(e.cause_rule_ptr), 
#endif
		val(e.val),
		flags(e.flags) {
	}

	// clang-3.1 screws up default assignment operator [synthesized]
	Event&
	operator = (const Event& e) {
		node = e.node;
		cause = e.cause;
		cause_rule = e.cause_rule;
#if EVENT_INCLUDE_RULE_POINTER
		cause_rule_ptr = e.cause_rule_ptr;
#endif
		val = e.val;
		flags = e.flags;
		return *this;
	}
#endif
#endif

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
		cause.node = ni;
	}

	void
	save_state(ostream&) const;

	void
	load_state(istream&);

	bool
	pending_interference(void) const {
		return flags & EVENT_FLAG_PENDING_INTERFERENCE;
	}

	void
	pending_interference(const bool i) {
		if (i)	flags |= EVENT_FLAG_PENDING_INTERFERENCE;
		else	flags &= ~EVENT_FLAG_PENDING_INTERFERENCE;
	}

	ostream&
	dump_raw(ostream&) const;

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
#if PARANOID_EVENT_FREE_LIST
	typedef	std::set<event_index_type>	free_list_type;
#else
	typedef	vector<event_index_type>	free_list_type;
#endif
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
			INVARIANT(ret);
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

	ostream&
	dump_memory_usage(ostream&) const;

};	// end class EventPool

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EVENT_H__

