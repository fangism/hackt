/**
	\file "sim/event.tcc"
	Implementation of prsim event structures.  
	$Id: event.tcc,v 1.4 2011/06/22 19:38:30 fang Exp $
 */

#ifndef	__HAC_SIM_EVENT_TCC__
#define	__HAC_SIM_EVENT_TCC__

#include "sim/event.h"
#if MULTIMAP_EVENT_QUEUE
#include <iterator>
#include <functional>
#endif

#if EVENT_QUEUE_FAST_ALLOCATOR
#include "util/memory/chunk_map_pool.tcc"
#include "util/memory/allocator_adaptor.tcc"
#endif

namespace HAC {
namespace SIM {
//=============================================================================
// class EventQueue method definitions

EVENT_QUEUE_TEMPLATE_SIGNATURE
EVENT_QUEUE_CLASS::EventQueue() : equeue()
#if CHECK_UNIQUE_EVENTS
		, index_set()
#endif
	{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EVENT_QUEUE_TEMPLATE_SIGNATURE
EVENT_QUEUE_CLASS::~EventQueue() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	std::queue doesn't necessarily have a clear() member function :S.
 */
EVENT_QUEUE_TEMPLATE_SIGNATURE
void
EVENT_QUEUE_CLASS::clear(void) {
	this->equeue.~queue_type();		// placement delete
	new (&this->equeue) queue_type();	// placement construct
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Copies the event queue into a target sequence, for the sake
	of inspecting or diagnosis.  
	This can be a slow operation, due to copying, but is not expected
	to be used outside of interactive commands.  
 */
EVENT_QUEUE_TEMPLATE_SIGNATURE
template <class S>
void
EVENT_QUEUE_CLASS::copy_to(S& s) const {
#if MULTIMAP_EVENT_QUEUE
	std::copy(this->equeue.begin(), this->equeue.end(), 
		std::back_inserter(s));
#else
	this_type c(*this);
	while (!c.empty()) {
		s.push_back(c.equeue.top());
		c.pop();
	}
#endif
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_EVENT_TCC__

