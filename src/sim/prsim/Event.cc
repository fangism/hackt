/**
	\file "sim/prsim/Event.cc"
	Implementation of prsim event structures.  
	$Id: Event.cc,v 1.2.26.4 2006/03/29 05:49:28 fang Exp $
 */

#include "sim/prsim/Event.h"
#include "sim/prsim/Event.tcc"
#include "sim/time.h"
#include "util/attributes.h"
#include "util/memory/index_pool.tcc"

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================
// class Event static initializations

/**
	First index is the guard's pulling state (F = OFF, T = ON, X = WEAK),
	second index is the pending event state.
 */
const char
Event::upguard[3][3] = {
	{	EVENT_VACUOUS, 		// guard F, event F: vacuous
		EVENT_UNSTABLE,		// guard F, event T: unstable
		EVENT_VACUOUS		// guard F, event X: vacuous
	},
	{	EVENT_INTERFERENCE,	// guard T, event F: interference
		EVENT_VACUOUS,		// guard T, event T: vacuous
		EVENT_VACUOUS		// guard T, event X: vacuous
	},
	{	EVENT_WEAK_INTERFERENCE,// guard X, event F:
		EVENT_WEAK_UNSTABLE,	// guard X, event T:
		EVENT_VACUOUS		// guard X, event X: vacuous
	}
};

const char
Event::dnguard[3][3] = {
	{	EVENT_UNSTABLE,		// guard F, event F: vacuous
		EVENT_VACUOUS,		// guard F, event T: unstable
		EVENT_VACUOUS		// guard F, event X: vacuous
	},
	{	EVENT_VACUOUS,		// guard T, event F: interference
		EVENT_INTERFERENCE,	// guard T, event T: vacuous
		EVENT_VACUOUS		// guard T, event X: vacuous
	},
	{	EVENT_WEAK_UNSTABLE,	// guard X, event F:
		EVENT_WEAK_INTERFERENCE,// guard X, event T:
		EVENT_VACUOUS		// guard X, event X: vacuous
	}
};

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

