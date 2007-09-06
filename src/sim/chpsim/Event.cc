/**
	\file "sim/chpsim/Event.cc"
	$Id: Event.cc,v 1.10.8.5 2007/09/06 06:17:50 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include "sim/chpsim/Event.h"
#include "sim/ISE.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/lang/CHP_base.h"
#include "sim/chpsim/nonmeta_context.h"
#include "sim/chpsim/graph_options.h"
#include "sim/chpsim/EventExecutor.h"
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
#include "Object/lang/CHP_event.h"
#else
#include "util/STL/valarray_iterator.h"
#endif
#include "util/stacktrace.h"
#include "util/iterator_more.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
#include "util/using_ostream.h"
using std::ostream_iterator;
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
using std::begin;
using std::end;
#endif
using std::copy;
using std::back_inserter;
using std::for_each;
using util::set_inserter;
using entity::expr_dump_context;
using entity::process_tag;
using entity::state_manager;
using entity::footprint;

//=============================================================================
// class EventNode method definitions

const char 
EventNode::node_prefix[] = "EVENT_";

const EventNode::time_type
EventNode::default_delay = 10;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EventNode::EventNode() :
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		__local_event(NULL),
#else
		action_ptr(NULL),
		successor_events(), 
		event_type(EVENT_NULL),
		predecessors(0),
		process_index(0),
#endif	// CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		countdown(0), 
		flags(0),
		delay(default_delay), 
		block_deps()
#if CHPSIM_READ_WRITE_DEPENDENCIES
		, anti_deps()
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
/**
	\param a the CHP::action pointer (may be NULL)
		using shallow reference, not reference counting.
 */
EventNode::EventNode(
		const action* a, 
		const unsigned short t, const size_t pid
		) :
		action_ptr(a),
		successor_events(), 
		event_type(t),
		predecessors(0),
		process_index(pid),
		countdown(0), 
		flags(0),
		delay(default_delay), 
		block_deps()
#if CHPSIM_READ_WRITE_DEPENDENCIES
		, anti_deps()
#endif
		{
}
#endif	// CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param a the CHP::action pointer (may be NULL)
		using shallow reference, not reference counting.
 */
EventNode::EventNode(
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		const local_event_type* e, 
#else
		const action* a, 
		const unsigned short t, const size_t pid, 
#endif
		const time_type d) :
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		__local_event(e),
#else
		action_ptr(a),
		successor_events(), 
		event_type(t),
		predecessors(0),
		process_index(pid),
#endif
		countdown(0), 
		flags(0),
		delay(d), 
		block_deps()
#if CHPSIM_READ_WRITE_DEPENDENCIES
		, anti_deps()
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EventNode::~EventNode() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
/**
	Need to manually define valarray assignment with 
	container-like semantics.  
 */
