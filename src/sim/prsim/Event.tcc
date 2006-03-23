/**
	\file "sim/prsim/Event.tcc"
	Implementation of prsim event structures.  
	$Id: Event.tcc,v 1.1.2.1 2006/03/23 07:05:18 fang Exp $
 */

#include "sim/prsim/Event.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================
// class EventQueue method definitions

EVENT_QUEUE_TEMPLATE_SIGNATURE
EVENT_QUEUE_CLASS::EventQueue() : equeue() { }

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
	this_type c(*this);
	while (!c.empty()) {
		s.push_back(c.equeue.top());
		c.pop();
	}
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

