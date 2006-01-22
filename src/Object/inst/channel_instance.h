/**
	\file "Object/inst/channel_instance.h"
	Definition of channel instance state.  
	$Id: channel_instance.h,v 1.4 2006/01/22 18:19:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CHANNEL_INSTANCE_H__
#define	__HAC_OBJECT_INST_CHANNEL_INSTANCE_H__

#include "Object/traits/chan_traits.h"
#include "Object/inst/state_instance.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Placeholder for channel state information.  
	An actual instantiated instance of a channel.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
class class_traits<channel_tag>::state_instance_base {
};	// end class state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_CHANNEL_INSTANCE_H__

