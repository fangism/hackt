/**
	\file "sim/chpsim/Channel.h"
	$Id: Channel.h,v 1.1.2.3 2006/12/19 23:44:10 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_CHANNEL_H__
#define	__HAC_SIM_CHPSIM_CHANNEL_H__

// obsolete
#include "Object/nonmeta_variable.h"

#if 0
#include <valarray>
#include "sim/chpsim/Variable.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
//=============================================================================
/**
	The raw data contained in the channel, whose interpretation
	will be determined by the corresponding global channel entry.
	If memory cost is not an isue, copy the fundamental channel 
	footprint pointer here.  
	To play with: different ways of data packing and unpacking
		trading off between memory and performance.  
		Layout will be defined by fundamental channel footprint.  
 */
class ChannelData {
public:
	typedef	valarray<BoolVariable::value_type>	bool_fields_type;
	typedef	valarray<IntVariable::value_type>	int_fields_type;
	bool_fields_type				bool_fields;
	int_fields_type					int_fields;

	// default constructor and destructor

};	// end class ChannelData

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This structure represents the state of a built-in (fundamental) typed
	channel, consisting of only abstract ints and bools.  
	The interpretation of the channel data is determined by the 
	complete type (footprint thereof) to which it is attached.  
	Q: will we ever support counterflow channel pipelines?  Oooo.
 */
class ChannelState : public ChannelData {
	// bitset or vector<bool>
	// flattened array of integers, currently limited to 32b for now
	/**
		State bit.  
		If this is true, channel is ready to be received, 
		else it is ready to be sent.  
	 */
	bool				full;
};	// end class ChannelState

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC
#endif

#endif	// __HAC_SIM_CHPSIM_CHANNEL_H__

