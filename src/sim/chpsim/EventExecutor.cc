/**
	\file "sim/chpsim/EventExecutor.cc"
	Visitor implementations for CHP events.  
	$Id: EventExecutor.cc,v 1.1.2.1 2007/03/10 02:52:05 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	ENABLE_STACKTRACE_CHPSIM		(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <iterator>
#include <algorithm>
#include <exception>

#include "sim/chpsim/EventExecutor.h"

#include "Object/lang/CHP.h"
#include "Object/expr/bool_expr.h"
#include "Object/expr/int_expr.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/expr/dynamic_meta_index_list.h"
#include "Object/expr/pbool_const.h"
#include "Object/def/footprint.h"
#include "Object/ref/data_nonmeta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/nonmeta_instance_reference_subtypes.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/persistent_type_hash.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/channel_type_reference_base.h"
#include "Object/traits/chan_traits.h"
#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/channel_instance_collection.h"
#include "Object/inst/instance_placeholder.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/inst/value_placeholder.h"
#include "Object/inst/value_scalar.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/connection_policy.h"
#include "Object/def/footprint.h"
#include "Object/unroll/unroll_context.h"
#include "Object/common/dump_flags.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_param_expr_list.h"
#if CHP_ACTION_DELAYS
#include "Object/expr/preal_const.h"
#endif
#include "Object/def/template_formals_manager.h"
#include "Object/nonmeta_context.h"
#include "Object/state_manager.h"
#include "Object/global_channel_entry.h"
#include "Object/nonmeta_channel_manipulator.h"

// chpsim headers
#include "sim/chpsim/StateConstructor.h"
#include "sim/chpsim/DependenceCollector.h"
#include "sim/chpsim/State.h"
#include "sim/chpsim/nonmeta_context.h"

#include "common/ICE.h"
#include "common/TODO.h"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/visitor_functor.h"
#include "util/value_saver.h"
#include "util/indent.h"
#include "util/IO_utils.tcc"
#include "util/STL/valarray_iterator.h"
#include "util/reference_wrapper.h"
#include "util/iterator_more.h"		// for set_inserter
#include "util/numeric/random.h"	// for rand48

#if ENABLE_STACKTRACE_CHPSIM
#define	STACKTRACE_CHPSIM_VERBOSE	STACKTRACE_VERBOSE
#else
#define	STACKTRACE_CHPSIM_VERBOSE
#endif

namespace HAC {
namespace entity {
namespace CHP {
#include "util/using_ostream.h"
}
}
namespace SIM {
namespace CHPSIM {
using std::equal;
using std::copy;
using std::find;
using std::transform;
using std::back_inserter;
using std::for_each;
using entity::expr_dump_context;
using entity::ChannelState;
using entity::CHP::selection_list_type;
using util::set_inserter;
using util::auto_indent;
using util::persistent_traits;
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
#if 0
using SIM::CHPSIM::EventNode;
using SIM::CHPSIM::RECHECK_NEVER_BLOCKED;
using SIM::CHPSIM::RECHECK_BLOCKED_THIS;
using SIM::CHPSIM::RECHECK_UNBLOCKED_THIS;
using SIM::CHPSIM::RECHECK_DEFERRED_TO_SUCCESSOR;
#endif
using util::reference_wrapper;
using util::numeric::rand48;
using util::memory::count_ptr;
#if CHP_ACTION_DELAYS
using entity::preal_const;
#endif

//=============================================================================
/// helper routines

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For all non-selection events that execute, schedule all successor
	events for recheck.
	Also decrement those events' countdown upon execution of this event.  
	Don't forget to decrement the selected event's counter for selections!
	This should be kept consistent with CHPSIM::EventNode::execute().
 */
