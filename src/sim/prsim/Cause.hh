/**
	\file "sim/prsim/Cause.hh"
	Structure of basic node event.  
	$Id: Cause.hh,v 1.5 2009/02/01 07:21:35 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_CAUSE_H__
#define	__HAC_SIM_PRSIM_CAUSE_H__

#include <iosfwd>
#include "sim/common.hh"
#include "sim/prsim/enums.hh"
#include "util/utypes.h"
#include "sim/prsim/devel_switches.hh"	// for PRSIM_TRACK_CAUSE_TIME
#if PRSIM_TRACK_CAUSE_TIME || PRSIM_TRACK_LAST_EDGE_TIME
#include "sim/time.hh"
#endif

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;
using std::istream;

#if PRSIM_TRACK_CAUSE_TIME || PRSIM_TRACK_LAST_EDGE_TIME
typedef	real_time				event_time_type;
#endif

//=============================================================================
/**
	(node, value) pair representing a past event.  
	With a time stamp, does this struct become large enough to
	warrant using pointers to Cause objects?
	Said Cause objects would need to expire over time,
	perhaps via reference count...
 */
struct EventCause {
	node_index_type				node;
	value_enum				val;
#if PRSIM_TRACK_CAUSE_TIME
	event_time_type				time;
#endif

	EventCause() : node(INVALID_NODE_INDEX), val(LOGIC_LOW)
#if PRSIM_TRACK_CAUSE_TIME
		, time(-1.0)
#endif
		{ }
	// initial value doesn't matter, could be LOGIC_OTHER

#if PRSIM_TRACK_CAUSE_TIME
	EventCause(const node_index_type n, const value_enum v, 
		const event_time_type& t) :
		node(n), val(v), time(t) { }
#else
	EventCause(const node_index_type n, const value_enum v) :
		node(n), val(v) { }
#endif

	/**
		For set-ordering relation.  
	 */
	bool
	operator < (const EventCause& e) const {
		// really, shouldn't be inserting events with same id,val...
		return (node < e.node) ||
			((node == e.node) && (
				(val < e.val)
#if 0 && PRSIM_TRACK_CAUSE_TIME
				|| ((val == e.val) && (time < e.time))
#endif
				));
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
#if PRSIM_TRACK_CAUSE_TIME
	event_time_type				cause_time[3];
#endif

	void
	initialize(void) {
		// caused_by_node({0}),
		// caused_by_value({0})
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
#if PRSIM_TRACK_CAUSE_TIME
		cause_time[0] = cause_time[1] = cause_time[2] = -1.0;
#endif
	}

	/**
		How the f-- do you initialize aggregate members?
	 */
	LastCause() {
		initialize();
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
		return event_cause_type(caused_by_node[i], caused_by_value[i]
#if PRSIM_TRACK_CAUSE_TIME
			, cause_time[i]
#endif
			);
	}

	void
	set_cause(const value_enum v, const event_cause_type& e) {
		const size_t i(v);
		caused_by_node[i] = e.node;
		caused_by_value[i] = e.val;
#if PRSIM_TRACK_CAUSE_TIME
		cause_time[i] = e.time;
#endif
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

