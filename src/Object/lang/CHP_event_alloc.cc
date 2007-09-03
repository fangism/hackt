/**
	\file "Object/lang/CHP_event_alloc.cc"
	Copy-ripped from "sim/chpsim/StateConstructor.cc"
	$Id: CHP_event_alloc.cc,v 1.1.2.2 2007/09/03 03:46:43 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <iterator>
#include "Object/lang/CHP_event_alloc.h"
#include "Object/lang/CHP.h"
#include "Object/expr/bool_expr.h"		// for guard->dump
#include "Object/traits/chan_traits.h"
#if ENABLE_STACKTRACE
#include "Object/expr/expr_dump_context.h"
#endif
#include "common/ICE.h"
#include "util/visitor_functor.h"
#include "util/stacktrace.h"
#include "util/memory/free_list.h"

/**
	Various levels of chpsim event generation optimizations, 
	such as fusion.  
	This might become necessary...
 */
#define	OPTIMIZE_CHPSIM_EVENTS			1

namespace HAC {
namespace entity {
namespace CHP {
using std::vector;
using util::memory::free_list_acquire;
using util::memory::free_list_release;
#include "util/using_ostream.h"

//=============================================================================
// class local_event_allocator method definitions

local_event_allocator::local_event_allocator(local_event_footprint& s) : 
		event_footprint(s), 
		free_list(), 
		// last_event_indices(), 
		last_event_index(event_index_type(-1))	// explicit cast
		{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Some clean-up to avoid printing dead nodes.  
 */
local_event_allocator::~local_event_allocator() {
	typedef free_list_type::const_iterator	const_iterator;
	const_iterator i(free_list.begin()), e(free_list.end());
	for ( ; i!=e; ++i) {
		get_event(*i).orphan();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: 0 is a valid local event index, 
	but special as a global event index.  
 */
bool
local_event_allocator::valid_last_event_index(void) const {
	// return last_event_index;		// old
	return last_event_index < event_footprint.size();
		// && not in free_list?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Clears the last_event_index to something invalid.
	Call this before visiting each process.  
 */
void
local_event_allocator::reset(void) {
	last_event_index = event_index_type(-1);	// explicit cast
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs a sequence of events backwards from back to
	front, where each event 'connects' to its successor(s).
	NOTE: no delay addition anywhere here
 */
void
local_event_allocator::visit(const action_sequence& l) {
	STACKTRACE_VERBOSE;
	action_sequence::const_iterator i(l.begin()), e(l.end());
	INVARIANT(i!=e);	// else would be empty
	do {
		--e;
		(*e)->accept(*this);
		// events will link themselves (callee responsible)
	} while (i!=e);
}

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
local_event_allocator::visit(const concurrent_actions& ca) {
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
	const size_t join_index =
		allocate_event(event_type(&ca, EVENT_CONCURRENT_JOIN));
	// no additional delay given
{
	STACKTRACE_INDENT_PRINT("join index: " << join_index << endl);
	// join shouldn't need an action ptr (unless we want back-reference)
	event_type& join_event(get_event(join_index));
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
	event_type& join_event(get_event(join_index));
	join_event.set_predecessors(branches);
	count_predecessors(join_event);
}

	// construct successor event graph edge? or caller's responsibility?
	const size_t fork_index =
		allocate_event(event_type(&ca, EVENT_CONCURRENT_FORK));
{
	STACKTRACE_INDENT_PRINT("fork index: " << fork_index << endl);
	event_type& fork_event(get_event(fork_index));

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
local_event_allocator::visit(const guarded_action& s) {
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
local_event_allocator::visit(const deterministic_selection& ds) {
	STACKTRACE_VERBOSE;
	// TODO: run-time check for guard exclusion
	const size_t branches = ds.size();
	const size_t merge_index =
		allocate_event(event_type(NULL, EVENT_SELECTION_END));
	// no delay at end of selection
	// don't pass this, as that would cause re-evaluation at join node!
{
	STACKTRACE_INDENT_PRINT("merge index: " << merge_index << endl);
	event_type& merge_event(get_event(merge_index));
	connect_successor_events(merge_event);
	merge_event.set_predecessors(1);	// expect ONE branch only
	count_predecessors(merge_event);
}

	// no dependency analysis here
	deterministic_selection::const_iterator i(ds.begin()), e(ds.end());
	vector<size_t> tmp;
	tmp.reserve(branches);
	// construct concurrent chains
	for ( ; i!=e; ++i) {
		last_event_index = merge_index;	// pass down
		(*i)->accept(*this);
		tmp.push_back(last_event_index);	// head of each chain
	}

	// construct successor event graph edge? or caller's responsibility?
	const size_t split_index = 
		allocate_event(event_type(&ds, EVENT_SELECTION_BEGIN));
	// delay value doesn't matter because this event is never
	// 'executed' in the traditional sense.
	// we CAN however use this delay value to incur additional delay
	// on its successors (but we don't do this yet)
{
	STACKTRACE_INDENT_PRINT("split index: " << split_index << endl);
	event_type& split_event(get_event(split_index));

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
local_event_allocator::visit(const nondeterministic_selection& ns) {
	STACKTRACE_VERBOSE;
	// TODO: run-time check for guard exclusion
	const size_t branches = ns.size();
	const size_t merge_index =
		allocate_event(event_type(NULL, EVENT_SELECTION_END));
	// don't pass 'this', as that would cause re-evaluation at join node!
	// no delay at end of selection
{
	STACKTRACE_INDENT_PRINT("merge index: " << merge_index << endl);
	event_type& merge_event(get_event(merge_index));
	connect_successor_events(merge_event);
	merge_event.set_predecessors(1);	// expect ONE branch only
	count_predecessors(merge_event);
}

	// no dependency analysis here
	nondeterministic_selection::const_iterator i(ns.begin()), e(ns.end());
	vector<size_t> tmp;
	tmp.reserve(branches);
	// construct concurrent chains
	for ( ; i!=e; ++i) {
		last_event_index = merge_index;	// pass down
		(*i)->accept(*this);
		tmp.push_back(last_event_index);	// head of each chain
	}

	// construct successor event graph edge? or caller's responsibility?
	const size_t split_index =
		allocate_event(event_type(&ns, EVENT_SELECTION_BEGIN));
	// NOTE: the delay value used here is the window of time from 
	// first unblocked evaluation (enqueue) to execution, during which
	// guards may change!
{
	STACKTRACE_INDENT_PRINT("split index: " << split_index << endl);
	event_type& split_event(get_event(split_index));

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
local_event_allocator::visit(const metaloop_selection&) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
local_event_allocator::visit(const metaloop_statement&) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
local_event_allocator::visit(const assignment& a) {
	STACKTRACE_VERBOSE;
	// construct successor event graph edge? or caller's responsibility?
	const size_t new_index =
		allocate_event(event_type(&a, EVENT_ASSIGN));
	// we give a medium delay to assignment
	// this may favor explicit communications (projection, refactoring)
	// over variable assignment.  
	STACKTRACE_INDENT_PRINT("new assigment: " << new_index << endl);
	event_type& new_event(get_event(new_index));

	// no dependencies in this structure

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
	Has no block dependencies.
 */
void
local_event_allocator::visit(const function_call_stmt& fc) {
	STACKTRACE_VERBOSE;
	const size_t new_index =
		allocate_event(event_type(&fc, EVENT_FUNCTION_CALL));
	// we give an arbitrary delay to function call
	STACKTRACE_INDENT_PRINT("new call: " << new_index << endl);
	event_type& new_event(get_event(new_index));
	connect_successor_events(new_event);
	last_event_index = new_index;
	count_predecessors(new_event);
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
local_event_allocator::visit(const condition_wait& cw) {
	STACKTRACE_VERBOSE;
	// register guard expression dependents
	// construct successor event graph edge? or caller's responsibility?
	const size_t new_index =
		allocate_event(event_type(&cw, EVENT_CONDITION_WAIT));
	// the delay value should have no impact, as this event just
	// unblocks its successor(s)
	STACKTRACE_INDENT_PRINT("wait index: " << new_index << endl);
	event_type& new_event(get_event(new_index));
	// no dependency analysis

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
local_event_allocator::visit(const channel_send& cs) {
	STACKTRACE_VERBOSE;
	// atomic event
	// construct event graph
	const size_t new_index =
		allocate_event(event_type(&cs, EVENT_SEND));
	// default to small delay
	STACKTRACE_INDENT_PRINT("send index: " << new_index << endl);
	event_type& new_event(get_event(new_index));

	// no depenendency analysis

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
local_event_allocator::visit(const channel_receive& cr) {
	STACKTRACE_VERBOSE;
	// atomic event
	// construct event graph
	const bool peek = cr.is_peek();
	const size_t new_index = allocate_event(
		event_type(&cr, peek ? EVENT_PEEK : EVENT_RECEIVE));
	// default to small delay
	// this delay would be more meaningful in a handshaking expansion
	STACKTRACE_INDENT_PRINT("receive index: " << new_index << endl);
	event_type& new_event(get_event(new_index));

	// no depenendency analysis

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
local_event_allocator::visit(const do_forever_loop& fl) {
	STACKTRACE_VERBOSE;
	// construct cyclic event graph
	// create a dummy event first (epilogue) and loop it around.
	// OR use the 0th event slot as the dummy!
	// -- works only if we need one dummy at a time
	const size_t loopback_index =
		allocate_event(event_type(&fl, EVENT_NULL));
	STACKTRACE_INDENT_PRINT("forever loopback: " << loopback_index << endl);
	last_event_index = loopback_index;	// point to dummy, pass down
	fl.get_body()->accept(*this);
	// never blocks, no need for dependency checking
{
	// find last event and loop it back to the beginning
	event_type& loopback_event(get_event(loopback_index));
	event_type& head_event(get_event(last_event_index));
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
#if 0
	const size_t back_index = event_pool_size() -1;
	event_type& back_event(get_event(back_index));
	STACKTRACE_INDENT_PRINT("considering back: " << back_index << endl);
if (back_event.is_movable()) {
	// 2) recycle the back event, involves re-linking up to two events
	// Q: does the loopback already point to back? (corner case)
	if (back_index != loopback_event.successor_events[0]) {
		forward_successor(loopback_index);	// pointers to loopback
	}
	substitute_successor(back_index, loopback_index, last_event_index);
	// INVARIANT(s.last_event_index != back_index);
	loopback_event = back_event;	// MOVE into placeholder slot!
	STACKTRACE_INDENT_PRINT("recycling back: " << back_index << endl);
	deallocate_event(back_index);		// recycle it!
	if (back_index == last_event_index) {
		STACKTRACE_INDENT_PRINT("back is s.last_event_index" << endl);
	// need to update return value (head index), using loopback slot
		last_event_index = loopback_index;
	}
} else
#endif
{
	// 1) then we can recycle the loopback slot safely (tested)
	// first: forward successors through condemned event
	// let later optimization pass re-compact events
	const size_t ret = forward_successor(loopback_index);
	INVARIANT(ret == last_event_index);
	// last: free the condemned event
	deallocate_event(loopback_index);		// recycle it!
	// loopback_event is now dead
	STACKTRACE_INDENT_PRINT("recycling loopback: " << loopback_index << endl);
//	last_event_index = ret;	// redundant
}
#else	// OPTIMIZE_CHPSIM_EVENTS
	// let compact_and_canonicalize take care of things...
	// TODO: pass to find NULL events with singletone-successor
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
local_event_allocator::visit(const do_while_loop& dw) {
	STACKTRACE_VERBOSE;
	// construct cyclic event graph
	// create a dummy event first (epilogue) and loop it around.
	const size_t branches = dw.size();
	const size_t loopback_index =
		allocate_event(event_type(&dw, EVENT_SELECTION_BEGIN));
	// give small delay for selection
	STACKTRACE_INDENT_PRINT("do-while loopback index: "
		<< loopback_index << endl);
{
	event_type& loopback_event(get_event(loopback_index));
	loopback_event.successor_events.resize(branches +1);
	if (valid_last_event_index()) {
		// exit
		loopback_event.successor_events[branches] = last_event_index;
	} else {
		// there was no successor, create a terminator
		const size_t terminal_index =
			allocate_event(event_type(NULL, EVENT_NULL));
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
	event_type& loopback_event(get_event(loopback_index));
	copy(tmp.begin(), tmp.end(), &loopback_event.successor_events[0]);
#if ENABLE_STACKTRACE
	cerr << "tmp: ";
	copy(tmp.begin(), tmp.end(), std::ostream_iterator<size_t>(cerr, ","));
	cerr << endl;
	cerr << "else: " << loopback_event.successor_events[branches] << endl;
#endif
	// loopback_event.successor_events[0] = s.last_event_index;
	// s.last_event_index now points to first action(s) in loop
	// event_type& head_event(event_pool[s.last_event_index]);
	// head_event.set_predecessors(1);	// but may have multiple entries
	count_predecessors(loopback_event);
	last_event_index = loopback_index;
}
}	// end visit(const do_while_loop&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Copy-modified from entity::cflat_visitor::visit()
	top-level CHP is already visited by the caller
	in CHPSIM::State::State().  
 */
void
local_event_allocator::visit(const state_manager& _sm) {
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
		if (valid_last_event_index()) {
			initial_events.push_back(last_event_index);
		}
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
local_event_allocator::event_type&
local_event_allocator::get_event(const event_index_type ei) {
	return event_footprint[ei];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const local_event_allocator::event_type&
local_event_allocator::get_event(const event_index_type ei) const {
	return event_footprint[ei];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
local_event_allocator::event_pool_size(void) const {
	return event_footprint.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates a new event, initializing to the argument.  
	Also recycles free-slots if available.  
	Reminder: push_back may invalidate references to array members.
 */
local_event_allocator::event_index_type
local_event_allocator::allocate_event(const event_type& e) {
	STACKTRACE_VERBOSE;
	if (free_list.empty()) {
		STACKTRACE_INDENT_PRINT("push_back" << endl);
		const event_index_type ret = event_pool_size();
		event_footprint.push_back(e);
		return ret;
	} else {
		// take first available index
		STACKTRACE_INDENT_PRINT("free-list" << endl);
		const event_index_type ret = free_list_acquire(free_list);
		event_footprint[ret] = e;
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recycles an event slot for recycling for a subsequent allocation
	request.  
 */
void
local_event_allocator::deallocate_event(const event_index_type ei) {
	// does invariant checking
	if (ei == event_pool_size() -1) {
		// if it happens to be the back entry, just pop it
		event_footprint.pop_back();
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
local_event_allocator::event_index_type
local_event_allocator::forward_successor(const event_index_type f) {
	STACKTRACE_VERBOSE;
	const event_type& skip_me(get_event(f));
	const event_index_type replacement = skip_me.successor_events[0];
	const event_type& r(get_event(replacement));
	INVARIANT(r.get_predecessors() < 2);	// 1 or 0
	substitute_successor(f, replacement, replacement);	// wrapped call
	return replacement;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Variation where the replacement is specified explicitly by caller.  
	\param f the index of the event to forward through.  
	\param replacement the index to replace occurrences of f.
	\param h the head event to start searching and replacing from.
	Algorithm O(E) (number of successor edges)
 */
void
local_event_allocator::substitute_successor(const event_index_type f, 
		const event_index_type replacement, 
		const event_index_type h) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("f,r,h = " << f << ", " <<
		replacement << ", " << h << endl);

	// collect event nodes reachable from the head (post-dominate?)
	// using worklist algorithm
	typedef	std::set<event_index_type>	reachable_set_type;
	// or discrete_interval_set for integers
	reachable_set_type visited_set;		// covered
	reachable_set_type worklist;		// to-do list
	worklist.insert(h);
	worklist.insert(replacement);
	while (!worklist.empty()) {
	const event_index_type ei = free_list_acquire(worklist);
	if (visited_set.find(ei) == visited_set.end()) {
		STACKTRACE_INDENT_PRINT("worklist: " << ei << endl);
		event_type::successor_list_type&
			s(get_event(ei).successor_events);
		STACKTRACE_INDENT_PRINT("size: " << s.size() << endl);
		event_type::successor_list_type::iterator
			si(s.begin()), se(s.end());
		// algorithm: find and replace
		// non-matches should be thrown into work-list
		for ( ; si!=se; ++si) {
			if (visited_set.find(*si) == visited_set.end()) {
			STACKTRACE_INDENT_PRINT("  enqueue: " << ei << endl);
				worklist.insert(*si);
			}
			if (*si == f) {
			STACKTRACE_INDENT_PRINT("  replace: " << *si << endl);
				*si = replacement;
			}
		}
		visited_set.insert(ei);	// mark as finished
	}	// end if
	// else already processed, move on
	}	// end while (worklist drained)
}	// end substitute_successor

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This updates the new event's list of successors using the 
	current list of successor events.  (Backwards construction.)
	If last_event_index is 0 (null), then don't bother.  
 */
void
local_event_allocator::connect_successor_events(event_type& ev) const {
	STACKTRACE_VERBOSE;
	if (valid_last_event_index()) {
		ev.successor_events.resize(1);
		ev.successor_events[0] = last_event_index;
	}
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
local_event_allocator::count_predecessors(const event_type& ev) const {
	STACKTRACE_VERBOSE;
//	typedef	event_type::event_index_type	event_index_type;
	// poor man's valarray iterator
	const event_index_type* i = &ev.successor_events[0], 
		*e = &ev.successor_events[ev.successor_events.size()];
	// const size_t n_pred = last_event_indices.size();
	for ( ; i!=e; ++i) {
		// state.event_pool[*i].set_predecessors(n_pred);
		event_footprint[*i].set_predecessors(1);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Moves the root event into index[0].  
	After allocation, there may be gaps in allocated events
	indicated by the freelist, and also there may be unreachable events.
	\pre last_event_index holds the root index to the event
		that will reach others in this process.  
 */
void
local_event_allocator::compact_and_canonicalize(void) {
	STACKTRACE_VERBOSE;
if (event_footprint.size()) {
	typedef	std::set<size_t>		set_type;
#if OPTIMIZE_CHPSIM_EVENTS
	// 1. fixed-point collection and deallocation of unreachable events
	bool removed_some;
do {
	STACKTRACE_INDENT_PRINT("removing not-reached events..." << endl);
	// this is at worst, O(VE)
	removed_some = false;
	set_type not_reached;
	size_t i;
	for (i=0; i < event_footprint.size(); ++i) { not_reached.insert(i); }
	
	// alternative: walk successors in DFS, like in ::forward_successor
	for (i=0; i < event_footprint.size(); ++i) {
		const event_type& e(event_footprint[i]);
		const event_type::successor_list_type& succ(e.successor_events);
		event_type::successor_list_type::const_iterator
			si(succ.begin()), se(succ.end());
		for ( ; si!=se; ++si) {
			not_reached.erase(*si);
		}
	}
	set_type::const_iterator j(not_reached.begin()), e(not_reached.end());
	for ( ; j!=e; ++j) {
		const size_t& n = *j;
		// root-node is always assumed reachable
		if ((n != last_event_index) &&
			(free_list.find(n) == free_list.end())) {
			STACKTRACE_INDENT_PRINT(
				"not reached: " << n << endl);
			deallocate_event(n);
			removed_some = true;
		}
	}	// end for
} while (removed_some);
#if ENABLE_STACKTRACE
	event_footprint.dump(cout, expr_dump_context::default_value);
#endif
	// 2. move events from tail end to free_list spots, 
	// remembering to keep track of root-node index!
	STACKTRACE_INDENT_PRINT("compacting events..." << endl);
while (!free_list.empty()) {
	// allocation will never push_back-invalidate
	event_index_type slot = allocate_event(event_footprint.back());
	STACKTRACE_INDENT_PRINT("filling hole: " << slot << endl);
	// new size, also index of the entry that was just popped.  
	const size_t ns = event_footprint.size() -1;
	if (last_event_index == ns) {
		last_event_index = slot;
	}
	substitute_successor(ns, slot, last_event_index);
	deallocate_event(ns);	// does event_footprint.pop_back();
}	// end while
#endif	// OPTIMIZE_CHPSIM_EVENTS
#if ENABLE_STACKTRACE
	event_footprint.dump(cout, expr_dump_context::default_value);
#endif
	// 3. move root-node to index[0], a swap
	// invariant: free_list is empty and events are densely packed
	STACKTRACE_INDENT_PRINT("canonicalizing root node..." << endl);
if (last_event_index && event_footprint.size()) {		// != 0
	STACKTRACE_INDENT_PRINT("swapping to event[0]..." << endl);
	const event_type copy(event_footprint[0]);
	// b/c allocation may leave reference invalid
	event_index_type temp = allocate_event(copy);
	substitute_successor(0, temp, last_event_index);
	event_footprint[0] = event_footprint[last_event_index];
	substitute_successor(last_event_index, 0, 0);
	event_footprint[last_event_index] = event_footprint[temp];
	substitute_successor(temp, last_event_index, 0);
	last_event_index = 0;
	deallocate_event(temp);
	INVARIANT(free_list.empty());
}
	// 4. if root-event has no predecessors, add one for the entry point
	event_footprint[0].mark_as_entry();
}	// end if event_footprint.size()
}	// end compact_and_canonicalize

//=============================================================================
// class EventSuccessorDumper method definitions
// ripped from "sim/chpsim/EventExecutor.cc"

/**
	Print outgoing edges adorned with guard expressions as labels. 
 */
static
ostream&
dump_selection_successor_edges(const selection_list_type& l, 
		ostream& o, const local_event& e, const size_t i, 
		const expr_dump_context& c) {
	typedef	local_event		event_type;
	typedef	selection_list_type::const_iterator const_iterator;
	const event_type::successor_list_type& succ(e.successor_events);
	event_type::successor_list_type::const_iterator
		si(succ.begin()), se(succ.end());
	const_iterator li(l.begin()), le(l.end());
	for ( ; li!=le; ++li, ++si) {
		const guarded_action::guard_ptr_type&
			g((*li)->get_guard());
		o << event_type::node_prefix << i << " -> " <<
			event_type::node_prefix << *si << "\t[label=\"";
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
		o << event_type::node_prefix << i << " -> " <<
			event_type::node_prefix << *si <<
			"\t[label=\"else\"];" << endl;
		++si;
		INVARIANT(si == se);
		
	}
	// check for else clause
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	By default, print all successor edges, unadorned.  
 */
#define DEFAULT_EVENT_SUCCESSOR_DUMPER(T)				\
void									\
EventSuccessorDumper::visit(const T&) {					\
	event.dump_successor_edges_default(os, index);			\
}

/**
	For selections, use different printer.
 */
#define SELECTION_EVENT_SUCCESSOR_DUMPER(T)				\
void									\
EventSuccessorDumper::visit(const T& s) {				\
	dump_selection_successor_edges(					\
		s, os, event, index, dump_context);			\
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DEFAULT_EVENT_SUCCESSOR_DUMPER(action_sequence)
DEFAULT_EVENT_SUCCESSOR_DUMPER(concurrent_actions)
DEFAULT_EVENT_SUCCESSOR_DUMPER(guarded_action)
DEFAULT_EVENT_SUCCESSOR_DUMPER(metaloop_selection)
DEFAULT_EVENT_SUCCESSOR_DUMPER(metaloop_statement)
DEFAULT_EVENT_SUCCESSOR_DUMPER(assignment)
DEFAULT_EVENT_SUCCESSOR_DUMPER(condition_wait)
DEFAULT_EVENT_SUCCESSOR_DUMPER(channel_send)
DEFAULT_EVENT_SUCCESSOR_DUMPER(channel_receive)
DEFAULT_EVENT_SUCCESSOR_DUMPER(function_call_stmt)
DEFAULT_EVENT_SUCCESSOR_DUMPER(do_forever_loop)

SELECTION_EVENT_SUCCESSOR_DUMPER(deterministic_selection)
SELECTION_EVENT_SUCCESSOR_DUMPER(nondeterministic_selection)
SELECTION_EVENT_SUCCESSOR_DUMPER(do_while_loop)

#undef	DEFAULT_EVENT_SUCCESSOR_DUMPER
#undef	SELECTION_EVENT_SUCCESSOR_DUMPER

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

