/**
	\file "sim/chpsim/StateConstructor.cc"
	$Id: StateConstructor.cc,v 1.1.2.5 2007/01/18 12:45:50 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <vector>
#include "sim/chpsim/StateConstructor.h"
#include "sim/chpsim/State.h"
#include "sim/chpsim/Event.h"
#include "Object/module.h"
#include "Object/global_entry.tcc"
#include "Object/lang/CHP_footprint.h"
#include "util/visitor_functor.h"
#include "util/stacktrace.h"
#include "util/memory/free_list.h"
#include "util/STL/valarray_iterator.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using std::begin;
using std::end;
using std::vector;
using std::endl;
using entity::process_tag;
using entity::global_entry_pool;
using util::memory::free_list_acquire;
using util::memory::free_list_release;

//=============================================================================
// class StateConstructor method definitions

StateConstructor::StateConstructor(State& s) : 
		state(s), 
		free_list(), 
		// last_event_indices(), 
		last_event_index(0), 
		current_process_index(0)	// top-level
		{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Some clean-up to avoid printing dead nodes.  
 */
StateConstructor::~StateConstructor() {
	typedef free_list_type::const_iterator	const_iterator;
	const_iterator i(free_list.begin()), e(free_list.end());
	for ( ; i!=e; ++i) {
		get_event(*i).orphan();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Clears the last_event_index.
	Call this before visiting each process.  
 */
void
StateConstructor::reset(void) {
	last_event_index = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Copy-modified from entity::cflat_visitor::visit()
	top-level CHP is already visited by the caller
	in CHPSIM::State::State().  
 */
void
StateConstructor::visit(const state_manager& _sm) {
	STACKTRACE_VERBOSE;
	node_index_type pid = 1;	// 0-indexed, but 0th entry is null
	// const global_entry_pool<process_tag>& proc_entry_pool(sm);
	const global_entry_pool<process_tag>&
		proc_entry_pool(_sm.get_pool<process_tag>());
	// Could re-write in terms of begin() and end() iterators.  
	const node_index_type plim = proc_entry_pool.size();
	initial_events.reserve(plim +1);	// +1 for top-level
	for ( ; pid < plim; ++pid) {
		// visit CHP instead
		reset();
		current_process_index = pid;
		entity::CHP_substructure<true>::accept(
			proc_entry_pool[pid], *this);
		// TODO: take root last_event_index and add it to
		// the State's list of ready events (how simulation begins)
		if (last_event_index) {
			initial_events.push_back(last_event_index);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const state_manager&
StateConstructor::get_state_manager(void) const {
	return state.get_module().get_state_manager();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference to current's processes footprint or
		the top-level footprint (if index is null)
 */
const entity::footprint&
StateConstructor::get_process_footprint(void) const {
	const module& m(state.get_module());
	return current_process_index ?
		*m.get_state_manager()
			.get_pool<process_tag>()[current_process_index]
			._frame._footprint
		: m.get_footprint();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StateConstructor::event_type&
StateConstructor::get_event(const event_index_type ei) {
	return state.event_pool[ei];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const StateConstructor::event_type&
StateConstructor::get_event(const event_index_type ei) const {
	return state.get_event(ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
StateConstructor::event_pool_size(void) const {
	return state.event_pool.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates a new event, initializing to the argument.  
	Also recycles free-slots if available.  
 */
event_index_type
StateConstructor::allocate_event(const event_type& e) {
	if (free_list.empty()) {
		const event_index_type ret = state.event_pool.size();
		state.event_pool.push_back(e);
		return ret;
	} else {
		// take first available index
		const event_index_type ret = free_list_acquire(free_list);
		state.event_pool[ret] = e;
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recycles an event slot for recycling for a subsequent allocation
	request.  
 */
void
StateConstructor::deallocate_event(const event_index_type ei) {
	// does invariant checking
	if (ei == event_pool_size() -1) {
		// if it happens to be the back entry, just pop it
		state.event_pool.pop_back();
	} else {
		// else remember it in free-list
		free_list_release(free_list, ei);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Performs an event substitution from @f to @f's only successor.  
	\pre this event @f must have a lone successor, and be trivial/null.
	\param f the index of the event to forward through.  
	\param h the hint to the head event from which reachable events 
		should be scanned and replaced.  
		This should be the sole successor to event f.
	\return the lone successor used to replace successors.
 */
event_index_type
StateConstructor::forward_successor(const event_index_type f) {
	STACKTRACE_VERBOSE;
	const event_type& skip_me(get_event(f));
	INVARIANT(skip_me.is_dispensible());	// redundant checks...
//	const size_t succs = skip_me.successor_events.size();
	const event_index_type replacement = skip_me.successor_events[0];
	const event_type& r(get_event(replacement));
	INVARIANT(r.get_predecessors() < 2);	// 1 or 0
	forward_successor(f, replacement, replacement);	// wrapped call
	return replacement;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Variation where the replacement is specified explicitly by caller.  
	\param f the index of the event to forward through.  
	\param replacement the index to replace.
	\param h the head event to start searching and replacing from.
 */
void
StateConstructor::forward_successor(const event_index_type f, 
		const event_index_type replacement, 
		const event_index_type h) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("f,r,h = " << f << ", " <<
		replacement << ", " << h << endl);
	const event_type& skip_me(get_event(f));
	INVARIANT(skip_me.is_dispensible());	// redundant checks...

	// collect event nodes reachable from the head (post-dominate?)
	// using worklist algorithm
	typedef	std::set<event_index_type>	reachable_set_type;
	reachable_set_type visited_set;		// covered
	reachable_set_type worklist;		// to-do list
	visited_set.insert(f);			// don't check f again
	worklist.insert(h);
	while (!worklist.empty()) {
	const event_index_type ei = free_list_acquire(worklist);
	if (visited_set.find(ei) == visited_set.end()) {
		STACKTRACE_INDENT_PRINT("worklist: " << ei << endl);
		event_type::successor_list_type&
			s(get_event(ei).successor_events);
		STACKTRACE_INDENT_PRINT("size: " << s.size() << endl);
		event_index_type* si = begin(s);
		event_index_type* se = end(s);
		// algorithm: find and replace
		// non-matches should be thrown into work-list
		for ( ; si!=se; ++si) {
			if (*si == f) {
		STACKTRACE_INDENT_PRINT("  replace: " << *si << endl);
				*si = replacement;
			} else {
		STACKTRACE_INDENT_PRINT("  enqueue: " << ei << endl);
				worklist.insert(*si);
			}
		}
		visited_set.insert(ei);	// mark as finished
	}	// end if
	// else already processed, move on
	}	// end while
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This updates the new event's list of successors using the 
	current list of successor events.  (Backwards construction.)
	If last_event_index is 0 (null), then don't bother.  
 */
void
StateConstructor::connect_successor_events(event_type& ev) const {
	STACKTRACE_VERBOSE;
#if 0
	typedef	return_indices_type::const_iterator	ret_iterator;
	const ret_iterator 
		b(last_event_indices.begin()), 
		e(last_event_indices.end());
	ev.successor_events.resize(last_event_indices.size());
	copy(b, e, &ev.successor_events[0]);
#else
	if (last_event_index) {
		ev.successor_events.resize(1);
		ev.successor_events[0] = last_event_index;
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For use with unique predecessors, such as in sequences.  
	TODO: rename this bad boy...
	\pre before this routine, caller has set the last_event_indices
		to correspond to the newly constructed event(s).
		For basic atomic events, it is a single event, 
		while for concurrent events, it will be more than one.
		Selection statements will also have *one* predecessor.  
	This updates the predecessor count for all successor events.  
 */
void
StateConstructor::count_predecessors(const event_type& ev) const {
	STACKTRACE_VERBOSE;
	typedef	event_type::event_index_type	event_index_type;
	// poor man's valarray iterator
	const event_index_type* i = &ev.successor_events[0], 
		*e = &ev.successor_events[ev.successor_events.size()];
	// const size_t n_pred = last_event_indices.size();
	for ( ; i!=e; ++i) {
		// state.event_pool[*i].set_predecessors(n_pred);
		state.event_pool[*i].set_predecessors(1);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
StateConstructor::visit(const entity::PRS::footprint& f) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
StateConstructor::visit(const footprint_rule&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
StateConstructor::visit(const footprint_expr_node&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
StateConstructor::visit(const footprint_macro&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
StateConstructor::visit(const entity::SPEC::footprint_directive&) { }

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

