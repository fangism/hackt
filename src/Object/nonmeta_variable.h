/**
	\file "Object/nonmeta_variable.h"
	$Id: nonmeta_variable.h,v 1.1.2.6 2007/01/12 03:11:34 fang Exp $
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

	// not needed for structural dumps, as this is a dynamic set
	ostream&
	dump_struct(ostream&) const;
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
	using parent_type::dump_struct;
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
	using parent_type::dump_struct;
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
	using parent_type::dump_struct;
};	// end class EnumVariable

//=============================================================================
template <class Tag>
struct channel_data_base {
	typedef	typename variable_type<Tag>::type::value_type
						member_variable_type;
	typedef	valarray<member_variable_type>	member_variable_array_type;

	member_variable_array_type		member_fields;

protected:
	void
	__resize(const fundamental_channel_footprint&);

};	// end struct channel_data_base

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
public:
	// default constructor and destructor

	void
	resize(const fundamental_channel_footprint&);

};	// end class ChannelData

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This structure represents the state of a built-in (fundamental) typed
	channel, consisting of only abstract ints and bools.  
	The interpretation of the channel data is determined by the 
	complete type (footprint thereof) to which it is attached.  
	Q: will we ever support counterflow channel pipelines?  Oooo.
 */
class ChannelState : public ChannelData, public nonmeta_variable_base {
	// bitset or vector<bool>
	// flattened array of integers, currently limited to 32b for now
	/**
		State bit.  
		If this is true, channel is ready to be received, 
		else it is ready to be sent.  
	 */
	bool				full;
public:
	ChannelState();

	bool
	can_receive(void) const { return full; }

	bool
	can_send(void) const { return !full; }

	void
	send(void) { full = true; }

	void
	receive(void) { full = false; }

};	// end class ChannelState

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_NONMETA_VARIABLE_H__

