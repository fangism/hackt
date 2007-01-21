/**
	\file "sim/event.h"
	Generic event placeholder type.  
	$Id: event.h,v 1.2 2007/01/21 06:00:29 fang Exp $
 */

#ifndef	__HAC_SIM_EVENT_H__
#define	__HAC_SIM_EVENT_H__

#include <iosfwd>
#include <queue>
#include <vector>
#include "sim/common.h"
#include "util/macros.h"
#include "util/utypes.h"

#define	DEBUG_EVENT_POOL_ALLOC				0

namespace HAC {
namespace SIM {
using std::istream;
using std::ostream;
using std::priority_queue;
using std::vector;

//=============================================================================
/**
	An ordered placeholder for events.  
	These will go into the event queue on behalf of the actual event.  
 */
template <typename T>
struct EventPlaceholder {
private:
	typedef	EventPlaceholder<T>	this_type;
public:
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

	/**
		Composition functor of member_select and equality comparison.
	 */
	struct index_finder {
		const event_index_type		index;
		explicit
		index_finder(const event_index_type i) : index(i) { }

		bool
		operator () (const this_type& t) {
			return t.event_index == this->index;
		}
	};
};	// end struct EventPlaceholder

//=============================================================================
#define	EVENT_QUEUE_TEMPLATE_SIGNATURE		template <class E>
#define	EVENT_QUEUE_CLASS			EventQueue<E>
/**
	For now, hard-coded to one type of event.  
	TODO: template this to use custom structures.  
	TODO: compare map vs. priority_queue
	TODO: document a consistent interface.  
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

	size_t
	size(void) const { return equeue.size(); }

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
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_EVENT_H__

