/**
	\file "sim/chpsim/Event.cc"
	$Id: Event.cc,v 1.1.2.2 2006/12/11 00:40:18 fang Exp $
 */

#include <iostream>
#include <iterator>
#include "sim/chpsim/Event.h"
#include "sim/ISE.h"
#include "Object/expr/bool_expr.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/lang/CHP_base.h"
#include "util/STL/valarray_iterator.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
#include "util/using_ostream.h"
using std::ostream_iterator;
using std::begin;
using std::end;
using entity::expr_dump_context;

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
EventNode::dump_struct(ostream& o) const {
	switch (event_type) {
	case EVENT_NULL: o << "null"; break;
	case EVENT_ASSIGN: o << "assign"; break;
	case EVENT_SEND: o << "send"; break;
	case EVENT_RECEIVE: o << "receive"; break;
	case EVENT_CONCURRENT_FORK: o << "fork"; break;
	case EVENT_SELECTION_BEGIN: o << "select"; break;
	default:
		ISE(cerr, cerr << "Invalid event type enum: "
			<< event_type << endl;)
	}
	o << ": ";
	if (guard_expr) {
		guard_expr->dump(o, expr_dump_context::default_value) << " -> ";
	}
	if (action_ptr) {
		// not the normal dump, but one used for event graphs
		action_ptr->dump_event(o);
	} else {
		o << "null";
	}
	// flags?
	o << ", pid: " << process_index;
	o << ", #pred: " << predecessors;
	// countdown? only in state dump
	// o << endl;
	o << ", succ: ";
	ostream_iterator<event_index_type> osi(o, " ");
	copy(begin(successor_events), end(successor_events), osi);
	o << endl;
	deps.dump(o);	// includes endl already
	return o;
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

