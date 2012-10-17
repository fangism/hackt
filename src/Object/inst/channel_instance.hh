/**
	\file "Object/inst/channel_instance.hh"
	Definition of channel instance state.  
	$Id: channel_instance.hh,v 1.4 2006/01/22 18:19:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CHANNEL_INSTANCE_H__
#define	__HAC_OBJECT_INST_CHANNEL_INSTANCE_H__

#include "Object/traits/chan_traits.hh"
#include "Object/inst/state_instance.hh"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Placeholder for channel state information.  
	An actual instantiated instance of a channel.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
struct class_traits<channel_tag>::state_instance_base {
};	// end struct state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_CHANNEL_INSTANCE_H__

