/**
	\file "sim/prsim/State.cc"
	Implementation of prsim simulator state.  
	$Id: State.cc,v 1.20.2.5 2006/08/11 02:05:49 fang Exp $
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
#include "sim/prsim/Rule.tcc"
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
#include "util/iterator_more.h"
#include "util/string.tcc"
#include "util/IO_utils.tcc"
#include "util/binders.h"

#if	DEBUG_STEP
#define	DEBUG_STEP_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_STEP		STACKTRACE_VERBOSE
#else
#define	DEBUG_STEP_PRINT(x)
#define	STACKTRACE_VERBOSE_STEP
#endif

#if	DEBUG_FANOUT
#define	DEBUG_FANOUT_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#else
#define	DEBUG_FANOUT_PRINT(x)
#endif

/**
	Currently, the rule map is just a map to structural information
	and contains no stateful information.
	If and when rules retain stateful information, 
	enabling expression-transforming optimizations might become a problem
	because expression structures are different, and will affect
	the contents of the checkpoint.
	Until that day, we won't worry about it.  
 */
#define	CHECKPOINT_RULE_STATE_MAP		0

namespace HAC {
namespace entity { }

namespace SIM {
namespace PRSIM {
using std::string;
using std::ostringstream;
using std::for_each;
using std::mem_fun_ref;
using std::distance;
using std::fill;
using std::find;
using std::copy;
using std::set_intersection;
using util::set_inserter;
using util::strings::string_to_num;
using util::read_value;
using util::write_value;
using util::bind2nd_argval;
using util::bind2nd_argval_void;
using entity::state_manager;
using entity::global_entry_pool;
using entity::bool_tag;
using entity::process_tag;
#include "util/using_ostream.h"
//=============================================================================
// class State::event_deallocator definition

/**
	Helper class using destructor semantics to automatically
	deallocate an event upon end-of-life.  
	This helps the State::step method in which there are
	multiple early return cases.  
	Except, there's one case where we don't want to deallocate... 
		(exception)
 */
class State::event_deallocator {
	State&				_state;
	node_type& 			_node;
	const event_index_type		_event;
public:
	event_deallocator(State& s, node_type& n, const event_index_type e) :
		_state(s), _node(n), _event(e) { }
	~event_deallocator() {
		_state.__deallocate_event(_node, _event);
	}
} __ATTRIBUTE_UNUSED__ ;	// end class State::event_deallocator

//=============================================================================
// class State method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string
State::magic_string("hackt-prsim-ckpt");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates pull up/dn to output value.  
	Reminder enumerations for pull-state are defined in the event_type.
	Keep it consistent.  
 */
const char
State::pull_to_value[3][3] = {
{ node_type::LOGIC_OTHER, node_type::LOGIC_LOW, node_type::LOGIC_OTHER },
{ node_type::LOGIC_HIGH, node_type::LOGIC_OTHER, node_type::LOGIC_OTHER },
{ node_type::LOGIC_OTHER, node_type::LOGIC_OTHER, node_type::LOGIC_OTHER }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates simulation state, given a module.
	TODO: do this work in module?
	TODO: add support for top-level PRS (outside of procs)
	TODO: expression minimization pass
	\param m the expanded module object.
	\pre m must already be past the allcoate phase.  
 */
State::State(const entity::module& m, const ExprAllocFlags& f) : 
		mod(m), 
		node_pool(), expr_pool(), expr_graph_node_pool(),
		event_pool(), event_queue(), 
		rule_map(), 
		mk_exhi(), mk_exlo(), 
		exclhi_queue(), excllo_queue(), 
		pending_queue(), 
		check_exhi_ring_pool(1), check_exlo_ring_pool(1), 
		check_exhi(), check_exlo(), 
		current_time(0), 
		uniform_delay(time_traits::default_delay), 
		watch_list(), 
		flags(FLAGS_DEFAULT),
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
		unstable_policy(ERROR_DEFAULT_UNSTABLE),
		weak_unstable_policy(ERROR_DEFAULT_WEAK_UNSTABLE),
		interference_policy(ERROR_DEFAULT_INTERFERENCE),
		weak_interference_policy(ERROR_DEFAULT_WEAK_INTERFERENCE),
#endif
		timing_mode(TIMING_DEFAULT),
		ifstreams(), 
		__scratch_expr_trace() {
	const state_manager& sm(mod.get_state_manager());
	const global_entry_pool<bool_tag>&
		bool_pool(sm.get_pool<bool_tag>());
	expr_graph_node_pool.set_chunk_size(1024);
	head_sentinel();
	// recall, the global node pool is 1-indexed because entry 0 is null
	// we mirror this in our own node state pool, by allocating
	// the same number of elements.  
	const size_t s = bool_pool.size();
	node_pool.resize(s);

	// not expect expression-trees deeper than 8, but is growable
	__scratch_expr_trace.reserve(8);
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
	ExprAlloc v(*this, f);


	// this may throw an exception!
	sm.accept(v);
	// top-level prs in the module
	mod.get_footprint().get_prs_footprint().accept(v);
}	// end State::State(const module&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: possibly run some checks?
 */
State::~State() {
	// dequeue all events and check consistency with event pool 
	// upon its destruction.
	while (!event_queue.empty()) {
		const event_placeholder_type next(event_queue.pop());
		event_pool.deallocate(next.event_index);
	}
}

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
	STACKTRACE_VERBOSE;
	for_each(node_pool.begin(), node_pool.end(), 
		mem_fun_ref(&node_type::initialize));
	for_each(expr_pool.begin(), expr_pool.end(), 
		mem_fun_ref(&expr_type::initialize));
	// the expr_graph_node_pool contains no stateful information.  
	while (!event_queue.empty()) {
		const event_placeholder_type next(event_queue.pop());
		event_pool.deallocate(next.event_index);
	}
	INVARIANT(event_pool.check_valid_empty());
	fill(check_exhi_ring_pool.begin(), check_exhi_ring_pool.end(), false);
	fill(check_exlo_ring_pool.begin(), check_exlo_ring_pool.end(), false);
	flags |= FLAGS_INITIALIZE_SET_MASK;
	flags &= ~FLAGS_INITIALIZE_CLEAR_MASK;
	// unwatchall()? no, preserved
	// timing mode preserved
	current_time = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets transition counts of all nodes.  
 */
void
State::reset_tcounts(void) {
	STACKTRACE_VERBOSE;
	for_each(node_pool.begin(), node_pool.end(), 
		mem_fun_ref(&node_type::reset_tcount));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the state of simulation, as if it had just started up.  
	Preserve the watch/break point state.
	\pre expressions are already properly sized.  
	TODO: this unfortunately still preserves interpreter aliases.  
		The 'unalias-all' command should clear all aliases.
 */
void
State::reset(void) {
	STACKTRACE_VERBOSE;
	for_each(node_pool.begin(), node_pool.end(), 
		mem_fun_ref(&node_type::reset));
	for_each(expr_pool.begin(), expr_pool.end(), 
		mem_fun_ref(&expr_type::reset));
	// the expr_graph_node_pool contains no stateful information.  
	while (!event_queue.empty()) {
		const event_placeholder_type next(event_queue.pop());
		event_pool.deallocate(next.event_index);
	}
	INVARIANT(event_pool.check_valid_empty());
	fill(check_exhi_ring_pool.begin(), check_exhi_ring_pool.end(), false);
	fill(check_exlo_ring_pool.begin(), check_exlo_ring_pool.end(), false);
	flags = FLAGS_DEFAULT;
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	unstable_policy = ERROR_DEFAULT_UNSTABLE;
	weak_unstable_policy = ERROR_DEFAULT_WEAK_UNSTABLE;
	interference_policy = ERROR_DEFAULT_INTERFERENCE;
	weak_interference_policy = ERROR_DEFAULT_WEAK_INTERFERENCE;
#endif
	timing_mode = TIMING_DEFAULT;
	unwatch_all_nodes();
	uniform_delay = time_traits::default_delay;
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
	check_exhi_ring_pool.resize(FIRST_VALID_LOCK);
	check_exlo_ring_pool.resize(FIRST_VALID_LOCK);
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
/**
	Wipes out the indexed node, to mark as deallocated and free.  
	Such nodes are skipped during dump.  
	Only called by ExprAlloc.  
 */
void
State::void_expr(const expr_index_type ei) {
	expr_pool[ei].wipe();
	expr_graph_node_pool[ei].wipe();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::check_node(const node_index_type i) const {
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
	const expr_type& e(expr_pool[i]);
	const graph_node_type& g(expr_graph_node_pool[i]);
if (!e.wiped()) {
	// check parent
	assert(e.parent);
	if (e.is_root()) {
		assert(e.parent < node_pool.size());
		const node_type& n
			__ATTRIBUTE_UNUSED_CTOR__((node_pool[e.parent]));
		assert(n.get_pull_expr(e.direction()) == i);
	} else {
		assert(e.parent < expr_pool.size());
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
		assert(c.second);
		if (c.first) {		// points to leaf node
			assert(c.second < node_pool.size());
			assert(node_pool[c.second].contains_fanout(i));
		} else {		// points to expression
			assert(c.second < expr_pool.size());
			assert(expr_pool[c.second].parent == i);
			assert(expr_graph_node_pool[c.second].offset == j);
		}
	}
}	// else skip wiped node
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a node to the exclhi ring and flags the node as exclhi.  
	Also tags each node's exclusive flag.  
	Implemented as a swap for efficiency.  
	\param r is a set of nodes in an exclusive ring.  
 */
void
State::append_mk_exclhi_ring(ring_set_type& r) {
	typedef	ring_set_type::const_iterator	const_iterator;
	const_iterator i(r.begin()), e(r.end());
	for ( ; i!=e; ++i) {
		get_node(*i).make_exclhi();
	}
	mk_exhi.push_back(ring_set_type());
	mk_exhi.back().swap(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a node to the excllo ring and flags the node as excllo.  
	Also tags each node's exclusive flag.  
	Implemented as a swap for efficiency.  
	\param r is a set of nodes in an exclusive ring.  
 */
void
State::append_mk_excllo_ring(ring_set_type& r) {
	typedef	ring_set_type::const_iterator	const_iterator;
	const_iterator i(r.begin()), e(r.end());
	for ( ; i!=e; ++i) {
		get_node(*i).make_excllo();
	}
	mk_exlo.push_back(ring_set_type());
	mk_exlo.back().swap(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates an exclusive high checking ring.  
	For all nodes in set r, add index to newly allocated lock flag.  
	Also set check_exhi flags of listed nodes.  
 */
void
State::append_check_exclhi_ring(const ring_set_type& r) {
	typedef	ring_set_type::const_iterator	const_iterator;
	const lock_index_type j = check_exhi_ring_pool.size();
	check_exhi_ring_pool.push_back(false);
	const_iterator i(r.begin()), e(r.end());
	for ( ; i!=e; ++i) {
		const node_index_type ni = *i;
		get_node(ni).check_exclhi();
		check_exhi[ni].push_back(j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates an exclusive low checking ring.  
	For all nodes in set r, add index to newly allocated lock flag.  
	Also set check_exlo flags of listed nodes.  
 */
void
State::append_check_excllo_ring(const ring_set_type& r) {
	typedef	ring_set_type::const_iterator	const_iterator;
	const lock_index_type j = check_exlo_ring_pool.size();
	check_exlo_ring_pool.push_back(false);
	const_iterator i(r.begin()), e(r.end());
	for ( ; i!=e; ++i) {
		const node_index_type ni = *i;
		get_node(ni).check_excllo();
		check_exlo[ni].push_back(j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Like the old prsim's newevent macro.  
	Should be inline only.  
	\param n the reference to the node.
	\param ni the referenced node's index.
	\param c the index of the node (and value) that caused this event to 
		enqueue, may be INVALID_NODE_INDEX.
	\param ri the index rule/expression that caused this event to fire, 
		for the purposes of delay computation.
	\param val the future value of the node.
	\pre n must not already have a pending event.
	\pre n must be the node corresponding to node index ni
 */
event_index_type
State::__allocate_event(node_type& n,
		const node_index_type ni,
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		const event_cause_type& c,
#else
		const node_index_type ci, 
#endif
		const rule_index_type ri,
		const char val) {
	STACKTRACE_VERBOSE;
	INVARIANT(!n.pending_event());
	n.set_event(event_pool.allocate(
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		event_type(ni, c, ri, val)
#else
		event_type(ni, ci, ri, val)
#endif
		));
#if !PRSIM_ASSIGN_CAUSE_ON_DEQUEUE
	n.set_cause_node(ci);
#endif
	return n.get_event();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is used when creating a temporary event for
	inserting a pending event when checking for true interference.
	\param n the reference to the node.
	\param ni the referenced node's index.
	\param ci the index of the node that caused this event to enqueue, 
		may be INVALID_NODE_INDEX.
	\param next the next value of the node (from event).  
 */
event_index_type
State::__allocate_pending_interference_event(node_type& n,
		const node_index_type ni,
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		const event_cause_type& c,
#else
		const node_index_type ci,
#endif
		const char next) {
	STACKTRACE_VERBOSE;
	// node may or may not have pending event (?)
	// don't care about the node value
	const event_index_type ne = event_pool.allocate(
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		event_type(ni, c, INVALID_RULE_INDEX, next)
#else
		event_type(ni, ci, INVALID_RULE_INDEX, next)
#endif
		);
	get_event(ne).pending_interference(true);
	// n.set_event(ne);
#if !PRSIM_ASSIGN_CAUSE_ON_DEQUEUE
	n.set_cause_node(ci);
#endif
	return ne;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variant is used to enqueue a pre-constructed event, 
	useful for loading checkpoints.  
	NOTE: only use this for loading checkpoints!
 */
event_index_type
State::__load_allocate_event(const event_type& ev) {
	node_type& n(get_node(ev.node));
	n.load_event(event_pool.allocate(ev));
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
	STACKTRACE_VERBOSE_STEP;
	DEBUG_STEP_PRINT("freeing index " << i << endl);
	n.clear_event();
	event_pool.deallocate(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Special case event deallocation for a pending interfering event.  
	This is unrelated to the actual pending event of the affected node.  
	\param i the event index to return to the freelist. 
 */
void
State::__deallocate_pending_interference_event(const event_index_type i) {
	STACKTRACE_VERBOSE_STEP;
	DEBUG_STEP_PRINT("freeing index " << i << endl);
	// pending-interference event is not the same as 
	// the node's true pending event.  
	event_pool.deallocate(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just deallocates a killed event, called from dequeue_event.  
 */
void
State::__deallocate_killed_event(const event_index_type i) {
	STACKTRACE_VERBOSE_STEP;
	DEBUG_STEP_PRINT("freeing index " << i << endl);
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
	DEBUG_STEP_PRINT("enqueuing event ID " << ei <<
		" at time " << t << endl);
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
	DEBUG_STEP_PRINT("enqueuing exclhi ID " << ei <<
		" at time " << t << endl);
	exclhi_queue.push_back(event_placeholder_type(t, ei));
	get_node(event_pool[ei].node).set_excl_queue();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers event in the exclusive low event queue.  
	(I think this is an unordered worklist.)
 */
void
State::enqueue_excllo(const time_type t, const event_index_type ei) {
	INVARIANT(t >= current_time);
	DEBUG_STEP_PRINT("enqueuing excllo ID " << ei <<
		" at time " << t << endl);
	excllo_queue.push_back(event_placeholder_type(t, ei));
	get_node(event_pool[ei].node).set_excl_queue();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers event in the pending queue.  
	(I think this is an unordered worklist.)
 */
void
State::enqueue_pending(const event_index_type ei) {
	DEBUG_STEP_PRINT("enqueuing pending ID " << ei << endl);
	pending_queue.push_back(ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Fetches next event from the priority queue.  
	Automatically skips and deallocates killed events.  
	NOTE: possible that last event in queue is killed, 
		in which case, need to return a NULL placeholder.  
 */
State::event_placeholder_type
State::dequeue_event(void) {
	STACKTRACE_VERBOSE_STEP;
	event_placeholder_type ret(event_queue.pop());
	while (get_event(ret.event_index).killed()) {
		__deallocate_killed_event(ret.event_index);
		if (event_queue.empty()) {
			return event_placeholder_type(
				current_time, INVALID_EVENT_INDEX);
		} else {
			ret = event_queue.pop();
		}
	};
	return ret;
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
	\param f whether or not the setting is forced, thereby cancelling
		previous pending events, if false, then pending events 
		have precedence.  
	\return status: 0 is accepted, 1 is warning.  
 */
int
State::set_node_time(const node_index_type ni, const char val,
		const time_type t, const bool f) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("setting " << get_node_canonical_name(ni) <<
		" to " << node_type::value_to_char[size_t(val)] <<
		" at " << t << endl);
	// we have ni = the canonically allocated index of the bool node
	// just look it up in the node_pool
	node_type& n(get_node(ni));
	const event_index_type pending = n.get_event();
	const char last_val = n.current_value();
	const bool unchanged = (val == last_val);
// If the value is the same as former value, then ignore it.
// What if delay differs?
// TODO: could invalidate and re-enqueue with min. time, e.g.
//	if (val == last_val) { return ENQUEUE_ACCEPT; }
// If node has pending even in queue already, warn and ignore.
if (pending) {
	// does it matter whether or not last_val == val?
	const string objname(get_node_canonical_name(ni));
	if (f) {
		// doesn't matter what what last_val was, override it
		// even if value is the same, reschedule it
		// cancel former event, but don't deallocate it until dequeued
		cout << "WARNING: pending event for node `" << objname <<
			"\' was overridden." << endl;
		get_event(pending).kill();
		n.clear_event();
	} else if (!unchanged) {
		// not forcing: if new value is different, issue warning
		cout << "WARNING: pending value for node `" << objname <<
			"\'; ignoring request" << endl;
		return ENQUEUE_WARNING;
	} else {
		// ignore
		return ENQUEUE_ACCEPT;
	}
} else if (unchanged) {
	// drop vacuous sets
	return ENQUEUE_ACCEPT;
}
	// otherwise, allocate and enqueue the event.  
	const event_index_type ei =
		// node cause to assign, since this is externally set
		__allocate_event(n, ni, 
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
			event_cause_type(), 
#else
			INVALID_NODE_INDEX, 
#endif
			INVALID_RULE_INDEX, val);
#if 0
	const event_type& e(get_event(ei));
	STACKTRACE_INDENT_PRINT("new event: (node,val)" << endl);
#endif
	if (f) {	// what if nothing was pending?
		// mark event as forced
		get_event(ei).force();
	}
	enqueue_event(t, ei);
	return ENQUEUE_ACCEPT;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In the case of nodes stuck because the user coercively set a 
	node, this causes the named node to be re-evaluated in terms
	of its current pull-state of its fanins which may not have changed
	since the last time.  
	If the appropriate event is already pending, this does nothing.  
	If there should be an event pending (possibly cancelled or overridden
	by a set-force, this will correct it, undoing the coercion.  
	This should be a "safe" command -- nothing should ever go wrong.  
	\param ni the index of the node to re-evaluate.  
 */
void
State::unset_node(const node_index_type ni) {
	STACKTRACE_VERBOSE;
	node_type& n(get_node(ni));
	const event_index_type pending = n.get_event();
	// evaluate node's pull-up and pull-down
	const expr_index_type u = n.pull_up_index;
	const expr_index_type d = n.pull_dn_index;
	// if there is no pull-*, it's as good as off!
	const char pu =
		(u ? expr_pool[u].pull_state() : char(expr_type::PULL_OFF));
	const char pd =
		(d ? expr_pool[d].pull_state() : char(expr_type::PULL_OFF));
if (pu != expr_type::PULL_OFF || pd != expr_type::PULL_OFF) {
	const char new_val = pull_to_value[size_t(pu)][size_t(pd)];
	if (pending) {
		event_type& e(get_event(pending));
		if (e.val != new_val) {
			cerr << "Overriding pending event\'s value on node `"
				<< get_node_canonical_name(ni) << "\' from " <<
				node_type::char_to_value(e.val) << " to " <<
				node_type::char_to_value(new_val) <<
				", keeping the same event time." << endl;
			e.val = new_val;
		}
		e.unforce();
		// else do nothing, correct value already pending
	} else {
		// no event pending, can make one
		const char current = n.current_value();
		if (current != new_val) {
#if 0
			rule_index_type ri;
			// not true, rule index may be from OR-combination...
			if (pu == char(expr_type::PULL_ON) &&
				pd == char(expr_type::PULL_OFF)) {
				ri = u;
			} else if (pd == char(expr_type::PULL_ON) &&
				pu == char(expr_type::PULL_OFF)) {
				ri = d;
			} else {
				ri = INVALID_RULE_INDEX;
			}
#else
			const rule_index_type ri = INVALID_RULE_INDEX;
			// a real rule index would help determine the delay
			// but we don't know which delay in a multi-delay
			// fanin to use!  Passing INVALID_RULE_INDEX
			// will use a delay of 0.  
#endif
			const event_index_type ei = __allocate_event(
				n, ni,
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
					event_cause_type(), 
#else
					INVALID_NODE_INDEX, // no cause
#endif
					ri, new_val);
			event_type& e(get_event(ei));
			time_type t;
			switch (new_val) {
			case node_type::LOGIC_HIGH:
				t = get_delay_up(e); break;
			case node_type::LOGIC_LOW:
				t = get_delay_dn(e); break;
			default: t = delay_policy<time_type>::zero;
			}
			enqueue_event(t, ei);
		}
		// else node is already in correct state
	}
}	// else neither side is pulling, leave node as is
}	// end State::unset_node

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Applies unset_node to all nodes.  
 */
void
State::unset_all_nodes(void) {
	node_index_type ni = FIRST_VALID_NODE;
	const node_index_type s = node_pool.size();
	for ( ; ni < s; ++ni) {
		unset_node(ni);
	}
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
/**
	Lists all nodes marked as breakpoints.  
 */
ostream&
State::dump_breakpoints(ostream& o) const {
	typedef	node_pool_type::const_iterator		const_iterator;
	const const_iterator b(node_pool.begin()), e(node_pool.end());
	const_iterator i(b);
	o << "breakpoints: ";
	for (++i; i!=e; ++i) {
	if (i->is_breakpoint()) {
		const node_index_type ni = distance(b, i);
		const watch_list_type::const_iterator
			f(watch_list.find(ni));
		/**
			If not found in the watchlist, or
			is found and also flagged as breakpoint, 
			then we have a true breakpoint.  
		 */
		if (f == watch_list.end() || f->second.breakpoint) {
			o << get_node_canonical_name(ni) << ' ';
		}
	}	// end if is_breakpoint
	}	// end for-all nodes
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
const char*
State::error_policy_string(const error_policy_enum e) {
	switch (e) {
	case ERROR_IGNORE:	return "ignore";
	case ERROR_WARN:	return "warn";
	case ERROR_BREAK:	return "break";
	default:		DIE;
	}	// end switch
	 return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Too lazy to write a map...
 */
State::error_policy_enum
State::string_to_error_policy(const string& s) {
	static const string _ignore("ignore");
	static const string _warn("warn");
	static const string _notify("notify");
	static const string _break("break");
	if (s == _ignore) {
		return ERROR_IGNORE;
	} else if (s == _warn || s == _notify) {
		return ERROR_WARN;
	} else if (s == _break) {
		return ERROR_BREAK;
	}
	// else
	return ERROR_INVALID;
}

#endif	// PRSIM_FINE_GRAIN_ERROR_CONTROL

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_mode(ostream& o) const {
	o << "mode: ";
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	o << "\tunstable events " <<
		(dequeue_unstable_events() ? "are dequeued" : "propagate Xs")
			<< endl;
	o << "\ton unstable: " <<
		error_policy_string(unstable_policy) << endl;
	o << "\ton weak-unstable: " <<
		error_policy_string(weak_unstable_policy) << endl;
	o << "\ton interference: " <<
		error_policy_string(interference_policy) << endl;
	o << "\ton weak-interference: " <<
		error_policy_string(weak_interference_policy) << endl;
#else
	if (flags & FLAG_NO_WEAK_INTERFERENCE)
		o << "reset (no weak-interference)";
	else	o << "run (with weak-interference)";
	return o << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_timing(ostream& o) const {
	o << "timing: ";
switch (timing_mode) {
	case TIMING_RANDOM:	o << "random";	break;
	case TIMING_UNIFORM:
		o << "uniform (" << uniform_delay << ")";
		break;
	case TIMING_AFTER:	o << "after";	break;
	default:		o << "unknown";
}
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if there is a syntax error.  
	TODO: use a map to parsers.  
	TODO: use random seed.  
 */
bool
State::set_timing(const string& m, const string_list& a) {
	static const string __random("random");
	static const string __uniform("uniform");
	static const string __after("after");
	if (m == __random) {
		timing_mode = TIMING_RANDOM;
		// TODO: use random seed
		switch (a.size()) {
		case 0:	return false;
		case 1:	FINISH_ME(Fang);
			cerr << "TODO: plant random seed." << endl;
			return false;
		default:	return true;
		}
	} else if (m == __uniform) {
		timing_mode = TIMING_UNIFORM;
		switch (a.size()) {
		case 0: return false;
		case 1: {
			return string_to_num(a.front(), uniform_delay);
		}
		default:
			return true;
		}
	} else if (m == __after) {
		timing_mode = TIMING_AFTER;
		return a.size();
	} else {
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::help_timing(ostream& o) {
	o << "available timing modes:" << endl;
	o << "\trandom [seed]" << endl;
	o << "\tuniform [delay]" << endl;
	o << "\tafter" << endl;
	return o;
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
	NOTE: possible reasons for null e.cause_rule:
		due to exclhi/exclo ring enforcement?
	NOTE: event's cause_rule is not checkpointed.  
 */
// inline
State::time_type
State::get_delay_up(const event_type& e) const {
return current_time +
	(timing_mode == TIMING_RANDOM ?
		(e.cause_rule && time_traits::is_zero(
				rule_map.find(e.cause_rule)->second.after) ?
			time_traits::zero : random_delay())
		:
	(timing_mode == TIMING_UNIFORM ? uniform_delay :
	// timing_mode == TIMING_AFTER
		(e.cause_rule ?
			rule_map.find(e.cause_rule)->second.after : 0)
	));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: event's cause_rule is not checkpointed.  
	\return absolute time of scheduled pull-down event.
 */
// inline
State::time_type
State::get_delay_dn(const event_type& e) const {
	return current_time +
		(timing_mode == TIMING_RANDOM ?
		(e.cause_rule && time_traits::is_zero(
				rule_map.find(e.cause_rule)->second.after) ?
			time_traits::zero : random_delay())
			:
		(timing_mode == TIMING_UNIFORM ? uniform_delay :
		// timing_mode == TIMING_AFTER
			(e.cause_rule ?
				rule_map.find(e.cause_rule)->second.after : 0)
		));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if expression is a root-level expression
	(and hence, has attributes)
 */
bool
State::is_rule_expr(const expr_index_type ei) const {
	return rule_map.find(ei) != rule_map.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	As rules are evaluated and propagated, events may be enqueued
	onto the pending queue.
	This subroutine processes the pending queue events until cleared.  
	Only called by step() member function.  
 */
State::break_type
State::flush_pending_queue(void) {
	typedef	pending_queue_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE_STEP;
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	break_type err = false;
#endif
	const_iterator i(pending_queue.begin()), e(pending_queue.end());
for ( ; i!=e; ++i) {
	const event_index_type ne = *i;
	DEBUG_STEP_PRINT("checking pending event ID: " << ne << endl);
	event_type& ev(get_event(ne));
	const node_index_type& _ni(ev.node);
	DEBUG_STEP_PRINT("... on node " <<
		get_node_canonical_name(_ni) << endl);
	node_type& _n(get_node(_ni));
	const char pull_up_state = expr_pool[_n.pull_up_index].pull_state();
	const char pull_dn_state = expr_pool[_n.pull_dn_index].pull_state();
	DEBUG_STEP_PRINT("current pull-states: up=" <<
		size_t(pull_up_state) << ", dn=" <<
		size_t(pull_dn_state) << endl);
	if ((pull_up_state != expr_type::PULL_OFF) &&
		(pull_dn_state != expr_type::PULL_OFF)) {
	/***
		There is interference.  If there is weak interference,
		suppress report unless explicitly requested.  
		weak = (X && T) or (T && X);
	***/
		DEBUG_STEP_PRINT("some interference." << endl);
		const bool pending_weak =
			(pull_up_state == expr_type::PULL_WEAK) ||
			(pull_dn_state == expr_type::PULL_WEAK);
			// not XOR (^), see pending_weak table in prs.c
		// issue diagnostic
		if (
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
			(weak_interference_policy != ERROR_IGNORE) ||
#else
			!(flags & FLAG_NO_WEAK_INTERFERENCE) ||
#endif
				!pending_weak) {
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
			err |=
#endif
			__report_interference(cout, pending_weak, _ni, ev);
		}
		if (ev.pending_interference()) {
			INVARIANT(_n.pending_event());
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
			_n.set_value_and_cause(node_type::LOGIC_OTHER, 
				ev.cause);
#else
			_n.set_cause_node(ev.cause_node);
			_n.set_value(node_type::LOGIC_OTHER);
#endif
			__deallocate_pending_interference_event(ne);
		} else {
			ev.val = node_type::LOGIC_OTHER;
			__flush_pending_event_with_interference(_n, ne, ev);
		}	// end if pending_interference
	} else {
		DEBUG_STEP_PRINT("no interference." << endl);
		const event_index_type pe = _n.get_event();
		DEBUG_STEP_PRINT("prior enqueued event on this node (possibly killed): " <<
			pe << endl);
		if (pe && UNLIKELY(pe != ne)) {
			// then one of these events must be tossed!
			// this arises as a result of instabilities
			// and the need to cancel events
			DEBUG_STEP_PRINT("node has enqueued different event "
				<< pe << " vs. this one " << ne << endl);
			event_type& pv(get_event(pe));
			// walk event_queue, find delay of the former event?
			// inspect event values?
			if (dequeue_unstable_events()) {
			DEBUG_STEP_PRINT("replacing original event" << endl);
				// new behavior: cancel the original event
				// which was updated to X
				_n.clear_event();
				INVARIANT(pv.killed());
				_n.set_event(ne);
				__flush_pending_event_replacement(_n, ne, ev);
			} else {
			DEBUG_STEP_PRINT("keeping original event" << endl);
				// original behavior: drop new event
				// b/c original event updated to value X
				__deallocate_pending_interference_event(ne);
				// leave original event alone
			}
		} else {	// LIKELY(pe == ne)
			__flush_pending_event_no_interference(_n, ne, ev);
		}	// end if (pe != ne)
	}	// end if may_be_pulled ...
}	// end for all in pending_queue
	pending_queue.clear();	// or .resize(0), same thing
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	return err;
#endif
}	// end method flush_pending_queue

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For the sake of shortening long code.  
 */
// inline
void
State::__flush_pending_event_with_interference(node_type& _n, 
		const event_index_type ne, event_type& ev) {
	switch (_n.current_value()) {
	case node_type::LOGIC_LOW:
	DEBUG_STEP_PRINT("moving - event to event queue" << endl);
		enqueue_event(get_delay_dn(ev), ne);
		break;
	case node_type::LOGIC_HIGH:
	DEBUG_STEP_PRINT("moving + event to event queue" << endl);
		enqueue_event(get_delay_up(ev), ne);
		break;
	case node_type::LOGIC_OTHER:
	DEBUG_STEP_PRINT("cancelling event" << endl);
		_n.clear_excl_queue();
		__deallocate_event(_n, ne);
		break;
	default:
		ICE(cerr, 
			cerr << "Invalid logic value." << endl;
		);
	}	// end switch
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For the sake of shortening long code.  
 */
// inline
void
State::__flush_pending_event_no_interference(node_type& _n, 
		const event_index_type ne, event_type& ev) {
	if (_n.current_value() != ev.val) {
		if (ev.val == node_type::LOGIC_HIGH) {
		DEBUG_STEP_PRINT("moving + event to event queue" << endl);
			enqueue_event(get_delay_up(ev), ne);
		} else {
		DEBUG_STEP_PRINT("moving - event to event queue" << endl);
			enqueue_event(get_delay_dn(ev), ne);
		}
	} else {
		DEBUG_STEP_PRINT("cancelling event" << endl);
		// no change in value, just cancel
		_n.clear_excl_queue();
		__deallocate_event(_n, ne);
	}	// end switch
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For the sake of shortening long code.  
 */
// inline
void
State::__flush_pending_event_replacement(node_type& _n, 
		const event_index_type ne, event_type& ev) {
	switch (ev.val) {
	case node_type::LOGIC_LOW:
	DEBUG_STEP_PRINT("moving - event to event queue" << endl);
		enqueue_event(get_delay_dn(ev), ne);
		break;
	case node_type::LOGIC_HIGH:
	DEBUG_STEP_PRINT("moving + event to event queue" << endl);
		enqueue_event(get_delay_up(ev), ne);
		break;
	case node_type::LOGIC_OTHER:
	DEBUG_STEP_PRINT("don't know what to do!" << endl);
		FINISH_ME(Fang);
		enqueue_event(get_delay_up(ev), ne);
		// _n.clear_excl_queue();
		// __deallocate_event(_n, ne);
		break;
	default:
		ICE(cerr, 
			cerr << "Invalid logic value." << endl;
		);
	}	// end switch
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Processes exclusive high ring enqueued events.  
	Place such events into the primary event queue if
		it does not violate any exclusive directives.  
 */
void
State::flush_exclhi_queue(void) {
	typedef	mk_excl_queue_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE_STEP;
	const_iterator i(exclhi_queue.begin()), e(exclhi_queue.end());
for ( ; i!=e; ++i) {
	const event_placeholder_type& ep(*i);
	const node_index_type epni = get_event(ep.event_index).node;
	node_type& epn(get_node(epni));
	/***
		Look through events: if any of them have a pending
		queue entry, then we're done (?)
	***/
	typedef	mk_excl_ring_map_type::const_iterator ring_iterator;
	ring_iterator ri(mk_exhi.begin()), re(mk_exhi.end());
	for ( ; ri!=re; ++ri) {
		/**
			TODO: rewite with set lookup to replace flag conditional
			flag just detects whether or not node member was found
		 */
		size_t prev = 0;
		bool flag = false;
		typedef	ring_set_type::const_iterator	element_iterator;
		element_iterator ii(ri->begin()), ie(ri->end());
		for ( ; ii!=ie; ++ii) {
			const node_index_type ni = *ii;
			const node_type& n(get_node(ni));
#if DEBUG_STEP
		DEBUG_STEP_PRINT("examining node: " <<
			get_node_canonical_name(ni) << endl);
		DEBUG_STEP_PRINT("n.val == " << size_t(n.current_value())
			<< endl);
		if (n.pending_event()) {
			DEBUG_STEP_PRINT("event.val == " <<
				size_t(event_pool[n.get_event()].val) << endl);
			DEBUG_STEP_PRINT("n.in_excl_queue == " <<
				size_t(n.in_excl_queue()) << endl);
		}
#endif
			if (n.current_value() == node_type::LOGIC_HIGH ||
				(n.pending_event() && 
				(event_pool[n.get_event()].val ==
					node_type::LOGIC_HIGH) &&
				!n.in_excl_queue())) {
				DEBUG_STEP_PRINT("++prev" << endl);
				++prev;
			}
			if (ni == epni) {
				DEBUG_STEP_PRINT("flag=true" << endl);
				flag = true;
			}
		}
		if (flag && !prev) {
		DEBUG_STEP_PRINT("enqueuing event" << endl);
			// then insert event into primary queue
			// keep the same event_index
			enqueue_event(ep.time, ep.event_index);
			epn.clear_excl_queue();
		}
	}	// end for all excl ring nodes
	if (epn.in_excl_queue()) {
		DEBUG_STEP_PRINT("cancelling event" << endl);
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
	typedef	mk_excl_queue_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE_STEP;
	const_iterator i(excllo_queue.begin()), e(excllo_queue.end());
for ( ; i!=e; ++i) {
	const event_placeholder_type& ep(*i);
	const node_index_type epni = get_event(ep.event_index).node;
	node_type& epn(get_node(epni));
	/***
		Look through events: if any of them have a pending
		queue entry, then we're done (?)
	***/
	typedef	mk_excl_ring_map_type::const_iterator ring_iterator;
	ring_iterator ri(mk_exlo.begin()), re(mk_exlo.end());
	for ( ; ri!=re; ++ri) {
		/**
			TODO: rewite with set lookup to replace flag conditional
			flag just detects whether or not node member was found
		 */
		size_t prev = 0;
		bool flag = false;
		typedef	ring_set_type::const_iterator	element_iterator;
		element_iterator ii(ri->begin()), ie(ri->end());
		for ( ; ii!=ie; ++ii) {
			const node_index_type ni = *ii;
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
		}
		if (flag && !prev) {
			DEBUG_STEP_PRINT("enqueuing event" << endl);
			// then insert event into primary queue
			// keep the same event_index
			enqueue_event(ep.time, ep.event_index);
			epn.clear_excl_queue();
		}
	}	// end for all excl ring nodes
	if (epn.in_excl_queue()) {
		DEBUG_STEP_PRINT("cancelling event" << endl);
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
	\param ni index of the node that changed that affects exclhi rings.  
 */
void
State::enforce_exclhi(
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		const event_cause_type& c
#else
		const node_index_type ni
#endif
		) {
	/***
		If n.exclhi and n is 0, check if any of the nodes
		in n's exclhi rings are enabled.  
		If so, insert them into the exclhi-queue.
	***/
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE_STEP;
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
	const node_index_type& ni(c.node);
#endif
	const_iterator i(mk_exhi.begin()), e(mk_exhi.end());
for ( ; i!=e; ++i) {
	typedef	std::iterator_traits<const_iterator>::value_type::const_iterator
								set_iter;
	const set_iter si(i->find(ni));
	if (si != i->end()) {
		set_iter ii(i->begin()), ie(i->end());
		for ( ; ii!=ie; ++ii) {
		if (ii!=si) {
			const node_index_type eri = *ii;
			node_type& er(get_node(eri));
			if (!er.pending_event() && er.pull_up_index &&
				// er->n->up->val == PRS_VAL_T
				// what if is pulling weakly?
				expr_pool[er.pull_up_index].pull_state()
					== expr_type::PULL_ON) {
			DEBUG_STEP_PRINT("enqueuing pull-up event" << endl);
				const event_index_type ne =
					// the pull-up index may not necessarily
					// correspond to the causing expression!
					__allocate_event(er, eri,
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
						c,
#else
						ni, 
#endif
						// not sure...
						// er.pull_up_index, 
						INVALID_RULE_INDEX, 
						node_type::LOGIC_HIGH);
				// ne->cause = ni
				enqueue_exclhi(get_delay_up(get_event(ne)), ne);
			}
		}	// end if (si != ii)
		}	// end for all set members
	}	// end if found member in ring
}	// end for (all exclhi members)
}	// end method enforce_exclhi()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enforces exclusive low rings by enqueue necessary events 
	into the exclusive low queue.  
	\param ni index of the node that changed that affects excllo rings.  
 */
void
State::enforce_excllo(
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		const event_cause_type& c
#else
		const node_index_type ni
#endif
		) {
	/***
		If n.excllo and n is 1, check if any of the nodes
		in n's excllo rings are enabled.  
		If so, insert them into the excllo-queue.
	***/
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE_STEP;
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
	const node_index_type& ni(c.node);
#endif
	const_iterator i(mk_exlo.begin()), e(mk_exlo.end());
for ( ; i!=e; ++i) {
	typedef	std::iterator_traits<const_iterator>::value_type::const_iterator
								set_iter;
	const set_iter si(i->find(ni));
	if (si != i->end()) {
		set_iter ii(i->begin()), ie(i->end());
		for ( ; ii!=ie; ++ii) {
		if (ii!=si) {
			const node_index_type eri = *ii;
			node_type& er(get_node(eri));
			if (!er.pending_event() && er.pull_dn_index &&
				// er->n->dn->val == PRS_VAL_T
				// what if is pulling weakly?
				expr_pool[er.pull_dn_index].pull_state()
					== expr_type::PULL_ON) {
			DEBUG_STEP_PRINT("enqueuing pull-dn event" << endl);
				const event_index_type ne =
					// same comment as enforce_exclhi
					__allocate_event(er, eri,
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
						c, 
#else
						ni, 
#endif
						// er.pull_dn_index, 
						INVALID_RULE_INDEX,
						node_type::LOGIC_LOW);
				// ne->cause = ni
				enqueue_excllo(get_delay_dn(get_event(ne)), ne);
			}
		}	// end if (si != ii)
		}	// end for all set members
	}	// end if found member in ring
}	// end for (all excllo members)
}	// end method enforce_excllo

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lock and unlock rings.  
	Before node is set, check state of rings for mutex.  
	\param ni index of changing node.
	\param n the node corresponding to ni
	\param prev former value
	\param next upcoming value
 */
State::excl_exception
State::check_excl_rings(const node_index_type ni, const node_type& n, 
		const char prev, const char next) {
	typedef	check_excl_ring_map_type::const_iterator	const_iterator;
	typedef	lock_index_list_type::const_iterator	lock_index_iterator;
	typedef	check_excl_lock_pool_type::reference	lock_reference;
	if (n.has_check_exclhi()) {
	if (next == node_type::LOGIC_HIGH) {
		// need to lock
		const const_iterator f(check_exhi.find(ni));
		INVARIANT(f != check_exhi.end());
		// make sure all locks are free
		lock_index_iterator
			i(f->second.begin()), e(f->second.end());
		for ( ; i!=e; ++i) {
			const lock_index_type li = *i;
			INVARIANT(li);
			INVARIANT(li < check_exhi_ring_pool.size());
			lock_reference l(check_exhi_ring_pool[li]);
			// set all locks
			if (l) {
				// identify lock ID, and node ID
				return excl_exception(true, li, ni);
			} else {
				l = true;	// lock ring
			}
		}
	} else if (prev == node_type::LOGIC_HIGH && next != prev) {
		// need to unlock
		const const_iterator f(check_exhi.find(ni));
		INVARIANT(f != check_exhi.end());
		// make sure all locks are free
		lock_index_iterator
			i(f->second.begin()), e(f->second.end());
		for ( ; i!=e; ++i) {
			const lock_index_type li = *i;
			INVARIANT(li);
			INVARIANT(li < check_exhi_ring_pool.size());
			// unlock ring
			check_exhi_ring_pool[li] = false;
		}
	}
	}	// end if n.has_check_exclhi()
	if (n.has_check_excllo()) {
	if (next == node_type::LOGIC_LOW) {
		// need to lock
		const const_iterator f(check_exlo.find(ni));
		INVARIANT(f != check_exlo.end());
		// make sure all locks are free
		lock_index_iterator
			i(f->second.begin()), e(f->second.end());
		for ( ; i!=e; ++i) {
			const lock_index_type li = *i;
			INVARIANT(li);
			INVARIANT(li < check_exlo_ring_pool.size());
			lock_reference l(check_exlo_ring_pool[li]);
			// set all locks
			if (l) {
				// identify lock ID, and node ID
				return excl_exception(false, li, ni);
			} else {
				l = true;	// lock ring
			}
		}
	} else if (prev == node_type::LOGIC_LOW && next != prev) {
		// need to unlock
		const const_iterator f(check_exlo.find(ni));
		INVARIANT(f != check_exlo.end());
		// make sure all locks are free
		lock_index_iterator
			i(f->second.begin()), e(f->second.end());
		for ( ; i!=e; ++i) {
			const lock_index_type li = *i;
			INVARIANT(li);
			INVARIANT(li < check_exlo_ring_pool.size());
			// unlock ring
			check_exlo_ring_pool[li] = false;
		}
	}
	}	// end if n.has_check_excllo()
	// if this point reached, we're good
	return excl_exception(true, INVALID_LOCK_INDEX, INVALID_NODE_INDEX);
}	// end method State::check_excl_rings()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Diagnostic subroutine for dissecting excl violation exceptions.  
	Uses extremely slow search because this only occurs on exception.  
 */
void
State::inspect_excl_exception(const excl_exception& exex, ostream& o) const {
	typedef	check_excl_ring_map_type::const_iterator	const_iterator;
	ring_set_type ring;
	const_iterator i, e;
	if (exex.type) {
		i = check_exhi.begin();
		e = check_exhi.end();
	} else {
		i = check_exlo.begin();
		e = check_exlo.end();
	}
	// find all nodes that contain the lock index
	for ( ; i!=e; ++i) {
		const lock_index_list_type::const_iterator
			le(i->second.end()), 
			lf(find(i->second.begin(), le, exex.lock_id));
		if (lf != le) {
			// then this node_index belongs to this ring
			ring.insert(i->first);
		}
	}
	INVARIANT(ring.size() > 1);
	o << "ERROR: excl" << (exex.type ? "hi" : "lo") << 
		" violation detected!" << endl;
	ring_set_type::const_iterator ri(ring.begin()), re(ring.end());
	o << "ring-state:" << endl;
	for (; ri!=re; ++ri) {
		o << "\t" << get_node_canonical_name(*ri) << " : ";
		get_node(*ri).dump_value(o) << endl;
	}
	o << "but node `" << get_node_canonical_name(exex.node_id) <<
		"\' tried to become " << (exex.type ? 1 : 0) << "." << endl;
	o << "The simulator state is no longer coherent; "
		"do not bother trying to continue the simulation, "
		"but you may further inspect the state." << endl;
}	// end method State::inspect_excl_exception

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Modeled after prs_step() from the original prsim.  
	Critical path through here.  
	TODO: possible add arguments later. 
	\pre the exclhi, excllo, and pending queues are empty.
	\post the exclhi, excllo, and pending queues are empty.
	\return index of the affected node, 
		INVALID_NODE_INDEX if nothing happened.  
	\throw excl_exception if there is a logical exclusion violation, 
		leaving violating event in queue.  
 */
State::step_return_type
State::step(void) THROWS_EXCL_EXCEPTION {
	typedef	State::step_return_type		return_type;
	STACKTRACE_VERBOSE;
	INVARIANT(pending_queue.empty());
	INVARIANT(exclhi_queue.empty());
	INVARIANT(excllo_queue.empty());
	if (event_queue.empty()) {
		return return_type(INVALID_NODE_INDEX, INVALID_NODE_INDEX);
	}
	const event_placeholder_type ep(dequeue_event());
	current_time = ep.time;
	DEBUG_STEP_PRINT("time = " << current_time << endl);
	const event_index_type& ei(ep.event_index);
	if (!ei) {
		// possible in the event that last events are killed
		return return_type(INVALID_NODE_INDEX, INVALID_NODE_INDEX);
	}
	DEBUG_STEP_PRINT("event_index = " << ei << endl);
	const event_type& pe(get_event(ei));
	const bool force = pe.forced();
	const node_index_type ni = pe.node;
	node_type& n(get_node(ni));
	const char prev = n.current_value();
	node_index_type _ci;	// just a copy
{
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
	const event_cause_type& cause(pe.cause);
	const node_index_type& ci(cause.node);
#else
	const node_index_type ci = pe.cause_node;
#endif
	_ci = ci;
	DEBUG_STEP_PRINT("examining node: " <<
		get_node_canonical_name(ni) << endl);
{
	// event-deallocation scope (optional)
	// const event_deallocator __d(*this, n, ei);	// auto-deallocate?
	DEBUG_STEP_PRINT("former value: " <<
		node_type::value_to_char[size_t(prev)] << endl);
	DEBUG_STEP_PRINT("new value: " <<
		node_type::value_to_char[size_t(pe.val)] << endl);
	if (pe.val == node_type::LOGIC_OTHER &&
		prev == node_type::LOGIC_OTHER) {
		// node being set to X, but is already X, this could occur
		// b/c there are other causes of X besides guards going X.
		DEBUG_STEP_PRINT("X: returning node index " << ni << endl);
		__deallocate_event(n, ei);
		return return_type(ni, ci);
	}
	// assert: vacuous firings on the event queue, 
	// but ONLY if unstable events don't cause vacuous events to
	// be resheduled, e.g. pulse
	if (dequeue_unstable_events()) {
		if (UNLIKELY(prev == pe.val)) {
			// Q: or is it better to catch this before enqueuing?
			__deallocate_event(n, ei);
			// then just silence this event
			// slow head-recusrion, but infrequent
			return step();
		}
		// else proceed
	} else {
		// vacuous event is allowed if set was forced by user
		INVARIANT(prev != pe.val || n.is_unstab() || force);
	}
	// saved previous value above already
	if (checking_excl()) {
		const excl_exception
			exex(check_excl_rings(ni, n, prev, pe.val));
		if (UNLIKELY(exex.lock_id)) {
			// to keep event queue coherent, re-enqueue event
			// because event will not be deallocated!
			// next attempt to step will hit same exception
			// forcing simulation to be stuck (intentional)
			enqueue_event(ep.time, ep.event_index);
			throw exex;
		}
	}
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
	n.set_value_and_cause(pe.val, cause);
#else
#if PRSIM_ASSIGN_CAUSE_ON_DEQUEUE
	n.set_cause_node(ci);
#endif
	n.set_value(pe.val);
#endif
	// count transition only if new value is not X
	if (pe.val != node_type::LOGIC_OTHER) {
		++n.tcount;
	}
	 __deallocate_event(n, ei);
}
}
	// note: pe is invalid, deallocated beyond this point, could scope it
	// reminder: do not reference pe beyond this point (deallocated)
	// could scope the reference to prevent it...
	const char next = n.current_value();
	// value propagation...
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
	const event_cause_type new_cause(ni, next);
#endif
{
	typedef	node_type::const_fanout_iterator	const_iterator;
	const_iterator i(n.fanout.begin()), e(n.fanout.end());
	for ( ; i!=e; ++i) {
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		if (UNLIKELY(propagate_evaluation(new_cause, *i, prev, next))) {
			stop();
		}
#else
		if (UNLIKELY(propagate_evaluation(ni, *i, prev, next))) {
			stop();
		}
#endif
#else
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		propagate_evaluation(new_cause, *i, prev, next);
#else
		propagate_evaluation(ni, *i, prev, next);
#endif
#endif
	}
}
	/***
		If an event is forced (say, by user), then check node's own
		guards to determine whether or not a new event needs to
		be registered on this node.  
	***/
	if (force && n.get_event()) {
		DEBUG_STEP_PRINT("detected a forced event vs. pending event" << endl);
		if (n.pull_up_index && 
			get_event(n.pull_up_index).val == node_type::LOGIC_HIGH
				&& next != node_type::LOGIC_HIGH) {
			DEBUG_STEP_PRINT("force pull-up" << endl);
			const event_index_type _ne =
				__allocate_event(n, ni, 
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
					event_cause_type(), 
#else
					INVALID_NODE_INDEX,	// no cause
#endif
					n.pull_up_index, 	// cause?
					node_type::LOGIC_HIGH);
			enqueue_pending(_ne);
		}
		else if (n.pull_dn_index && 
			get_event(n.pull_dn_index).val == node_type::LOGIC_LOW
				&& next != node_type::LOGIC_LOW) {
			DEBUG_STEP_PRINT("force pull-dn" << endl);
			const event_index_type _ne =
				__allocate_event(n, ni, 
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
					event_cause_type(), 
#else
					INVALID_NODE_INDEX,	// no cause
#endif
					n.pull_dn_index, 	// cause?
					node_type::LOGIC_LOW);
			enqueue_pending(_ne);
		}
		// no weak events yet
	}

	// exclhi ring enforcement
	if (n.has_mk_exclhi() && (next == node_type::LOGIC_LOW)) {
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		enforce_exclhi(new_cause);
#else
		enforce_exclhi(ni);
#endif
	}	// end if (exclhi enforcement)

	// excllo ring enforcement
	if (n.has_mk_excllo() && (next == node_type::LOGIC_HIGH)) {
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		enforce_excllo(new_cause);
#else
		enforce_excllo(ni);
#endif
	}	// end if (excllo enforcement)

	// energy estimation?  TODO later for a different sim variant

	// check and flush pending queue, spawn fanout events
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	if (UNLIKELY(flush_pending_queue())) {
		stop();
	}
#else
	flush_pending_queue();
#endif

	// check and flush pending queue against exclhi/lo events
	flush_exclhi_queue();
	flush_excllo_queue();

	// return the affected node's index
	DEBUG_STEP_PRINT("returning node index " << ni << endl);
	return return_type(ni, _ci);
}	// end method step()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	This is code is invoked when cancelling events due to
	instablity.  This will also enqueue new events in some cases where
	schedule events already fired.  

	This code is far off critical-path, in the rare event of processing
	unstable events.  

	\param ni index of the node whose value change was unstable.  
	\param ui the expression id used to traverse up tree.  
	\param prev the current value of the node.
	\param next the 'would-be' new value of the node 
		(from the unstable event).  
	Algorithm:
		Figure out which events are affected by the unstable node.  
		NOTE: 'causing' in this context doesn't necessarily mean
			the last arriving input, but rather, the value
			affects the pull-state of the output at all.  
		For each expression in the node's fanout:
			
 */
void
State::kill_evaluation(const node_index_type ni, expr_index_type ui, 
		char prev, char next) {
	FINISH_ME(Fang);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluates expression changes without propagating/generating events.  
	Useful for expression state reconstruction from checkpoint.  
	\return pair(root expression, new pull value) if event propagated
		to the root, else (INVALID_NODE_INDEX, whatever)
	\param ni index of the node that changed value, 
		not really needed, only used for diagnostic.  
	\param ui the expression id used to traverse up tree.  
	\param prev previous value of node.
		Locally used as old pull state of subexpression.  
	\param next new value of node.
		Locally used as new pull state of subexpression.  
	Side effect (sort of): trace of expressions visited is in
		the __scratch_expr_trace array.  
	CAUTION: distinguish between expression value and pull-state!
 */
// inline
State::evaluate_return_type
State::evaluate(const node_index_type ni, expr_index_type ui, 
		char prev, char next) {
	STACKTRACE_VERBOSE_STEP;
	DEBUG_STEP_PRINT("node " << ni << " from " <<
		node_type::value_to_char[size_t(prev)] << " -> " <<
		node_type::value_to_char[size_t(next)] << endl);
	expr_type* u;
	__scratch_expr_trace.clear();
do {
	char old_pull, new_pull;	// pulling state of the subexpression
	u = &expr_pool[ui];
	__scratch_expr_trace.push_back(ui);
#if DEBUG_STEP
	DEBUG_STEP_PRINT("examining expression ID: " << ui << endl);
	u->dump_struct(STACKTRACE_INDENT) << endl;
	u->dump_state(STACKTRACE_INDENT << "before: ") << endl;
#endif
	// trust compiler to effectively perform branch-invariant
	// code-motion
	if (u->is_disjunctive()) {
		// is disjunctive (or)
		DEBUG_STEP_PRINT("is_or()" << endl);
		// countdown represents the number of 1's
		old_pull = u->or_pull_state();
		u->unknowns += (next >> 1) - (prev >> 1);
		u->countdown += (next & node_type::LOGIC_VALUE)
			- (prev & node_type::LOGIC_VALUE);
		new_pull = u->or_pull_state();
	} else {
		DEBUG_STEP_PRINT("is_and()" << endl);
		// is conjunctive (and)
		old_pull = u->and_pull_state();
		// countdown represents the number of 0's
		u->unknowns += (next >> 1) - (prev >> 1);
		u->countdown += !next - !prev;
		new_pull = u->and_pull_state();
	}	// end if
#if DEBUG_STEP
	u->dump_state(STACKTRACE_INDENT << "after : ") << endl;
#endif
	if (old_pull == new_pull) {
		// then the pull-state did not change.
		DEBUG_STEP_PRINT("end of propagation." << endl);
		return evaluate_return_type();
	}
	// already accounted for negation in pull_state()
	// NOTE: cannot equate pull with value!
	prev = old_pull;
	next = new_pull;
	ui = u->parent;
} while (!u->is_root());
	// made it to root
	// negation already accounted for
	return evaluate_return_type(ui, u, next);
}	// end State::evaluate()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main expression evaluation method, ripped off of
	old prsim's propagate_up.  
	\param ni the index of the node causing this propagation (root),
		only used for diagnostic purposes.
	\param ui the index of the sub expression being evaluated, 
		this is already a parent expression of the causing node, 
		unlike original prsim.  
		Locally, this is used as the index of the affected node.  
	\param prev the former value of the node/subexpression
	\param next the new value of the node/subexpression.
		Locally, this is used as index of the root expression, 
		in the event that an evaluation propagates to root.  
	NOTE: the action table here depends on the expression-type's
		subtype encoding.  For now, we use the Expr's encoding.  
 */
State::break_type
State::propagate_evaluation(
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		const event_cause_type& c,
#else
		const node_index_type ni,
#endif
		expr_index_type ui, 
		char prev, char next) {
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
	const node_index_type& ni(c.node);
#endif
	const evaluate_return_type
		ev_result(evaluate(ni, ui, prev, next));
	if (!ev_result.node_index)
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
		return false;
#else
		return;
#endif
	next = ev_result.root_pull;
	ui = ev_result.node_index;
	const expr_type* const u(ev_result.root_ex);
	// we delay the root rule search until here to reduce the amount
	// of searching required to find the responsible rule expression.  
	rule_index_type root_rule;
{
	typedef	expr_trace_type::const_reverse_iterator	trace_iterator;
	trace_iterator ri(__scratch_expr_trace.rbegin()),
		re(__scratch_expr_trace.rend());
	// search from root down, find the first valid rule expr visited
	while (ri!=re && (rule_map.find(*ri) == rule_map.end())) { ++ri; }
	root_rule = *ri;
}
	INVARIANT(root_rule);
// propagation made it to the root node, indexed by ui (now node_index_type)
	node_type& n(get_node(ui));
	DEBUG_STEP_PRINT("propagated to output node: " <<
		get_node_canonical_name(ui) << " with pull state " <<
		size_t(next) << endl);
	const event_index_type ei = n.get_event();
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	break_type err = false;
#endif
if (u->direction()) {
	// pull-up
/***
	The node is either T, F, or X. Either way, it's a change.
	If the node is T or X, insert into pending Q.
	If the guard becomes false, this could be an instability.  It is
	an instability IF the output would have been turned on by the guard.
***/
if (!n.pending_event()) {
	DEBUG_STEP_PRINT("no pending event on this node being pulled up."
		<< endl);
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
		DEBUG_STEP_PRINT("pulling up (on or weak)" << endl);
		const event_index_type pe =
			__allocate_event(n, ui, 
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
				c,
#else
				ni, 
#endif
				root_rule, next);
		const event_type& e(get_event(pe));
		// pe->cause = root
		if (n.has_mk_exclhi()) {
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
		DEBUG_STEP_PRINT(
			"pull-up turned off, yielding to opposing pull-down."
			<< endl);
		const event_index_type pe =
			__allocate_event(n, ui, 
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
				c,
#else
				ni, 
#endif
				root_rule, node_type::LOGIC_LOW);
		// pe->cause = root
		if (n.has_mk_excllo()) {
			const event_type& e(get_event(pe));
			enqueue_excllo(get_delay_up(e), pe);
		} else {
			enqueue_pending(pe);
		}
	}
} else if (!n.in_excl_queue()) {
	DEBUG_STEP_PRINT("pending, but not excl event on this node." << endl);
	// there is a pending event, not in the exclusive queue
	event_type& e(get_event(ei));
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
		DEBUG_STEP_PRINT("changing pending X to 0 in queue." << endl);
		e.val = node_type::LOGIC_LOW;
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		e.cause.node = ni;
#else
		e.cause_node = ni;
#endif
	} else {
		DEBUG_STEP_PRINT("checking for upguard anomaly: guard=" <<
			size_t(next) << ", val=" << size_t(e.val) << endl);
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
		err |=
#endif
		__diagnose_violation(cout, next, ei, e, ui, n, 
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
			c,
#else
			ni, 
#endif
			u->direction());
	}	// end if diagnostic
}	// end if (!n.ex_queue)
} else {
	// pull-dn
if (!n.pending_event()) {
	DEBUG_STEP_PRINT("no pending event on this node being pulled down."
		<< endl);
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
		DEBUG_STEP_PRINT("pulling down (on or weak)" << endl);
		const event_index_type pe = __allocate_event(n, ui,
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
			c,
#else
			ni, 
#endif
			root_rule, node_type::invert_value[size_t(next)]);
		const event_type& e(get_event(pe));
		if (n.has_mk_excllo()) {
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
		DEBUG_STEP_PRINT(
			"pull-down turned off, yielding to opposing pull-up."
			<< endl);
		const event_index_type pe = __allocate_event(n, ui, 
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
				c,
#else
				ni, 
#endif
				root_rule, node_type::LOGIC_HIGH);
		// pe->cause = root
		if (n.has_mk_exclhi()) {
			const event_type& e(get_event(pe));
			enqueue_exclhi(get_delay_dn(e), pe);
		} else {
			enqueue_pending(pe);
		}
	}
} else if (!n.in_excl_queue()) {
	// there is a pending event, not in an exclusive queue
	event_type& e(get_event(ei));
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
		DEBUG_STEP_PRINT("changing pending X to 1 in queue." << endl);
		// there is a pending 'X' in the queue
		e.val = node_type::LOGIC_HIGH;
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		e.cause.node = ni;
#else
		e.cause_node = ni;
#endif
	} else {
		DEBUG_STEP_PRINT("checking for dnguard anomaly: guard=" <<
			size_t(next) << ", val=" << size_t(e.val) << endl);
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
		err |=
#endif
		__diagnose_violation(cout, next, ei, e, ui, n, 
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
			c,
#else
			ni, 
#endif
			u->direction());
	}	// end if diagonstic
}	// end if (!n.ex_queue)
}	// end if (u->direction())
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	return err;
#endif
}	// end method propagate_evaluation

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::__report_cause(ostream& o, const event_type& ev) const {
	const node_index_type&
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		ni(ev.cause.node);
#else
		ni(ev.cause_node);
#endif
	if (ni) {
		o << ">> cause: `" <<
			get_node_canonical_name(ni) << "\' (val: ";
		get_node(ni).dump_value(o) << ')' << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if error causes break in events.  
 */
State::break_type
State::__report_interference(ostream& o, const bool weak, 
		const node_index_type _ni, const event_type& ev) const {
	if (weak) {
	if (
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
		(weak_interference_policy != ERROR_IGNORE)
#else
		!(flags & FLAG_NO_WEAK_INTERFERENCE)
#endif
			) {
		o << "WARNING: weak-interference `" <<
			get_node_canonical_name(_ni) << "\'" << endl;
		__report_cause(o, ev);
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
		return weak_interference_policy == ERROR_BREAK;
#endif
	}	// endif weak_interference_policy
	} else {	// !weak
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	if (interference_policy != ERROR_IGNORE) {
#endif
		o << "WARNING: interference `" <<
			get_node_canonical_name(_ni) << "\'" << endl;
		__report_cause(o, ev);
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
		return interference_policy == ERROR_BREAK;
	}	// endif interference_policy
#endif
	}	// endif weak
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	return false;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if error causes break in events.  
 */
State::break_type
State::__report_instability(ostream& o, const bool weak, const bool dir, 
		const node_index_type _ni, const event_type& ev) const {
	if (weak) {
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	if (weak_unstable_policy != ERROR_IGNORE) {
#endif
		o << "WARNING: weak-unstable `" <<
			get_node_canonical_name(_ni) << "\'" <<
				(dir ? '+' : '-') << endl;
		__report_cause(o, ev);
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
		return weak_unstable_policy == ERROR_BREAK;
	}	// endif weak_unstable_policy
#endif
	} else {	// !weak
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	if (unstable_policy != ERROR_IGNORE) {
#endif
		o << "WARNING: unstable `" <<
			get_node_canonical_name(_ni) << "\'" <<
				(dir ? '+' : '-') << endl;
		__report_cause(o, ev);
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
		return unstable_policy == ERROR_BREAK;
	}	// endif unstable_policy
#endif
	}	// endif weak
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	return false;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function for repetitive diagnostic code.  
	\param o error output stream
	\param next the next value of this node
	\param ei index of the event in question
	\param e the event in question
	\param ui index of the node that fired
	\param n the node that fired
	\param ni the node involved in event e
	\param dir the direction of pull of the causing rule
	\return true if error causes break.
 */
State::break_type
State::__diagnose_violation(ostream& o, const char next, 
		const event_index_type ei, event_type& e, 
		const node_index_type ui, node_type& n, 
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		const event_cause_type& c, 
#else
		const node_index_type ni, 
#endif
		const bool dir) {
	STACKTRACE_VERBOSE;
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
	const node_index_type& ni(c.node);
#endif
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	break_type err = false;
#endif
	// something is amiss!
	const char eu = dir ?
		event_type::upguard[size_t(next)][size_t(e.val)] :
		event_type::dnguard[size_t(next)][size_t(e.val)];
	DEBUG_STEP_PRINT("event_update = " << size_t(eu) << endl);
	const bool vacuous = eu & event_type::EVENT_VACUOUS;
	if (!vacuous) {
		// then must be unstable or interfering (exclusive)
		const bool instability =
			(eu & event_type::EVENT_UNSTABLE) &&
			!n.is_unstab();
		/***
			This last condition !unstab violates exact exclution 
			between unstable and interference!
			Do not use this undocumented feature, it is not 
			expected to work as presently coded.  
		***/
		const bool interference =
			eu & event_type::EVENT_INTERFERENCE;
		const string cause_name(get_node_canonical_name(ni));
		const string out_name(get_node_canonical_name(ui));

			// causing rule only used by propagate
			// don't care about value, 
			// event is for sake of checking
			// check for conflicting/redundant events 
			// on pending queue (result of instability)
		if (instability) {
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
			e.cause.node = ni;
#else
			e.cause_node = ni;
#endif
			if (dequeue_unstable_events()) {
				// let dequeuer deallocate killed events
				const size_t pe = n.get_event();
				DEBUG_STEP_PRINT("dequeuing unstable event " << pe << endl);
				get_event(pe).kill();
				// n.clear_event();
				// or punt clearing until later?
#if 0
	{	// pardon momentary ugly indentation...
		typedef	node_type::const_fanout_iterator	const_iterator;
		const_iterator i(n.fanout.begin()), e(n.fanout.end());
		for ( ; i!=e; ++i) {
			kill_evaluation(ni, *i, prev, next);
		}
	}
#endif
			} else {
				e.val = node_type::LOGIC_OTHER;
			}
		}
		if (interference) {
			/***
				Q: may actually be an instability, 
				so we insert the event into pending-queue
				to check, and punt the setting to X.  
			***/
			const event_index_type pe =
				__allocate_pending_interference_event(
					n, ui,
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
					c, 
#else
					ni,
#endif
					dir ? node_type::LOGIC_HIGH :
						node_type::LOGIC_LOW);
			enqueue_pending(pe);
		}
		// diagnostic message
		// suppress message for interferences until pending queue
		if (instability) {
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
			err |=
#endif
			__report_instability(o, eu & event_type::EVENT_WEAK, 
				dir, ui, e);
		}	// end if unstable
	}	// end if !vacuous
	// else vacuous is OK
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	return err;
#endif
}	// end method __diagnose_violation

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Run until event queue is empty or breakpoint reached.  
	\return null node index if queue is emptied, else
		the ID of the node that tripped a breakpoint.  
 */
State::step_return_type
State::cycle(void) THROWS_EXCL_EXCEPTION {
	step_return_type ret;
	while ((ret = step()).first) {
		if (get_node(ret.first).is_breakpoint() || stopped())
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
	for ( ; i!=e; ++i) {
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
	Prints list of explicitly watched nodes.  
	Doesn't count watchall flag.  
 */
ostream&
State::dump_watched_nodes(ostream& o) const {
	typedef	watch_list_type::const_iterator		const_iterator;
	const_iterator i(watch_list.begin()), e(watch_list.end());
	o << "watched nodes: ";
	for (; i!=e; ++i) {
		o << get_node_canonical_name(i->first) << ' ';
	}
	return o << endl;
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
		STACKTRACE_INDENT_PRINT("checking Expr " << i << ":" << endl);
		check_expr(i);
	}
}
{
	const node_index_type nodes = node_pool.size();
	node_index_type j = FIRST_VALID_NODE;
	for ( ; j<nodes; ++j) {
		STACKTRACE_INDENT_PRINT("checking Node " << j << ":" << endl);
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
		const expr_type& e(expr_pool[i]);
	if (!e.wiped()) {
		e.dump_struct(o << "expr[" << i << "]: ") << endl;
		expr_graph_node_pool[i].dump_struct(o << '\t') << endl;
	}
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
	Good for visualizing a decent 2D cell/wire/transistor placement.
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
	typedef	temp_queue_type::const_iterator		const_iterator;
	temp_queue_type temp;
	event_queue.copy_to(temp);
	const_iterator i(temp.begin()), e(temp.end());
	o << "event queue:" << endl;
	for ( ; i!=e; ++i) {
		DEBUG_STEP_PRINT(i->event_index);
		const event_type& ev(get_event(i->event_index));
		if (!ev.killed()) {
			o << '\t' << i->time << '\t' <<
				get_node_canonical_name(ev.node) << " : " <<
				node_type::value_to_char[ev.val] << endl;
		}
#if DEBUG_STEP
		else {
			o << '\t' << i->time << '\t' <<
				get_node_canonical_name(ev.node) << " : " <<
				node_type::value_to_char[ev.val] <<
				'\t' << "(killed)" << endl;
		}
#endif
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
	DEBUG_FANOUT_PRINT("ni = " << ni << endl);
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
			DEBUG_FANOUT_PRINT("ei = " << ei << endl);
			ei = e->parent;
			e = &expr_pool[ei];
		}
		DEBUG_FANOUT_PRINT("ei = " << ei << endl);
		// ei is an index to the expression whose parent is *node*.
		const node_index_type nr = e->parent;
		// nr is an index to the root *node*.
		DEBUG_FANOUT_PRINT("nr = " << nr << endl);
		const node_type& no(get_node(nr));
		// track the direction of propagation (pull-up/dn)
		const bool dir = e->direction();
		// then print the entire fanin rule for that node, 
		const expr_index_type pi =
			(dir ? no.pull_up_index : no.pull_dn_index);
		DEBUG_FANOUT_PRINT("pi = " << pi << endl);
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
	\param ptype the parent's expression type, only used if cp is true.
	\param pr whether or not parent is root
		(if so, ignore type comparison for parenthesization).  
 */
ostream&
State::dump_subexpr(ostream& o, const expr_index_type ei, 
		const char ptype, const bool pr) const {
#if DEBUG_FANOUT
	STACKTRACE_VERBOSE;
#endif
	INVARIANT(ei);
	INVARIANT(ei < expr_pool.size());
	const expr_type& e(expr_pool[ei]);
	const graph_node_type& g(expr_graph_node_pool[ei]);
	// can elaborate more on when parens are needed
	const bool need_parens = e.parenthesize(ptype, pr);
	const char _type = e.type;
	// check if this sub-expression is a root expression by looking
	// up the expression index in the rule_map.  
	typedef	rule_map_type::const_iterator	rule_iterator;
	const rule_iterator ri(rule_map.find(ei));
	const bool is_rule (ri != rule_map.end());
	if (is_rule) {
		// then we can print out its attributes
		ri->second.dump(o << '[') << "]\t";
	}
	if (e.is_not()) {
		o << '~';
	}
	const char* op = e.is_disjunctive() ? " | " : " & ";
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
			if (e.is_or() &&
				rule_map.find(ci->second) != rule_map.end()) {
				// to place each 'rule' on its own line
				o << endl;
			}
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_mk_excl_ring(ostream& o, const ring_set_type& r) const {
	typedef	ring_set_type::const_iterator	const_iterator;
	INVARIANT(r.size() > 1);
	const_iterator i(r.begin()), e(r.end());
	o << "{ ";
	o << get_node_canonical_name(*i);
	for (++i; i!=e; ++i) {
		o << ", " << get_node_canonical_name(*i);
	}
	return o << " }";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_mk_exclhi_rings(ostream& o) const {
	o << "forced exclhi rings:" << endl;
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	const_iterator i(mk_exhi.begin()), e(mk_exhi.end());
	for ( ; i!=e; ++i) {
		dump_mk_excl_ring(o, *i) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_mk_excllo_rings(ostream& o) const {
	o << "forced excllo rings:" << endl;
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	const_iterator i(mk_exlo.begin()), e(mk_exlo.end());
	for ( ; i!=e; ++i) {
		dump_mk_excl_ring(o, *i) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints excl-ring fanout of node.  
 */
ostream&
State::dump_node_mk_excl_rings(ostream& o, const node_index_type ni) const {
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	const string nn(get_node_canonical_name(ni));
{
	o << "forced exclhi rings of which `" << nn <<
		"\' is a member:" << endl;
	const_iterator i(mk_exhi.begin()), e(mk_exhi.end());
	for ( ; i!=e; ++i) {
		if (i->find(ni) != i->end()) {
			dump_mk_excl_ring(o, *i) << endl;
		}
	}
}{
	o << "forced excllo rings of which `" << nn <<
		"\' is a member:" << endl;
	const_iterator i(mk_exlo.begin()), e(mk_exlo.end());
	for ( ; i!=e; ++i) {
		if (i->find(ni) != i->end()) {
			dump_mk_excl_ring(o, *i) << endl;
		}
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_check_excl_ring(ostream& o, const lock_index_list_type& r) const {
	typedef	lock_index_list_type::const_iterator	const_iterator;
	INVARIANT(r.size() > 1);
	const_iterator i(r.begin()), e(r.end());
	o << "{ ";
	o << get_node_canonical_name(*i);
	for (++i; i!=e; ++i) {
		o << ", " << get_node_canonical_name(*i);
	}
	return o << " }";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dump all checked exclhi rings.  
 */
ostream&
State::dump_check_exclhi_rings(ostream& o) const {
	o << "checked exclhi rings:" << endl;
	check_excl_array_type temp(check_exhi_ring_pool.size());
	__collate_check_excl_reverse_map(check_exhi, temp);
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	const_iterator i(temp.begin()), e(temp.end());
	// skip first b/c [0] is reserved
	for (++i; i!=e; ++i) {
		dump_mk_excl_ring(o, *i) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dump all checked excllo rings.  
 */
ostream&
State::dump_check_excllo_rings(ostream& o) const {
	o << "checked excllo rings:" << endl;
	check_excl_array_type temp(check_exlo_ring_pool.size());
	__collate_check_excl_reverse_map(check_exlo, temp);
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	const_iterator i(temp.begin()), e(temp.end());
	// skip first b/c [0] is reserved
	for (++i; i!=e; ++i) {
		dump_mk_excl_ring(o, *i) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generates map of lock_index to set of nodes in ring.  
	\pre r is sized to the corresponding lock-pool (array) size.  
 */
void
State::__collate_check_excl_reverse_map(const check_excl_ring_map_type& m, 
		check_excl_array_type& r) const {
	typedef	check_excl_ring_map_type::const_iterator	const_iterator;
	const_iterator i(m.begin()), e(m.end());
	for ( ; i!=e; ++i) {
		typedef	lock_index_list_type::const_iterator
							lock_index_iterator;
		const lock_index_list_type& ll(i->second);
		lock_index_iterator li(ll.begin()), le(ll.end());
		for ( ; li!=le; ++li) {
			r[*li].insert(i->first);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generates partial map of lock_index to set of nodes in ring.  
	\pre r should be empty.  
 */
void
State::__partial_collate_check_excl_reverse_map(
		const check_excl_ring_map_type& m, 
		const lock_index_list_type& l, 
		check_excl_reverse_map_type& r) const {
	typedef	check_excl_ring_map_type::const_iterator	const_iterator;
	typedef	std::set<lock_index_type>		lock_set_type;
	lock_set_type ref;
	copy(l.begin(), l.end(), set_inserter(ref));
	const_iterator i(m.begin()), e(m.end());
	for ( ; i!=e; ++i) {
		typedef	lock_index_list_type::const_iterator
							lock_index_iterator;
		const lock_index_list_type& ll(i->second);
		lock_index_iterator li(ll.begin()), le(ll.end());
		for ( ; li!=le; ++li) {
			// predicated
			if (ref.find(*li) != ref.end()) {
				r[*li].insert(i->first);
			}
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_node_check_excl_rings(ostream& o, const node_index_type ni) const {
	typedef	check_excl_reverse_map_type::const_iterator	const_iterator;
	const string nn(get_node_canonical_name(ni));
	const node_type n(get_node(ni));
{
	o << "checked exclhi rings of which `" << nn <<
		"\' is a member:" << endl;
	if (n.has_check_exclhi()) {
		check_excl_reverse_map_type temp;
		__partial_collate_check_excl_reverse_map(check_exhi, 
			check_exhi.find(ni)->second, temp);
		const_iterator i(temp.begin()), e(temp.end());
		for ( ; i!=e; ++i) {
			dump_mk_excl_ring(o, i->second) << endl;
		}
	}
}{
	o << "checked excllo rings of which `" << nn <<
		"\' is a member:" << endl;
	if (n.has_check_excllo()) {
		check_excl_reverse_map_type temp;
		__partial_collate_check_excl_reverse_map(check_exlo, 
			check_exlo.find(ni)->second, temp);
		const_iterator i(temp.begin()), e(temp.end());
		for ( ; i!=e; ++i) {
			dump_mk_excl_ring(o, i->second) << endl;
		}
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: need to check consistency with module.  
	Write out a header for safety checks.  
	TODO: save state only? without structure?
	\return true if to signal that an error occurred. 
 */
bool
State::save_checkpoint(ostream& o) const {
	write_value(o, magic_string);
{
	// node_pool
	write_value(o, node_pool.size());
	for_each(node_pool.begin() +1, node_pool.end(), 
#if 1
		bind2nd_argval(mem_fun_ref(&node_type::save_state), o)
#else
	// doesn't quite work yet!?
	// might need to wrap the mem_fun_ref to take a wrapper?
		std::bind2nd(mem_fun_ref(&node_type::save_state),
			util::ref(o))
#endif
	);
}
	// graph_pool -- structural only
{
#if CHECKPOINT_RULE_STATE_MAP
	// rule_map -- currently structural only, but we include code anyways
	typedef	rule_map_type::const_iterator		const_iterator;
	write_value(o, rule_map.size());
	const_iterator i(rule_map.begin()), e(rule_map.end());
	for ( ; i!=e; ++i) {
		write_value(o, i->first);
		i->second.save_state(o);
	}
#endif
}{
	// event_pool -- only selected entries
	// event_queue
	typedef	temp_queue_type::const_iterator		const_iterator;
	temp_queue_type temp;
	event_queue.copy_to(temp);
	const_iterator i(temp.begin()), e(temp.end());
	write_value(o, temp.size());
	for ( ;i!=e; ++i) {
		write_value(o, i->time);
		event_pool[i->event_index].save_state(o);
	}
}
	// excl_rings -- structural only
	// excl and pending queues should be empty!
	INVARIANT(exclhi_queue.empty());
	INVARIANT(excllo_queue.empty());
	INVARIANT(pending_queue.empty());
	write_value(o, current_time);
	write_value(o, uniform_delay);
{
	// watch_list? yes, because needs to be kept consistent with nodes
	typedef	watch_list_type::const_iterator		const_iterator;
	write_value(o, watch_list.size());
	const_iterator i(watch_list.begin()), e(watch_list.end());
	for ( ; i!=e; ++i) {
		write_value(o, i->first);
		i->second.save_state(o);
	}
}
	write_value(o, flags);
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	write_value(o, unstable_policy);
	write_value(o, weak_unstable_policy);
	write_value(o, interference_policy);
	write_value(o, weak_interference_policy);
#endif
	write_value(o, timing_mode);
	// interrupted flag, just ignore
	// ifstreams? don't bother managing input stream stack.
	// __scratch_expr_trace -- never needed, ignore
	write_value(o, magic_string);
	return !o;
}	// end State::save_checkpoint

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Has a ton of consistency checking.  
	TODO: need some sort of consistency check with module.  
	\pre the State is already allocated b/c no resizing is done
		during checkpoint loading.  
	\return true if to signal that an error occurred. 
 */
bool
State::load_checkpoint(istream& i) {
	initialize();		// start by initializing everything
	// or reset(); ?
	string header_check;
	read_value(i, header_check);
	if (header_check != magic_string) {
		cerr << "ERROR: not a hackt prsim checkpoint file." << endl;
		return true;
	}
{
	// node_pool
	size_t s;
	read_value(i, s);
	if (node_pool.size() != s) {
		cerr << "ERROR: checkpoint\'s node_pool size is inconsistent."
			<< endl;
		return true;
	}
	typedef node_pool_type::iterator	iterator;
	const iterator b(node_pool.begin() +1), e(node_pool.end());
#if 0
	// doesn't work :(
	for_each(b, e, 
		bind2nd_argval_void(mem_fun_ref(&node_type::load_state), i)
	);
#else
	iterator j(b);
	for ( ; j!=e; ++j) {
		j->load_state(i);
	}
#endif
}{
	// to reconstruct from nodes only, we perform propagation evaluation
	// on every node, as if it had just fired out of X state.  
	typedef node_pool_type::const_iterator	const_iterator;
	const const_iterator nb(node_pool.begin()), ne(node_pool.end());
	const_iterator ni(nb);
	const char prev = node_type::LOGIC_OTHER;
	for (++ni; ni!=ne; ++ni) {
		typedef	node_type::const_fanout_iterator
					const_fanout_iterator;
		const node_type& n(*ni);
		const_fanout_iterator fi(n.fanout.begin()), fe(n.fanout.end());
		const char next = n.current_value();
		if (next != prev) {
			const expr_index_type nj(distance(nb, ni));
			for ( ; fi!=fe; ++fi) {
				evaluate(nj, *fi, prev, next);
				// evaluate does not modify any queues
				// just updates expression states
			}
			// but we don't actually use these event queues, 
			// those are loaded from the checkpoint.  
		}
	}	// end for-all nodes
}
	// graph_pool -- structural only
{
#if CHECKPOINT_RULE_STATE_MAP
	// rule_map -- currently structural only, but we include code anyways
	typedef	rule_map_type::iterator		iterator;
	size_t s;
	read_value(i, s);
	if (rule_map.size() != s) {
		cerr << "ERROR: checkpoint\'s rule_map size is inconsistent."
			<< endl;
		return true;
	}
	iterator j(rule_map.begin()), e(rule_map.end());
	for ( ; j!=e; ++j) {
		rule_map_type::key_type k;
		read_value(i, k);
		if (j->first != k) {
			cerr << "ERROR: checkpoint\'s rule_map key is "
				"inconsistent.  got: " << k << ", expected: "
				<< j->first << endl;
			return true;
		}
		j->second.load_state(i);
	}
#endif
}{
	// event_pool -- only selected entries
	// event_queue
	size_t s;
	read_value(i, s);
	for ( ; s; --s) {
		time_type t;
		read_value(i, t);
		event_type ev;
		ev.load_state(i);
		enqueue_event(t, __load_allocate_event(ev));
	}
}
	// excl_rings -- structural only
	// excl and pending queues should be empty!
	INVARIANT(exclhi_queue.empty());
	INVARIANT(excllo_queue.empty());
	INVARIANT(pending_queue.empty());
	read_value(i, current_time);
	read_value(i, uniform_delay);
{
	// watch_list? yes, because needs to be kept consistent with nodes
	size_t s;
	read_value(i, s);
	for ( ; s; --s) {
		watch_list_type::key_type k;
		read_value(i, k);
		watch_list[k].load_state(i);
	}
}
	read_value(i, flags);
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
	read_value(i, unstable_policy);
	read_value(i, weak_unstable_policy);
	read_value(i, interference_policy);
	read_value(i, weak_interference_policy);
#endif
	read_value(i, timing_mode);
	// interrupted flag, just ignore
	// ifstreams? don't bother managing input stream stack.
	// __scratch_expr_trace -- never needed, ignore

	// this must be run *after* mode flags are loaded
if (checking_excl()) {
	typedef node_pool_type::const_iterator	const_iterator;
	const const_iterator nb(node_pool.begin()), ne(node_pool.end());
	const_iterator ni(nb);
	const char prev = node_type::LOGIC_OTHER;
	for (++ni; ni!=ne; ++ni) {
	// lock exclusive check rings
		const node_type& n(*ni);
		const char next = n.current_value();
		const excl_exception
			e(check_excl_rings(distance(nb, ni), n, prev, next));
		if (e.lock_id) {
			inspect_excl_exception(e, cerr);
			// don't bother throwing
		}
	}
}
{
	read_value(i, header_check);
	if (header_check != magic_string) {
		cerr << "ERROR: detected checkpoint misalignment!" << endl;
		return true;
	}
}
	return !i;
}	// end State::load_checkpoint

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Textual dump of checkpoint without loading it.  
	Keep this consistent with the save/load methods above.  
	\param i the input file stream for the checkpoint.
	\param o the output stream to dump to.  
 */
ostream&
State::dump_checkpoint(ostream& o, istream& i) {
	string header_check;
	read_value(i, header_check);
	o << "header string: " << header_check << endl;
{
	// node_pool
	size_t s;
	read_value(i, s);
	size_t j = 1;
	o << "Have " << s << " unique boolean nodes:" << endl;
	node_type::dump_checkpoint_state_header(o << '\t') << endl;
	for ( ; j<s; ++j) {
		node_type::dump_checkpoint_state(o << j << '\t', i) << endl;
	}
}
	// graph_pool -- structural only
{
#if CHECKPOINT_RULE_STATE_MAP
	// rule_map -- currently structural only, but we include code anyways
	size_t s;
	read_value(i, s);
	o << "Have " << s << " rule attribute map entries:" << endl;
	size_t j = 0;
	for ( ; j<s; ++j) {
		rule_map_type::key_type k;
		read_value(i, k);
		o << k << '\t';
		rule_type::dump_checkpoint_state(o, i) << endl;
	}
#endif
}{
	// event_pool -- only selected entries
	// event_queue
	size_t s;
	read_value(i, s);
	o << "Have " << s << " events in queue:" << endl;
	event_type::dump_checkpoint_state_header(o << '\t') << endl;
	for ( ; s; --s) {
		time_type t;
		read_value(i, t);
		o << t << '\t';
		event_type::dump_checkpoint_state(o, i) << endl;
	}
}
	time_type current_time, uniform_delay;
	read_value(i, current_time);
	read_value(i, uniform_delay);
	o << "current time: " << current_time << endl;
	o << "uniform delay: " << uniform_delay << endl;
{
	// watch_list? yes, because needs to be kept consistent with nodes
	size_t s;
	read_value(i, s);
	o << "Have " << s << " nodes in watch-list:" << endl;
	for ( ; s; --s) {
		watch_list_type::key_type k;
		read_value(i, k);
		o << k << '\t';
		watch_entry::dump_checkpoint_state(o, i) << endl;
	}
}
	flags_type flags;
	read_value(i, flags);
	o << "flags: " << size_t(flags) << endl;
#if PRSIM_FINE_GRAIN_ERROR_CONTROL
{
	error_policy_enum p;
	read_value(i, p);
	o << "unstable policy: " << error_policy_string(p) << endl;
	read_value(i, p);
	o << "weak-unstable policy: " << error_policy_string(p) << endl;
	read_value(i, p);
	o << "interference policy: " << error_policy_string(p) << endl;
	read_value(i, p);
	o << "weak-interference policy: " << error_policy_string(p) << endl;
}
#endif
	char timing_mode;
	read_value(i, timing_mode);
	o << "timing mode: " << size_t(timing_mode) << endl;
	read_value(i, header_check);
	o << "footer string: " << header_check << endl;
	return o;
}	// end State::dump_checkpoint

//=============================================================================
// struct watch_entry method definitions

void
watch_entry::save_state(ostream& o) const {
	write_value(o, breakpoint);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
watch_entry::load_state(istream& i) {
	read_value(i, breakpoint);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
watch_entry::dump_checkpoint_state(ostream& o, istream& i) {
	char breakpoint;
	read_value(i, breakpoint);
	return o << size_t(breakpoint);
}

//=============================================================================
// class State::signal_handler method definitions

/**
	Global static initializer for handler's bound State reference.  
 */
State*
State::signal_handler::_state = NULL;

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

