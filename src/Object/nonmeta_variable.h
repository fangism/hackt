/**
	\file "Object/nonmeta_variable.h"
	$Id: nonmeta_variable.h,v 1.1.2.2.2.2 2006/12/25 02:58:26 fang Exp $
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
using std::ostream;
using std::valarray;
typedef	std::set<size_t>		event_subscribers_type;
	// size_t -> event_index_type

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
class ChannelState : public ChannelData, public nonmeta_variable_base {
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
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_CHANNEL_H__

