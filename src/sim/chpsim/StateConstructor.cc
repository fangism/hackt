/**
	\file "sim/chpsim/StateConstructor.cc"
	$Id: StateConstructor.cc,v 1.2.8.1 2007/03/10 02:52:05 fang Exp $
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
#include "Object/lang/CHP_footprint.h"
#include "Object/lang/CHP.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/bool_expr.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/traits/chan_traits.h"
#include "common/ICE.h"
#include "util/visitor_functor.h"
#include "util/stacktrace.h"
#include "util/memory/free_list.h"
#include "util/STL/valarray_iterator.h"

/**
	Various levels of chpsim event generation optimizations, 
	such as fusion.  
	TODO: control in execute-time switch.  
 */
#define	OPTIMIZE_CHPSIM_EVENTS			1

namespace HAC {
namespace SIM {
namespace CHPSIM {
using std::begin;
using std::end;
using std::vector;
using std::endl;
using entity::process_tag;
using entity::global_entry_pool;
using entity::preal_const;
using util::memory::free_list_acquire;
using util::memory::free_list_release;
#include "util/using_ostream.h"

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
#if 1
/**
	Constructs a sequence of events backwards from back to
	front, where each event 'connects' to its successor(s).
	NOTE: no delay addition anywhere here
 */
void
StateConstructor::visit(const action_sequence& l) {
	STACKTRACE_VERBOSE;
//	for_each(rbegin(), rend(), util::visitor_ptr(s));
	action_sequence::const_iterator i(l.begin()), e(l.end());
	INVARIANT(i!=e);	// else would be empty
	do {
		--e;
		(*e)->accept(*this);
		// events will link themselves (callee responsible)
	} while (i!=e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Do we need to construct event successor edges and graphs?
	NOTE: this does not actually allocate an event for itself.  
	Constructs events in a backwards order to simplify event-chaining
	of predecessors to successors.  
 */
void
action_sequence::accept(StateConstructor& s) const {
	accept_sequence(*this, s);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Fork and join graph structure.  
	TODO: this dependencies for this event is the union of 
		all dependencies of the respective guards.  
		If there is an else clause, we may omit dependencies, 
		as a branch is always taken immediately.
		Will we ever check for guard stability?
 */
void
StateConstructor::visit(const concurrent_actions& ca) {
	// TODO: using footprint frame, allocate event edge graph
	// there will be multiple outgoing edges
	STACKTRACE_VERBOSE;
	const size_t branches = ca.size();
	STACKTRACE_INDENT_PRINT("branches: " << branches << endl);
// check for degenerate cases first: 0, 1
// these can arise from meta-expansions
if (!branches) {
	return;
} else if (branches == 1) {
	// don't bother forking and joining
	ca.front()->accept(*this);
	return;
}
// else do the normal thing
	// create a join event first (bottom-up)
	const size_t join_index = allocate_event(
		EventNode(&ca, SIM::CHPSIM::EVENT_CONCURRENT_JOIN, 
			current_process_index, 0));
	// no additional delay given
{
	STACKTRACE_INDENT_PRINT("join index: " << join_index << endl);
	// join shouldn't need an action ptr (unless we want back-reference)
	EventNode& join_event(get_event(join_index));
	connect_successor_events(join_event);
	join_event.set_predecessors(branches);	// expect number of branches
	// reminder, reference may be invalidated after push_back
}
	// NOTE: no dependencies to track, unblocked concurrency
	concurrent_actions::const_iterator i(ca.begin()), e(ca.end());
	vector<size_t> tmp;
	tmp.reserve(branches);
	// construct concurrent chains
	for ( ; i!=e; ++i) {
		last_event_index = join_index;	// pass down
		(*i)->accept(*this);
		tmp.push_back(last_event_index);	// head of each chain
	}
{
	// have to set it again!?  must get clobbered by above loop...
	EventNode& join_event(get_event(join_index));
	join_event.set_predecessors(branches);
	count_predecessors(join_event);
}

	// construct successor event graph edge? or caller's responsibility?
	const size_t fork_index = allocate_event(
		EventNode(&ca, SIM::CHPSIM::EVENT_CONCURRENT_FORK, 
			current_process_index, 
#if CHP_ACTION_DELAYS
			// assert dynamic_cast
			ca.get_delay() ?
				ca.get_delay().is_a<const preal_const>()
				->static_constant_value() :
#endif
			1));	// small delay
{
	STACKTRACE_INDENT_PRINT("fork index: " << fork_index << endl);
	EventNode& fork_event(get_event(fork_index));

	fork_event.successor_events.resize(branches);
	copy(tmp.begin(), tmp.end(), &fork_event.successor_events[0]);

	// updates successors' predecessor-counts
	count_predecessors(fork_event);
}
	// leave trail of this event for predecessor
	last_event_index = fork_index;
	// construct an event join graph-node?
}	// end visit(const concurrent_actions&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre a slection-merge event is pointed to by s.last_event_index.
	Plan: construct guards in all branches first
	(guarded actions), then move them into a union.  
	It is the selection' (caller) responsibility to collect
		guard-dependencies for subscription sets.  
		The dependencies for the guarded action are computed
		by the action sequence.  
 */
void
StateConstructor::visit(const guarded_action& s) {
	STACKTRACE_VERBOSE;
	if (s.get_action()) {
		s.get_action()->accept(*this);
	// it is the selection's responsibility to evaluate the guards
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Almost exact same code for non-deterministic selection.
	TODO: static dependency sets of guards.
 */
void
StateConstructor::visit(const deterministic_selection& ds) {
	STACKTRACE_VERBOSE;
	// TODO: run-time check for guard exclusion
	const size_t branches = ds.size();
	const size_t merge_index = allocate_event(
		EventNode(NULL, SIM::CHPSIM::EVENT_SELECTION_END, 
			current_process_index, 0));
	// no delay at end of selection
	// don't pass this, as that would cause re-evaluation at join node!
{
	STACKTRACE_INDENT_PRINT("merge index: " << merge_index << endl);
	EventNode& merge_event(get_event(merge_index));
	connect_successor_events(merge_event);
	merge_event.set_predecessors(1);	// expect ONE branch only
	count_predecessors(merge_event);
}

	deterministic_selection::const_iterator i(ds.begin()), e(ds.end());
	SIM::CHPSIM::DependenceSetCollector deps(*this);	// args
	vector<size_t> tmp;
	tmp.reserve(branches);
	// construct concurrent chains
	for ( ; i!=e; ++i) {
		last_event_index = merge_index;	// pass down
		(*i)->accept(*this);
		tmp.push_back(last_event_index);	// head of each chain
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

	// construct successor event graph edge? or caller's responsibility?
	const size_t split_index = allocate_event(
		EventNode(&ds, SIM::CHPSIM::EVENT_SELECTION_BEGIN, 
			current_process_index, 
#if CHP_ACTION_DELAYS
			// assert dynamic_cast
			ds.get_delay() ?
				ds.get_delay().is_a<const preal_const>()
				->static_constant_value() :
#endif
			1));
	// delay value doesn't matter because this event is never
	// 'executed' in the traditional sense.
	// we CAN however use this delay value to incur additional delay
	// on its successors (but we don't do this yet)
{
	STACKTRACE_INDENT_PRINT("split index: " << split_index << endl);
	EventNode& split_event(get_event(split_index));
	split_event.import_block_dependencies(deps);

	split_event.successor_events.resize(branches);
	copy(tmp.begin(), tmp.end(), &split_event.successor_events[0]);

	// updates successors' predecessor-counts
	count_predecessors(split_event);
}
	// leave trail of this event for predecessor
	last_event_index = split_index;
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
	const size_t merge_index = allocate_event(
		EventNode(NULL, SIM::CHPSIM::EVENT_SELECTION_END, 
			current_process_index, 0));
	// don't pass 'this', as that would cause re-evaluation at join node!
	// no delay at end of selection
{
	STACKTRACE_INDENT_PRINT("merge index: " << merge_index << endl);
	EventNode& merge_event(get_event(merge_index));
	connect_successor_events(merge_event);
	merge_event.set_predecessors(1);	// expect ONE branch only
	count_predecessors(merge_event);
}

	nondeterministic_selection::const_iterator i(ns.begin()), e(ns.end());
	SIM::CHPSIM::DependenceSetCollector deps(*this);	// args
	vector<size_t> tmp;
	tmp.reserve(branches);
	// construct concurrent chains
	for ( ; i!=e; ++i) {
		last_event_index = merge_index;	// pass down
		(*i)->accept(*this);
		tmp.push_back(last_event_index);	// head of each chain
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

	// construct successor event graph edge? or caller's responsibility?
	const size_t split_index = allocate_event(
		EventNode(&ns, SIM::CHPSIM::EVENT_SELECTION_BEGIN, 
			current_process_index, 
#if CHP_ACTION_DELAYS
			// assert dynamic_cast
			ns.get_delay() ?
				ns.get_delay().is_a<const preal_const>()
				->static_constant_value() :
#endif
			15));
	// NOTE: the delay value used here is the window of time from 
	// first unblocked evaluation (enqueue) to execution, during which
	// guards may change!
{
	STACKTRACE_INDENT_PRINT("split index: " << split_index << endl);
	EventNode& split_event(get_event(split_index));
	split_event.import_block_dependencies(deps);

	split_event.successor_events.resize(branches);
	copy(tmp.begin(), tmp.end(), &split_event.successor_events[0]);

	// updates successors' predecessor-counts
	count_predecessors(split_event);
}
	// leave trail of this event for predecessor
	last_event_index = split_index;
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
	// construct successor event graph edge? or caller's responsibility?
	const size_t new_index = allocate_event(
		EventNode(&a, SIM::CHPSIM::EVENT_ASSIGN, 
			current_process_index, 
#if CHP_ACTION_DELAYS
			// assert dynamic_cast
			a.get_delay() ? a.get_delay().is_a<const preal_const>()
				->static_constant_value() :
#endif
			10));
	// we give a medium delay to assignment
	// this may favor explicit communications (projection, refactoring)
	// over variable assignment.  
	STACKTRACE_INDENT_PRINT("new assigment: " << new_index << endl);
	EventNode& new_event(get_event(new_index));

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

	connect_successor_events(new_event);
	// assignments are atomic and never block
	// thus we need no dependencies.  

	// leave trail of this event for predecessor
	// s.last_event_indices.resize(1);
	// s.last_event_indices[0] = new_index;
	last_event_index = new_index;

	// updates successors' predecessor-counts
	count_predecessors(new_event);
}	// end visit(const assignment&)

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
	// register guard expression dependents
	// construct successor event graph edge? or caller's responsibility?
	const size_t new_index = allocate_event(
		EventNode(&cw, SIM::CHPSIM::EVENT_NULL, 
			current_process_index, 
#if CHP_ACTION_DELAYS
			// assert dynamic_cast
			cw.get_delay() ?
				cw.get_delay().is_a<const preal_const>()
				->static_constant_value() :
#endif
			0));
	// the delay value should have no impact, as this event just
	// unblocks its successor(s)
	STACKTRACE_INDENT_PRINT("wait index: " << new_index << endl);
	EventNode& new_event(get_event(new_index));
	if (cw.get_guard()) {
		SIM::CHPSIM::DependenceSetCollector deps(*this);
		cw.get_guard()->accept(deps);
		new_event.import_block_dependencies(deps);
	}

	connect_successor_events(new_event);

	// leave trail of this event for predecessor
	// s.last_event_indices.resize(1);
	// s.last_event_indices[0] = new_index;
	last_event_index = new_index;

	// updates successors' predecessor-counts
	count_predecessors(new_event);
}	// end visit(const condition_wait&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
StateConstructor::visit(const channel_send& cs) {
	STACKTRACE_VERBOSE;
	// atomic event
	// construct event graph
	const size_t new_index = allocate_event(
		EventNode(&cs, SIM::CHPSIM::EVENT_SEND, 
			current_process_index, 
#if CHP_ACTION_DELAYS
			// assert dynamic_cast
			cs.get_delay() ?
				cs.get_delay().is_a<const preal_const>()
				->static_constant_value() :
#endif
			2));
	// default to small delay
	STACKTRACE_INDENT_PRINT("send index: " << new_index << endl);
	EventNode& new_event(get_event(new_index));

{
	// can block on channel, so we add dependencies
	SIM::CHPSIM::DependenceSetCollector deps(*this);
	cs.get_chan()->accept(deps);
	new_event.import_block_dependencies(deps);
}

	connect_successor_events(new_event);

	// leave trail of this event for predecessor
	// s.last_event_indices.resize(1);
	// s.last_event_indices[0] = new_index;
	last_event_index = new_index;

	// updates successors' predecessor-counts
	count_predecessors(new_event);
}	// end visit(const channel_send&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
StateConstructor::visit(const channel_receive& cr) {
	STACKTRACE_VERBOSE;
	// atomic event
	// construct event graph
	const size_t new_index = allocate_event(
		EventNode(&cr, SIM::CHPSIM::EVENT_RECEIVE, 
			current_process_index, 
#if CHP_ACTION_DELAYS
			// assert dynamic_cast
			cr.get_delay() ?
				cr.get_delay().is_a<const preal_const>()
				->static_constant_value() :
#endif
			5));
	// default to small delay
	// this delay would be more meaningful in a handshaking expansion
	STACKTRACE_INDENT_PRINT("receive index: " << new_index << endl);
	EventNode& new_event(get_event(new_index));

{
	// can block on channel, so we add dependencies
	SIM::CHPSIM::DependenceSetCollector deps(*this);
	cr.get_chan()->accept(deps);
	new_event.import_block_dependencies(deps);
}
	connect_successor_events(new_event);

	// leave trail of this event for predecessor
	// s.last_event_indices.resize(1);
	// s.last_event_indices[0] = new_index;
	last_event_index = new_index;

	// updates successors' predecessor-counts
	count_predecessors(new_event);
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
	// construct cyclic event graph
	// create a dummy event first (epilogue) and loop it around.
	// OR use the 0th event slot as the dummy!
	// -- works only if we need one dummy at a time
	const size_t loopback_index = allocate_event(
		EventNode(&fl, SIM::CHPSIM::EVENT_NULL,
			current_process_index, 
#if (0 && CHP_ACTION_DELAYS)
			// assert dynamic_cast
			fl.get_delay() ?
				fl.get_delay().is_a<const preal_const>()
				->static_constant_value() :
#endif
			0));	// no additional delay
	STACKTRACE_INDENT_PRINT("forever loopback: " << loopback_index << endl);
	last_event_index = loopback_index;	// point to dummy, pass down
	fl.get_body()->accept(*this);
	// never blocks, no need for dependency checking
{
	// find last event and loop it back to the beginning
	EventNode& loopback_event(get_event(loopback_index));
	EventNode& head_event(get_event(last_event_index));
	STACKTRACE_INDENT_PRINT("head of body: " << last_event_index << endl);
// some redundant operations going on here...
	// re-link loop
	loopback_event.successor_events.resize(1);
	loopback_event.successor_events[0] = last_event_index;
#if OPTIMIZE_CHPSIM_EVENTS
/***
	Two exclusive strategies possible from here:
	1) move the back event to the loopback placeholder slot, 
		this way minimizes dead-events in the middle.
		We do this only if it is 'convenient', i.e.
		the back event is trivial and has a lone successor.  
	2) just free the loopback placeholder, by forwarding around it, 
		return a reference to the head_event
***/
	const size_t back_index = event_pool_size() -1;
	EventNode& back_event(get_event(back_index));
	STACKTRACE_INDENT_PRINT("considering back: " << back_index << endl);
if (back_event.is_dispensible()) {
	// 2) recycle the back event, involves re-linking up to two events
	// Q: does the loopback already point to back? (corner case)
	if (back_index != loopback_event.successor_events[0]) {
		forward_successor(loopback_index);	// pointers to loopback
	}
	forward_successor(back_index, loopback_index, last_event_index);
	// INVARIANT(s.last_event_index != back_index);
	loopback_event = back_event;	// MOVE into placeholder slot!
	STACKTRACE_INDENT_PRINT("recycling back: " << back_index << endl);
	deallocate_event(back_index);		// recycle it!
	if (back_index == last_event_index) {
		STACKTRACE_INDENT_PRINT("back is s.last_event_index" << endl);
	// need to update return value (head index), using loopback slot
		last_event_index = loopback_index;
	}
} else {
	// 1) then we can recycle the loopback slot safely (tested)
	// first: forward successors through condemned event
	const size_t ret = forward_successor(loopback_index);
	INVARIANT(ret == last_event_index);
	// last: free the condemned event
	deallocate_event(loopback_index);		// recycle it!
	// loopback_event is now dead
	STACKTRACE_INDENT_PRINT("recycling loopback: " << loopback_index << endl);
//	s.last_event_index = ret;	// redundant
}
#endif	// OPTIMIZE_CHPSIM_EVENTS
	head_event.set_predecessors(1);	// but may have multiple entries
	// caller will count_predecessors
}
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
	// construct cyclic event graph
	// create a dummy event first (epilogue) and loop it around.
	const size_t branches = dw.size();
	const size_t loopback_index = allocate_event(
		EventNode(&dw, SIM::CHPSIM::EVENT_SELECTION_BEGIN,
			current_process_index, 
#if CHP_ACTION_DELAYS
			// assert dynamic_cast
			dw.get_delay() ? 
				dw.get_delay().is_a<const preal_const>()
				->static_constant_value() :
#endif
			3));
	// give small delay for selection
	STACKTRACE_INDENT_PRINT("do-while loopback index: "
		<< loopback_index << endl);
{
	EventNode& loopback_event(get_event(loopback_index));
	loopback_event.successor_events.resize(branches +1);
	if (last_event_index) {
		// exit
		loopback_event.successor_events[branches] = last_event_index;
	} else {
		// there was no successor, create a terminator
		const size_t terminal_index = allocate_event(
			EventNode(NULL, SIM::CHPSIM::EVENT_NULL,
				current_process_index, 0));
		// loopback_event reference invalidated by push_back
		get_event(loopback_index).successor_events[branches]
			= terminal_index;
		STACKTRACE_INDENT_PRINT("new terminal index: "
			<< terminal_index << endl);
	}
	// convention: 
	// 1st events will go into the body of the do-while loop
	// last event will be the exit branch, corresponding to the else clause
}
	// NOTE: no need to add guard dependencies because
	// action is always taken immediately (implicit else-clause skips body)
	vector<size_t> tmp;
	tmp.reserve(branches);
{
	do_while_loop::const_iterator i(dw.begin()), e(dw.end());
	// construct concurrent chains
	for ( ; i!=e; ++i) {
		last_event_index = loopback_index;	// pass down
		(*i)->accept(*this);		// guarded actions
		tmp.push_back(last_event_index);	// head of each chain
	}
}
{
	// find last event and loop it back to the beginning
	EventNode& loopback_event(get_event(loopback_index));
	copy(tmp.begin(), tmp.end(), &loopback_event.successor_events[0]);
#if ENABLE_STACKTRACE
	cerr << "tmp: ";
	copy(tmp.begin(), tmp.end(), std::ostream_iterator<size_t>(cerr, ","));
	cerr << endl;
	cerr << "else: " << loopback_event.successor_events[branches] << endl;
#endif
	// loopback_event.successor_events[0] = s.last_event_index;
	// s.last_event_index now points to first action(s) in loop
	// EventNode& head_event(event_pool[s.last_event_index]);
	// head_event.set_predecessors(1);	// but may have multiple entries
	count_predecessors(loopback_event);
	last_event_index = loopback_index;
}
}	// end visit(const do_while_loop&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#endif	// accept-visitors

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

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

