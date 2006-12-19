/**
	\file "sim/chpsim/State.h"
	$Id: InstancePools.h,v 1.1.2.2 2006/12/19 23:44:12 fang Exp $
	Structure that contains the state information of chpsim.  
 */

#ifndef	__HAC_SIM_CHPSIM_INSTANCE_POOL_H__
#define	__HAC_SIM_CHPSIM_INSTANCE_POOL_H__

#if 0
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
	class bool_tag;
	class int_tag;
	class channel_tag;
}
namespace SIM {
namespace CHPSIM {
using std::vector;
using std::ostream;
using std::istream;
class Event;
using entity::state_manager;
using entity::footprint;
using entity::bool_tag;
using entity::int_tag;
using entity::channel_tag;

//=============================================================================
/**
	TODO: define this in corresponding class_traits?
 */
template <class Tag>
struct variable_type { };

template <>
struct variable_type<bool_tag> {
	typedef	BoolVariable			type;
};

template <>
struct variable_type<int_tag> {
	typedef	IntVariable			type;
};

template <>
struct variable_type<channel_tag> {
	typedef	ChannelState			type;
};

//=============================================================================
/**
	Policy-based base class wrapper for simulator state.  
 */
template <class Tag>
class instance_pool_base {
	typedef	variable_type<Tag>		implementation;
	typedef	typename implementation::type	instance_type;
protected:
	vector<instance_type>			pool;

	explicit
	instance_pool_base(const state_manager&);

	~instance_pool_base();

};	// end class instance_pool_base

//=============================================================================
/**
	The complete state of the CHPSIM simulator.  
	TODO: consider how to partition and distribute statically
	for parallel simulation, with min-cut locking.  
	TODO: think about hooks for profiling.
	TODO: how to estimate energy and delay.
 */
class InstancePools :
	protected instance_pool_base<bool_tag>,
	protected instance_pool_base<int_tag>,
	protected instance_pool_base<channel_tag> {
friend class Event;
	typedef	InstancePools			this_type;
	typedef	instance_pool_base<bool_tag>	bool_base_type;
	typedef	instance_pool_base<int_tag>	int_base_type;
	typedef	instance_pool_base<channel_tag>	channel_base_type;
	enum {
		FIRST_VALID_NODE = SIM::INVALID_NODE_INDEX +1
	};
private:
#if 0
	// shopping list:
	// channel state pool -- channels will have a char
	//	to indicate send/receive state
	// variable pools (bool, int, mpz_t)
	//	no need for user-defined, as they are just composites
	//	of the fundamental types.
	vector<BoolVariable>			bool_pool;
	vector<IntVariable>			int_pool;
	vector<ChannelState>			channel_pool;
#endif
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
#else
// #include "Object/nonmeta_state.h"
namespace HAC {
namespace entity {
	class nonmeta_state_manager;
}
namespace SIM {
namespace CHPSIM {
typedef	entity::nonmeta_state_manager		InstancePools;
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC
#endif

#endif	// __HAC_SIM_CHPSIM_INSTANCE_POOL_H__

