/**
	\file "Object/nonmeta_variable.cc"
	$Id: nonmeta_variable.cc,v 1.1.2.2 2006/12/27 06:01:35 fang Exp $
 */

#include "Object/nonmeta_variable.h"

namespace HAC {
namespace entity {
//=============================================================================
// class nonmeta_variable_base method definitions

nonmeta_variable_base::nonmeta_variable_base() : event_subscribers() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_variable_base::~nonmeta_variable_base() { }

//=============================================================================
// class ChannelState method definitions

/**
	All channels should be initialized empty, ready to send.  
 */
ChannelState::ChannelState() :
		ChannelData(), nonmeta_variable_base(), full(false) {
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

