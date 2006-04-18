/**
	\file "sim/prsim/State.cc"
	Implementation of prsim simulator state.  
	$Id: State.cc,v 1.6.2.1 2006/04/18 05:57:23 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_FANOUT			(0 && ENABLE_STACKTRACE)
#define	DEBUG_STEP			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <algorithm>
#include <functional>
#include <string>
#include "sim/prsim/State.h"
#include "sim/prsim/ExprAlloc.h"
#include "sim/prsim/Event.tcc"
#include "sim/random_time.h"
#include "util/list_vector.tcc"
#include "Object/module.h"
#include "Object/state_manager.h"
#include "Object/traits/classification_tags.h"
#include "Object/global_entry.h"
#include "common/ICE.h"
#include "common/TODO.h"
#include "util/attributes.h"
#include "util/signal.h"
#include "util/sstream.h"
#include "util/stacktrace.h"
#include "util/memory/index_pool.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/likely.h"

namespace HAC {
namespace entity { }

namespace SIM {
namespace PRSIM {
using std::string;
using std::ostringstream;
using std::for_each;
using std::mem_fun_ref;
using entity::state_manager;
using entity::global_entry_pool;
using entity::bool_tag;
using entity::process_tag;
#include "util/using_ostream.h"
//=============================================================================
// class State method definitions

#if 0
/**
	TODO: pick reasonable chunk size for expr_pool.  
 */
