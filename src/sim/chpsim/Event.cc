/**
	\file "sim/chpsim/Event.cc"
	$Id: Event.cc,v 1.1.2.8 2006/12/25 03:28:00 fang Exp $
 */

#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include "sim/chpsim/Event.h"
#include "sim/ISE.h"
#include "Object/expr/bool_expr.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/lang/CHP_base.h"
#include "util/STL/valarray_iterator.h"
#include "sim/chpsim/nonmeta_context.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
#include "util/using_ostream.h"
using std::ostream_iterator;
using std::begin;
using std::end;
using std::copy;
using std::back_inserter;
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
/**
	Need to manually define valarray assignment with 
	container-like semantics.  
 */
EventNode&
EventNode::operator = (const this_type& e) {
	guard_expr = e.guard_expr;
	action_ptr = e.action_ptr;
	successor_events.resize(e.successor_events.size());
	copy(begin(e.successor_events), end(e.successor_events), 
		begin(successor_events));
	event_type = e.event_type;
	flags = e.flags;
	process_index = e.process_index;
	predecessors = e.predecessors;
	countdown = e.countdown;
	deps = e.deps;		// re-defined!
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventNode::set_guard_expr(const count_ptr<const bool_expr>& g) {
	guard_expr = g;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-evaluates the guard expression of this event to determine
	whether or not event should be enqueued.  

	\param sm the structural state manager (hierarchy)
	\param p the run-time state of variables and channels.
	\param enqueue return list of event(s) to enqueue for execution.
	Don't forget to countdown predecessors.
	Is there a problem with guarded selection statements?
	TODO: Need to examine how they are constructed...
 */
void
EventNode::recheck(const nonmeta_context& c) {
//	const entity::nonmeta_context c(sm, f, p, *this, enqueue);
	if (guard_expr) {
		// if (guard_expr->nonmeta_resolve(c))
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this really should just be inlined
	\param enqueue list of successor events to enqueue immediately
	\return the type-enumerated index referring to the variable
		or channel that was modified.
	What if channel receive? (two modifications?)
 */
void
EventNode::execute(const nonmeta_context& c, 
		vector<instance_reference>& updates) {
#if 0
	// actually, guard expression should be checked before an event
	// is enqueued.
	// a check here would verify that the guard is *stably* true.  
	if (guard_expr) {
		...
	}
#endif
	if ((event_type != EVENT_NULL) && action_ptr) {
		// const entity::nonmeta_context c(sm, f, p, *this, enqueue);
		// at the same time, enqueue successors, depending on event_type
#if ENABLE_CHP_EXECUTE
		action_ptr->execute(c, updates);
		// action_ptr->evaluate_successors(enqueue);
#endif
	} else {	// event is NULL or action_ptr is NULL
		// else do nothing
		// enqueue all successors
		copy(begin(successor_events), end(successor_events), 
			back_inserter(c.queue));
	}
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
		action_ptr->dump_event(o, expr_dump_context::default_value);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't print guards here, put guards on edges.  
 */
ostream&
EventNode::dump_dot_node(ostream& o) const {
	o << "[shape=";
	switch (event_type) {
	case EVENT_NULL:
		o << ((predecessors > 1) ? "diamond" : "ellipse"); 
		break;
	case EVENT_ASSIGN: o << "box"; break;
	case EVENT_SEND: o << "house"; break;
	case EVENT_RECEIVE: o << "invhouse"; break;
	case EVENT_CONCURRENT_FORK: o << "hexagon"; break;
	case EVENT_SELECTION_BEGIN: o << "trapezium"; break;
	default:
		ISE(cerr, cerr << "Invalid event type enum: "
			<< event_type << endl;)
	}
	if (action_ptr) {
		o << ", ";
		action_ptr->dump_event(o << "label=\"", 
			expr_dump_context::default_value) 
			// TODO: pass context scope to suppress scope qualifier
			<< "\\npid=" << process_index << "\"";
	}
	// no edges
	// no deps
	return o << "];";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just prints edge label if applicable.
 */
ostream&
EventNode::dump_dot_edge(ostream& o) const {
	if (guard_expr) {
		guard_expr->dump(o << "\t[label=\"",
			expr_dump_context::default_value) << "\"]";
	}
	return o;
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

