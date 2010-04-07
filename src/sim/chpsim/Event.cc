/**
	\file "sim/chpsim/Event.cc"
	$Id: Event.cc,v 1.14 2010/04/07 00:13:07 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <sstream>
#include "sim/chpsim/Event.h"
#include "common/TODO.h"
#include "sim/ISE.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/lang/CHP_base.h"
#include "Object/lang/CHP_context_printer.h"
#include "sim/chpsim/nonmeta_context.h"
#include "sim/chpsim/graph_options.h"
#include "sim/chpsim/EventExecutor.h"
#include "Object/lang/CHP_event.h"
#include "Object/lang/CHP_event_alloc.h"	// for Dumper
#include "sim/chpsim/StateConstructor.h"	// for dependencies
#include "util/stacktrace.h"
#include "util/iterator_more.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
#include "util/using_ostream.h"
using std::ostream_iterator;
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

const EventNode::time_type
EventNode::default_delay = 10;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EventNode::EventNode() :
		__local_event(NULL),
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
/**
	\param a the CHP::action pointer (may be NULL)
		using shallow reference, not reference counting.
 */
EventNode::EventNode(const local_event_type* e, 
		const time_type d) :
		__local_event(e),
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
EventNode::num_successors(void) const {
	NEVER_NULL(__local_event);
	return __local_event->successor_events.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a pointer iterator to the front of the local successors list.
 */
const event_index_type*
EventNode::local_successors_begin(void) const {
	NEVER_NULL(__local_event);
	return &*__local_event->successor_events.begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a pointer iterator to the end of the local successors list.
 */
const event_index_type*
EventNode::local_successors_end(void) const {
	NEVER_NULL(__local_event);
	return &*__local_event->successor_events.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventNode::make_global_root(const local_event_type* g) {
	NEVER_NULL(g);
	__local_event = g;
	// INVARIANT event type is concurrent fork
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventNode::setup(const local_event_type* l, const State& s) {
	STACKTRACE_VERBOSE;
	__local_event = l;
	NEVER_NULL(__local_event);
	StateConstructor v(s, *this);
	const action* action_ptr = get_chp_action();
	if (action_ptr) {
		action_ptr->accept(v);
	} else {
		// set default delay for NULL events
		delay = 0;
	}
}

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
	const action* action_ptr = get_chp_action();
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
	STACKTRACE_VERBOSE;
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
	const action* action_ptr = get_chp_action();
	if (!__local_event->is_null() && action_ptr) {
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
		const local_event_type::successor_list_type&
			successor_events(__local_event->successor_events);
		copy(successor_events.begin(), successor_events.end(), 
			set_inserter(c));
		c.first_check_all_successors();
	}
}	// end method execute

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventNode::subscribe_deps(const nonmeta_context& c, 
		const event_index_type ei) const {
	block_deps.subscribe(c, ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
EventNode::dump_brief(ostream& o, 
		const expr_dump_context& edc) const {
	return __local_event->dump_brief(o, edc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: source-annotated delays?
 */
ostream&
EventNode::dump_source(ostream& o, const expr_dump_context& edc) const {
	return __local_event->dump_source(o, edc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
EventNode::dump_source_context(ostream& o, const expr_dump_context& edc) const {
	const action* a = get_chp_action();
	if (a) {
		__local_event->dump_type(o) << ": ";
		entity::CHP::chp_context_printer P(*a, o, edc);
		P();
		return o;
	} else {
		return o << "[null]" << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For diagnostics.  
	TODO: delays?
 */
ostream&
EventNode::dump_struct(ostream& o, const expr_dump_context& edc,
		const size_t pid, const event_index_type offset) const {
	NEVER_NULL(__local_event);
	__local_event->dump_struct(o, edc, pid, offset);
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
		const graph_options& g, const expr_dump_context& edc,
		const size_t process_index, 
		const event_index_type offset) const {
	// pass extra text info to printer
	std::ostringstream extra;
	if (g.show_delays) {
		extra << '@' << delay << ' ';
	}
	if (!g.process_event_clusters || !process_index) {
		// always show pid 0 because top-level is not clustered
		extra << "pid=" << process_index;
	}
	__local_event->dump_dot_node(o, i, g, edc, extra.str().c_str(), offset);
#if CHPSIM_READ_WRITE_DEPENDENCIES
	if (g.show_instances) {
		block_deps.dump_dependence_edges(o, i);
		anti_deps.dump_antidependence_edges(o, i);
	}
#endif
	return o;
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

