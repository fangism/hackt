/**
	\file "sim/prsim/Event.cc"
	Implementation of prsim event structures.  
	$Id: Event.cc,v 1.2.26.1 2006/03/23 07:05:17 fang Exp $
 */

#include "sim/prsim/Event.h"
#include "sim/prsim/Event.tcc"
#include "util/attributes.h"
#include "util/memory/index_pool.tcc"

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================
// class EventPool method definitions

/**
	This always reserves the 0th entry as an invalid entry.  
	Thus, 0 should never be in the freelist.  
 */
EventPool::EventPool() : event_pool(), free_indices() {
	const event_index_type zero __ATTRIBUTE_UNUSED__ =
		event_pool.allocate();
	INVARIANT(!zero);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EventPool::~EventPool() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventPool::clear(void) {
	free_indices.clear();
	event_pool.clear();
	const event_index_type zero __ATTRIBUTE_UNUSED__ =
		event_pool.allocate();
	INVARIANT(!zero);
}

//=============================================================================
// class EventQueue method definitions
// explicit class instantiation

template class EventQueue<EventPlaceholder<real_time> >;

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