EventNode&
EventNode::operator = (const this_type& e) {
	action_ptr = e.action_ptr;
	successor_events.resize(e.successor_events.size());
	copy(begin(e.successor_events), end(e.successor_events), 
		begin(successor_events));
	event_type = e.event_type;
	flags = e.flags;
	process_index = e.process_index;
	predecessors = e.predecessors;
	countdown = e.countdown;
	delay = e.delay;
	block_deps = e.block_deps;		// re-defined!
#if CHPSIM_READ_WRITE_DEPENDENCIES
	anti_deps = e.anti_deps;
#endif
	return *this;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
bool
EventNode::is_trivial(void) const {
	NEVER_NULL(__local_event);
	return __local_event->is_trivial();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
EventNode::has_trivial_delay(void) const {
	NEVER_NULL(__local_event);
	return __local_event->has_trivial_delay();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const action*
EventNode::get_chp_action(void) const {
	NEVER_NULL(__local_event);
	return __local_event->get_chp_action();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unsigned short
EventNode::get_event_type(void) const {
	NEVER_NULL(__local_event);
	return __local_event->get_event_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unsigned short
EventNode::get_predecessors(void) const {
	NEVER_NULL(__local_event);
	return __local_event->get_predecessors();
}
#endif	// CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
/**
	Completely resets the event.  
 */
void
EventNode::orphan(void) {
	this->~EventNode();
	new (this) EventNode();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the state of the event, for beginning simulation.  
 */
void
EventNode::reset(void) {
	countdown = get_predecessors();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-evaluates the guard expression of this event to determine
	whether or not event should be enqueued.  
	Caller is responsible for un/subscribing *this* event (by ID)
		to/from its dependent variables.  
	Only called by State::recheck_transformer, 
		and now CHP::selection's recheck().

	\param ei the index of this event to check
	\param enqueue return list of event(s) to enqueue for execution.
	\return true if *this* event should be enqueued.  
		This avoids having to pass the index of this event down.  
	\return true if check resulted in enqueuing an event.
	Is there a problem with guarded selection statements?
	TODO: Need to examine how they are constructed...
 */
bool
EventNode::recheck(const nonmeta_context& c, const event_index_type ei) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("examining event " << ei << endl);
	bool ret = false;
if (countdown) {
	// then awaiting more predecessors to arrive
	// NOTE: caller should NOT subscribe deps in this case
	STACKTRACE_INDENT_PRINT("countdown at: " << countdown << endl);
	// don't bother subscribing, let the last arrived event cause subscribe
	// return RECHECK_COUNT_BLOCK;
} else {
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	const action* action_ptr = get_chp_action();
#endif
	if (action_ptr) {
		EventRechecker rc(c);
		action_ptr->accept(rc);
		const char r = rc.ret;
		if (r & __RECHECK_ENQUEUE_THIS) {
			STACKTRACE_INDENT_PRINT("ready to fire!" << endl);
			ret = true;
		} else {
			STACKTRACE_INDENT_PRINT("blocked." << endl);
		}
		if (r & __RECHECK_SUBSCRIBE_THIS) {
			STACKTRACE_INDENT_PRINT("subscribed." << endl);
			block_deps.subscribe(c, ei);
		} else if (r & __RECHECK_UNSUBSCRIBE_THIS) {
			STACKTRACE_INDENT_PRINT("unsubscribed." << endl);
			block_deps.unsubscribe(c, ei);
		}
	} else {
		// RECHECK_NEVER_BLOCKED
		STACKTRACE_INDENT_PRINT("null fire." << endl);
		ret = true;
	}
}
	return ret;
}	// end method recheck

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predecessor barrier count is now done when an event is actually
	checked for its first time, because it can now appear multiply
	in the first-check-queue as a result of delaying successor checks
	at join events.  
 */
bool
EventNode::first_check(const nonmeta_context& c, const event_index_type ei) {
	// same as countdown_decrementer
	if (get_predecessors()) {	// event 0 has no predecessors!
		// TODO: give it an artificial one, to avoid this check
		INVARIANT(countdown);
		--countdown;
	}
	return recheck(c, ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this really should just be inlined
	\param c context containing the list of successor events to 
		enqueue immediately.
	\return the type-enumerated index referring to the variable
		or channel that was modified (in @updates).
	What if channel receive? (two modifications?)
 */
void
EventNode::execute(nonmeta_context& c) {
	STACKTRACE_VERBOSE;
	// reset countdown FIRST (because of self-reference event cycles)
	reset_countdown();
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	const action* action_ptr = get_chp_action();
#endif
	if (
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		__local_event->is_null()
#else
		(event_type != EVENT_NULL)
#endif
		&& action_ptr) {
		STACKTRACE_INDENT_PRINT("got action" << endl);
		// at the same time, enqueue successors, depending on event_type
		// execute is responsible for scheduling successors for recheck
		// and decrement the predecessor-arrival countdown
		EventExecutor x(c);
		action_ptr->accept(x);
	} else {	// event is NULL or action_ptr is NULL
		STACKTRACE_INDENT_PRINT("no action" << endl);
		// else do nothing
		// recheck all successors
		// see also "Object/lang/CHP.cc":recheck_all_successor_events()
		// whatever is done here should be consistent with that!
		// remember: decrement successors' predecessor-arrival countdown
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		const local_event_type::successor_list_type&
			successor_events(__local_event->successor_events);
		copy(successor_events.begin(), successor_events.end(), 
			set_inserter(c));
#else
		copy(begin(successor_events), end(successor_events), 
			set_inserter(c));
#endif
	}
}	// end method execute

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventNode::subscribe_deps(const nonmeta_context& c, 
		const event_index_type ei) const {
	block_deps.subscribe(c, ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	For printing the event queue.  
	TODO: a line/position in source might be nice, 
		in case of repetition.  
 */
ostream&
EventNode::dump_brief(ostream& o) const {
	o << process_index << '\t';
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	NEVER_NULL(__local_event);
	return __local_event->dump_brief(o, expr_dump_context::default_value);
#else
	if (action_ptr) {
		action_ptr->dump_event(o, expr_dump_context::default_value);
	} else {
		o << "null";
	}
	// countdown/predecessors?
	return o;
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
EventNode::dump_brief(ostream& o, 
		const expr_dump_context& edc) const {
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	if (action_ptr) {
#endif
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		return __local_event->dump_event(o, edc);
#else
		action_ptr->dump_event(o, edc);
	} else {
		o << "null";
	}
	// countdown/predecessors?
	return o;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	TODO: source-annotated delays?
 */
ostream&
EventNode::dump_source(ostream& o) const {
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	NEVER_NULL(__local_event);
	return __local_event->dump_source(expr_dump_context::default_value);
#else
	if (action_ptr) {
		action_ptr->dump(o, expr_dump_context::default_value);
	} else {
		o << "null";
	}
	return o;
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: source-annotated delays?
 */
ostream&
EventNode::dump_source(ostream& o, const expr_dump_context& edc) const {
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	if (action_ptr) {
#endif
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		return __local_event->dump_source(o, edc);
#else
		action_ptr->dump(o, edc);
	} else {
		o << "null";
	}
	return o;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Dumps more stateful information about node.  
 */
ostream&
EventNode::dump_pending(ostream& o) const {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For diagnostics.  
	TODO: delays?
 */
ostream&
EventNode::dump_struct(ostream& o, const expr_dump_context& edc) const {
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	__local_event->dump_struct(o, edc);
	// missing pid and global event offset!
#else
	switch (event_type) {
	case EVENT_NULL: o << (predecessors > 1 ? "join" : "null"); break;
	case EVENT_ASSIGN: o << "assign"; break;
	case EVENT_SEND: o << "send"; break;
	case EVENT_RECEIVE: o << "receive"; break;
	case EVENT_PEEK: o << "peek"; break;
	case EVENT_CONCURRENT_FORK: o << "fork"; break;
	case EVENT_SELECTION_BEGIN: o << "select"; break;
	case EVENT_CONDITION_WAIT: o << "wait"; break;
	case EVENT_FUNCTION_CALL: o << "call"; break;
	default:
		ISE(cerr, cerr << "Invalid event type enum: "
			<< event_type << endl;)
	}
	o << ": ";
	// factored out code
	dump_brief(o, edc);
	// flags?
	o << ", pid: " << process_index;
	o << ", #pred: " << predecessors;
	// countdown? only in state dump
	// o << endl;
	o << ", succ: ";
	ostream_iterator<event_index_type> osi(o, " ");
	copy(begin(successor_events), end(successor_events), osi);
	o << endl;
#endif	// CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	block_deps.dump(o);	// includes endl already
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't print guards here, put guards on edges.  
	TODO: delays?
 */
ostream&
EventNode::dump_dot_node(ostream& o, const event_index_type i, 
		const graph_options& g, const expr_dump_context& edc) const {
	o << node_prefix << i << '\t';
	o << "[shape=";
	switch (event_type) {
	case EVENT_NULL:
		o << ((predecessors > 1) ? "diamond" : "ellipse"); 
		break;
	case EVENT_ASSIGN: o << "box"; break;
	case EVENT_SEND: o << "house"; break;
	case EVENT_RECEIVE: o << "invhouse"; break;
	case EVENT_PEEK: o << "invhouse"; break;	// or ASSIGN shape?
	case EVENT_CONCURRENT_FORK: o << "hexagon"; break;
	case EVENT_SELECTION_BEGIN: o << "trapezium"; break;
	case EVENT_CONDITION_WAIT: o << "ellipse"; break;
	case EVENT_FUNCTION_CALL: o << "octogon"; break;
		// TODO: flag for non-deterministic? extra periphery?
	default:
		ISE(cerr, cerr << "Invalid event type enum: "
			<< event_type << endl;)
	}
	o << ", label=\"";
	if (g.show_event_index) {
		o << "[" << i << "] ";
	}
	if (g.show_delays) {
		o << '@' << delay << ' ';
	}
	if (!g.process_event_clusters || !process_index) {
		// always show pid 0 because top-level is not clustered
		o << "pid=" << process_index;
	}
	// seems a waste to do this multiple times for same process...
	// can't change until we-reorganize events into contiguous ranges.
	if (action_ptr) {
		// TODO: pass context scope to suppress scope qualifier
		action_ptr->dump_event(o << "\\n", edc);
	}
	// no edges
	// no deps
	o << "\"];" << endl;
	if (action_ptr) {
		EventSuccessorDumper d(o, *this, i, edc);
		action_ptr->accept(d);
	} else {
		dump_successor_edges_default(o, i);
	}
#if CHPSIM_READ_WRITE_DEPENDENCIES
	if (g.show_instances) {
		block_deps.dump_dependence_edges(o, i);
		anti_deps.dump_antidependence_edges(o, i);
	}
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just prints edge label if applicable.
	TODO: somehow print guards on edges of selection.  
		Do this in the print of selection.
 */
ostream&
EventNode::dump_successor_edges_default(ostream& o, 
		const event_index_type i) const {
	// iterate over edges
	const event_index_type* j = begin(successor_events);
	const event_index_type* z = end(successor_events);
	for ( ; j!=z; ++j) {
		const event_index_type h = *j;
		o << node_prefix << i << " -> " << node_prefix << h <<
			';' << endl;
	}
	return o;
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

