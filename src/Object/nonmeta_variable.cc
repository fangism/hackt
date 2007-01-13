/**
	\file "Object/nonmeta_variable.cc"
	$Id: nonmeta_variable.cc,v 1.1.2.4 2007/01/13 21:06:54 fang Exp $
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
// class BoolVariable method definitions

/**
	Arbitrarily reset to some value.  
 */
void
BoolVariable::reset(void) {
	value = 0;
}

//=============================================================================
// class IntVariable method definitions

void
IntVariable::reset(void) {
	value = 0;
}

//=============================================================================
// class EnumVariable method definitions

void
EnumVariable::reset(void) {
	value = 0;
}

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
channel_data_base<Tag>::__reset(void) {
	member_fields = 0;	// valarray mass assign of value
}

//=============================================================================
// class ChannelData method definitions

void
ChannelData::resize(const fundamental_channel_footprint& f) {
	channel_data_base<bool_tag>::__resize(f);
	channel_data_base<int_tag>::__resize(f);
	channel_data_base<enum_tag>::__resize(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ChannelData::reset(void) {
	channel_data_base<bool_tag>::__reset();
	channel_data_base<int_tag>::__reset();
	channel_data_base<enum_tag>::__reset();
}

//=============================================================================
// class ChannelState method definitions

/**
	All channels should be initialized empty, ready to send.  
 */
ChannelState::ChannelState() :
		ChannelData(), nonmeta_variable_base(), full(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Channels are assumed to reset in the empty state.  
	Technically, resetting data should be unnecessary, 
	as they cannot be read until they are written.  
 */
void
ChannelState::reset(void) {
	ChannelData::reset();
	full = false;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