static
inline
void
recheck_all_successor_events(const nonmeta_context& c) {
	typedef	EventNode	event_type;
	typedef	size_t		event_index_type;
	STACKTRACE_CHPSIM_VERBOSE;
	const event_type::successor_list_type&
		succ(c.get_event().successor_events);
	copy(std::begin(succ), std::end(succ), set_inserter(c.rechecks));
	for_each(std::begin(succ), std::end(succ), 
		event_type::countdown_decrementer(c.event_pool));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print outgoing edges adorned with guard expressions as labels. 
 */
static
ostream&
dump_selection_successor_edges(const selection_list_type& l, 
		ostream& o, const EventNode& e, const size_t i, 
		const expr_dump_context& c) {
	typedef	selection_list_type::const_iterator const_iterator;
	const EventNode::successor_list_type& succ(e.successor_events);
	const size_t* si = std::begin(succ);
	const size_t* se = std::end(succ);
	const_iterator li(l.begin()), le(l.end());
	for ( ; li!=le; ++li, ++si) {
		const guarded_action::guard_ptr_type&
			g((*li)->get_guard());
		o << EventNode::node_prefix << i << " -> " <<
			EventNode::node_prefix << *si << "\t[label=\"";
		if (g) {
			g->dump(o, c);
		} else {
			o << "else";
		}
		o << "\"];" << endl;
	}
	// guard list may have ONE less than successor list
	// if there is an implicit else-clause
	if (si != se) {
		o << EventNode::node_prefix << i << " -> " <<
			EventNode::node_prefix << *si <<
			"\t[label=\"else\"];" << endl;
		++si;
		INVARIANT(si == se);
		
	}
	// check for else clause
	return o;
}

//=============================================================================
// class action method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	By default print all successor edge, unadorned.  
 */
#define DEFAULT_EVENT_SUCCESSOR_DUMPER(T)				\
void									\
EventSuccessorDumper::visit(const T&) {					\
	event.dump_successor_edges_default(os, index);			\
}

//=============================================================================
// class action_sequence method definitions

DEFAULT_EVENT_SUCCESSOR_DUMPER(action_sequence)
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sequences should never be used as leaf events, 
	so this does nothing.  
 */
void
EventExecutor::visit(const action_sequence&) {
	// no-op
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sequences should never be used as leaf events, 
	so this does nothing.  
 */
void
EventRechecker::visit(const action_sequence&) {
	// no-op
	ICE_NEVER_CALL(cerr);
	ret = RECHECK_UNBLOCKED_THIS;	// don't care
}

//=============================================================================
// class concurrent_actions method definitions

DEFAULT_EVENT_SUCCESSOR_DUMPER(concurrent_actions)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Action groups should never be used as leaf events, 
	so this does nothing.  
	Conditionally enqueue all successors, blocking on ones that are not
	ready to execute.  
 */
void
EventExecutor::visit(const concurrent_actions& ca) {
	STACKTRACE_CHPSIM_VERBOSE;
	recheck_all_successor_events(context);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Action groups should never be used as leaf events, 
	so this does nothing.  
	This event also never blocks, although its successors may block.
 */
void
EventRechecker::visit(const concurrent_actions&) {
	STACKTRACE_CHPSIM_VERBOSE;
	// no-op
	ret = RECHECK_NEVER_BLOCKED;
}

//=============================================================================
// struct guarded_action::unroll_resolver method definitions

//=============================================================================
// class guarded_action::selection_evaluator definition
#if 1
/**
	Functor for evaluating guarded statements.  
	This class can be given hidden visibility (local to module).  
 */
struct guarded_action::selection_evaluator {
	// operator on selection_list_type::const_reference
	typedef	const count_ptr<const guarded_action>&	argument_type;
	const nonmeta_context_base& 		context;
	/// successor index (induction variable)
	size_t					index;
	/// list of successors whose guards evaluated true (accumulate)
	vector<size_t>				ready;

	explicit
	selection_evaluator(const nonmeta_context_base& c) :
			context(c), index(0), ready() {
		ready.reserve(2);
	}

	void
	operator () (argument_type g) {
		STACKTRACE_CHPSIM_VERBOSE;
		NEVER_NULL(g);
		STACKTRACE_INDENT_PRINT("evaluating guard " << index << endl);
	if (g->guard) {
		const count_ptr<const pbool_const>
			b(g->guard->__nonmeta_resolve_rvalue(
				context, g->guard));
		// error handling
		if (!b) {
			cerr <<
				"Run-time error evaluating guard expression."
				<< endl;
			THROW_EXIT;
		}
		if (b->static_constant_value()) {
			STACKTRACE_INDENT_PRINT("true guard" << endl);
			ready.push_back(index);
		} else {
			STACKTRACE_INDENT_PRINT("false guard" << endl);
		}
	} else {
		// NULL guard is an else clause (in last position)
		STACKTRACE_INDENT_PRINT("else guard" << endl);
		if (ready.empty()) {
			ready.push_back(index);
		}
	}
		++index;
	}	// end operator ()

private:
	typedef	selection_evaluator		this_type;

	/// no-copy or assign
	explicit
	selection_evaluator(const this_type&);

};	// end class selection_evaluator

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor reference wrapper.
 */
class guarded_action::selection_evaluator_ref :
	public util::reference_wrapper<guarded_action::selection_evaluator> {
	typedef	util::reference_wrapper<guarded_action::selection_evaluator>
				parent_type;
	typedef	parent_type::type::argument_type	argument_type;
public:
	selection_evaluator_ref(reference r) : parent_type(r) { }

	/**
		Forwarding operator to underlying reference.  
	 */
	void
	operator () (argument_type a) {
		get()(a);
	}

};	// end class selection_evaluator_ref
#endif

//=============================================================================
DEFAULT_EVENT_SUCCESSOR_DUMPER(guarded_action)

void
EventExecutor::visit(const guarded_action&) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventRechecker::visit(const guarded_action&) {
	ICE_NEVER_CALL(cerr);
}

//=============================================================================
// class deterministic_selection method definitions

/**
	Action groups should never be used as leaf events, 
	so this does nothing other than evaluate guards, via recheck().  
	Q: this is checked twice: pre-enqueue, and during execution.
		What if guard is unstable?  and conditions change?
 */
void
EventExecutor::visit(const deterministic_selection&) {
	STACKTRACE_CHPSIM_VERBOSE;
#if 0
	const bool b = recheck(c);
	INVARIANT(b);
#else
	// never enqueues itself, only successors
	// see recheck() below
	ICE_NEVER_CALL(cerr);
#endif
	// violation is possible if guard was true but because
	// false due to concurrent events
	// we should alert user with run-time error
	// TODO: Is it possible to re-subscribe this event for re-checking?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Selections act like a proxy event that dispatches one of its
	successors, but never executes "itself" when unblocked.  
	Action groups should never be used as leaf events, 
	so this does nothing.
	When a successor is ready to enqueue, unsubscribe this event from
	its dependencies.  
	\return false signaling that this event is never enqueued, 
		(only successors are enqueued).
	Q: this is checked twice? A: no, doesn't follow conventional execution
	TODO: test nested selections!
 */
void
EventRechecker::visit(const deterministic_selection& ds) {
	// 1) evaluate all clauses, which contain guard expressions
	//	Use functional pass.
	// 2) if exactly one is true, return reference to it as the successor
	//	event to enqueue (not execute right away)
	//	a) if more than one true, signal a run-time error
	//	b) if none are true, and there is an else clause, use it
	//	c) if none are true, without else clause, 'block',
	//		subscribing this event to its set of dependents.  
	STACKTRACE_CHPSIM_VERBOSE;
	guarded_action::selection_evaluator G(context);	// needs reference wrap
	for_each(ds.begin(), ds.end(),
		guarded_action::selection_evaluator_ref(G));
	switch (G.ready.size()) {
	case 0: {
		ret = RECHECK_BLOCKED_THIS;	// no successor to enqueue
		// caller will subscribe this event's dependencies
		break;
	}
	case 1: {
		EventNode& t(context.get_event());	// this event
		const size_t ei = t.successor_events[G.ready.front()];
		STACKTRACE_INDENT_PRINT("have a winner! eid: " << ei << endl);
		t.reset_countdown();
		// act like this event (its predecessor) executed
		EventNode::countdown_decrementer(context.event_pool)(ei);
		// recheck it on the spot
		EventNode& suc(context.event_pool[ei]);
		const nonmeta_context::event_setter x(context, &suc);
		// temporary, too lazy to copy, will restore upon destruction
		suc.recheck(context, ei);
		ret = RECHECK_DEFERRED_TO_SUCCESSOR;
		break;
	}
	default:
		cerr << "Run-time error: multiple exclusive guards of "
			"deterministic selection evaluated TRUE!" << endl;
		THROW_EXIT;
		ret = RECHECK_BLOCKED_THIS;		// unreachable
	}	// end switch
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventSuccessorDumper::visit(const deterministic_selection& ds) {
	dump_selection_successor_edges(ds, os, event, index, dump_context);
}

//=============================================================================
// class nondeterministic_selection method definitions

/**
	TODO: we may want this to behave differently than determinstic sel.
	We may want to introduce a delay window from recheck to enqueue
	so multiple input guards may become true to require arbitration, 
	not just the arrival of the first true guard.
	In this case, we want recheck behavior upon execution.  
	Recheck will be slightly different.  
	Note that the window of delay also allows for the possibility
	of unstable guards upon re-evaluation!
	Deterministic selection does not have this problem, 
	as branch selection occurs immediately.  

	Action groups should never be used as leaf events, 
	so this does nothing other than evaluate guards, via recheck().  
	Q: this is checked twice: pre-enqueue, and during execution.
		What if guard is unstable?  and conditions change?
 */
void
EventExecutor::visit(const nondeterministic_selection& ns) {
	STACKTRACE_CHPSIM_VERBOSE;
#if 0
	ICE_NEVER_CALL(cerr);
#else
	// 1) evaluate all clauses, which contain guard expressions
	//	Use functional pass.
	// 2) if exactly one is true, return reference to it as the successor
	//	event to enqueue (not execute right away)
	//	a) if more than one true, signal a run-time error
	//	b) if none are true, and there is an else clause, use it
	//	c) if none are true, without else clause, 'block',
	//		subscribing this event to its set of dependents.  
	STACKTRACE_CHPSIM_VERBOSE;
	guarded_action::selection_evaluator G(context);	// needs reference wrap
	for_each(ns.begin(), ns.end(),
		guarded_action::selection_evaluator_ref(G));
	const size_t m = G.ready.size();
	EventNode& t(context.get_event());
	switch (m) {
	case 0: {
		// this can happen because guards may be unstable, 
		// and can thus become invalidated between enqueue and execute
		context.subscribe_this_event();	// deduce own event index
		break;
	}
	case 1: {
		const size_t ei = t.successor_events[G.ready.front()];
#if 0
		t.reset_countdown();
		EventNode::countdown_decrementer(c.event_pool)(ei);
		// recheck it on the spot
		EventNode& suc(c.event_pool[ei]);
		const nonmeta_context::event_setter x(c, &suc);
		// temporary, too lazy to copy, will restore upon destruction
		suc.recheck(c, ei);
#else
		context.rechecks.insert(ei);
		EventNode::countdown_decrementer(context.event_pool)(ei);
#endif
		break;
	}
	default: {
		// pick one at random
		static rand48<unsigned long> rgen;
		const size_t r = rgen();	// random-generate
		const size_t ei = t.successor_events[G.ready[r%m]];
#if 0
		t.reset_countdown();
		EventNode::countdown_decrementer(c.event_pool)(ei);
		// recheck it on the spot
		EventNode& suc(c.event_pool[ei]);
		const nonmeta_context::event_setter x(c, &suc);
		// temporary, too lazy to copy, will restore upon destruction
		suc.recheck(c, ei);
#else
		context.rechecks.insert(ei);
		EventNode::countdown_decrementer(context.event_pool)(ei);
#endif
	}
	}	// end switch
#endif
}	// end visit(const nondeterministic_selection&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: implement this differently, see comment in execute().
	Action groups should never be used as leaf events, 
	so this does nothing.
	When a successor is ready to enqueue, unsubscribe this event from
	its dependencies.  
	\pre selection has no else clause.  
	\return false signaling that this event is never enqueued, 
		(only successors are enqueued).
	Q: this is checked twice?
 */
void
EventRechecker::visit(const nondeterministic_selection& ns) {
	// 1) evaluate all clauses, which contain guard expressions
	//	Use functional pass.
	// 2) if exactly one is true, return reference to it as the successor
	//	event to enqueue (not execute right away)
	//	a) if more than one true, signal a run-time error
	//	b) if none are true, and there is an else clause, use it
	//	c) if none are true, without else clause, 'block',
	//		subscribing this event to its set of dependents.  
	STACKTRACE_CHPSIM_VERBOSE;
	guarded_action::selection_evaluator G(context);	// needs reference wrap
	for_each(ns.begin(), ns.end(),
		guarded_action::selection_evaluator_ref(G));
#if 0
	const size_t m = G.ready.size();
	EventNode& t(c.get_event());
	switch (m) {
	case 0: {
		// TODO: see determinstic selection
		return RECHECK_BLOCKED_THIS;	// no successor to enqueue
	}
	case 1: {
		const size_t ei = t.successor_events[G.ready.front()];
		t.reset_countdown();
		EventNode::countdown_decrementer(c.event_pool)(ei);
#if 0
		c.rechecks.insert(ei);
		return true;
#else
		// recheck it on the spot
		EventNode& suc(c.event_pool[ei]);
		const nonmeta_context::event_setter x(c, &suc);
		// temporary, too lazy to copy, will restore upon destruction
		suc.recheck(c, ei);
		return RECHECK_DEFERRED_TO_SUCCESSOR;
#endif
	}
	default: {
		// pick one at random
		static rand48<long> rgen;
		const size_t r = rgen();	// random-generate
		const size_t ei = t.successor_events[G.ready[r%m]];
		t.reset_countdown();
		EventNode::countdown_decrementer(c.event_pool)(ei);
#if 0
		c.rechecks.insert(ei);
		return true;
#else
		// recheck it on the spot
		EventNode& suc(c.event_pool[ei]);
		const nonmeta_context::event_setter x(c, &suc);
		// temporary, too lazy to copy, will restore upon destruction
		suc.recheck(c, ei);
		return RECHECK_DEFERRED_TO_SUCCESSOR;
#endif
	}
	}	// end switch
	return RECHECK_BLOCKED_THIS;
#else
	if (G.ready.size()) {
		// defer actual selection until the execution phase, 
		// with some delay
		ret = RECHECK_UNBLOCKED_THIS;
	} else {
		ret = RECHECK_BLOCKED_THIS;
	}
#endif
}	// end visit(const nondeterministic_selection&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventSuccessorDumper::visit(const nondeterministic_selection& ns) {
	dump_selection_successor_edges(ns, os, event, index, dump_context);
}

//=============================================================================
// class metaloop_selection method definitions

DEFAULT_EVENT_SUCCESSOR_DUMPER(metaloop_selection)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never called, always expanded.  
 */
void
EventExecutor::visit(const metaloop_selection&) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never called, always expanded.  
 */
void
EventRechecker::visit(const metaloop_selection&) {
	ICE_NEVER_CALL(cerr);
	ret = RECHECK_BLOCKED_THIS;	// don't care
}

//=============================================================================
// class metaloop_statement method definitions

DEFAULT_EVENT_SUCCESSOR_DUMPER(metaloop_statement)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never called, always expanded.  
 */
void
EventExecutor::visit(const metaloop_statement&) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never called, always expanded.  
 */
void
EventRechecker::visit(const metaloop_statement&) {
	ICE_NEVER_CALL(cerr);
	ret = RECHECK_BLOCKED_THIS;	// don't care
}

//=============================================================================
// class assignment method definitions

DEFAULT_EVENT_SUCCESSOR_DUMPER(assignment)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	lvalue must be bool, int, or enum reference.  
	\param u collection of references updated by the assignment execution,
		namely, the lvalues.
 */
void
EventExecutor::visit(const assignment& a) {
	typedef	EventNode		event_type;
	STACKTRACE_CHPSIM_VERBOSE;
	a.get_lval()->nonmeta_assign(a.get_rval(), context, global_refs);
		// also tracks updated reference
	recheck_all_successor_events(context);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assignments are non-blocking, and thus need no re-evaluation.
 */
void
EventRechecker::visit(const assignment&) {
	STACKTRACE_CHPSIM_VERBOSE;
	// no-op
	ret = RECHECK_NEVER_BLOCKED;
}

//=============================================================================
// class condition_wait method definitions

DEFAULT_EVENT_SUCCESSOR_DUMPER(condition_wait)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does nothing, is a NULL event.  
	NOTE: it is possible that guard is no longer true, 
		as it may be invalidated since the time it was enqueued.
		We do not check for guard stability... yet.  
 */
void
EventExecutor::visit(const condition_wait& cw) {
	STACKTRACE_CHPSIM_VERBOSE;
	recheck_all_successor_events(context);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: (Q?) condition-wait is currently non-atomic, see execute() note.
	If we wanted true atomicity, then we would have the recheck()
	enqueue the *succcessor* events, rather than itself, just like
	deterministic_selection::recheck().  

	The 'guarded' action is a NULL event, which can always occur.  
	The guard expression is already checked by the caller
	as a part of event processing.  
 */
void
EventRechecker::visit(const condition_wait& cw) {
	STACKTRACE_CHPSIM_VERBOSE;
	const condition_wait::cond_ptr_type& cond(cw.get_guard());
	if (cond) {
		// TODO: decide error handling via exceptions?
		const count_ptr<const pbool_const>
			g(cond->__nonmeta_resolve_rvalue(context, cond));
		if (!g) {
			cerr << "Failure resolving run-time value of "
				"boolean expression: ";
			cond->dump(cerr,
				expr_dump_context::default_value) << endl;
			// temporary
			THROW_EXIT;
		}
		ret = g->static_constant_value() ?
			RECHECK_UNBLOCKED_THIS : RECHECK_BLOCKED_THIS;
	} else {
		ret = RECHECK_UNBLOCKED_THIS;
	}
}

//=============================================================================
// class channel_send method definitions

DEFAULT_EVENT_SUCCESSOR_DUMPER(channel_send)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns the 'fields' of the channel and flips the (lock) state bit.  
	Only the channel is 'modified' by a send, so we register it
	with the update set.  
	\throws an exception if anything goes wrong with expression
		evaluation.  
 */
void
EventExecutor::visit(const channel_send& cs) {
	STACKTRACE_CHPSIM_VERBOSE;
	const channel_send::chan_ptr_type& chan(cs.get_chan());
	const size_t chan_index = chan->lookup_nonmeta_global_index(context);
#define	ASSERT_CHAN_INDEX						\
	if (!chan_index) {						\
		chan->dump(cerr << "at: ",				\
			expr_dump_context::default_value) << endl;	\
		THROW_EXIT;						\
	}
	ASSERT_CHAN_INDEX
	ChannelState& nc(context.values.get_pool<channel_tag>()[chan_index]);
#if 0
	// don't need
	const global_entry<channel_tag>&
		ch(c.sm->get_pool<channel_tag>()[chan_index]);
#endif
	// evaluate rvalues of channel send statement (may throw!)
	// write to the ChannelState using canonical_fundamental_type
	for_each(cs.get_exprs().begin(), cs.get_exprs().end(), 
		entity::nonmeta_expr_evaluator_channel_writer(context, nc));
	// track the updated-reference (channel)
	// expressions are only read, no lvalue data modified
	global_refs.push_back(std::make_pair(
		size_t(entity::META_TYPE_CHANNEL), chan_index));
	NEVER_NULL(nc.can_send());	// else run-time exception
	nc.send();
	recheck_all_successor_events(context);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enqueue event if it is ready to execute.  
	\return true if this event can be unblocked and enqueued for execution.
 */
void
EventRechecker::visit(const channel_send& cs) {
	STACKTRACE_CHPSIM_VERBOSE;
	// see if referenced channel is ready to send
	const channel_send::chan_ptr_type& chan(cs.get_chan());
	const size_t chan_index = chan->lookup_nonmeta_global_index(context);
	ASSERT_CHAN_INDEX
	const ChannelState&
		nc(context.values.get_pool<channel_tag>()[chan_index]);
	ret = nc.can_send() ? RECHECK_UNBLOCKED_THIS : RECHECK_BLOCKED_THIS;
}

//=============================================================================
// class channel_receive method definitions

DEFAULT_EVENT_SUCCESSOR_DUMPER(channel_receive)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns the 'fields' of the channel and flips the (lock) state bit.  
	Both the channel and lvalues are 'modified' by a receive, 
	so we register them all with the update set.  
 */
void
EventExecutor::visit(const channel_receive& cr) {
	STACKTRACE_CHPSIM_VERBOSE;
	const channel_receive::chan_ptr_type& chan(cr.get_chan());
	const size_t chan_index = chan->lookup_nonmeta_global_index(context);
	ASSERT_CHAN_INDEX
	ChannelState& nc(context.values.get_pool<channel_tag>()[chan_index]);
#if 0
	// don't need
	const global_entry<channel_tag>&
		ch(c.sm->get_pool<channel_tag>()[chan_index]);
#endif
	// evaluate lvalues of channel receive statement (may throw!)
	// read from the ChannelState using canonical_fundamental_type
	for_each(cr.get_insts().begin(), cr.get_insts().end(), 
		entity::nonmeta_reference_lookup_channel_reader(
			context, nc, global_refs));
	// track the updated-reference (channel)
	global_refs.push_back(std::make_pair(
		size_t(entity::META_TYPE_CHANNEL), chan_index));
	INVARIANT(nc.can_receive());	// else run-time exception
	nc.receive();
	recheck_all_successor_events(context);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enqueue event if it is ready to execute.  
	\return true if this event can be unblocked and enqueued for execution.
 */
void
EventRechecker::visit(const channel_receive& cr) {
	STACKTRACE_CHPSIM_VERBOSE;
	// see if referenced channel is ready to receive
	const channel_receive::chan_ptr_type& chan(cr.get_chan());
	const size_t chan_index = chan->lookup_nonmeta_global_index(context);
	ASSERT_CHAN_INDEX
	const ChannelState&
		nc(context.values.get_pool<channel_tag>()[chan_index]);
	ret = nc.can_receive() ? RECHECK_UNBLOCKED_THIS : RECHECK_BLOCKED_THIS;
}
#undef	ASSERT_CHAN_INDEX

//=============================================================================
// class do_forever_loop method definitions

DEFAULT_EVENT_SUCCESSOR_DUMPER(do_forever_loop)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op, this should never be called from simulator, as loop
	body events are expanded.  
 */
void
EventExecutor::visit(const do_forever_loop&) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventRechecker::visit(const do_forever_loop&) {
	ICE_NEVER_CALL(cerr);
	ret = RECHECK_BLOCKED_THIS;	// don't care
}

//=============================================================================
// class do_while_loop method definitions

/**
	Evaluate the guards immediately.
	If evaluation is true, execute the body branch, else take the
	else-clause (exit) successor, enumerated as the last branch.
	NOTE: there's no else clause.
	\return true, as this node is never blocking.  
 */
void
EventExecutor::visit(const do_while_loop& dw) {
	STACKTRACE_CHPSIM_VERBOSE;
	guarded_action::selection_evaluator G(context);	// needs reference wrap
	for_each(dw.begin(), dw.end(),
		guarded_action::selection_evaluator_ref(G));
	size_t si = 0;
	switch (G.ready.size()) {
	case 0:	si = dw.size();
		// this case should never be reached because the else-clause
		// has been expanded and is always evaluated true
		STACKTRACE_INDENT_PRINT("no guards true, fall-through " << si << endl);
		break;
	case 1:	si = G.ready.front();
		STACKTRACE_INDENT_PRINT("guard true, taking successor " << si << endl);
		break;
	default:
		cerr << "Run-time error: multiple exclusive guards of "
			"do-while-loop evaluated TRUE!" << endl;
		THROW_EXIT;
	}	// end switch
	const size_t ei = context.get_event().successor_events[si];
	context.rechecks.insert(ei);
	EventNode::countdown_decrementer(context.event_pool)(ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This event never blocks and thus never needs to be rechecked for 
	unblocking because the loop-condition comes with an implicit
	else-clause which is taken if the evaluated condition is false.  
	\return true because this event never rechecks itself (never blocks), 
		only selects a succcessor.  
 */
void
EventRechecker::visit(const do_while_loop&) {
	STACKTRACE_CHPSIM_VERBOSE;
	ret = RECHECK_NEVER_BLOCKED;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventSuccessorDumper::visit(const do_while_loop& dw) {
	dump_selection_successor_edges(dw, os, event, index, dump_context);
}

#undef	DEFAULT_EVENT_SUCCESSOR_DUMPER
//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

