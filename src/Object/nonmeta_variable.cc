/**
	\file "Object/nonmeta_variable.cc"
	$Id: nonmeta_variable.cc,v 1.3.10.1 2007/04/21 04:33:57 fang Exp $
 */

#include <iostream>
#include <iterator>
#include "Object/nonmeta_variable.h"
#include "Object/nonmeta_channel_manipulator.h"
#include "Object/def/fundamental_channel_footprint.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#include "Object/type/canonical_generic_datatype.h"
#include "util/IO_utils.tcc"
#include "util/STL/valarray_iterator.h"

namespace HAC {
namespace entity {
using std::ostream_iterator;
using std::begin;
using std::end;
using util::write_value;
using util::write_array;
using util::read_value;
using util::read_sequence_resize;

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
	unsubscribe_all();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
BoolVariable::write(ostream& o) const {
	write_value(o, value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
BoolVariable::read(istream& i) {
	read_value(i, value);
	unsubscribe_all();
}

//=============================================================================
// class IntVariable method definitions

void
IntVariable::reset(void) {
	value = 0;
	unsubscribe_all();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
IntVariable::write(ostream& o) const {
	write_value(o, value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
IntVariable::read(istream& i) {
	read_value(i, value);
	unsubscribe_all();
}

//=============================================================================
// class EnumVariable method definitions

void
EnumVariable::reset(void) {
	value = 0;
	unsubscribe_all();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EnumVariable::write(ostream& o) const {
	write_value(o, value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EnumVariable::read(istream& i) {
	read_value(i, value);
	unsubscribe_all();
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: Pass in a reference to the corresponding channel type
	so the size can be deduced instead of being stored
	redundantly (and repeatedly!).  (Attach trace to object file.)
	For now, just screw it, waste some space...
 */
template <class Tag>
void
channel_data_base<Tag>::__write(ostream& o) const {
	write_array(o, this->member_fields);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
channel_data_base<Tag>::__read(istream& i) {
	read_sequence_resize(i, this->member_fields);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sheer laziness.  
 */
template <class Tag>
ostream&
channel_data_base<Tag>::__raw_dump(ostream& o) const {
	copy(begin(this->member_fields), end(this->member_fields),
		ostream_iterator<size_t>(o, ", "));
	return o;
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
void
ChannelData::write(ostream& o) const {
	channel_data_base<bool_tag>::__write(o);
	channel_data_base<int_tag>::__write(o);
	channel_data_base<enum_tag>::__write(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ChannelData::read(istream& i) {
	channel_data_base<bool_tag>::__read(i);
	channel_data_base<int_tag>::__read(i);
	channel_data_base<enum_tag>::__read(i);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
ChannelData::raw_dump(ostream& o) const {
	channel_data_base<bool_tag>::__raw_dump(o);
	channel_data_base<int_tag>::__raw_dump(o);
	channel_data_base<enum_tag>::__raw_dump(o);
	return o;
}

//=============================================================================
// class channel_state_base method definitions

/**
	All channels should be initialized empty, ready to send.  
 */
channel_state_base::channel_state_base() :
		ChannelData(), 
#if CHPSIM_COUPLED_CHANNELS
		aux_enqueue(0), 
		status(CHANNEL_INACTIVE)
#else
		full(false)
#endif
{
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
#if CHPSIM_COUPLED_CHANNELS
	aux_enqueue = 0;
	status = CHANNEL_INACTIVE;
#else
	full = false;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes out binary to a trace stream.  
	NOTE: only need to write data when channel become full, 
		occupied with data (just sent).  When it is empty, 
		data doesn't change, and doesn't matter.  
 */
void
channel_state_base::write(ostream& o) const {
#if CHPSIM_COUPLED_CHANNELS
	write_value(o, status);
	if (status == CHANNEL_SENDER_BLOCKED ||
			status == CHANNEL_RECEIVER_BLOCKED) {
		write_value(o, aux_enqueue);
	}
	if (status == CHANNEL_SENDER_BLOCKED) {
		ChannelData::write(o);
	}
#else
	write_value(o, full);	// takes 1 byte
	if (full) {
		ChannelData::write(o);
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_state_base::read(istream& i) {
#if CHPSIM_COUPLED_CHANNELS
	read_value(i, status);
	if (status == CHANNEL_SENDER_BLOCKED ||
			status == CHANNEL_RECEIVER_BLOCKED) {
		read_value(i, aux_enqueue);
	}
	if (status == CHANNEL_SENDER_BLOCKED) {
		ChannelData::read(i);
	}
#else
	read_value(i, full);	// takes 1 byte
	if (full) {
		ChannelData::read(i);
	}
#endif
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
#if CHPSIM_COUPLED_CHANNELS
	switch (status) {
	case CHANNEL_INACTIVE: o << " [empty]"; break;
	case CHANNEL_RECEIVER_BLOCKED: o << " [wait]"; break;
	case CHANNEL_SENDER_BLOCKED: o << " [full]"; break;
	default: break;
	}
	return o;
#else
	return o << (full ? " [full]" : " [empty]");
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_state_base::raw_dump(ostream& o) const {
#if CHPSIM_COUPLED_CHANNELS
	if (status == CHANNEL_SENDER_BLOCKED) {
		ChannelData::raw_dump(o);
	} else if (status == INACTIVE) {
		o << "[ack]";
	} else {
		o << "[wait]";
	}
#else
	if (full) {
		ChannelData::raw_dump(o);
	} else {
		o << "[ack]";
	}
#endif
	return o;
}

//=============================================================================
// class ChannelState method definitions

ChannelState::ChannelState() :
		nonmeta_variable_base(), channel_state_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ChannelState::reset(void) {
	channel_state_base::reset();
	unsubscribe_all();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ChannelState::read(istream& i) {
	channel_state_base::read(i);
	unsubscribe_all();
}

//=============================================================================
// class state_data_extractor method definitions

/**
	Simple POD write suffices for these types.  
 */
template <class Tag>
void
state_data_extractor<Tag>::write(ostream& o, const value_type& v) {
	write_value(o, v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Simple POD write suffices for these types.  
 */
template <class Tag>
void
state_data_extractor<Tag>::read(istream& i, value_type& v) {
	read_value(i, v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Out of laziness, we convert all of these types to unsigned integers.
 */
template <class Tag>
ostream&
state_data_extractor<Tag>::dump(ostream& o, const value_type& v) {
	return o << size_t(v);
}

//-----------------------------------------------------------------------------
/**
	TODO: take a canonical channel footprint type parameter
	to avoid having to save away the size of each meta-field.
	For writing, the channel type is only used to verify (overkill).  
 */
void
state_data_extractor<channel_tag>::write(ostream& o, const value_type& v) {
	v.write(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: take a canonical channel footprint type parameter
	to avoid having to save away the size of each meta-field.
	For reading, the channel type is used to determine how to read.  
 */
void
state_data_extractor<channel_tag>::read(istream& i, value_type& v) {
	v.read(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This raw dump doesn't reflect the structure of the channel, 
	just unformatted contents that are better interpreted base on
	the canonical_fundamental_chan_type_base. 
 */
ostream&
state_data_extractor<channel_tag>::dump(ostream& o, const value_type& v) {
	return v.raw_dump(o);
}

//=============================================================================
// explicit template instantiations

template class state_data_extractor<bool_tag>;
template class state_data_extractor<int_tag>;
template class state_data_extractor<enum_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

