/**
	\file "sim/prsim/Event.cc"
	Implementation of prsim event structures.  
	$Id: Event.cc,v 1.1.2.2 2005/12/16 02:43:19 fang Exp $
 */

#include "sim/prsim/Event.h"
#include "util/memory/index_pool.tcc"

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================
// class EventPool method definitions

EventPool::EventPool() : event_pool(), free_indices() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EventPool::~EventPool() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventPool::clear(void) {
	free_indices.clear();
	event_pool.clear();
}

//=============================================================================
// class EventQueue method definitions

EventQueue::EventQueue() : equeue() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EventQueue::~EventQueue() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	std::queue doesn't necessarily have a clear() member function :S.
 */
void
EventQueue::clear(void) {
	equeue.~queue_type();		// placement delete
	new (&equeue) queue_type();	// placement construct
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

