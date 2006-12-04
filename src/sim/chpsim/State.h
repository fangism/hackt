/**
	\file "sim/chpsim/State.h"
	$Id: State.h,v 1.1.2.2 2006/12/04 09:55:57 fang Exp $
	Structure that contains the state information of chpsim.  
 */

#ifndef	__HAC_SIM_CHPSIM_STATE_H__
#define	__HAC_SIM_CHPSIM_STATE_H__

#include <iosfwd>

namespace HAC {
namespace entity {
	class module;
}
namespace SIM {
namespace CHPSIM {

/**
	The complete state of the CHPSIM simulator.  
	TODO: consider how to partition and distribute statically
	for parallel simulation, with min-cut locking.  
	TODO: think about hooks for profiling.
	TODO: how to estimate energy and delay.
 */
class State {
	// shopping list:
	// channel state pool -- channels will have a char
	//	to indicate send/receive state
	// variable pools (bool, int, mpz_t)
	//	no need for user-defined, as they are just composites
	//	of the fundamental types.
	// event pools: for each type of event
	// event queue: unified priority queue of event_placeholders
	// do we need a successor graph representing allocated
	//	CHP dataflow constructs?  
	//	predecessors? (if we want them, construct separate)ly


public:


	// step_return_type
	void
	step(void);	// THROWS_STEP_EXCEPTION


};	// end class State

}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_STATE_H__

