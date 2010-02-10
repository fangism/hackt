/**
	\file "sim/chpsim/StateConstructor.cc"
	$Id: StateConstructor.cc,v 1.7.46.2 2010/02/10 06:43:15 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <vector>
#include "sim/chpsim/StateConstructor.h"
#include "sim/chpsim/DependenceCollector.h"
#include "sim/chpsim/State.h"
#include "sim/chpsim/Event.h"
#include "Object/module.h"
#include "Object/global_entry.tcc"
#include "Object/lang/CHP.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/bool_expr.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
// #include "Object/traits/chan_traits.h"
#include "common/ICE.h"
#include "common/TODO.h"
// #include "util/visitor_functor.h"
#include "util/stacktrace.h"
// #include "util/STL/valarray_iterator.h"

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
// TEMPORARY
namespace HAC {
namespace SIM {
namespace CHPSIM {
using std::vector;
using entity::process_tag;
using entity::global_entry_pool;
using entity::preal_const;
using entity::CHP::delay_ptr_type;
#include "util/using_ostream.h"

//=============================================================================
// class StateConstructor method definitions

StateConstructor::StateConstructor(
		const State& s, event_type& e) : 
		state(s),
		event(e),
		current_process_index(state.get_process_id(event)) {
	STACKTRACE_INDENT_PRINT("pid = " << current_process_index << endl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Some clean-up to avoid printing dead nodes.  
 */
