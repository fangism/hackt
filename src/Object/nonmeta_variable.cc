/**
	\file "Object/nonmeta_variable.cc"
	$Id: nonmeta_variable.cc,v 1.2.2.1 2007/01/28 22:42:13 fang Exp $
 */

#include <iostream>
#include <iterator>
#include "Object/nonmeta_variable.h"
#include "Object/nonmeta_channel_manipulator.h"
#include "Object/def/fundamental_channel_footprint.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#include "Object/type/canonical_generic_datatype.h"

namespace HAC {
namespace entity {
using std::ostream_iterator;

//=============================================================================
// class nonmeta_variable_base method definitions

nonmeta_variable_base::nonmeta_variable_base() : event_subscribers() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_variable_base::~nonmeta_variable_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
nonmeta_variable_base::dump_subscribers(ostream& o) const {
	if (!event_subscribers.empty()) {
		copy(event_subscribers.begin(), event_subscribers.end(), 
			ostream_iterator<size_t>(o, " "));
	} else {
		o << "(none)";
	}
	return o;
}

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Non-default copy-ctor because of valarray assignment.
 */
template <class Tag>
channel_data_base<Tag>::channel_data_base(const this_type& c) :
		member_fields(c.member_fields.size()) {
	this->member_fields = c.member_fields;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Non-default assignment because of valarray assignment.
 */
template <class Tag>
channel_data_base<Tag>&
channel_data_base<Tag>::operator = (const this_type& t) {
	// if size is different...
	if (this->member_fields.size() != t.member_fields.size()) {
		this->member_fields.resize(t.member_fields.size());
	}
	this->member_fields = t.member_fields;
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

ChannelData::ChannelData(const this_type& t) :
		channel_data_base<bool_tag>(t), 
		channel_data_base<int_tag>(t), 
		channel_data_base<enum_tag>(t) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ChannelData&
ChannelData::operator = (const this_type& t) {
	channel_data_base<bool_tag>::operator=(t);
	channel_data_base<int_tag>::operator=(t);
	channel_data_base<enum_tag>::operator=(t);
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
ChannelData::dump(ostream& o,
		const canonical_fundamental_chan_type_base& t) const {
	const canonical_fundamental_chan_type_base::datatype_list_type&
		l(t.get_datatype_list());
	for_each(l.begin(), l.end(), channel_data_dumper(*this, o));
	return o;
}

//=============================================================================
// class channel_state_base method definitions

/**
	All channels should be initialized empty, ready to send.  
 */
channel_state_base::channel_state_base() :
		ChannelData(), full(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Channels are assumed to reset in the empty state.  
	Technically, resetting data should be unnecessary, 
	as they cannot be read until they are written.  
 */
void
channel_state_base::reset(void) {
	ChannelData::reset();
	full = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Formatted print of channel contents.  
	If empty, then shows the last value that was written to it.  
 */
ostream&
channel_state_base::dump(ostream& o, 
		const canonical_fundamental_chan_type_base& t) const {
	ChannelData::dump(o << '(', t) << ')';
	return o << (full ? " [full]" : " [empty]");
}

//=============================================================================
// class ChannelState method definitions

ChannelState::ChannelState() :
		nonmeta_variable_base(), channel_state_base() {
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

