/**
	\file "sim/chpsim/State.h"
	$Id: InstancePools.h,v 1.1.2.1 2006/12/16 23:54:10 fang Exp $
	Structure that contains the state information of chpsim.  
 */

#ifndef	__HAC_SIM_CHPSIM_INSTANCE_POOL_H__
#define	__HAC_SIM_CHPSIM_INSTANCE_POOL_H__

#include <iosfwd>
#include <vector>
// #include "util/size_t.h"
#include "sim/common.h"
#include "sim/chpsim/Variable.h"
#include "sim/chpsim/Channel.h"

namespace HAC {
namespace entity {
	class state_manager;
	class footprint;
}
namespace SIM {
namespace CHPSIM {
using std::vector;
using std::ostream;
using std::istream;
class Event;
using entity::state_manager;
using entity::footprint;

//=============================================================================
/**
	The complete state of the CHPSIM simulator.  
	TODO: consider how to partition and distribute statically
	for parallel simulation, with min-cut locking.  
	TODO: think about hooks for profiling.
	TODO: how to estimate energy and delay.
 */
class InstancePools {
friend class Event;
	typedef	InstancePools			this_type;
	enum {
		FIRST_VALID_NODE = SIM::INVALID_NODE_INDEX +1
	};
private:
	// shopping list:
	// channel state pool -- channels will have a char
	//	to indicate send/receive state
	// variable pools (bool, int, mpz_t)
	//	no need for user-defined, as they are just composites
	//	of the fundamental types.
	vector<BoolVariable>			bool_pool;
	vector<IntVariable>			int_pool;
	vector<ChannelState>			channel_pool;
	
public:
	explicit
	InstancePools(const state_manager&);

	~InstancePools();

	void
	initialize(void);

	void
	reset(void);

	ostream&
	dump_struct(ostream&, const state_manager&, const footprint&) const;

#if 0
	ostream&
	dump_struct_dot(ostream&) const;
#endif

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

};	// end class State

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_INSTANCE_POOL_H__

