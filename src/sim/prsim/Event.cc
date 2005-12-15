/**
	\file "sim/prsim/Event.cc"
	Implementation of prsim event structures.  
	$Id: Event.cc,v 1.1.2.1 2005/12/15 04:46:05 fang Exp $
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

//=============================================================================

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

