/**
	\file "Object/nonmeta_variable.cc"
	$Id: nonmeta_variable.cc,v 1.1.2.3 2007/01/12 03:11:33 fang Exp $
 */

#include "Object/nonmeta_variable.h"
#include "Object/def/fundamental_channel_footprint.h"

namespace HAC {
namespace entity {
//=============================================================================
// class nonmeta_variable_base method definitions

nonmeta_variable_base::nonmeta_variable_base() : event_subscribers() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_variable_base::~nonmeta_variable_base() { }

//=============================================================================
// class channel_data_base method definitions

/**
	Resizes fields array according to channel type's footprint summary.
 */
template <class Tag>
void
channel_data_base<Tag>::__resize(const fundamental_channel_footprint& f) {
	member_fields.resize(f.template size<Tag>());
}

//=============================================================================
// class ChannelData method definitions

void
ChannelData::resize(const fundamental_channel_footprint& f) {
	channel_data_base<bool_tag>::__resize(f);
	channel_data_base<int_tag>::__resize(f);
	channel_data_base<enum_tag>::__resize(f);
}

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

