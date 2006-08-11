/**
	\file "sim/prsim/Cause.h"
	Structure of basic node event.  
	$Id: Cause.h,v 1.1.2.3 2006/08/11 22:52:00 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_CAUSE_H__
#define	__HAC_SIM_PRSIM_CAUSE_H__

#include <iosfwd>
#include "sim/common.h"
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
	uchar					val;

	EventCause() : node(INVALID_NODE_INDEX), val(0) { }

	EventCause(const node_index_type n, const uchar v) :
		node(n), val(v) { }

	/**
		For set-ordering relation.  
	 */
	bool
	operator < (const EventCause& e) const {
		return node < e.node || val < e.val;
	}

	void
	save_state(ostream&) const;

	void
	load_state(istream&);

};	// end struct EventCause

//-----------------------------------------------------------------------------
/**
	Structure for tracking event causality in greater detail.  
	Members kept in separate arrays for better alignment.  
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
	uchar					caused_by_value[4];

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
		caused_by_value[0] = 0;	// don't care
		caused_by_value[1] = 0;	// don't care
		caused_by_value[2] = 0;	// don't care
	}

	/**
		\param the value with which to lookup the last cause.
		\return the index of the last node to cause this change.
	 */
	node_index_type
	get_cause_node(const uchar v) const {
		return caused_by_node[size_t(v)];
	}

	event_cause_type
	get_cause(const uchar v) const {
		const size_t i(v);
		return event_cause_type(caused_by_node[i], caused_by_value[i]);
	}

	void
	set_cause(const uchar v, const event_cause_type& e) {
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