State::State() : node_pool(), expr_pool(), expr_graph_node_pool(),
		event_pool(), event_queue() {
	expr_graph_node_pool.set_chunk_size(1024);
	head_sentinel();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates simulation state, given a module.
	TODO: do this work in module?
	TODO: add support for top-level PRS (outside of procs)
	TODO: expression minimization pass
	\param m the expanded module object.
	\pre m must already be past the allcoate phase.  
 */
#if 0
State::State(const count_ptr<const entity::module>& m) : 
#else
State::State(const entity::module& m) : 
#endif
		mod(m), 
		node_pool(), expr_pool(), expr_graph_node_pool(),
		event_pool(), event_queue(), 
		rule_map(), 
		exhi(), exlo(), 
		exclhi_queue(), excllo_queue(), 
		pending_queue(), 
		current_time(0), 
		watch_list(), 
		flags(FLAGS_DEFAULT),
		timing_mode(TIMING_DEFAULT),
		ifstreams() {
#if 0
	NEVER_NULL(m);
	const state_manager& sm(m->get_state_manager());
#else
	const state_manager& sm(mod.get_state_manager());
#endif
	const global_entry_pool<bool_tag>&
		bool_pool(sm.get_pool<bool_tag>());
	expr_graph_node_pool.set_chunk_size(1024);
	head_sentinel();
	// recall, the global node pool is 1-indexed because entry 0 is null
	// we mirror this in our own node state pool, by allocating
	// the same number of elements.  
	const size_t s = bool_pool.size();
	node_pool.resize(s);
	// only if these are vectors, not if they are maps (template policy!)
	exhi.resize(s);
	exlo.resize(s);
	// then go through all processes to generate expressions
#if 0
	const global_entry_pool<process_tag>&
		proc_pool(sm.get_pool<process_tag>());
#endif
#if 0
	// create a temporary vector of expressions using list_vector first
	// then transfer them over to the expr_pool
	temp_expr_pool_type build_exprs;
	build_exprs.set_chunk_size(1024);
#endif
	// use a cflat-prs-like pass to construct the expression netlist
	// got a walker? and prs_expr_visitor?
	// see "ExprAlloc.h"

	// NOTE: we're referencing 'this' during construction, however, we 
	// are done constructing this State's members at this point.  
	ExprAlloc v(*this);

	sm.accept(v);
	// TODO: expression tree minimization pass
	// during minimization, need to create a map from old
	// to new indices, and keep track of unused indices
	// for a mark-sweep-compaction into target State
}	// end State::State(const module&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: possibly run some checks?
 */
State::~State() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Destroys the simulator state, releasing all of its memory too.  
 */
void
State::destroy(void) {
	node_pool.clear();
#if 0
	expr_pool.clear();
	// BAH! clear() instantiates operator =, which requires assignability!
#else
	expr_pool.~expr_pool_type();
	new (&expr_pool) expr_pool_type();
#endif
	expr_graph_node_pool.clear();
	event_pool.clear();
	event_queue.clear();
	head_sentinel();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the state of simulation by X-ing all nodes, but
	also preserves some simulator modes, such as the 
	watch/break point state.
	\pre expressions are already properly sized.  
 */
void
State::initialize(void) {
	for_each(node_pool.begin(), node_pool.end(), 
		mem_fun_ref(&node_type::initialize));
	for_each(expr_pool.begin(), expr_pool.end(), 
		mem_fun_ref(&expr_type::initialize));
	// the expr_graph_node_pool contains no stateful information.  
	while (!event_queue.empty()) {
		const event_placeholder_type next(event_queue.pop());
		event_pool.deallocate(next.event_index);
	}
	flags |= FLAGS_INITIALIZE_SET_MASK;
	flags &= ~FLAGS_INITIALIZE_CLEAR_MASK;
	// unwatchall()? no, preserved
	// timing mode preserved
	current_time = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the state of simulation, as if it had just started up.  
	Preserve the watch/break point state.
	\pre expressions are already properly sized.  
 */
void
State::reset(void) {
	for_each(node_pool.begin(), node_pool.end(), 
		mem_fun_ref(&node_type::reset));
	for_each(expr_pool.begin(), expr_pool.end(), 
		mem_fun_ref(&expr_type::reset));
	// the expr_graph_node_pool contains no stateful information.  
	while (!event_queue.empty()) {
		const event_placeholder_type next(event_queue.pop());
		event_pool.deallocate(next.event_index);
	}
	flags = FLAGS_DEFAULT;
	timing_mode = TIMING_DEFAULT;
	// unwatchall()? no, preserved
	current_time = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pre-allocates one element in node and expr pools, which are 1-indexed.
	NOTE: the event pool takes care of itself already.  
	\pre all pools are empty, as if just clear()ed.
	\post the pools have one null element at position 0.  
 */
void
State::head_sentinel(void) {
	node_pool.resize(FIRST_VALID_NODE);
	expr_pool.resize(FIRST_VALID_EXPR);
	expr_graph_node_pool.push_back(graph_node_type());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Node accessor, const.
	Can remove bounds checks if we're really confident.
 */
const State::node_type&
State::get_node(const node_index_type i) const {
	INVARIANT(i);
	INVARIANT(i < node_pool.size());
	return node_pool[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Node accessor, mutable.  
	Can remove bounds checks if we're really confident.
 */
State::node_type&
State::get_node(const node_index_type i) {
	INVARIANT(i);
	INVARIANT(i < node_pool.size());
	return node_pool[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::check_node(const node_index_type i) const {
	const node_type& n(node_pool[i]);
	// check pull-up/dn if applicable
	const expr_index_type upi = n.pull_up_index;
	if (is_valid_expr_index(upi)) {
		const expr_type& e __ATTRIBUTE_UNUSED_CTOR__((expr_pool[upi]));
		assert(e.is_root());
		assert(e.direction());
		assert(e.parent == i);
	}
	const expr_index_type dni = n.pull_dn_index;
	if (is_valid_expr_index(dni)) {
		const expr_type& e __ATTRIBUTE_UNUSED_CTOR__((expr_pool[dni]));
		assert(e.is_root());
		assert(!e.direction());
		assert(e.parent == i);
	}
	// check fanout
	const size_t fs = n.fanout.size();
	size_t j = 0;
	for ( ; j < fs; ++j) {
		assert(expr_graph_node_pool[n.fanout[j]]
			.contains_node_fanin(i));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Double-checks parent-child relationships.  
	\param i index of the expression, must be < expr_pool.size().  
 */
void
State::check_expr(const expr_index_type i) const {
	const expr_type& e(expr_pool[i]);
	const graph_node_type& g(expr_graph_node_pool[i]);
	// check parent
	if (e.is_root()) {
		const node_type& n
			__ATTRIBUTE_UNUSED_CTOR__((node_pool[e.parent]));
		assert(n.get_pull_expr(e.direction()) == i);
	} else {
		// const Expr& pe(expr_pool[e.parent]);
		const graph_node_type& pg(expr_graph_node_pool[e.parent]);
		const graph_node_type::child_entry_type&
			pc __ATTRIBUTE_UNUSED_CTOR__((pg.children[g.offset]));
		assert(!pc.first);	// this is an expression, not node
		assert(pc.second == i);
	}
	// check children
	assert(e.size == g.children.size());
	size_t j = 0;
	for ( ; j<e.size; ++j) {
		const graph_node_type::child_entry_type& c(g.children[j]);
		if (c.first) {		// points to leaf node
			assert(node_pool[c.second].contains_fanout(i));
		} else {		// points to expression
			assert(expr_pool[c.second].parent == i);
			assert(expr_graph_node_pool[c.second].offset == j);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Like the old prsim's newevent macro.  
	Should be inline only.  
	\param n the reference to the node.
	\param ni the referenced node's index.
	\param val the future value of the node.
	\pre n must not already have a pending event.
	\pre n must be the node corresponding to node index ni
 */
event_index_type
State::__allocate_event(node_type& n,
		const node_index_type ni, const char val) {
	INVARIANT(!n.pending_event());
	n.set_event(event_pool.allocate(event_type(ni, val)));
	return n.get_event();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param n the node associated with the event being freed.  
	\param i the event index to return to the freelist. 
	\pre n's former pending event is event index i
	\pre i is not already deallocated.  (not checked!)
 */
void
State::__deallocate_event(node_type& n, const event_index_type i) {
	INVARIANT(n.get_event() == i);
	n.clear_event();
	event_pool.deallocate(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const State::event_type&
State::get_event(const event_index_type i) const {
	return event_pool[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
State::event_type&
State::get_event(const event_index_type i) {
	return event_pool[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers event in the primary event queue.  
 */
void
State::enqueue_event(const time_type t, const event_index_type ei) {
	INVARIANT(t >= current_time);
#if DEBUG_STEP
	STACKTRACE_INDENT << "enqueuing event ID " << ei <<
		" at time " << t << endl;
#endif
	event_queue.push(event_placeholder_type(t, ei));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers event in the exclusive high event queue.  
	(I think this is an unordered worklist.)
 */
void
State::enqueue_exclhi(const time_type t, const event_index_type ei) {
	INVARIANT(t >= current_time);
#if DEBUG_STEP
	STACKTRACE_INDENT << "enqueuing exclhi ID " << ei <<
		" at time " << t << endl;
#endif
	exclhi_queue.push_back(event_placeholder_type(t, ei));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers event in the exclusive low event queue.  
	(I think this is an unordered worklist.)
 */
void
State::enqueue_excllo(const time_type t, const event_index_type ei) {
	INVARIANT(t >= current_time);
#if DEBUG_STEP
	STACKTRACE_INDENT << "enqueuing excllo ID " << ei <<
		" at time " << t << endl;
#endif
	excllo_queue.push_back(event_placeholder_type(t, ei));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers event in the pending queue.  
	(I think this is an unordered worklist.)
 */
void
State::enqueue_pending(const event_index_type ei) {
#if DEBUG_STEP
	STACKTRACE_INDENT << "enqueuing pending ID " << ei << endl;
#endif
	pending_queue.push_back(ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
State::event_placeholder_type
State::dequeue_event(void) {
	return event_queue.pop();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre There must be at least one event in queue before this is called.  
	\return the scheduled time of the next event.  
 */
State::time_type
State::next_event_time(void) const {
	return event_queue.top().time;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param ni the canonically allocated global index of the bool node.
	\param val the new value to set the node.
	\param t the time at which the event should occur.  
	\return status: 0 is accepted, 1 is warning.  
 */
int
State::set_node_time(const node_index_type ni, const char val,
		const time_type t) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "setting " << get_node_canonical_name(ni) <<
		" to " << node_type::value_to_char[size_t(val)] <<
		" at " << t << endl;
#endif
	// we have ni = the canonically allocated index of the bool node
	// just look it up in the node_pool
	node_type& n(get_node(ni));
	const bool pending = n.pending_event();
	const char last_val = n.current_value();
/***
	If node is in the event queue already and is set to the 
	same value, then issue warning.  
***/
	if (pending && val == last_val) {
		const string objname(get_node_canonical_name(ni));
		cout << "WARNING: ignoring set_node on `" << objname <<
			"\' [interferes with pending event]" << endl;
		return ENQUEUE_WARNING;
	}
// If the value is the same as former value, then ignore it.
// What if delay differs?
	if (val == last_val) { return ENQUEUE_ACCEPT; }
// If node has pending even in queue already, warn and ignore.
	if (pending) {
		const string objname(get_node_canonical_name(ni));
		cout << "WARNING: pending value for node `" << objname <<
			"\'; ignoring request" << endl;
		return ENQUEUE_WARNING;
	}
// otherwise, enqueue the event.  
	const event_index_type ei = __allocate_event(n, ni, val);
#if 0
	const event_type& e(get_event(ei));
	STACKTRACE_INDENT << "new event: (node,val)" << endl;
#endif
	enqueue_event(t, ei);
	return ENQUEUE_ACCEPT;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If this isn't self-documenting enough, I don't know what is.
 */
void
State::set_node_breakpoint(const node_index_type ni) {
	get_node(ni).set_breakpoint();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If this isn't self-documenting enough, I don't know what is.
 */
void
State::clear_node_breakpoint(const node_index_type ni) {
	get_node(ni).clear_breakpoint();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If this isn't self-documenting enough, I don't know what is.
 */
void
State::clear_all_breakpoints(void) {
	for_each(node_pool.begin(), node_pool.end(),
		mem_fun_ref(&node_type::clear_breakpoint));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
State::time_type
State::random_delay(void) {
	typedef	random_time<random_time_limit<time_type>::type>
				random_generator_type;
	return random_generator_type()();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return absolute time of scheduled pull-up event.
	TODO: figure out how to get delay of rule (sparse map?).  
 */
// inline
State::time_type
State::get_delay_up(const event_type& e) const {
	return current_time +
		(timing_mode == TIMING_RANDOM ? random_delay() :
		(timing_mode == TIMING_UNIFORM ? time_type(10) :
		// timing_mode == TIMING_AFTER
			time_type(10)
		));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return absolute time of scheduled pull-down event.
 */
// inline
State::time_type
State::get_delay_dn(const event_type& e) const {
	return current_time +
		(timing_mode == TIMING_RANDOM ? random_delay() :
		(timing_mode == TIMING_UNIFORM ? time_type(10) :
		// timing_mode == TIMING_AFTER
			time_type(10)
		));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	As rules are evaluated and propagated, events may be enqueued
	onto the pending queue.
	This subroutine processes the pending queue events until cleared.  
	Only called by step() member function.  
 */
void
State::flush_pending_queue(void) {
	typedef	pending_queue_type::const_iterator	const_iterator;
#if DEBUG_STEP
	STACKTRACE_VERBOSE;
#endif
	const_iterator i(pending_queue.begin()), e(pending_queue.end());
for ( ; i!=e; ++i) {
	const event_index_type ne = *i;
#if DEBUG_STEP
	STACKTRACE_INDENT << "checking pending event ID: " << ne << endl;
#endif
	event_type& ev(get_event(ne));
	const node_index_type& _ni(ev.node);
#if DEBUG_STEP
	STACKTRACE_INDENT << "... on node " <<
		get_node_canonical_name(_ni) << endl;
#endif
	node_type& _n(get_node(_ni));
	const char pull_up_state = expr_pool[_n.pull_up_index].pull_state();
	const char pull_dn_state = expr_pool[_n.pull_dn_index].pull_state();
#if DEBUG_STEP
	STACKTRACE_INDENT << "current pull-states: up=" <<
		size_t(pull_up_state) << ", dn=" <<
		size_t(pull_dn_state) << endl;
#endif
	if ((pull_up_state != expr_type::PULL_OFF) &&
		(pull_dn_state != expr_type::PULL_OFF)) {
	/***
		There is interference.  If there is weak interference,
		suppress report unless explicitly requested.  
		weak = (X && T) or (T && X);
	***/
#if DEBUG_STEP
		STACKTRACE_INDENT << "some interference." << endl;
#endif
		const bool pending_weak =
			(pull_up_state == expr_type::PULL_WEAK) ||
			(pull_dn_state == expr_type::PULL_WEAK);
			// not XOR (^), see pending_weak table in prs.c
		// issue diagnostic
		if (!(flags & FLAG_NO_WEAK_INTERFERENCE) ||
				!pending_weak) {
			cout << "WARNING: ";
			if (pending_weak)
				cout << "weak-";
			cout << "interference `";
			cout << get_node_canonical_name(_ni) <<
				"\'" << endl;
			// TODO: if (ne->cause)
			//	... caused by ...
		}
		ev.val = node_type::LOGIC_OTHER;
		switch (_n.current_value()) {
		case node_type::LOGIC_LOW:
			enqueue_event(get_delay_dn(ev), ne);
			break;
		case node_type::LOGIC_HIGH:
			enqueue_event(get_delay_up(ev), ne);
			break;
		case node_type::LOGIC_OTHER:
			_n.clear_excl_queue();
			__deallocate_event(_n, ne);
			break;
		default:
			ICE(cerr, 
				cerr << "Invalid logic value." << endl;
			);
		}	// end switch
	} else {
#if DEBUG_STEP
		STACKTRACE_INDENT << "no interference." << endl;
#endif
		if (_n.current_value() != ev.val) {
			if (ev.val == node_type::LOGIC_HIGH) {
				enqueue_event(get_delay_up(ev), ne);
			} else {
				enqueue_event(get_delay_dn(ev), ne);
			}
		} else {
			// no change in value, just cancel
			_n.clear_excl_queue();
			__deallocate_event(_n, ne);
		}	// end switch
	}	// end if may_be_pulled ...
}	// end for all in pending_queue
	pending_queue.clear();	// or .resize(0), same thing
}	// end method flush_pending_queue

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Processes exclusive high ring enqueued events.  
	Place such events into the primary event queue if
		it does not violate any exclusive directives.  
 */
void
State::flush_exclhi_queue(void) {
	typedef	excl_queue_type::const_iterator	const_iterator;
#if DEBUG_STEP
	STACKTRACE_VERBOSE;
#endif
	const_iterator i(exclhi_queue.begin()), e(exclhi_queue.end());
for ( ; i!=e; ++i) {
	const event_placeholder_type& ep(*i);
	const node_index_type epni = get_event(ep.event_index).node;
	node_type& epn(get_node(epni));
	/***
		Look through events: if any of them have a pending
		queue entry, then we're done (?)
	***/
	typedef	excl_ring_map_type::const_iterator ring_iterator;
	ring_iterator ri(exhi.begin()), re(exhi.end());
	for ( ; ri!=re; ++ri) {
		node_index_type ni = *ri;
		size_t prev = 0;
		bool flag = false;
		do {
			const node_type& n(get_node(ni));
			if (n.current_value() == node_type::LOGIC_HIGH ||
				(n.pending_event() && 
				(event_pool[n.get_event()].val ==
					node_type::LOGIC_HIGH) &&
				!n.in_excl_queue())) {
				++prev;
			}
			if (ni == epni) {
				flag = true;
			}
			ni = exhi[ni];
		} while (ni != *ri);
		if (flag && !prev) {
			// then insert event into primary queue
			// keep the same event_index
			enqueue_event(ep.time, ep.event_index);
			epn.clear_excl_queue();
		}
	}	// end for all excl ring nodes
	if (epn.in_excl_queue()) {
		// then violates some excl directive, just cancel the event
		epn.clear_excl_queue();
		__deallocate_event(epn, ep.event_index);
	}
}	// end for all exclhi_queue events
	exclhi_queue.clear();
}	// end method flush_exclhi_queue

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Processes exclusive low ring enqueued events.  
 */
void
State::flush_excllo_queue(void) {
	typedef	excl_queue_type::const_iterator	const_iterator;
#if DEBUG_STEP
	STACKTRACE_VERBOSE;
#endif
	const_iterator i(excllo_queue.begin()), e(excllo_queue.end());
for ( ; i!=e; ++i) {
	const event_placeholder_type& ep(*i);
	const node_index_type epni = get_event(ep.event_index).node;
	node_type& epn(get_node(epni));
	/***
		Look through events: if any of them have a pending
		queue entry, then we're done (?)
	***/
	typedef	excl_ring_map_type::const_iterator ring_iterator;
	ring_iterator ri(exlo.begin()), re(exlo.end());
	for ( ; ri!=re; ++ri) {
		node_index_type ni = *ri;
		size_t prev = 0;
		bool flag = false;
		do {
			const node_type& n(get_node(ni));
			if (n.current_value() == node_type::LOGIC_LOW ||
				(n.pending_event() && 
				(event_pool[n.get_event()].val ==
					node_type::LOGIC_LOW) &&
				!n.in_excl_queue())) {
				++prev;
			}
			if (ni == epni) {
				flag = true;
			}
			ni = exlo[ni];
		} while (ni != *ri);
		if (flag && !prev) {
			// then insert event into primary queue
			// keep the same event_index
			enqueue_event(ep.time, ep.event_index);
			epn.clear_excl_queue();
		}
	}	// end for all excl ring nodes
	if (epn.in_excl_queue()) {
		// then violates some excl directive, just cancel the event
		epn.clear_excl_queue();
		__deallocate_event(epn, ep.event_index);
	}
}	// end for all excllo_queue events
	excllo_queue.clear();
}	// end method flush_excllo_queue

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enforces exclusive high rings by enqueue necessary events 
	into the exclusive high queue.  
 */
void
State::enforce_exclhi(const node_index_type ni) {
	/***
		If n.exclhi and n is 0, check if any of the nodes
		in n's exclhi rings are enabled.  
		If so, insert them into the exclhi-queue.
	***/
	typedef	excl_ring_map_type::const_iterator	const_iterator;
#if DEBUG_STEP
	STACKTRACE_VERBOSE;
#endif
	const_iterator i(exhi.begin()), e(exhi.end());
for ( ; i!=e; ++i) {
	INVARIANT(*i);
	node_index_type eri = exhi[*i];
	// can do without const node_type& er, just compare ni index!
	bool flag = false;
	do {
		if (eri == ni) {
			flag = true;
			break;
		}
		eri = exhi[eri];
	} while (*i != eri);
	if (flag) {
		eri = exhi[*i];
		INVARIANT(eri);
		do {
			node_type& er(get_node(eri));
			if (!er.pending_event() && er.pull_up_index &&
				// er->n->up->val == PRS_VAL_T
				// what if is pulling weakly?
				expr_pool[er.pull_up_index].pull_state()
					== expr_type::PULL_ON) {
				const event_index_type ne =
					__allocate_event(er, eri,
						node_type::LOGIC_HIGH);
				// ne->cause = ni
				enqueue_exclhi(get_delay_up(get_event(ne)), ne);
			}
			eri = exhi[eri];
		} while (*i != eri);
	}	// end if flag
}	// end for (all exclhi members)
}	// end method enforce_exclhi()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enforces exclusive low rings by enqueue necessary events 
	into the exclusive low queue.  
 */
void
State::enforce_excllo(const node_index_type ni) {
	/***
		If n.excllo and n is 1, check if any of the nodes
		in n's excllo rings are enabled.  
		If so, insert them into the excllo-queue.
	***/
	typedef	excl_ring_map_type::const_iterator	const_iterator;
#if DEBUG_STEP
	STACKTRACE_VERBOSE;
#endif
	const_iterator i(exlo.begin()), e(exlo.end());
for ( ; i!=e; ++i) {
	INVARIANT(*i);
	node_index_type eri = exlo[*i];
	// can do without const node_type& er, just compare ni index!
	bool flag = false;
	do {
		if (eri == ni) {
			flag = true;
			break;
		}
		eri = exlo[eri];
	} while (*i != eri);
	if (flag) {
		eri = exlo[*i];
		INVARIANT(eri);
		do {
			node_type& er(get_node(eri));
			if (!er.pending_event() && er.pull_dn_index &&
				// er->n->dn->val == PRS_VAL_T
				// what if is pulling weakly?
				expr_pool[er.pull_dn_index].pull_state()
					== expr_type::PULL_ON) {
				const event_index_type ne =
					__allocate_event(er, eri,
						node_type::LOGIC_LOW);
				// ne->cause = ni
				enqueue_excllo(get_delay_dn(get_event(ne)), ne);
			}
			eri = exlo[eri];
		} while (*i != eri);
	}	// end if flag
}	// end for (all excllo members)
}	// end method enforce_excllo

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Modeled after prs_step() from the original prsim.  
	TODO: possible add arguments later. 
	\pre the exclhi, excllo, and pending queues are empty.
	\post the exclhi, excllo, and pending queues are empty.
	\return index of the affected node, 
		INVALID_NODE_INDEX if nothing happened.  
 */
node_index_type
State::step(void) {
	STACKTRACE_VERBOSE;
	INVARIANT(pending_queue.empty());
	INVARIANT(exclhi_queue.empty());
	INVARIANT(excllo_queue.empty());
	if (event_queue.empty()) {
		return INVALID_NODE_INDEX;
	}
	const event_placeholder_type ep(dequeue_event());
	current_time = ep.time;
	const event_index_type& ei(ep.event_index);
	INVARIANT(ei);
#if DEBUG_STEP
	STACKTRACE_INDENT << "event_index = " << ei << endl;
#endif
	const event_type& pe(get_event(ei));
	const node_index_type ni = pe.node;
#if DEBUG_STEP
	STACKTRACE_INDENT << "examining node: " <<
		get_node_canonical_name(ni) << endl;
#endif
	node_type& n(get_node(ni));
	const char prev = n.current_value();
#if DEBUG_STEP
	STACKTRACE_INDENT << "former value: " <<
		node_type::value_to_char[size_t(prev)] << endl;
	STACKTRACE_INDENT << "new value: " <<
		node_type::value_to_char[size_t(pe.val)] << endl;
#endif
	// comment?
	if (pe.val == node_type::LOGIC_OTHER &&
		prev == node_type::LOGIC_OTHER) {
		// cause propagation?
		// if (cause) *cause = pe->cause;
#if DEBUG_STEP
		STACKTRACE_INDENT << "X: returning node index " << ni << endl;
#endif
		return ni;
	}
	// assert: vacuous firings on the event queue
	assert(prev != pe.val || n.is_unstab());
#if 0
	// more cause propagation debugging statements
#endif
#if 0
	if (cause) {
		*cause = pe->cause;
	}
#endif
	// saved previous value above already
	n.set_value(pe.val);
	__deallocate_event(n, ei);
	const char next = n.current_value();
	// value propagation...
{
	typedef	node_type::const_fanout_iterator	const_iterator;
	const_iterator i(n.fanout.begin()), e(n.fanout.end());
	for ( ; i!=e; ++i) {
		propagate_evaluation(ni, *i, prev, next);
	}
}
	// exclhi ring enforcement (TODO: move into subroutine)
	if (n.has_exclhi() && (next == node_type::LOGIC_LOW)) {
		enforce_exclhi(ni);
	}	// end if (exclhi enforcement)

	// excllo ring enforcement (copy-modified from exclhi code, above)
	if (n.has_excllo() && (next == node_type::LOGIC_HIGH)) {
		enforce_excllo(ni);
	}	// end if (excllo enforcement)

	// energy estimation?  TODO later for a different sim variant
	// transition counts ++

	// check and flush pending queue, spawn fanout events
	flush_pending_queue();

	// check and flush pending queue against exclhi/lo events
	flush_exclhi_queue();
	flush_excllo_queue();

	// return the affected node's index
#if DEBUG_STEP
	STACKTRACE_INDENT << "returning node index " << ni << endl;
#endif
	return ni;
}	// end method step()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main expression evaluation method, ripped off of
	old prsim's propagate_up.  
	\param ni the index of the node causing this propagation (root),
		only used for diagnostic purposes.
	\param ui the index of the sub expression being evaluated, 
		this is already a parent expression of the causing node, 
		unlike original prsim.  
	\param prev the former value of the node/subexpression
	\param next the new value of the node/subexpression
	NOTE: the action table here depends on the expression-type's
		subtype encoding.  For now, we use the Expr's encoding.  
 */
void
State::propagate_evaluation(const node_index_type ni, expr_index_type ui, 
		char prev, char next) {
#if DEBUG_STEP
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT << "node " << ni << " from " <<
		node_type::value_to_char[size_t(prev)] << " -> " <<
		node_type::value_to_char[size_t(next)] << endl;
#endif
	expr_type* u;
do {
	char old_pull, new_pull;	// pulling state of the subexpression
	u = &expr_pool[ui];
#if DEBUG_STEP
	STACKTRACE_INDENT << "examining expression ID: " << ui << endl;
	u->dump_struct(STACKTRACE_INDENT) << endl;
	u->dump_state(STACKTRACE_INDENT << "before: ") << endl;
#endif
	// trust compiler to effectively perform branch-invariant
	// code-motion
	if (u->is_or()) {
		// is disjunctive (or)
		// countdown represents the number of 1's
		old_pull = u->or_pull_state();
		u->unknowns += (next >> 1) - (prev >> 1);
		u->countdown += (next & node_type::LOGIC_VALUE)
			- (prev & node_type::LOGIC_VALUE);
		new_pull = u->or_pull_state();
	} else {
		// is conjunctive (and)
		old_pull = u->and_pull_state();
		// countdown represents the number of 0's
		u->unknowns += (next >> 1) - (prev >> 1);
		u->countdown += !next - !prev;
		new_pull = u->and_pull_state();
	}
#if DEBUG_STEP
	u->dump_state(STACKTRACE_INDENT << "after : ") << endl;
#endif
	if (old_pull == new_pull) {
		// then the pull-state did not change.
#if DEBUG_STEP
		STACKTRACE_INDENT << "end of propagation." << endl;
#endif
		return;
	}
#if 0
	// whether we're inverting the result (e.g. NOT, NAND, NOR)
	if (u->is_not()) {
#if DEBUG_STEP
		STACKTRACE_INDENT << "negated..." << endl;
#endif
		prev = node_type::invert_value[size_t(old_pull)];
		next = node_type::invert_value[size_t(new_pull)];
	} else {
		prev = old_pull;
		next = new_pull;
	}
#else
	// already accounted for negation in pull_state()
	prev = old_pull;
	next = new_pull;
#endif
	ui = u->parent;
} while (!u->is_root());
// propagation made it to the root node, indexed by ui (now node_index_type)
	node_type& n(get_node(ui));
#if DEBUG_STEP
	STACKTRACE_INDENT << "propagated to output node: " <<
		get_node_canonical_name(ui) << " with pull state " <<
		size_t(next) << endl;
#endif
if (u->direction()) {
	// pull-up
if (!n.pending_event()) {
#if DEBUG_STEP
	STACKTRACE_INDENT << "no pending event on this node being pulled up."
		<< endl;
#endif
	// no former event pending, ok to enqueue
	if ((next == expr_type::PULL_ON &&
			n.current_value() != node_type::LOGIC_HIGH) ||
		(next == expr_type::PULL_WEAK &&
			n.current_value() == node_type::LOGIC_LOW)) {
		/***
			if (PULL_ON and wasn't already HIGH ||
				PULL_WEAK and was LOW before)
			then we enqueue the event somewhere.
		***/
#if DEBUG_STEP
		STACKTRACE_INDENT << "pulling up (on or weak)" << endl;
#endif
		const event_index_type pe = __allocate_event(n, ui, next);
		const event_type& e(get_event(pe));
		// pe->cause = root
		if (n.has_exclhi()) {
			// insert into exclhi queue
			enqueue_exclhi(get_delay_up(e), pe);
		} else {
			if (n.pull_dn_index) {
				enqueue_pending(pe);
			} else {
				enqueue_event(get_delay_up(e), pe);
			}
		}
	}
	// "Is this right??" expr_pool[n.pull_dn_index] 
	// might not have been updated yet...
	else if (next == expr_type::PULL_OFF && n.pull_dn_index &&
		// n->dn->val == PRS_VAL_T
		expr_pool[n.pull_dn_index].pull_state() == expr_type::PULL_ON) {
		/***
			if (PULL_OFF and opposing pull-down is ON)
			then enqueue the pull-down event.  
		***/
#if DEBUG_STEP
		STACKTRACE_INDENT <<
			"pull-up turned off, yielding to opposing pull-down."
			<< endl;
#endif
		const event_index_type pe =
			__allocate_event(n, ui, node_type::LOGIC_LOW);
		// pe->cause = root
		if (n.has_excllo()) {
			const event_type& e(get_event(pe));
			enqueue_excllo(get_delay_up(e), pe);
		} else {
			enqueue_pending(pe);
		}
	}
} else if (!n.in_excl_queue()) {
#if DEBUG_STEP
	STACKTRACE_INDENT << "pending, but not excl event on this node."
		<< endl;
#endif
	// there is a pending event, not in the exclusive queue
	event_type& e(get_event(n.get_event()));
	if (next == expr_type::PULL_OFF && n.pull_dn_index &&
		expr_pool[n.pull_dn_index].pull_state() == expr_type::PULL_ON &&
		e.val == node_type::LOGIC_OTHER &&
		n.current_value() != node_type::LOGIC_LOW) {
		/***
			if (PULL_OFF and opposing pull-down is ON and
			the pending event's value is X and
			the current node value is NOT LOW)
			The pending X should be cancelled and replaced
			with a pending LOW (keeping the same time).
		***/
#if DEBUG_STEP
		STACKTRACE_INDENT << "changing pending X to 0 in queue."
			<< endl;
#endif
		e.val = node_type::LOGIC_LOW;
		// e.cause = ni
	} else {
#if DEBUG_STEP
		STACKTRACE_INDENT << "checking for upguard anomaly: guard=" <<
			size_t(next) << ", val=" << size_t(e.val) << endl;
#endif
		// something is amiss!
		const char eu =
			event_type::upguard[size_t(next)][size_t(e.val)];
#if DEBUG_STEP
		STACKTRACE_INDENT << "event_update = " << size_t(eu) << endl;
#endif
		const bool vacuous = eu & event_type::EVENT_VACUOUS;
		if (!vacuous) {
			// then must be unstable or interfering (exclusive)
			const bool instability =
				(eu & event_type::EVENT_UNSTABLE) &&
				!n.is_unstab();
			const bool interference =
				eu & event_type::EVENT_INTERFERENCE;
			const string cause_name(get_node_canonical_name(ni));
			const string out_name(get_node_canonical_name(ui));
			// e.cause = ni
			e.val = node_type::LOGIC_OTHER;
			// diagnostic message
			cout << "WARNING: ";
			if (eu & event_type::EVENT_WEAK)
				cout << "weak-";
			if (interference) {
				cout << "interference `";
			} else if (instability) {
				cout << "unstable `";
			}
			cout << out_name << "\'+" << endl;
			cout << ">> cause: `" << cause_name << "\' (val: ";
			get_node(ni).dump_value(cout) << 	// " -> " <<
			// cout << node_type::value_to_char[size_t(e.val)] <<
				")" << endl;
		}
		// else is OK
	}	// end if diagnostic
}	// end if (!n.ex_queue)
} else {
	// pull-dn
if (!n.pending_event()) {
#if DEBUG_STEP
	STACKTRACE_INDENT << "no pending event on this node being pulled down."
		<< endl;
#endif
	// no former event pending, ok to enqueue
	if ((next == expr_type::PULL_ON &&
			n.current_value() != node_type::LOGIC_LOW) ||
		(next == expr_type::PULL_WEAK &&
			n.current_value() == node_type::LOGIC_HIGH)) {
		/***
			if (PULL_ON and wasn't already LOW ||
				PULL_WEAK and was HIGH before)
			then we enqueue the event somewhere.
		***/
#if DEBUG_STEP
		STACKTRACE_INDENT << "pulling down (on or weak)" << endl;
#endif
		const event_index_type pe = __allocate_event(n, ui,
			node_type::invert_value[size_t(next)]);
		const event_type& e(get_event(pe));
		// pe->cause = root
		if (n.has_excllo()) {
			// insert into exclhi queue
			enqueue_excllo(get_delay_dn(e), pe);
		} else {
			if (n.pull_up_index) {
				enqueue_pending(pe);
			} else {
				enqueue_event(get_delay_dn(e), pe);
			}
		}
	}
	// "Is this right??" expr_pool[n.pull_dn_index] 
	// might not have been updated yet...
	else if (next == expr_type::PULL_OFF && n.pull_up_index &&
		// n->up->val == PRS_VAL_T
		expr_pool[n.pull_up_index].pull_state() == expr_type::PULL_ON) {
		/***
			if (PULL_OFF and opposing pull-up is ON)
			then enqueue the pull-up event.  
		***/
#if DEBUG_STEP
		STACKTRACE_INDENT <<
			"pull-down turned off, yielding to opposing pull-up."
			<< endl;
#endif
		const event_index_type pe =
			__allocate_event(n, ui, node_type::LOGIC_HIGH);
		// pe->cause = root
		if (n.has_exclhi()) {
			const event_type& e(get_event(pe));
			enqueue_exclhi(get_delay_dn(e), pe);
		} else {
			enqueue_pending(pe);
		}
	}
} else if (!n.in_excl_queue()) {
	// there is a pending event, not in an exclusive queue
	event_type& e(get_event(n.get_event()));
	if (next == node_type::LOGIC_LOW && n.pull_up_index &&
		expr_pool[n.pull_up_index].pull_state() == expr_type::PULL_ON &&
		e.val == node_type::LOGIC_OTHER &&
		n.current_value() != node_type::LOGIC_HIGH) {
		/***
			if (PULL_OFF and opposing pull-up is ON and
			the pending event's value is X and
			the current node value is NOT HIGH)
			The pending X should be cancelled and replaced
			with a pending HIGH (keeping the same time).
		***/
#if DEBUG_STEP
		STACKTRACE_INDENT << "changing pending X to 1 in queue."
			<< endl;
#endif
		// there is a pending 'X' in the queue
		e.val = node_type::LOGIC_HIGH;
		// e.cause = ni
	} else {
#if DEBUG_STEP
		STACKTRACE_INDENT << "checking for dnguard anomaly: guard=" <<
			size_t(next) << ", val=" << size_t(e.val) << endl;
#endif
		// something is amiss!
		const char eu =
			event_type::dnguard[size_t(next)][size_t(e.val)];
#if DEBUG_STEP
		STACKTRACE_INDENT << "event_update = " << size_t(eu) << endl;
#endif
		const bool vacuous = eu & event_type::EVENT_VACUOUS;
		if (!vacuous) {
			// then must be unstable or interfering (exclusive)
			const bool instability =
				(eu & event_type::EVENT_UNSTABLE) &&
				!n.is_unstab();
			const bool interference =
				eu & event_type::EVENT_INTERFERENCE;
			const string cause_name(get_node_canonical_name(ni));
			const string out_name(get_node_canonical_name(ui));
			// e.cause = ni
			e.val = node_type::LOGIC_OTHER;
			// diagnostic message
			cout << "WARNING: ";
			if (eu & event_type::EVENT_WEAK)
				cout << "weak-";
			if (interference) {
				cout << "interference `";
			} else if (instability) {
				cout << "unstable `";
			}
			cout << out_name << "\'-" << endl;
			cout << ">> cause: `" << cause_name << "\' (val: ";
			get_node(ni).dump_value(cout) <<	// " -> " <<
			// cout << node_type::value_to_char[size_t(e.val)] <<
				")" << endl;
		}
		// else is OK
	}	// end if diagonstic
}	// end if (!n.ex_queue)
}	// end if (u->direction())
}	// end method propagate_evaluation

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Run until event queue is empty or breakpoint reached.  
	\return null node index if queue is emptied, else
		the ID of the node that tripped a breakpoint.  
 */
node_index_type
State::cycle(void) {
	node_index_type ret;
	while ((ret = step())) {
		if (get_node(ret).is_breakpoint() || stopped())
			break;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a watch point to the indexed node.
	\param ni the index of the node to be watched.  
 */
void
State::watch_node(const node_index_type ni) {
	// this will create an entry if doesn't already exist
	watch_entry& w(watch_list[ni]);
	node_type& n(get_node(ni));
	// remember whether or not this is a breakpoint or a watchpoint
	w.breakpoint = n.is_breakpoint();
	n.set_breakpoint();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Removes node from the watchlist.  
	Q: if node is set as a breakpoint while it is watched, 
		but then unwatched, will it undo the effect of the breakpoint?
 */
void
State::unwatch_node(const node_index_type ni) {
	typedef	watch_list_type::iterator		iterator;
	iterator i(watch_list.find(ni));	// won't add an element
	if (i != watch_list.end()) {
		node_type& n(get_node(ni));
		if (i->second.breakpoint) {
			n.set_breakpoint();
		} else {
			n.clear_breakpoint();
		}
		watch_list.erase(i);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
State::is_watching_node(const node_index_type ni) const {
	return (watch_list.find(ni) != watch_list.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Clears the watch-list, restoring nodes' former breakpoint states.  
 */
void
State::unwatch_all_nodes(void) {
	typedef	watch_list_type::const_iterator		const_iterator;
	const_iterator i(watch_list.begin()), e(watch_list.end());
	for (; i!=e; ++i) {
		node_type& n(get_node(i->first));
		if (i->second.breakpoint) {
			n.set_breakpoint();
		} else {
			n.clear_breakpoint();
		}
	}
	watch_list.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param val node_type::LOGIC_{LOW,HIGH,OTHER}.  
 */
ostream&
State::status_nodes(ostream& o, const char val) const {
	INVARIANT(node_type::is_valid_value(val));
	const size_t ns = node_pool.size();
	size_t i = INVALID_NODE_INDEX +1;
	o << node_type::value_to_char[size_t(val)] << " nodes:" << endl;
	for ( ; i<ns; ++i) {
		if (node_pool[i].current_value() == val) {
			o << get_node_canonical_name(i) << ' ';
		}
	}
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Structural assertions.  
	TODO: run-time flag to enable/disable calls to this.  
 */
void
State::check_structure(void) const {
	STACKTRACE_VERBOSE;
{
	const expr_index_type exprs = expr_pool.size();
	INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_EXPR;
	for ( ; i<exprs; ++i) {
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "checking Expr " << i << ":" << endl;
#endif
		check_expr(i);
	}
}
{
	const node_index_type nodes = node_pool.size();
	node_index_type j = FIRST_VALID_NODE;
	for ( ; j<nodes; ++j) {
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "checking Node " << j << ":" << endl;
#endif
		check_node(j);
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i global node index.  
	\return string of the canonical node name.  
 */
string
State::get_node_canonical_name(const node_index_type i) const {
	INVARIANT(i);
	INVARIANT(i < node_pool.size());
	const state_manager& sm(mod.get_state_manager());
	const entity::footprint& topfp(mod.get_footprint());
	const global_entry_pool<bool_tag>& bp(sm.get_pool<bool_tag>());
	ostringstream oss;
	bp[i].dump_canonical_name(oss, topfp, sm);
	return oss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_struct(ostream& o) const {
{
	o << "Nodes: " << endl;
	const state_manager& sm(mod.get_state_manager());
	const entity::footprint& topfp(mod.get_footprint());
	const global_entry_pool<bool_tag>& bp(sm.get_pool<bool_tag>());
	const node_index_type nodes = node_pool.size();
	node_index_type i = FIRST_VALID_NODE;
	for ( ; i<nodes; ++i) {
		o << "node[" << i << "]: \"";
		bp[i].dump_canonical_name(o, topfp, sm);
		node_pool[i].dump_struct(o << "\" ") << endl;
	}
}
{
	o << "Expressions: " << endl;
	const expr_index_type exprs = expr_pool.size();
	INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_EXPR;
	for ( ; i<exprs; ++i) {
		expr_pool[i].dump_struct(o << "expr[" << i << "]: ") << endl;
		expr_graph_node_pool[i].dump_struct(o << '\t') << endl;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ostream&
State::dump_state(ostream& o) const {
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This prints out the netlist of nodes and expressions
	in dot-form for visualization.  
 */
ostream&
State::dump_struct_dot(ostream& o) const {
	o << "digraph G {" << endl;
{
	const state_manager& sm(mod.get_state_manager());
	const entity::footprint& topfp(mod.get_footprint());
	const global_entry_pool<bool_tag>& bp(sm.get_pool<bool_tag>());
	o << "# nodes: " << endl;
	// box or plaintext
	o << "node [shape=box, fillcolor=white];" << endl;
	const node_index_type nodes = node_pool.size();
	node_index_type i = FIRST_VALID_NODE;
	for ( ; i<nodes; ++i) {
		ostringstream oss;
		oss << "NODE_" << i;
		const string& s(oss.str());
		o << s;
		bp[i].dump_canonical_name(o << "\t[label=\"", topfp, sm)
			<< "\"];" << endl;
		node_pool[i].dump_fanout_dot(o, s) << endl;
	}
}
{
	o << "# Expressions: " << endl;
	const expr_index_type exprs = expr_pool.size();
	INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_EXPR;
	for ( ; i<exprs; ++i) {
		o << "EXPR_" << i << "\t[label=\"" << i << "\", shape=";
		const expr_type& e(expr_pool[i]);
		e.dump_type_dot_shape(o) << "];" << endl;
		e.dump_parent_dot_edge(o << "EXPR_" << i << " -> ")
			<< ';'<< endl;
	}
}
	o << "}" << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the event queue.  
 */
ostream&
State::dump_event_queue(ostream& o) const {
	typedef	vector<event_queue_type::value_type>	temp_type;
	typedef	temp_type::const_iterator		const_iterator;
	temp_type temp;
	event_queue.copy_to(temp);
	const_iterator i(temp.begin()), e(temp.end());
	o << "event queue:" << endl;
	for ( ; i!=e; ++i) {
		o << '\t' << i->time << '\t';
		const event_type& ev(get_event(i->event_index));
		o << get_node_canonical_name(ev.node) << " : " <<
			node_type::value_to_char[ev.val] << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: this uses the node's canonical name.  
 */
ostream&
State::dump_node_value(ostream& o, const node_index_type ni) const {
	const node_type& n(get_node(ni));
	n.dump_value(o << get_node_canonical_name(ni) << " : ");
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out nodes affected by the node argument.  
	Note: this only requires structural information, no stateful info.  
	TODO: check for replications.  
	TODO: including all OR combinations, or just the paths that
		include this node conjunctively?
 */
ostream&
State::dump_node_fanout(ostream& o, const node_index_type ni) const {
	typedef	node_type::fanout_array_type	fanout_array_type;
	typedef	fanout_array_type::const_iterator	const_iterator;
#if DEBUG_FANOUT
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT << "ni = " << ni << endl;
#endif
	const node_type& n(get_node(ni));
	const fanout_array_type& foa(n.fanout);
	const_iterator fi(foa.begin()), fe(foa.end());
	for ( ; fi!=fe; ++fi) {
		// for each leaf expression in the fanout list, 
		// trace up the propagation path to find the affected node.
		expr_index_type ei = *fi;
		const expr_type* e = &expr_pool[ei];
		while (!e->is_root()) {
#if DEBUG_FANOUT
			STACKTRACE_INDENT << "ei = " << ei << endl;
#endif
			ei = e->parent;
			e = &expr_pool[ei];
		}
#if DEBUG_FANOUT
		STACKTRACE_INDENT << "ei = " << ei << endl;
#endif
		// ei is an index to the expression whose parent is *node*.
		const node_index_type nr = e->parent;
		// nr is an index to the root *node*.
#if DEBUG_FANOUT
		STACKTRACE_INDENT << "nr = " << nr << endl;
#endif
		const node_type& no(get_node(nr));
		// track the direction of propagation (pull-up/dn)
		const bool dir = e->direction();
		// then print the entire fanin rule for that node, 
		const expr_index_type pi =
			(dir ? no.pull_up_index : no.pull_dn_index);
#if DEBUG_FANOUT
		STACKTRACE_INDENT << "pi = " << pi << endl;
#endif
		dump_subexpr(o, pi) << " -> ";
		o << get_node_canonical_name(nr) << (dir ? '+' : '-') << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: OR-combinations on separate lines?
 */
ostream&
State::dump_node_fanin(ostream& o, const node_index_type ni) const {
	const node_type& n(get_node(ni));
	const string cn(get_node_canonical_name(ni));
	if (n.pull_up_index) {
		dump_subexpr(o, n.pull_up_index) << " -> " << cn << '+' << endl;
	}
	if (n.pull_dn_index) {
		dump_subexpr(o, n.pull_dn_index) << " -> " << cn << '-' << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive expression printer.  
	Should be modeled after cflat's expression printer.  
 */
ostream&
State::dump_subexpr(ostream& o, const expr_index_type ei, 
		const char ptype) const {
#if DEBUG_FANOUT
	STACKTRACE_VERBOSE;
#endif
	INVARIANT(ei);
	INVARIANT(ei < expr_pool.size());
	const expr_type& e(expr_pool[ei]);
	const graph_node_type& g(expr_graph_node_pool[ei]);
	// can elaborate more on when parens are needed
	const bool need_parens = e.parenthesize(ptype);
	const char _type = e.to_prs_enum();
	if (e.is_not()) {
		o << '~';
	}
	const char* op = e.is_or() ? " | " : " & ";
	typedef	graph_node_type::const_iterator		const_iterator;
	const_iterator ci(g.begin()), ce(g.end());
	if (need_parens) {
		o << '(';
	}
	// peel out first iteration for infix printing
	if (ci->first) {
		o << get_node_canonical_name(ci->second);
	} else {
		dump_subexpr(o, ci->second, _type);
	}
	if (g.children.size() >= 1) {
	for (++ci; ci!=ce; ++ci) {
		o << op;
		if (ci->first) {
			o << get_node_canonical_name(ci->second);
		} else {
			dump_subexpr(o, ci->second, _type);
		}
	}
	}
	if (need_parens) {
		o << ')';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_source_paths(ostream& o) const {
	o << "source paths:" << endl;
	return ifstreams.dump_paths(o);
}

//=============================================================================
// class State::signal_handler method definitions

/**
	Global static initializer for handler's bound State reference.  
 */
State*
State::signal_handler::_state = NULL;

#if 0
void
(*State::signal_handler::_current_handler)(int) = NULL;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Preserves the current State* and handler for restoration.  
	Swaps the current signal handler out for this one.  
 */
State::signal_handler::signal_handler(State* s) :
		_prev(_state), _main(signal(SIGINT, main)) {
	_state = s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Upon destruction, restores the former signal handler.  
	Swaps the former signal handler back in.  
 */
State::signal_handler::~signal_handler() {
	_state = _prev;
	signal(SIGINT, _main);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::signal_handler::main(int sig) {
	if (_state) {
		_state->stop();
	}
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