StateConstructor::~StateConstructor() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
StateConstructor::visit(const action_sequence& l) {
	STACKTRACE_VERBOSE;
	// no-op
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
StateConstructor::visit(const concurrent_actions& ca) {
	STACKTRACE_VERBOSE;
	switch (event.get_event_type()) {
	case entity::CHP::EVENT_CONCURRENT_JOIN:
		event.set_delay(0);
		break;
	case entity::CHP::EVENT_CONCURRENT_FORK: {
		const delay_ptr_type& d(ca.get_delay());
		event.set_delay(d ?
			d.is_a<const preal_const>()->static_constant_value() :
			1);	// small delay
		}
		break;
	default:
		DIE;
	}
}	// end visit(const concurrent_actions&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
StateConstructor::visit(const guarded_action& s) {
	STACKTRACE_VERBOSE;
	// no-op
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Almost exact same code for non-deterministic selection.
 */
void
StateConstructor::visit(const deterministic_selection& ds) {
	STACKTRACE_VERBOSE;
	const size_t branches = ds.size();
	switch (event.get_event_type()) {
	case entity::CHP::EVENT_SELECTION_BEGIN: {
		const delay_ptr_type& d(ds.get_delay());
		event.set_delay(d ?
			d.is_a<const preal_const>()->static_constant_value() :
			1);
		}
		break;
	case entity::CHP::EVENT_SELECTION_END:
		event.set_delay(0);
		break;
	default: DIE;
	}
	deterministic_selection::const_iterator i(ds.begin()), e(ds.end());
	SIM::CHPSIM::DependenceSetCollector deps(*this);	// args
	vector<size_t> tmp;
	tmp.reserve(branches);
	// construct concurrent chains
	for ( ; i!=e; ++i) {
		const guarded_action::guard_ptr_type& g((*i)->get_guard());
		if (g) {
			g->accept(deps);
		} else {
			// is else clause, don't need any guard dependencies!
			deps.clear();
			// TODO: check terminating clause *first*
			// before bothering...
		}
	}
	event.import_block_dependencies(deps);
}	// end visit(const deterministic_selection&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Code ripped from deterministic_selection::accept().
 */
void
StateConstructor::visit(const nondeterministic_selection& ns) {
	STACKTRACE_VERBOSE;
	// TODO: run-time check for guard exclusion
	const size_t branches = ns.size();
	switch (event.get_event_type()) {
	case entity::CHP::EVENT_SELECTION_BEGIN: {
		const delay_ptr_type& d(ns.get_delay());
		event.set_delay(d ?
			d.is_a<const preal_const>()->static_constant_value() :
			15);
		}
		break;
	case entity::CHP::EVENT_SELECTION_END:
		event.set_delay(0);
		break;
	default: DIE;
	}

	nondeterministic_selection::const_iterator i(ns.begin()), e(ns.end());
	SIM::CHPSIM::DependenceSetCollector deps(*this);	// args
	vector<size_t> tmp;
	tmp.reserve(branches);
	// construct concurrent chains
	for ( ; i!=e; ++i) {
		const guarded_action::guard_ptr_type& g((*i)->get_guard());
		if (g) {
			g->accept(deps);
		} else {
			// is else clause, don't need any guard dependencies!
			deps.clear();
			// TODO: check terminating clause *first*
			// before bothering...
		}
	}
	event.import_block_dependencies(deps);
}	// end visit(const nondeterminstic_selection&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
StateConstructor::visit(const metaloop_selection&) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
StateConstructor::visit(const metaloop_statement&) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
StateConstructor::visit(const assignment& a) {
	STACKTRACE_VERBOSE;
{
	const delay_ptr_type& d(a.get_delay());
	event.set_delay(d ?
		d.is_a<const preal_const>()->static_constant_value() :
		10);
}
#if CHPSIM_READ_WRITE_DEPENDENCIES
{
	SIM::CHPSIM::ReadDependenceSetCollector rdeps(*this);	// rvalues
	SIM::CHPSIM::WriteDependenceSetCollector wdeps(*this);	// lvalues
	rval->accept(rdeps);
	lval->accept(wdeps);
	new_event.import_read_dependencies(rdeps);
	new_event.import_write_dependencies(wdeps);
}
#endif
}	// end visit(const assignment&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Has no block dependencies.
 */
void
StateConstructor::visit(const function_call_stmt& fc) {
	STACKTRACE_VERBOSE;
{
	const delay_ptr_type& d(fc.get_delay());
	event.set_delay(d ?
		d.is_a<const preal_const>()->static_constant_value() :
		5);
}
#if CHPSIM_READ_WRITE_DEPENDENCIES
	// collect rvalues, no lvalues
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: alternative: fuse this event with successor if single.  
		rationale: every CHPSIM event is "guarded"
	TODO: what if several conditional waits occur in succession?
		Take conjunction or sequential evaluation
		using auxiliary null events?
 */
void
StateConstructor::visit(const condition_wait& cw) {
	STACKTRACE_VERBOSE;
{
	const delay_ptr_type& d(cw.get_delay());
	event.set_delay(d ?
		d.is_a<const preal_const>()->static_constant_value() :
		0);
}
	if (cw.get_guard()) {
		SIM::CHPSIM::DependenceSetCollector deps(*this);
		cw.get_guard()->accept(deps);
		event.import_block_dependencies(deps);
	}
}	// end visit(const condition_wait&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
StateConstructor::visit(const channel_send& cs) {
	STACKTRACE_VERBOSE;
{
	const delay_ptr_type& d(cs.get_delay());
	event.set_delay(d ?
		d.is_a<const preal_const>()->static_constant_value() :
		2);
}
{
	// can block on channel, so we add dependencies
	SIM::CHPSIM::DependenceSetCollector deps(*this);
	cs.get_chan()->accept(deps);
	event.import_block_dependencies(deps);
}
}	// end visit(const channel_send&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
StateConstructor::visit(const channel_receive& cr) {
	STACKTRACE_VERBOSE;
{
	const delay_ptr_type& d(cr.get_delay());
	event.set_delay(d ?
		d.is_a<const preal_const>()->static_constant_value() :
		(cr.is_peek() ? 3 : 5));
}
{
	// receive can block on channel, so we add dependencies
	// channel peeks can also block
	SIM::CHPSIM::DependenceSetCollector deps(*this);
	cr.get_chan()->accept(deps);
	event.import_block_dependencies(deps);
}
}	// end visit(const channel_receive&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: nothing can follow a do-forever loop, 
	so we need not worry about an initial successor.  
	However, there may be entries into an infinite loop, so we
	must return the index to the first event in the loop.  
	TODO: optimization: overwrite the loopback null event slot
		if the event is trivial -- may result in self-reference, OK.
 */
void
StateConstructor::visit(const do_forever_loop& fl) {
	STACKTRACE_VERBOSE;
// NOTE: the loopback event is always eliminated by event-graph optimization
// thus there is no need to set any delays here
//	event.set_delay(0);
}	// end visit(const do_forever_loop&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Code ripped from do_forever_loop::accept().
	Need to synthesize a deterministic selection with an exit branch.
	Semantics: never blocking, as there is an implicit else-clause
	that skips past the loop.  
	Reminder: guards cannot include an explicit else clause.  
 */
void
StateConstructor::visit(const do_while_loop& dw) {
	STACKTRACE_VERBOSE;
{
	const delay_ptr_type& d(dw.get_delay());
	event.set_delay(d ?
		d.is_a<const preal_const>()->static_constant_value() :
		((event.get_event_type() ==
			entity::CHP::EVENT_SELECTION_BEGIN) ? 3 : 0));
}
}	// end visit(const do_while_loop&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
const state_manager&
StateConstructor::get_state_manager(void) const {
	return state.get_module().get_state_manager();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference to current's processes footprint or
		the top-level footprint (if index is null)
 */
const entity::footprint&
StateConstructor::get_process_footprint(void) const {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	FINISH_ME_EXIT(Fang);
	return state.get_module().get_footprint();
#else
	const module& m(state.get_module());
	return current_process_index ?
		*m.get_state_manager()
			.get_pool<process_tag>()[current_process_index]
			._frame._footprint
		: m.get_footprint();
#endif
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC
#endif

