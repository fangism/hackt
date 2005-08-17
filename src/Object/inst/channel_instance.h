/**
	\file "Object/inst/channel_instance.h"
	Definition of channel instance state.  
	$Id: channel_instance.h,v 1.1.2.2 2005/08/17 03:15:01 fang Exp $
 */

#ifndef	__OBJECT_INST_CHANNEL_INSTANCE_H__
#define	__OBJECT_INST_CHANNEL_INSTANCE_H__

#include "Object/traits/chan_traits.h"
#include "Object/inst/state_instance.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Placeholder for channel state information.  
	An actual instantiated instance of a channel.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
class class_traits<channel_tag>::state_instance_base {
#if !EMPTY_PLACEHOLDER_STATE_INSTANCE
	typedef	state_instance_actuals_ptr_type		actuals_ptr_type;
protected:
	actuals_ptr_type				actuals_ptr;
public:
	STATE_INSTANCE_GET_ACTUALS_PROTO {
		return actuals_ptr;
	}

	STATE_INSTANCE_SET_ACTUALS_PROTO {
		INVARIANT(!actuals_ptr);
		actuals_ptr = arg;
	}

	STATE_INSTANCE_PERSISTENCE_PROTOS
#endif
};	// end class state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_CHANNEL_INSTANCE_H__

