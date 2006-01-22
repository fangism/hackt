/**
	\file "sim/prsim/Event.h"
	A firing event, and the queue associated therewith.  
	$Id: Event.h,v 1.2 2006/01/22 06:53:27 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EVENT_H__
#define	__HAC_SIM_PRSIM_EVENT_H__

#include <map>
#include <queue>
#include <vector>
#include "sim/common.h"
#include "sim/time.h"
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
	/**
		The index of the node to switch.
	 */
	node_index_type			node;
	/**
		The node's new value: 0, 1, X.
	 */
	unsigned char			val;

};	// end struct Event

//=============================================================================
/**
	An ordered placeholder for events.  
	These will go into the event queue on behalf of the actual event.  
 */
struct EventPlaceholder {
	/**
		The time at which the event should occur.  
		This is also the Key.  
	 */
	real_time			time;
	/**
		The index of the corresponding event belonging to the
		global event pool.  
		This will be used to index directly into the pool.  
	 */
	event_index_type		event_index;

	bool
	operator < (const EventPlaceholder& r) const {
		return time < r.time;
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
		return event_pool[i];
	}

	Event&
	operator [] (const event_index_type i) {
		return event_pool[i];
	}

	event_index_type
	allocate(void) {
		if (free_indices.empty()) {	// UNLIKELY
			const event_index_type ret = event_pool.size();
			event_pool.allocate();	// will realloc
			return ret;
		} else {			// LIKELY
			return free_list_acquire(free_indices);
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
/**
	For now, hard-coded to one type of event.  
	TODO: template this to use custom structures.  
	TODO: compare map vs. priority_queue
	TODO: document a consistent interface.  
	TODO: instead of using pointers, recycle ID numbers.  
	TODO: use list_vector pool allocation.  
 */
class EventQueue {
	/**
		Heap-like structure. 
	 */
	typedef	priority_queue<EventPlaceholder, vector<EventPlaceholder> >
						queue_type;
	queue_type				equeue;

public:
	EventQueue();
	~EventQueue();

	bool
	empty(void) const {
		return equeue.empty();
	}

	void
	push(const EventPlaceholder& p) {
		equeue.push(p);
	}

	/**
		\pre queue is not empty.  
	 */
	EventPlaceholder
	pop(void) {
		const EventPlaceholder ret(equeue.top());
		equeue.pop();
		return ret;
	}

	void
	clear(void);

};	// end struct EventQueue

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EVENT_H__

