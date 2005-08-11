/**
	\file "Object/inst/channel_instance.h"
	Definition of channel instance state.  
	$Id: channel_instance.h,v 1.1.2.1 2005/08/11 00:20:18 fang Exp $
 */

#ifndef	__OBJECT_INST_CHANNEL_INSTANCE_H__
#define	__OBJECT_INST_CHANNEL_INSTANCE_H__

#include "Object/traits/chan_traits.h"
#include "Object/inst/state_instance.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Channel state information.  
	An actual instantiated instance of a channel.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
class class_traits<channel_tag>::state_instance_base {
// state information, persistence...
};	// end class state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_CHANNEL_INSTANCE_H__

