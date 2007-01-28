/**
	\file "Object/nonmeta_variable.h"
	$Id: nonmeta_variable.h,v 1.2.2.1 2007/01/28 22:42:13 fang Exp $
	TODO: consider including history tracing capabilities here?
 */

#ifndef	__HAC_OBJECT_NONMETA_VARIABLE_H__
#define	__HAC_OBJECT_NONMETA_VARIABLE_H__

#include "util/size_t.h"
#include "Object/expr/types.h"
#include <iosfwd>
#include <valarray>
#include <set>

namespace HAC {
namespace entity {
struct bool_tag;
struct int_tag;
struct enum_tag;
struct channel_tag;
class BoolVariable;
class IntVariable;
class EnumVariable;
class ChannelState;
class fundamental_channel_footprint;
class canonical_fundamental_chan_type_base;
	// defined in "Object/def/fundamental_channel_footprint.h"
using std::ostream;
using std::valarray;

/**
	Consider overriding with a faster allocator for efficient
	insertion and removal.
 */
typedef	std::set<size_t>		event_subscribers_type;
	// size_t -> event_index_type

//=============================================================================
/**
	TODO: define this in corresponding class_traits?
 */
template <class Tag>
struct variable_type { };

template <>
struct variable_type<bool_tag> {
	typedef	BoolVariable		type;
};

template <>
struct variable_type<int_tag> {
	typedef	IntVariable		type;
};

template <>
struct variable_type<enum_tag> {
	typedef	EnumVariable		type;
};

template <>
struct variable_type<channel_tag> {
	typedef	ChannelState		type;
};

//=============================================================================
/**
	Every variable maintains a set of run-time subscribers.  
 */
class nonmeta_variable_base {
	/**
		This set changes over the course of execution.
		subscribers: events to wake and recheck when this variable
		has changed value.
		Blocked events will subscribe themselves to all possibly
		dependent variables.  
		For each event that becomes unblocked as a result of 
		some re-evaluation, unsubscribe the event from this set. 
	 */
	event_subscribers_type			event_subscribers;
public:
	nonmeta_variable_base();
	~nonmeta_variable_base();

	const event_subscribers_type&
	get_subscribers(void) const { return event_subscribers; }

	void
	subscribe(const size_t e) { event_subscribers.insert(e); }

	void
	unsubscribe(const size_t e) { event_subscribers.erase(e); }

	// not needed for structural dumps, as this is a dynamic set
	ostream&
	dump_subscribers(ostream&) const;

	bool
	has_subscribers(void) const { return !event_subscribers.empty(); }

};	// end class nonmeta_variable_base

//=============================================================================
/**
	Boolean (nonmeta) variable class.  
	These are allocated during creation of the simulator state.  
	NOTE: eventually, there bools (abstract data) will be differentiated
		from the physical (node) bools.  
 */
class BoolVariable : public nonmeta_variable_base {
	typedef	nonmeta_variable_base			parent_type;
public:
	/**
		The value type.
	 */
	typedef	char				value_type;
	value_type				value;

	// for now...
	using parent_type::dump_subscribers;

	void
	reset(void);

};	// end class BoolVariable

//=============================================================================
/**
	Integer (nonmeta) variable class.
	Currently limited to 32b until we support mpz_t.  
 */
class IntVariable : public nonmeta_variable_base {
	typedef	nonmeta_variable_base			parent_type;
public:
	/// the value type
	typedef	unsigned int			value_type;
	value_type				value;

	// for now...
	using parent_type::dump_subscribers;

	void
	reset(void);

};	// end clas IntVariable

//=============================================================================
/**
	Provisional enum variable class, not really used yet.
 */
class EnumVariable : public nonmeta_variable_base {
	typedef	nonmeta_variable_base			parent_type;
public:
	/// the value type
	typedef	unsigned int			value_type;
	value_type				value;

	// for now...
	using parent_type::dump_subscribers;

	void
	reset(void);

};	// end class EnumVariable

//=============================================================================
template <class Tag>
class channel_data_base {
	typedef	channel_data_base<Tag>		this_type;
public:
	typedef	typename variable_type<Tag>::type::value_type
						member_variable_type;
	typedef	valarray<member_variable_type>	member_variable_array_type;

	member_variable_array_type		member_fields;

protected:
	channel_data_base() : member_fields() { }

	// custom copy-ctor because of valarray
	channel_data_base(const this_type&);

	// custom assignment because of valarray
	this_type&
	operator = (const this_type&);

	// possible direct assign, when we assert that sizes are the same?

	void
	__resize(const fundamental_channel_footprint&);

	void
	__reset(void);

};	// end class channel_data_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The raw data contained in the channel, whose interpretation
	will be determined by the corresponding global channel entry.
	If memory cost is not an isue, copy the fundamental channel 
	footprint pointer here.  
	To play with: different ways of data packing and unpacking
		trading off between memory and performance.  
		Layout will be defined by fundamental channel footprint.  
 */
class ChannelData : 
	public channel_data_base<bool_tag>,
	public channel_data_base<int_tag>,
	public channel_data_base<enum_tag> {
	typedef	ChannelData			this_type;
public:
	// default constructor and destructor
	ChannelData() : channel_data_base<bool_tag>(),
		channel_data_base<int_tag>(),
		channel_data_base<enum_tag>() { }

	// custom copy-ctor because of valarray
	ChannelData(const this_type&);

	// custom assignment because of valarray
	this_type&
	operator = (const this_type&);

	// possible direct assign, when we assert that sizes are the same?
	// may be needed for X!(Y?) -style events
	// will be needed to sustain channel performance

	void
	resize(const fundamental_channel_footprint&);

	void
	reset(void);

	ostream&
	dump(ostream&, const canonical_fundamental_chan_type_base&) const;

};	// end class ChannelData

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We factor out this class for the sake of having a pure state
	or value structure that can be checkpointed and traced.  
 */
class channel_state_base : public ChannelData {
protected:
	/**
		State bit.  
		If this is true, channel is ready to be received, 
		else it is ready to be sent.  
	 */
	bool				full;
public:
	channel_state_base();

	bool
	can_receive(void) const { return full; }

	bool
	can_send(void) const { return !full; }

	void
	send(void) { full = true; }

	void
	receive(void) { full = false; }

	void
	reset(void);

	ostream&
	dump(ostream&, const canonical_fundamental_chan_type_base&) const;

};	// end class channel_state_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This structure represents the state of a built-in (fundamental) typed
	channel, consisting of only abstract ints and bools.  
	The interpretation of the channel data is determined by the 
	complete type (footprint thereof) to which it is attached.  
	Q: will we ever support counterflow channel pipelines?  Oooo.
 */
class ChannelState : public nonmeta_variable_base, public channel_state_base {
	// bitset or vector<bool>
	// flattened array of integers, currently limited to 32b for now
public:
	ChannelState();

};	// end class ChannelState

//=============================================================================
/**
	General raw-data extractor.  
 */
template <class Tag>
struct state_data_extractor {
	typedef typename variable_type<Tag>::type	var_type;
	typedef	typename var_type::value_type		value_type;

	/**
		To extract data variables, access the value member.  
	 */
	const value_type&
	operator () (const var_type& v) const {
		return v.value;
	}
};	// end struct state_data_extractor

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct state_data_extractor<channel_tag> {
	typedef variable_type<channel_tag>::type	var_type;
	typedef	channel_state_base			value_type;

	/**
		To extract channel state, just static_cast.
	 */
	const value_type&
	operator () (const var_type& v) const {
		return v;
	}
};	// end struct state_data_extractor

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_NONMETA_VARIABLE_H__

