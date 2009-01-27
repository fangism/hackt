/**
	\file "sim/prsim/Cause.h"
	Structure of basic node event.  
	$Id: Cause.h,v 1.4.6.1 2009/01/27 00:18:51 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_CAUSE_H__
#define	__HAC_SIM_PRSIM_CAUSE_H__

#include <iosfwd>
#include "sim/common.h"
#include "sim/prsim/enums.h"
#include "util/utypes.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;
using std::istream;

//=============================================================================
/**
	(node, value) pair representing a past event.  
 */
struct EventCause {
	node_index_type				node;
	value_enum				val;

	EventCause() : node(INVALID_NODE_INDEX), val(LOGIC_LOW) { }
	// initial value doesn't matter, could be LOGIC_OTHER

	EventCause(const node_index_type n, const value_enum v) :
		node(n), val(v) { }

	/**
		For set-ordering relation.  
	 */
	bool
	operator < (const EventCause& e) const {
		return node < e.node || val < e.val;
	}

	ostream&
	dump_raw(ostream&) const;

	void
	save_state(ostream&) const;

	void
	load_state(istream&);

};	// end struct EventCause

//-----------------------------------------------------------------------------
/**
	Structure for tracking event causality in greater detail.  
	Members kept in separate arrays for better alignment.  
	Note: don't want to trace critical trace index here, costs memory.
 */
struct LastCause {
	typedef	EventCause			event_cause_type;
	/**
		The causing node.  
		Indexed by node's current value.  
	 */
	node_index_type				caused_by_node[3];
	/**
		The value of the causing node.  
		3 of 4 slots used for better padding.  
		Indexed by node's current value. 
	 */
	value_enum				caused_by_value[4];

	/**
		How the f-- do you initialize aggregate members?
	 */
	LastCause()
		// caused_by_node({0}),
		// caused_by_value({0})
	{
		// caused_by_node = {0};
		// caused_by_value = {0};
		// *this = {{0,0,0}, {0,0,0}};
		caused_by_node[0] = INVALID_NODE_INDEX;
		caused_by_node[1] = INVALID_NODE_INDEX;
		caused_by_node[2] = INVALID_NODE_INDEX;
		caused_by_value[0] = LOGIC_LOW;	// don't care
		caused_by_value[1] = LOGIC_LOW;	// don't care
		caused_by_value[2] = LOGIC_LOW;	// don't care
		caused_by_value[3] = LOGIC_LOW;	// really don't care
		// but needs to be initialized else binary will be
		// non-deterministic
	}

	/**
		\param the value with which to lookup the last cause.
		\return the index of the last node to cause this change.
	 */
	node_index_type
	get_cause_node(const value_enum v) const {
		return caused_by_node[size_t(v)];
	}

	event_cause_type
	get_cause(const value_enum v) const {
		const size_t i(v);
		return event_cause_type(caused_by_node[i], caused_by_value[i]);
	}

	void
	set_cause(const value_enum v, const event_cause_type& e) {
		const size_t i(v);
		caused_by_node[i] = e.node;
		caused_by_value[i] = e.val;
	}

	void
	save_state(ostream&) const;

	void
	load_state(istream&);

	ostream&
	dump_checkpoint_state(ostream&) const;

};	// end struct LastCause

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_CAUSE_H__

