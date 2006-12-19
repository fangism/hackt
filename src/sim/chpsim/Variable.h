/**
	\file "sim/chpsim/Variable.h"
	$Id: Variable.h,v 1.1.2.3 2006/12/19 23:44:13 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_VARIABLE_H__
#define	__HAC_SIM_CHPSIM_VARIABLE_H__

// obsolete, superceded by:
#include "Object/nonmeta_variable.h"
#if 0
#include "sim/common.h"
#include "Object/expr/types.h"
#include <iosfwd>
#include <valarray>
#include <set>
// #include "sim/chpsim/Dependence.h"
#endif

namespace HAC {
namespace SIM {
namespace CHPSIM {
using std::ostream;
using std::valarray;
using entity::event_subscribers_type;
using entity::BoolVariable;
using entity::IntVariable;
using entity::ChannelState;
#if 0
// typedef	valarray<Dependence>			fanout_list_type;
typedef	std::set<event_index_type>		event_subscribers_type;
// typedef	std::valarray<event_index_type>		event_subscribers_type;

//=============================================================================
class VariableBase {
#if 0
	/**
		Event dependency set.  
	 */
	fanout_list_type			fanouts;
#endif
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
	VariableBase();
	~VariableBase();

	// not needed for structural dumps, as this is a dynamic set
	ostream&
	dump_struct(ostream&) const;
};	// end class VariableBase

//=============================================================================
/**
	Boolean (nonmeta) variable class.  
	These are allocated during creation of the simulator state.  
	NOTE: eventually, there bools (abstract data) will be differentiated
		from the physical (node) bools.  
 */
class BoolVariable : public VariableBase {
	typedef	VariableBase			parent_type;
public:
	/**
		The value type.
	 */
	typedef	char				value_type;

	// for now...
	using parent_type::dump_struct;
};	// end class BoolVariable

//=============================================================================
/**
	Integer (nonmeta) variable class.
	Currently limited to 32b until we support mpz_t.  
 */
class IntVariable : public VariableBase {
	typedef	VariableBase			parent_type;
public:
	/// the value type
	typedef	unsigned int			value_type;

	// for now...
	using parent_type::dump_struct;
};	// end clas IntVariable
#endif
//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_VARIABLE_H__

