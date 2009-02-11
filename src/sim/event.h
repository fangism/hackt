/**
	\file "sim/event.h"
	Generic event placeholder type.  
	$Id: event.h,v 1.5 2009/02/11 02:35:15 fang Exp $
 */

#ifndef	__HAC_SIM_EVENT_H__
#define	__HAC_SIM_EVENT_H__

/**
	Define to 1 to use multimap event queue instead of priority queue.
	Multimap is better for consistency in checkpointing because of
	first-come-first-serve ordering among equal keys, but may be 
	slightly slower.  
 */
#define	MULTIMAP_EVENT_QUEUE		1

#include <iosfwd>
#if MULTIMAP_EVENT_QUEUE
#include <set>
#else
#include <queue>
#endif
#include <vector>
#include "sim/common.h"
#include "util/macros.h"
#include "util/utypes.h"

/**
	invariant: each event-id is in the queue at most once.  
	Has to be really messed up for this sanity check to fail.
	Default: off (0)
 */
#ifndef	CHECK_UNIQUE_EVENTS
#define	CHECK_UNIQUE_EVENTS		0
#endif

#if CHECK_UNIQUE_EVENTS
#include <set>
#endif

namespace HAC {
namespace SIM {
using std::istream;
using std::ostream;
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
	typedef	event_index_type	index_type;
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
#if MULTIMAP_EVENT_QUEUE
		return time < r.time;
#else
		return time > r.time;
#endif
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
	TODO: compare multimap vs. priority_queue
	TODO: document a consistent interface.  
	TODO: use list_vector pool allocation.  
	\param E is the event-placeholder type.  
 */
EVENT_QUEUE_TEMPLATE_SIGNATURE
class EventQueue {
	typedef	EVENT_QUEUE_CLASS		this_type;
public:
	typedef	E				value_type;
	typedef	const value_type&		const_reference;
private:
	/**
		Heap-like structure. 
		Also consider trying multimap.  
	 */
#if MULTIMAP_EVENT_QUEUE
	typedef	std::multiset<value_type>
#else
	typedef	std::priority_queue<value_type, vector<value_type> >
#endif
						queue_type;
#if CHECK_UNIQUE_EVENTS
	typedef	typename value_type::index_type	index_type;
	typedef	std::set<index_type>		index_set_type;
	typedef	typename index_set_type::iterator	index_iterator;
	/**
		Optional: side data structure for tracking event uniqueness.
	 */
	index_set_type				index_set;
#endif
public:
#if MULTIMAP_EVENT_QUEUE
	typedef	typename queue_type::const_iterator	const_iterator;
#endif
private:
	/**
		Internal event queue.
		invariant: placeholders' event_index must be unique, 
			i.e., no duplicates.  
	 */
	queue_type				equeue;

public:
	EventQueue();
	// allow default copy-constructor
	~EventQueue();

	bool
	empty(void) const {
		return equeue.empty();
	}

#if MULTIMAP_EVENT_QUEUE
	const_iterator
	begin(void) const { return equeue.begin(); }

	const_iterator
	end(void) const { return equeue.end(); }
#endif

	void
	push(const value_type& p) {
#if MULTIMAP_EVENT_QUEUE
		equeue.insert(p);
#else
		equeue.push(p);
#endif
#if CHECK_UNIQUE_EVENTS
		const std::pair<index_iterator, bool>
			i(index_set.insert(p.event_index));
		INVARIANT(i.second);
#endif
	}

#if MULTIMAP_EVENT_QUEUE
	// for set insertion interface
	void
	insert(const value_type& p) { push(p); }
#endif

	size_t
	size(void) const { return equeue.size(); }

	/**
		\pre queue is not empty.  
	 */
	value_type
	pop(void) {
#if MULTIMAP_EVENT_QUEUE
		const typename queue_type::const_iterator i(equeue.begin());
		const value_type ret(*i);	// copy and return
		equeue.erase(i);		// log time
#else
		const value_type ret(equeue.top());
		equeue.pop();
#endif
#if CHECK_UNIQUE_EVENTS
		const index_iterator f(index_set.find(ret.event_index));
		INVARIANT(f != index_set.end());
		index_set.erase(f);
#endif
		return ret;
	}

	// semantically equivalent variation, but guaranteed to elide copy
	void
	pop(value_type& v) {
#if MULTIMAP_EVENT_QUEUE
		const typename queue_type::const_iterator i(equeue.begin());
		v = *i;
		equeue.erase(i);		// log time
#else
		v = equeue.top();
#endif
#if CHECK_UNIQUE_EVENTS
		const index_iterator f(index_set.find(v.event_index));
		INVARIANT(f != index_set.end());
		index_set.erase(f);
#endif
#if MULTIMAP_EVENT_QUEUE
		equeue.erase(i);		// log time
#else
		equeue.pop();
#endif
	}

	/**
		\pre This queue must not be empty.  
		\return the next event time.  
	 */
	const value_type&
	top(void) const {
		INVARIANT(!empty());
#if MULTIMAP_EVENT_QUEUE
		return *equeue.begin();
#else
		return equeue.top();
#endif
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

