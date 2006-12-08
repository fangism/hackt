/**
	\file "sim/chpsim/State.h"
	$Id: State.h,v 1.1.2.5 2006/12/08 07:51:26 fang Exp $
	Structure that contains the state information of chpsim.  
 */

#ifndef	__HAC_SIM_CHPSIM_STATE_H__
#define	__HAC_SIM_CHPSIM_STATE_H__

#include <iosfwd>
#include <vector>
#include "sim/time.h"
#include "sim/signal_handler.h"
#include "sim/chpsim/Event.h"
#include "util/string_fwd.h"

namespace HAC {
namespace entity {
	class module;
}
namespace SIM {
namespace CHPSIM {
using std::vector;
using std::string;
using entity::module;
class StateConstructor;

//=============================================================================
/**
	The complete state of the CHPSIM simulator.  
	TODO: consider how to partition and distribute statically
	for parallel simulation, with min-cut locking.  
	TODO: think about hooks for profiling.
	TODO: how to estimate energy and delay.
 */
class State {
friend class StateConstructor;
	typedef	State				this_type;
public:
	typedef	real_time			time_type;
	typedef	signal_handler<this_type>	signal_handler;
private:
	typedef	EventNode			event_type;
	typedef	vector<event_type>		event_pool_type;
	typedef	unsigned int			flags_type;
	enum {
		/**
			Whether or not the simulation was halted for any 
			reason, self-stopped on error, or interrupted.  
		 */
		FLAG_STOP_SIMULATION = 0x0001,
		FLAGS_DEFAULT = 0x0000
	};
private:
	const module&				mod;
	// shopping list:
	// channel state pool -- channels will have a char
	//	to indicate send/receive state
	// variable pools (bool, int, mpz_t)
	//	no need for user-defined, as they are just composites
	//	of the fundamental types.
	// event pools: for each type of event?
public:
	// to give CHP action classes access ...
	event_pool_type				event_pool;
	// event queue: unified priority queue of event_placeholders
	// do we need a successor graph representing allocated
	//	CHP dataflow constructs?  
	//	predecessors? (if we want them, construct separate)ly
private:
	time_type				current_time;
	// time_type				uniform_delay;

	// mode flags
	bool					interrupted;
	flags_type				flags;

	// for command-interpreter
	// ifstream_manager			ifstreams;
public:
	explicit
	State(const module&);

	~State();

	const module&
	get_module(void) const { return mod; }

	void
	initialize(void);

	// step_return_type
	void
	step(void);	// THROWS_STEP_EXCEPTION

	void
	stop(void) {
		flags |= FLAG_STOP_SIMULATION;
		interrupted = true;
	}

};	// end class State

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_STATE_H__

