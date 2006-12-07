/**
	\file "sim/chpsim/Event.cc"
	$Id: Event.cc,v 1.1.2.1 2006/12/07 07:48:37 fang Exp $
 */

#include "sim/chpsim/Event.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
//=============================================================================
// class EventNode method definitions

EventNode::EventNode() :
		guard_expr(NULL), 
		action_ptr(NULL),
		successor_events(), 
		event_type(EVENT_NULL),
		flags(0),
		process_index(0),
		predecessors(0),
		countdown(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param a the CHP::action pointer (may be NULL)
		using shallow reference, not reference counting.
 */
EventNode::EventNode(const action* a, 
		const unsigned short t, const size_t pid) :
		guard_expr(NULL), 
		action_ptr(a),
		successor_events(), 
		event_type(t),
		flags(0),
		process_index(pid),
		predecessors(0),
		countdown(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EventNode::~EventNode() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventNode::set_guard_expr(const count_ptr<const bool_expr>& g) {
	guard_expr = g;
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

