/**
	\file "sim/prsim/Event.h"
	A firing event, and the queue associated therewith.  
	$Id: Event.h,v 1.4.6.2 2006/05/01 02:59:54 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EVENT_H__
#define	__HAC_SIM_PRSIM_EVENT_H__

#include <map>
#include <queue>
#include <vector>
#include "sim/common.h"
#include "sim/devel_switches.h"
#include "util/likely.h"
#include "util/macros.h"
#include "util/memory/index_pool.h"
#include "util/memory/free_list.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::priority_queue;
using std::vector;
using std::map;
using util::memory::index_pool;
using util::memory::free_list_acquire;
using util::memory::free_list_release;

//=============================================================================
/**
	Based on struct prs_event (PrsEvent).  
 */
struct Event {
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
public:
	static const char		upguard[3][3];
	static const char		dnguard[3][3];
public:
	/**
		The index of the node to switch.
	 */
	node_index_type			node;
#if ENABLE_PRSIM_CAUSE_TRACKING
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
	 */
	unsigned char			val;

	Event() : node(INVALID_NODE_INDEX),
#if ENABLE_PRSIM_CAUSE_TRACKING
		cause_node(INVALID_NODE_INDEX), 
#endif
		cause_rule(INVALID_RULE_INDEX) { }

	/**
		The rule index is allowed to be NULL (invalid), 
		to indicate an external (perhaps user) cause.  
	 */
	Event(const node_index_type n,
#if ENABLE_PRSIM_CAUSE_TRACKING
		const node_index_type c, 
#endif
		const rule_index_type r, 
		const unsigned char v) :
		node(n),
#if ENABLE_PRSIM_CAUSE_TRACKING
		cause_node(c), 
#endif
		cause_rule(r),
		val(v) { }

};	// end struct Event

//=============================================================================
/**
	An ordered placeholder for events.  
	These will go into the event queue on behalf of the actual event.  
 */
template <typename T>
struct EventPlaceholder {
	typedef	T			time_type;
	/**
		The time at which the event should occur.  
		This is also the Key.  
	 */
	time_type			time;
	/**
		The index of the corresponding event belonging to the
		global event pool.  
		This will be used to index directly into the pool.  
	 */
	event_index_type		event_index;

	EventPlaceholder(const time_type t, const event_index_type i)
		: time(t), event_index(i) { }

	/**
		Smaller (sooner) time gets higher priority.  
	 */
	bool
	operator < (const EventPlaceholder& r) const {
		return time > r.time;
	}
};	// end struct EventPlaceholder

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

	void
	deallocate(const event_index_type i) {
		free_list_release(free_indices, i);
	}

	void
	clear(void);

};	// end class EventPool

//=============================================================================
#define	EVENT_QUEUE_TEMPLATE_SIGNATURE		template <class E>
#define	EVENT_QUEUE_CLASS			EventQueue<E>
/**
	For now, hard-coded to one type of event.  
	TODO: template this to use custom structures.  
	TODO: compare map vs. priority_queue
	TODO: document a consistent interface.  
	TODO: instead of using pointers, recycle ID numbers.  
	TODO: use list_vector pool allocation.  
	\param E is the event-placeholder type.  
 */
EVENT_QUEUE_TEMPLATE_SIGNATURE
class EventQueue {
	typedef	EVENT_QUEUE_CLASS		this_type;
public:
	typedef	E				value_type;
private:
	/**
		Heap-like structure. 
		Also consider trying multimap.  
	 */
	typedef	priority_queue<value_type, vector<value_type> >
						queue_type;
private:
	queue_type				equeue;

public:
	EventQueue();
	// allow default copy-constructor
	~EventQueue();

	bool
	empty(void) const {
		return equeue.empty();
	}

	void
	push(const value_type& p) {
		equeue.push(p);
	}

	/**
		\pre queue is not empty.  
	 */
	value_type
	pop(void) {
		const value_type ret(equeue.top());
		equeue.pop();
		return ret;
	}

	// semantically equivalent variation, but guaranteed to elide copy
	void
	pop(value_type& v) {
		v = equeue.top();
		equeue.pop();
	}

	/**
		\pre This queue must not be empty.  
		\return the next event time.  
	 */
	const value_type&
	top(void) const {
		INVARIANT(!empty());
		return equeue.top();
	}

	void
	clear(void);

	template <class S>
	void
	copy_to(S&) const;

};	// end struct EventQueue

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EVENT_H__

