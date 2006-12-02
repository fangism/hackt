/**
	\file "sim/chpsim/Channel.h"
	$Id: Channel.h,v 1.1.2.1 2006/12/02 22:10:09 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_CHANNEL_H__
#define	__HAC_SIM_CHPSIM_CHANNEL_H__

#include <valarray>

namespace HAC {
namespace SIM {
namespace CHPSIM {

/**
	This structure represents the state of a built-in (fundamental) typed
	channel, consisting of only abstract ints and bools.  
	The interpretation of the channel data is determined by the 
	complete type (footprint thereof) to which it is attached.  
 */
class ChannelState {
	// bitset or vector<bool>
	// flattened array of integers, currently limited to 32b for now
	/**
		State bit.  
		If this is true, channel is ready to be received, 
		else it is ready to be sent.  
	 */
	bool				full;
};	// end class ChannelState

}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_CHANNEL_H__

