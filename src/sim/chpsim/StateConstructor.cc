/**
	\file "sim/chpsim/StateConstructor.cc"
	$Id: StateConstructor.cc,v 1.1.2.4 2006/12/14 23:43:27 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <vector>
#include "sim/chpsim/StateConstructor.h"
#include "sim/chpsim/Event.h"
#include "Object/module.h"
#include "Object/global_entry.tcc"
#include "Object/lang/CHP_footprint.h"
#include "util/visitor_functor.h"
#include "util/stacktrace.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using std::vector;
using entity::process_tag;
using entity::global_entry_pool;

//=============================================================================
// class StateConstructor method definitions

StateConstructor::StateConstructor(State& s) : 
		state(s), 
		// last_event_indices(), 
		last_event_index(0), 
		current_process_index(0)	// top-level
		{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StateConstructor::~StateConstructor() { }

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
/**
	This updates the new event's list of successors using the 
	current list of successor events.  (Backwards construction.)
	If last_event_index is 0 (null), then don't bother.  
 */
void
StateConstructor::connect_successor_events(event_type& ev) const {
	// STACKTRACE_VERBOSE;
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

