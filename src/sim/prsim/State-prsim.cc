/**
	\file "sim/prsim/State-prsim.cc"
	Implementation of prsim simulator state.  
	$Id: State-prsim.cc,v 1.18.2.8 2008/08/26 01:26:43 fang Exp $

	This module was renamed from:
	Id: State.cc,v 1.32 2007/02/05 06:39:55 fang Exp
	to avoid object base name collisions.
	A sad discontinuity in revision history...
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_FANOUT			(0 && ENABLE_STACKTRACE)
#define	DEBUG_STEP			(0 && ENABLE_STACKTRACE)
#define	DEBUG_CHECK			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <algorithm>
#include <functional>
#include <string>
#include <set>
#include "sim/prsim/State-prsim.h"
#include "sim/prsim/ExprAlloc.h"
#include "sim/event.tcc"
#include "sim/prsim/Rule.tcc"
#include "sim/random_time.h"
#include "sim/signal_handler.tcc"
#include "util/list_vector.tcc"
#include "Object/module.h"
#include "Object/state_manager.h"
#include "Object/traits/classification_tags.h"
#include "Object/traits/bool_traits.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/traits/proc_traits.h"	// for diagnostic
#include "Object/global_entry.h"
#include "sim/ISE.h"
#include "common/TODO.h"
#include "util/attributes.h"
#include "util/sstream.h"
#include "util/stacktrace.h"
#include "util/memory/index_pool.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/likely.h"
#include "util/iterator_more.h"
#include "util/string.tcc"
#include "util/IO_utils.tcc"
#include "util/binders.h"
#include "util/utypes.h"
#include "util/indent.h"

#if	DEBUG_STEP
#define	DEBUG_STEP_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_STEP		STACKTRACE_VERBOSE
#else
#define	DEBUG_STEP_PRINT(x)
#define	STACKTRACE_VERBOSE_STEP
#endif

#if	DEBUG_CHECK
#define	DEBUG_CHECK_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_CHECK	STACKTRACE_VERBOSE
#else
#define	DEBUG_CHECK_PRINT(x)
#define	STACKTRACE_VERBOSE_CHECK
#endif

#if	DEBUG_FANOUT
#define	DEBUG_FANOUT_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#else
#define	DEBUG_FANOUT_PRINT(x)
#endif

// PRSIM_SEPARATE_CAUSE_NODE_DIRECTION == 1
#define	EMPTY_CAUSE			event_cause_type()

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

/**
	For debugging only.  To clearly see section breaks.  
 */
#define	EXTRA_ALIGN_MARKERS			0


namespace HAC {
namespace entity { }

namespace SIM {
namespace PRSIM {
using std::set;
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
using util::auto_indent;
using util::indent;
using entity::state_manager;
using entity::global_entry_pool;
using entity::bool_tag;
using entity::process_tag;
#include "util/using_ostream.h"

//=============================================================================
// class unique_process_subgraph method definitions
unique_process_subgraph::unique_process_subgraph() :
		expr_pool(), expr_graph_node_pool(),
		rule_pool(), rule_map()
#if PRSIM_INDIRECT_EXPRESSION_MAP
		, local_faninout_map()
#endif
{
#if !PRSIM_INDIRECT_EXPRESSION_MAP
	expr_graph_node_pool.set_chunk_size(1024);
// else just use default
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unique_process_subgraph::~unique_process_subgraph() { }

//=============================================================================
#if PRSIM_INDIRECT_EXPRESSION_MAP
void
process_sim_state::allocate_from_type(const unique_process_subgraph& t) {
	expr_states.resize(t.expr_pool.size());
	rule_states.resize(t.rule_pool.size());
	// default constructors of these must initalize state values
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// class process_sim_state method definitions
void
process_sim_state::clear(void) {
	expr_states.resize(0);
	rule_states.resize(0);
}
#endif

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
const value_enum
State::pull_to_value[3][3] = {
{ LOGIC_OTHER, LOGIC_LOW, LOGIC_OTHER },
{ LOGIC_HIGH, LOGIC_OTHER, LOGIC_OTHER },
{ LOGIC_OTHER, LOGIC_OTHER, LOGIC_OTHER }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates simulation state, given a module.
	TODO: do this work in module?
	\param m the expanded module object.
	\pre m must already be past the allcoate phase.  
	\throw exception if there is an error
 */
State::State(const entity::module& m, const ExprAllocFlags& f) : 
		state_base(m, "prsim> "), 
		node_pool(),
		event_pool(), event_queue(), 
		mk_exhi(), mk_exlo(), 
		exclhi_queue(), excllo_queue(), 
		pending_queue(), 
		check_exhi_ring_pool(1), check_exlo_ring_pool(1), 
		check_exhi(), check_exlo(), 
		current_time(0), 
		uniform_delay(time_traits::default_delay), 
		watch_list(), 
#if PRSIM_CHANNEL_SUPPORT
		_channel_manager(), 
#endif
		flags(FLAGS_DEFAULT),
		unstable_policy(ERROR_DEFAULT_UNSTABLE),
		weak_unstable_policy(ERROR_DEFAULT_WEAK_UNSTABLE),
		interference_policy(ERROR_DEFAULT_INTERFERENCE),
		weak_interference_policy(ERROR_DEFAULT_WEAK_INTERFERENCE),
		timing_mode(TIMING_DEFAULT),
		__scratch_expr_trace(),
		__shuffle_indices(0) {
	const state_manager& sm(mod.get_state_manager());
	const global_entry_pool<bool_tag>&
		bool_pool(sm.get_pool<bool_tag>());
	head_sentinel();
	// recall, the global node pool is 1-indexed because entry 0 is null
	// we mirror this in our own node state pool, by allocating
	// the same number of elements.  
	const size_t s = bool_pool.size();
	node_pool.resize(s);

	// not expect expression-trees deeper than 8, but is growable
	__scratch_expr_trace.reserve(8);
	__shuffle_indices.reserve(32);
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
#if PRSIM_INDIRECT_EXPRESSION_MAP
	// top-level prs in the module, pid=0
	mod.get_footprint().get_prs_footprint().accept(v);
#endif
	// this may throw an exception!
try {
	sm.accept(v);
} catch (const entity::cflat_visitor::process_exception& e) {
	const global_entry_pool<process_tag>&
		proc_entry_pool(sm.get_pool<process_tag>());
	cerr << "Error with process instance: ";
	proc_entry_pool[e.pid].dump_canonical_name(cerr, 
		mod.get_footprint(), sm) << endl;
	THROW_EXIT;
}
#if !PRSIM_INDIRECT_EXPRESSION_MAP
	// top-level prs in the module
	mod.get_footprint().get_prs_footprint().accept(v);
#endif
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
#if PRSIM_INDIRECT_EXPRESSION_MAP
	// clear process_sim_state array?
#if 0
	for_each(process_state_array.begin(), process_state_array.end(), 
		mem_fun_ref(&process_sim_state::clear));
#else
	// unique_process_pool.clear();	// necessary?
	process_state_array.clear();
#endif
#else
	expr_pool.~expr_pool_type();
	new (&expr_pool) expr_pool_type();
	expr_graph_node_pool.clear();
#endif
	event_pool.clear();
	event_queue.clear();
	head_sentinel();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Clears internal data structures.
	Procedure is common to initialize() and reset().
 */
void
State::__initialize(void) {
	STACKTRACE_VERBOSE;
	for_each(node_pool.begin(), node_pool.end(), 
		mem_fun_ref(&node_type::initialize));
#if PRSIM_INDIRECT_EXPRESSION_MAP
	for_each(process_state_array.begin(), process_state_array.end(), 
		mem_fun_ref(&process_sim_state::initialize));
#else
	for_each(expr_pool.begin(), expr_pool.end(), 
		mem_fun_ref(&expr_state_type::initialize));
#endif
	// the expr_graph_node_pool contains no stateful information.  
	while (!event_queue.empty()) {
		const event_placeholder_type next(event_queue.pop());
		event_pool.deallocate(next.event_index);
	}
	ISE_INVARIANT(event_pool.check_valid_empty());
	fill(check_exhi_ring_pool.begin(), check_exhi_ring_pool.end(), false);
	fill(check_exlo_ring_pool.begin(), check_exlo_ring_pool.end(), false);
	// unwatchall()? no, preserved
	// timing mode preserved
	current_time = 0;
#if PRSIM_CHANNEL_SUPPORT
	_channel_manager.initialize();
#endif
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
	__initialize();
	flags |= FLAGS_INITIALIZE_SET_MASK;
	flags &= ~FLAGS_INITIALIZE_CLEAR_MASK;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A node is driven if it has fanin rule, or can be driven from channel.
 */
bool
State::node_is_driven(const node_index_type ni) const {
	return get_node(ni).has_fanin() || node_is_driven_by_channel(ni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A node is used if it has fanout or can cause a channel event.  
 */
bool
State::node_is_used(const node_index_type ni) const {
	return get_node(ni).fanout.size() || node_drives_any_channel(ni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
State::node_is_driven_by_channel(const node_index_type ni) const {
#if PRSIM_CHANNEL_SUPPORT
	return get_node(ni).in_channel() &&
		_channel_manager.node_has_fanin(ni);
#else
	return false;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
State::node_drives_any_channel(const node_index_type ni) const {
#if PRSIM_CHANNEL_SUPPORT
	return get_node(ni).in_channel() &&
		_channel_manager.node_has_fanout(ni);
#else
	return false;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_SUPPORT
	// Q: is this the best place to handle this?
/**
	\return true if there is an event exception
 */
State::break_type
State::flush_channel_events(const vector<env_event_type>& env_events, 
		const event_cause_type& c) {
	STACKTRACE_VERBOSE;
	bool err = false;
	// cause of these events must be 'ni', this node
	vector<env_event_type>::const_iterator
		i(env_events.begin()), e(env_events.end());
	// const event_cause_type c(ni, next);
	for ( ; i!=e; ++i) {
#if 0
		cerr << "channel event on node: " <<
			get_node_canonical_name(i->first) << endl;
#endif
		node_type& _n(get_node(i->first));
		const value_enum _v = i->second;
		if (_n.current_value() != _v) {
		const event_index_type pe = _n.get_event();

		if (pe) {
	// interaction with other enqueued events? anomalies?
	// for now, give up if there are conflicting events in queue
			// instability!?
			event_type& ev(get_event(pe));
			err |= __report_instability(cout,
				_v == LOGIC_OTHER, 
				ev.val == LOGIC_HIGH, ev.node, ev);
			if (dequeue_unstable_events()) {
				// overtake
				kill_event(pe, ev.node);
			} else {
				ev.val = LOGIC_OTHER;
				continue;
			}
		}
		// __allocate_event
		const event_index_type pn =
			__allocate_event(_n, i->first, c,
				INVALID_RULE_INDEX, _v
#if PRSIM_WEAK_RULES
				, false	// environment never weak
#endif
			);
			// enqueue_event
			const event_type& ev(get_event(pn));
			switch (_v) {
			case LOGIC_LOW:
				enqueue_event(get_delay_dn(ev), pn);
				break;
			case LOGIC_HIGH:
				enqueue_event(get_delay_up(ev), pn);
				break;
			default: enqueue_event(current_time
				// +delay_policy<time_type>::zero
				, pn);
			}
		} // else filter out vacuous events
	}
	return err;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Set a single channel into reset state.  
	NB: flush_channel_events may result in instabilities!
 */
bool
State::reset_channel(const string& cn) {
	vector<env_event_type> temp;
	if (_channel_manager.reset_channel(cn, temp))	return true;
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER);
	flush_channel_events(temp, c);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reset all channels.  
	NB: flush_channel_events may result in instabilities!
 */
void
State::reset_all_channels(void) {
	vector<env_event_type> temp;
	_channel_manager.reset_all_channels(temp);
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER);
	flush_channel_events(temp, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Set a single channel into reset state.  
	NB: flush_channel_events may result in instabilities!
 */
bool
State::resume_channel(const string& cn) {
	vector<env_event_type> temp;
	if (_channel_manager.resume_channel(*this, cn, temp))	return true;
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER);
	flush_channel_events(temp, c);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reset all channels.  
	NB: flush_channel_events may result in instabilities!
 */
void
State::resume_all_channels(void) {
	vector<env_event_type> temp;
	_channel_manager.resume_all_channels(*this, temp);
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER);
	flush_channel_events(temp, c);
}
#endif	// PRSIM_CHANNEL_SUPPORT

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
	__initialize();
	flags = FLAGS_DEFAULT;
	unstable_policy = ERROR_DEFAULT_UNSTABLE;
	weak_unstable_policy = ERROR_DEFAULT_WEAK_UNSTABLE;
	interference_policy = ERROR_DEFAULT_INTERFERENCE;
	weak_interference_policy = ERROR_DEFAULT_WEAK_INTERFERENCE;
	timing_mode = TIMING_DEFAULT;
	unwatch_all_nodes();
	uniform_delay = time_traits::default_delay;
#if PRSIM_CHANNEL_SUPPORT
	_channel_manager.clobber_all();
#endif
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
#if !PRSIM_INDIRECT_EXPRESSION_MAP
	expr_pool.resize(FIRST_VALID_EXPR);
	expr_graph_node_pool.push_back(graph_node_type());
#endif
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
	ISE_INVARIANT(i);
	ISE_INVARIANT(i < node_pool.size());
	return node_pool[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Node accessor, mutable.  
	Can remove bounds checks if we're really confident.
 */
State::node_type&
State::get_node(const node_index_type i) {
	ISE_INVARIANT(i);
	ISE_INVARIANT(i < node_pool.size());
	return node_pool[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Traces backwards in event history until repeat (cycle) found.  
 */
void
State::backtrace_node(ostream& o, const node_index_type ni) const {
	typedef	set<event_cause_type>		event_set_type;
	// start from the current value of the referenced node
	const node_type* n(&get_node(ni));
	const value_enum v = n->current_value();
	event_cause_type e(ni, v);
	o << "node at: `" << get_node_canonical_name(ni) <<
		"\' : " << node_type::value_to_char[size_t(v)] << endl;
	event_set_type l;
	bool cyc = l.insert(e).second;	// return true if actually inserted
	ISE_INVARIANT(cyc);	// not already in collection
	ISE_INVARIANT(e.node);
	// there's a better way to rewrite this...
	do {
		// print
		n = &get_node(e.node);
		e = n->get_cause(e.val);
		if (e.node) {
			o << "caused by: `" << get_node_canonical_name(e.node)
				<< "\' : " <<
				node_type::value_to_char[size_t(e.val)] << endl;
			cyc = l.insert(e).second;
		} else {
			break;
		}
	} while (cyc);
	if (e.node) {
		o << "(cycle reached)" << endl;
	} else {
		o << "(no cycle)" << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wipes out the indexed node, to mark as deallocated and free.  
	Such nodes are skipped during dump.  
	Only called by ExprAlloc.  
 */
void
#if PRSIM_INDIRECT_EXPRESSION_MAP
unique_process_subgraph::void_expr(const expr_index_type ei)
#else
State::void_expr(const expr_index_type ei)
#endif
{
	expr_pool[ei].wipe();
	expr_graph_node_pool[ei].wipe();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::check_node(const node_index_type i) const {
	STACKTRACE_VERBOSE_CHECK;
#if PRSIM_INDIRECT_EXPRESSION_MAP
	// TODO: move this check into unique_process_subgraph
#else
	const node_type& n(node_pool[i]);
	// check pull-up/dn if applicable
#if PRSIM_WEAK_RULES
size_t k = 0;	// NORMAL_RULE = 0, WEAK_RULE = 1 (Node.h)
for ( ; k<2; ++k) {
#endif
	const expr_index_type upi = n.pull_up_index STR_INDEX(k);
	if (is_valid_expr_index(upi)) {
		const expr_state_type& e
			__ATTRIBUTE_UNUSED_CTOR__((expr_pool[upi]));
		assert(e.is_root());
		assert(e.direction());
		assert(e.parent == i);
	}
	const expr_index_type dni = n.pull_dn_index STR_INDEX(k);
	if (is_valid_expr_index(dni)) {
		const expr_state_type& e
			__ATTRIBUTE_UNUSED_CTOR__((expr_pool[dni]));
		assert(e.is_root());
		assert(!e.direction());
		assert(e.parent == i);
	}
#if PRSIM_WEAK_RULES
}
#endif
	// check fanout
	const size_t fs = n.fanout.size();
	size_t j = 0;
	for ( ; j < fs; ++j) {
		assert(expr_graph_node_pool[n.fanout[j]]
			.contains_node_fanin(i));
	}
#endif	// PRSIM_INDIRECT_EXPRESSION_MAP
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Double-checks parent-child relationships.  
	\param i index of the expression, must be < expr_pool.size().  
 */
void
#if PRSIM_INDIRECT_EXPRESSION_MAP
unique_process_subgraph::check_expr(const expr_index_type i) const
#else
State::check_expr(const expr_index_type i) const
#endif
{
	STACKTRACE_VERBOSE_CHECK;
#if PRSIM_INDIRECT_EXPRESSION_MAP
	// TODO: move this check into unique_process_subgraph
	const faninout_map_type& node_pool(local_faninout_map);
	const expr_struct_type& e(expr_pool[i]);
#else
	const expr_state_type& e(expr_pool[i]);
#endif
	const graph_node_type& g(expr_graph_node_pool[i]);
if (!e.wiped()) {
	// check parent
	assert(e.parent);
	if (e.is_root()) {
		assert(e.parent < node_pool.size());
		const node_type& n
			__ATTRIBUTE_UNUSED_CTOR__((node_pool[e.parent]));
#if 0
		// root expression may still be OR-combined by parent!
		rule_map_type::const_iterator fi(rule_map.begin());
		const rule_map_type::const_iterator fe(rule_map.end());
		for ( ; fi!=fe; ++fi) {
			cerr << fi->first << ',';
		}
		cerr << endl;
		const rule_map_type::const_iterator f(rule_map.find(i));
		assert(f != rule_map.end());
#endif
#if PRSIM_WEAK_RULES
#if PRSIM_INDIRECT_EXPRESSION_MAP
		const fanin_array_type&
			fin(n.get_pull_expr(e.direction(), NORMAL_RULE));
		const fanin_array_type&
			wfin(n.get_pull_expr(e.direction(), WEAK_RULE));
		// can use binary search if sorted
		assert(count(fin.begin(), fin.end(), i) == 1);
		assert(count(wfin.begin(), wfin.end(), i) == 1);
#else
		assert(n.get_pull_expr(e.direction(), NORMAL_RULE) == i ||
			n.get_pull_expr(e.direction(), WEAK_RULE) == i);
#endif
#else
#if PRSIM_INDIRECT_EXPRESSION_MAP
		const fanin_array_type& fin(n.get_pull_expr(e.direction()));
		assert(count(fin.begin(), fin.end(), i) == 1);
#else
		assert(n.get_pull_expr(e.direction()) == i);
#endif
#endif	// PRSIM_WEAK_RULES
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
	Duplicates an event *without* binding the node's pending event.
	\return new id of copied event.
 */
event_index_type
State::__copy_event(const event_type& e) {
	return event_pool.allocate(e);
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
		Q: What if set by user (set_node)?
	\param val the future value of the node.
	\pre n must not already have a pending event.
	\pre n must be the node corresponding to node index ni
 */
event_index_type
State::__allocate_event(node_type& n,
		const node_index_type ni,
		cause_arg_type c, 
		const rule_index_type ri,
		const value_enum val
#if PRSIM_WEAK_RULES
		, const bool weak
#endif
		) {
	STACKTRACE_VERBOSE;
	ISE_INVARIANT(!n.pending_event());
	n.set_event(event_pool.allocate(event_type(ni, c, ri, val
#if PRSIM_WEAK_RULES
		, weak
#endif
		)));
	// n.set_cause_node(ci);	// now assign *after* dequeue_event
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
		cause_arg_type c, 
		const value_enum next
#if PRSIM_WEAK_RULES
		, const bool weak
#endif
		) {
	STACKTRACE_VERBOSE;
	// node may or may not have pending event (?)
	// don't care about the node value
	const event_index_type ne = event_pool.allocate(
		event_type(ni, c, INVALID_RULE_INDEX, next
#if PRSIM_WEAK_RULES
		, weak
#endif
		));
	get_event(ne).pending_interference(true);
	// not yet because hasn't been committed to event queue yet
	// n.set_event(ne);		// for consistency?
	// n.set_cause_node(ci);	// now assign *after* dequeue_event
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
	const size_t ne = event_pool.allocate(ev);
	if (!ev.killed()) {
		n.load_event(ne);
	}
	return ne;
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
// inline
const State::event_type&
State::get_event(const event_index_type i) const {
	return event_pool[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// inline
State::event_type&
State::get_event(const event_index_type i) {
	return event_pool[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Removes event from main event queue and dissociates it from
	its affected node.  (node and event are invariably consistent)
	This is only a non-static member function because we want to
	check the state's watch-list for dequeue feedback.  
	Yes, usually the node& and event& are already lookedup, 
	but this should be infrequent enough to not suffer from pessimization.
 */
void
State::kill_event(const event_index_type ei, const node_index_type ni) {
	get_event(ei).kill();
	get_node(ni).clear_event();
	if (UNLIKELY(watching_all_event_queue() ||
			(watching_event_queue() && is_watching_node(ni)))) {
		dump_event_force(cout << "killed  :", ei,
			delay_policy<time_type>::zero, true);
		// flush?
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers event in the primary event queue.  
	Only this is allowed to load killed events.  
 */
// inline
node_index_type
State::load_enqueue_event(const time_type t, const event_index_type ei) {
	INVARIANT(ei);
	ISE_INVARIANT(t >= current_time);
	DEBUG_STEP_PRINT("enqueuing event ID " << ei <<
		" at time " << t << endl);
	const event_type& e(get_event(ei));
	const node_index_type ni = e.node;
	if (UNLIKELY(watching_all_event_queue() ||
		(watching_event_queue() &&
			is_watching_node(e.node)))) {
		dump_event(cout << "enqueued:", ei, t);
	}
	event_queue.push(event_placeholder_type(t, ei));
	return ni;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers event in the primary event queue.  
	\pre corresponding node must back-reference event ei
 */
void
State::enqueue_event(const time_type t, const event_index_type ei) {
	const node_index_type ni = load_enqueue_event(t, ei);
	const node_type& n(get_node(ni));
	ISE_INVARIANT(n.pending_event());
	ISE_INVARIANT(n.get_event() == ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Move event to front of queue.
	Tricky: breaking ties at front of queue!
 */
bool
State::reschedule_event_now(const node_index_type ni) {
	node_type& n(get_node(ni));
	const event_index_type ei = n.get_event();
	if (ei) {
		const event_index_type ne = __copy_event(get_event(ei));
		kill_event(ei, ni);
		n.set_event(ne);
		// HACK the event queue to handle ties at current time
		temp_queue_type tmp;
		while (!event_queue.empty() &&
				event_queue.top().time == current_time) {
			tmp.push_back(event_queue.pop());
		}
		enqueue_event(current_time, ne);
		// silently reschedule the remaining events 
		// behind the rescheduled one
		copy(tmp.begin(), tmp.end(), set_inserter(event_queue));
		return false;
	} else {
		cerr << "Error: there is no pending event on node `" <<
			get_node_canonical_name(ni) << "\'" << endl;
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reschedules a pending event at a different time.  
	Within a group of events at the same time, the rescheduled event will
	be *last*.  
	\param ni the node whose event is to be rescheduled
	\param t the absolute time at which to reschedule, 
		must be >= current_time.
	\return true to signal error: there was no pending event on that node,
		or time is illegal (in the past).
 */
bool
State::reschedule_event(const node_index_type ni, const time_type t) {
if (t < current_time) {
	cerr << "Error: reschedule time must be >= current time." << endl;
	return true;
}
	node_type& n(get_node(ni));
	const event_index_type ei = n.get_event();
	if (ei) {
		const event_index_type ne = __copy_event(get_event(ei));
		kill_event(ei, ni);
		n.set_event(ne);
		enqueue_event(t, ne);
		return false;
	} else {
		cerr << "Error: there is no pending event on node `" <<
			get_node_canonical_name(ni) << "\'" << endl;
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param dt is relative time to previously scheduled event.  
 */
bool
State::reschedule_event_relative(const node_index_type ni, const time_type dt) {
	node_type& n(get_node(ni));
	const event_index_type ei = n.get_event();
	if (ei) {
		// bah! copy and linear search
		typedef	temp_queue_type::const_iterator		const_iterator;
		temp_queue_type temp;
		event_queue.copy_to(temp);
		const_iterator i(temp.begin()), e(temp.end());
		time_type t = delay_policy<time_type>::invalid_value;
		while (i!=e) {
			if (i->event_index == ei) {
				t = i->time;
				break;
			}
			++i;
		}
		INVARIANT(t != delay_policy<time_type>::invalid_value);
		return reschedule_event(ni, t +dt);
	} else {
		cerr << "Error: there is no pending event on node `" <<
			get_node_canonical_name(ni) << "\'" << endl;
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param dt is relative time into the future, must be >= 0.
 */
bool
State::reschedule_event_future(const node_index_type ni, const time_type dt) {
	return reschedule_event(ni, current_time +dt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers event in the exclusive high event queue.  
	(I think this is an unordered worklist.)
 */
void
State::enqueue_exclhi(const time_type t, const event_index_type ei) {
	ISE_INVARIANT(t >= current_time);
	// FAILED ONCE! (no test case) 20071213 after weak rules added
	DEBUG_STEP_PRINT("enqueuing exclhi ID " << ei <<
		" at time " << t << endl);
	typedef	mk_excl_queue_type::value_type		value_type;
	typedef	mk_excl_queue_type::iterator		iterator;
	const pair<iterator, bool> i(exclhi_queue.insert(value_type(ei, t)));
	// eliminate duplicates, check time consistency
	ISE_INVARIANT(i.second || (t == i.first->second));
	get_node(get_event(ei).node).set_excl_queue();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers event in the exclusive low event queue.  
	(I think this is an unordered worklist.)
 */
void
State::enqueue_excllo(const time_type t, const event_index_type ei) {
	ISE_INVARIANT(t >= current_time);
	DEBUG_STEP_PRINT("enqueuing excllo ID " << ei <<
		" at time " << t << endl);
	typedef	mk_excl_queue_type::value_type		value_type;
	typedef	mk_excl_queue_type::iterator		iterator;
	const pair<iterator, bool> i(excllo_queue.insert(value_type(ei, t)));
	// eliminate duplicates, check time consistency
	ISE_INVARIANT(i.second || (t == i.first->second));
	get_node(get_event(ei).node).set_excl_queue();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers event in the pending queue.  
	(I think this is an unordered worklist.)
 */
void
State::enqueue_pending(const event_index_type ei) {
	DEBUG_STEP_PRINT("enqueuing pending ID " << ei << endl);
#if UNIQUE_PENDING_QUEUE
	typedef	pending_queue_type::iterator	iterator;
	const pair<iterator, bool> p(pending_queue.insert(ei));
	INVARIANT(p.second);		// was inserted uniquely
#else
	pending_queue.push_back(ei);
#endif
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
//	n.clear_event();	???
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
State::set_node_time(const node_index_type ni, const value_enum val,
		const time_type t, const bool f) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("setting " << get_node_canonical_name(ni) <<
		" to " << node_type::value_to_char[size_t(val)] <<
		" at " << t << endl);
	// we have ni = the canonically allocated index of the bool node
	// just look it up in the node_pool
	node_type& n(get_node(ni));
	const event_index_type pending = n.get_event();
	const value_enum last_val = n.current_value();
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
		kill_event(pending, ni);
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
			EMPTY_CAUSE,
			INVALID_RULE_INDEX, val
#if PRSIM_WEAK_RULES
			, NORMAL_RULE	// normal strength of 'set'
#endif
			);
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
#if PRSIM_WEAK_RULES
	// strong events take precedence over weak ones
size_t w = 0;	// NORMAL_RULE
do {
#endif
	// evaluate node's pull-up and pull-down
#if PRSIM_INDIRECT_EXPRESSION_MAP
	const pull_enum pu = n.pull_up_state STR_INDEX(w).pull();
	const pull_enum pd = n.pull_dn_state STR_INDEX(w).pull();
#else
	const expr_index_type u = n.pull_up_index STR_INDEX(w);
	const expr_index_type d = n.pull_dn_index STR_INDEX(w);
	// if there is no pull-*, it's as good as off!
	const pull_enum pu = get_pull(u);
	const pull_enum pd = get_pull(d);
#endif
if (pu != PULL_OFF || pd != PULL_OFF) {
	const value_enum new_val = pull_to_value[size_t(pu)][size_t(pd)];
	if (pending) {
		event_type& e(get_event(pending));
		if (e.val != new_val) {
			cerr << "Overriding pending event\'s value on node `"
				<< get_node_canonical_name(ni) << "\' from " <<
				node_type::value_to_char[e.val] << " to " <<
				node_type::value_to_char[new_val] <<
				", keeping the same event time." << endl;
			e.val = new_val;
		}
#if PRSIM_WEAK_RULES
		e.set_weak(w);
#endif
		e.unforce();
		// else do nothing, correct value already pending
	} else {
		// no event pending, can make one
		const value_enum current = n.current_value();
		if (current == LOGIC_LOW &&
				pu == PULL_OFF) {
			// nothing pulling up, no change
			return;
		} else if (current == LOGIC_HIGH &&
				pd == PULL_OFF) {
			// nothing pulling down, no change
			return;
		} else if (current != new_val) {
#if 0
			rule_index_type ri;
			// not true, rule index may be from OR-combination...
			if (pu == char(PULL_ON) &&
				pd == char(PULL_OFF)) {
				ri = u;
			} else if (pd == char(PULL_ON) &&
				pu == char(PULL_OFF)) {
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
				n, ni, EMPTY_CAUSE, ri, new_val
#if PRSIM_WEAK_RULES
				, w
#endif
				);
			event_type& e(get_event(ei));
			time_type t;
			switch (new_val) {
			case LOGIC_HIGH:
				t = get_delay_up(e); break;
			case LOGIC_LOW:
				t = get_delay_dn(e); break;
			default: t = current_time;
				// +delay_policy<time_type>::zero;
			}
			enqueue_event(t, ei);
		}
		// else node is already in correct state
	}
#if PRSIM_WEAK_RULES
	break;	// ignore weak rules, normal rules will take precedence
#endif
}	// else neither side is pulling, leave node as is
#if PRSIM_WEAK_RULES
	++w;
} while (weak_rules_enabled() && w<2);
#endif
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print the current mode.  
 */
ostream&
State::dump_mode(ostream& o) const {
	o << "mode: ";
	o << "\tweak rules " << (weak_rules_enabled() ? "on" : "off") << endl;
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
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Show current timing mode.
	Show random seed?
 */
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
 */
bool
State::set_timing(const string& m, const string_list& a) {
	static const string __random("random");
	static const string __uniform("uniform");
	static const string __after("after");
	if (m == __random) {
		timing_mode = TIMING_RANDOM;
		switch (a.size()) {
		case 0:	return false;
		case 1:	cerr << "use \'seed48' to set random number seed"
			<< endl;
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
	o << "\trandom" << endl;
	o << "\tuniform [delay]" << endl;
	o << "\tafter" << endl;
	o <<
"Random mode uses a heavy-tailed distribution random-variable for delay, "
"*except* where a delay is marked with [after=0].\n"
"Uniform mode ignores all after-delay annotations and uses a fixed delay "
"for all events, which can be used to count transitions.\n"
"After mode uses fixed after-annotated delays for timing, and assumes "
"default delays where none are given.\n"
"Use \'seed48\' to set a random number seed." << endl;
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
	const rule_type* r(lookup_rule(e.cause_rule));
return current_time +
	(timing_mode == TIMING_RANDOM ?
		(e.cause_rule && time_traits::is_zero(r->after) ?
			time_traits::zero : ((0x01 << 11)*random_delay()))
		:
	(timing_mode == TIMING_UNIFORM ? uniform_delay :
	// timing_mode == TIMING_AFTER
	//	(e.cause_rule ?
	//		r->after : 0)
		(e.cause_rule ?
			(r->is_always_random() ? 
				(r->after * random_delay())
			: r->after) : 0)
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
	const rule_type* r(lookup_rule(e.cause_rule));
	return current_time +
		(timing_mode == TIMING_RANDOM ?
		(e.cause_rule && time_traits::is_zero(r->after) ?
			time_traits::zero : ((0x01 << 11)*random_delay()))
			:
		(timing_mode == TIMING_UNIFORM ? uniform_delay :
		// timing_mode == TIMING_AFTER
		//	(e.cause_rule ?
		//		r->after : 0)
			(e.cause_rule ?
				(r->is_always_random() ?
					(r->after * random_delay())
                        : r->after) : 0)	
		));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if expression is a root-level expression
	(and hence, has attributes)
 */
bool
State::is_rule_expr(const expr_index_type ei) const {
#if PRSIM_INDIRECT_EXPRESSION_MAP
	return lookup_rule(ei);
#else
	return rule_map.find(ei) != rule_map.end();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenience function, instead of calling rule_map.find() directly.
 */
const State::rule_type*
State::lookup_rule(const expr_index_type ei) const {
#if PRSIM_INDIRECT_EXPRESSION_MAP
	// translate global expression index to 
	// global process index with local expression index,
	// and forward the lookup
#else
	if (ei) {
		typedef	rule_map_type::const_iterator	rule_map_iterator;
		const rule_map_iterator i(rule_map.find(ei));
		if (i != rule_map.end()) {
			return &i->second;
		}
	}
	return NULL;
#endif
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
	break_type err = false;
	const_iterator i(pending_queue.begin()), e(pending_queue.end());
for ( ; i!=e; ++i) {
	const event_index_type ne = *i;
	DEBUG_STEP_PRINT("checking pending event ID: " << ne << endl);
	event_type& ev(get_event(ne));
	const node_index_type& _ni(ev.node);
	DEBUG_STEP_PRINT("... on node " <<
		get_node_canonical_name(_ni) << endl);
	node_type& _n(get_node(_ni));

	// are weak events ever inserted into the pending queue?
#if PRSIM_INDIRECT_EXPRESSION_MAP
	const pull_enum pull_up_state =
		_n.pull_up_state STR_INDEX(NORMAL_RULE).pull();
	const pull_enum pull_dn_state =
		_n.pull_dn_state STR_INDEX(NORMAL_RULE).pull();
#else
	const expr_index_type up_ex = _n.pull_up_index STR_INDEX(NORMAL_RULE);
	const expr_index_type dn_ex = _n.pull_dn_index STR_INDEX(NORMAL_RULE);
	const pull_enum pull_up_state = get_pull(up_ex);
	const pull_enum pull_dn_state = get_pull(dn_ex);
#endif
	DEBUG_STEP_PRINT("current pull-states: up=" <<
		size_t(pull_up_state) << ", dn=" <<
		size_t(pull_dn_state) << endl);
#if PRSIM_WEAK_RULES
#if PRSIM_INDIRECT_EXPRESSION_MAP
	const pull_enum wpull_up_state =
		_n.pull_up_state STR_INDEX(WEAK_RULE).pull();
	const pull_enum wpull_dn_state =
		_n.pull_dn_state STR_INDEX(WEAK_RULE).pull();
#else
	const expr_index_type wup_ex = _n.pull_up_index STR_INDEX(WEAK_RULE);
	const expr_index_type wdn_ex = _n.pull_dn_index STR_INDEX(WEAK_RULE);
	const pull_enum wpull_up_state = get_pull(wup_ex);
	const pull_enum wpull_dn_state = get_pull(wdn_ex);
#endif
	DEBUG_STEP_PRINT("weak pull-states:    up=" <<
		size_t(wpull_up_state) << ", dn=" <<
		size_t(wpull_dn_state) << endl);
#endif
	// check strong (normal) rules first
	if ((pull_up_state != PULL_OFF) &&
		(pull_dn_state != PULL_OFF)) {
	/***
		There is interference.  If there is weak interference,
		suppress report unless explicitly requested.  
		weak = (X && T) or (T && X);
	***/
		DEBUG_STEP_PRINT("some interference." << endl);
		const bool pending_weak =
			(pull_up_state == PULL_WEAK) ||
			(pull_dn_state == PULL_WEAK);
			// not XOR (^), see pending_weak table in prs.c
		// issue diagnostic
		if ((weak_interference_policy != ERROR_IGNORE) ||
				!pending_weak) {
			err |=
			__report_interference(cout, pending_weak, _ni, ev);
		}
		if (ev.pending_interference()) {
			DEBUG_STEP_PRINT("immediate -> X." << endl);
			// ISE_INVARIANT(_n.pending_event());
			// might have been dequeued due to unstable-dequeue
			// in which case, enqueue this event
			const bool still_pending = _n.pending_event();
		if (still_pending) {
			// always set the cause and new value together
			event_type& pe(get_event(_n.get_event()));
			pe.val = LOGIC_OTHER;
			pe.cause.node = ev.cause.node;
			pe.cause.val = ev.cause.val;
			__deallocate_pending_interference_event(ne);
		} else {
			// INVARIANT(dequeue_unstable_events());
			/**
				This can happen b/c pending_queue may
				contain duplicates for the node, 
				from interference and evaluation ordering.  
			**/
			DEBUG_STEP_PRINT("re-queue to X." << endl);
			ev.val = LOGIC_OTHER;
			__flush_pending_event_with_interference(_n, ne, ev);
		}	// end if still_pending
		} else {
			DEBUG_STEP_PRINT("overwrite to X." << endl);
			ev.val = LOGIC_OTHER;
			__flush_pending_event_with_interference(_n, ne, ev);
		}	// end if pending_interference
#if PRSIM_WEAK_RULES
	} else if (weak_rules_enabled() &&
		(pull_up_state == PULL_OFF) &&
		(pull_dn_state == PULL_OFF) &&
		(wpull_up_state != PULL_OFF) &&
			(wpull_dn_state != PULL_OFF)) {
	/***
		There is interference between weak rules. 
		Rest of the code in this clause is copied from above.
	***/
		DEBUG_STEP_PRINT("some interference (weak rules)." << endl);
		const bool pending_weak =
			(wpull_up_state == PULL_WEAK) ||
			(wpull_dn_state == PULL_WEAK);
			// not XOR (^), see pending_weak table in prs.c
		// issue diagnostic
		if ((weak_interference_policy != ERROR_IGNORE) ||
				!pending_weak) {
			err |=
			__report_interference(cout, pending_weak, _ni, ev);
		}
		if (ev.pending_interference()) {
			DEBUG_STEP_PRINT("immediate -> X." << endl);
			// ISE_INVARIANT(_n.pending_event());
			// might have been dequeued due to unstable-dequeue
			// in which case, enqueue this event
			const bool still_pending = _n.pending_event();
		if (still_pending) {
			// always set the cause and new value together
			event_type& pe(get_event(_n.get_event()));
			pe.val = LOGIC_OTHER;
			pe.cause.node = ev.cause.node;
			pe.cause.val = ev.cause.val;
			__deallocate_pending_interference_event(ne);
		} else {
			INVARIANT(dequeue_unstable_events());
			DEBUG_STEP_PRINT("re-queue to X." << endl);
			ev.val = LOGIC_OTHER;
			__flush_pending_event_with_interference(_n, ne, ev);
		}	// end if still_pending
		} else {
			DEBUG_STEP_PRINT("overwrite to X." << endl);
			ev.val = LOGIC_OTHER;
			__flush_pending_event_with_interference(_n, ne, ev);
		}	// end if pending_interference
#endif	// PRSIM_WEAK_RULES
	} else {
		// should also cover overpowered weak-rules
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
			// sanity: referring to same node
			ISE_INVARIANT(pv.node == _ni);
			// walk event_queue, find delay of the former event?
			// inspect event values?
			if (dequeue_unstable_events()) {
			DEBUG_STEP_PRINT("replacing original event" << endl);
				// new behavior: cancel the original event
				// which was updated to X
				_n.clear_event();
#if 1
				if (UNLIKELY(!pv.killed())) {
				cerr << "former event: (pe)" << endl;
				dump_event(cerr, pe, 0.0) << endl;
				cerr << "new event: (ne)" << endl;
				dump_event(cerr, ne, 0.0) << endl;
				}
#endif
				ISE_INVARIANT(pv.killed());
				// FAILED ONCE: 20071217, since weak rules
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
	return err;
}	// end method flush_pending_queue

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For the sake of shortening long code.  
	\param ev pending event, corresponds to index...
	\param ne index of the pending event.
 */
// inline
void
State::__flush_pending_event_with_interference(node_type& _n, 
		const event_index_type ne, event_type& ev) {
	STACKTRACE_VERBOSE_STEP;
	switch (_n.current_value()) {
	case LOGIC_LOW:
	DEBUG_STEP_PRINT("moving - event to event queue" << endl);
		_n.set_event_consistent(ne);	// not necessarily linked yet
		enqueue_event(get_delay_dn(ev), ne);
		break;
	case LOGIC_HIGH:
	DEBUG_STEP_PRINT("moving + event to event queue" << endl);
		_n.set_event_consistent(ne);	// not necessarily linked yet
		enqueue_event(get_delay_up(ev), ne);
		break;
	case LOGIC_OTHER:
	DEBUG_STEP_PRINT("cancelling new event" << endl);
		_n.clear_excl_queue();
		if (_n.get_event() == ne) {
			__deallocate_event(_n, ne);
		} else {
			__deallocate_pending_interference_event(ne);
		}
		// difference: n.clear_event()
		break;
	default:
		ISE(cerr, 
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
	STACKTRACE_VERBOSE_STEP;
	// if event is weak rule, require the opposing pull to be off
	if (_n.current_value() != ev.val) {
#if PRSIM_WEAK_RULES
		const bool w = ev.is_weak();
#endif
		// not necessarily linked yet
		_n.set_event_consistent(ne);
		if (ev.val == LOGIC_HIGH) {
		DEBUG_STEP_PRINT("moving + event to event queue" << endl);
#if PRSIM_WEAK_RULES
			// the opposing strong pull:
			const pull_enum opp =
#if PRSIM_INDIRECT_EXPRESSION_MAP
				_n.pull_dn_state STR_INDEX(NORMAL_RULE).pull();
#else
				get_pull(_n.pull_dn_index[NORMAL_RULE]);
#endif
			if (!w || (opp == PULL_OFF)) {
#endif	// PRSIM_WEAK_RULES
			enqueue_event(get_delay_up(ev), ne);
			return;
#if PRSIM_WEAK_RULES
			}
#endif
		} else {
		DEBUG_STEP_PRINT("moving - event to event queue" << endl);
#if PRSIM_WEAK_RULES
			// the opposing strong pull:
			const pull_enum opp =
#if PRSIM_INDIRECT_EXPRESSION_MAP
				_n.pull_up_state STR_INDEX(NORMAL_RULE).pull();
#else
				get_pull(_n.pull_up_index[NORMAL_RULE]);
#endif
			if (!w || (opp == PULL_OFF)) {
#endif	// PRSIM_WEAK_RULES
			enqueue_event(get_delay_dn(ev), ne);
			return;
#if PRSIM_WEAK_RULES
			}
#endif
		}
	}
	// fall-through
	{
		DEBUG_STEP_PRINT("cancelling event" << endl);
		// no change in value, just cancel
		_n.clear_excl_queue();
	// fixed? copied from above, with interference
	// TODO: need test case to reach this code
	if (_n.get_event() == ne) {
		__deallocate_event(_n, ne);
	} else {
		__deallocate_pending_interference_event(ne);
	}
		// difference: n.clear_event()
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
	STACKTRACE_VERBOSE_STEP;
	_n.set_event(ne);
	switch (ev.val) {
	case LOGIC_LOW:
	DEBUG_STEP_PRINT("moving - event to event queue" << endl);
		enqueue_event(get_delay_dn(ev), ne);
		break;
	case LOGIC_HIGH:
	DEBUG_STEP_PRINT("moving + event to event queue" << endl);
		enqueue_event(get_delay_up(ev), ne);
		break;
	case LOGIC_OTHER:
	DEBUG_STEP_PRINT("don't know what to do!" << endl);
		// should depend on pull up or down...
		FINISH_ME(Fang);
		enqueue_event(get_delay_up(ev), ne);
		// _n.clear_excl_queue();
		// __deallocate_event(_n, ne);
		break;
	default:
		ISE(cerr, 
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
	typedef	mk_excl_queue_type::value_type		value_type;
	typedef	mk_excl_queue_type::iterator		iterator;
	typedef	mk_excl_queue_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE_STEP;
	// where graduating events are collected, and made unique
	mk_excl_queue_type staging_q;
	const_iterator i(exclhi_queue.begin()), e(exclhi_queue.end());
for ( ; i!=e; ++i) {
	const event_placeholder_type ep(i->second, i->first);
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
			if (n.current_value() == LOGIC_HIGH ||
				(n.pending_event() && 
				(get_event(n.get_event()).val ==
					LOGIC_HIGH) &&
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
			const pair<iterator, bool>
				r(staging_q.insert(value_type(
					ep.event_index, ep.time)));
			ISE_INVARIANT(r.second || r.first->second == ep.time);
			// not sure whether or not this should be delayed
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
	// now we've guaranteed uniqueness
	const_iterator si(staging_q.begin()), se(staging_q.end());
	for ( ; si!=se; ++si) {
		enqueue_event(si->second, si->first);
		// get_node(get_event(si->first).node).clear_excl_queue();
	}
	exclhi_queue.clear();
}	// end method flush_exclhi_queue

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Processes exclusive low ring enqueued events.  
 */
void
State::flush_excllo_queue(void) {
	typedef	mk_excl_queue_type::value_type		value_type;
	typedef	mk_excl_queue_type::iterator		iterator;
	typedef	mk_excl_queue_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE_STEP;
	// where graduating events are collected, and made unique
	mk_excl_queue_type staging_q;
	const_iterator i(excllo_queue.begin()), e(excllo_queue.end());
for ( ; i!=e; ++i) {
	const event_placeholder_type ep(i->second, i->first);
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
			if (n.current_value() == LOGIC_LOW ||
				(n.pending_event() && 
				(get_event(n.get_event()).val ==
					LOGIC_LOW) &&
				!n.in_excl_queue())) {
				++prev;
			}
			if (ni == epni) {
				flag = true;
			}
		}
		// alert: can belong to more than one ring!
		if (flag && !prev) {
			DEBUG_STEP_PRINT("enqueuing event" << endl);
			// then insert event into primary queue
			// keep the same event_index
			const pair<iterator, bool>
				r(staging_q.insert(value_type(
					ep.event_index, ep.time)));
			ISE_INVARIANT(r.second || r.first->second == ep.time);
			// not sure whether or not this should be delayed
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
	// now we've guaranteed uniqueness
	const_iterator si(staging_q.begin()), se(staging_q.end());
	for ( ; si!=se; ++si) {
		enqueue_event(si->second, si->first);
		// get_node(get_event(si->first).node).clear_excl_queue();
	}
	excllo_queue.clear();
}	// end method flush_excllo_queue

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enforces exclusive high rings by enqueue necessary events 
	into the exclusive high queue.  
	\param ni index of the node that changed that affects exclhi rings.  
 */
void
State::enforce_exclhi(cause_arg_type c) {
	/***
		If n.exclhi and n is 0, check if any of the nodes
		in n's exclhi rings are enabled.  
		If so, insert them into the exclhi-queue.
	***/
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE_STEP;
	const node_index_type& ni(c.node);
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
			// strong rules only
			const pull_enum pei =
#if PRSIM_INDIRECT_EXPRESSION_MAP
				er.pull_up_state STR_INDEX(NORMAL_RULE).pull();
#else
				get_pull(er.pull_up_index STR_INDEX(NORMAL_RULE));
#endif
			if (!er.pending_event() &&
				// er->n->up->val == PRS_VAL_T
				// what if is pulling weakly?
				pei == PULL_ON) {
			DEBUG_STEP_PRINT("enqueuing pull-up event" << endl);
				const event_index_type ne =
					// the pull-up index may not necessarily
					// correspond to the causing expression!
					__allocate_event(er, eri, c,
						// not sure...
						// er.pull_up_index, 
						INVALID_RULE_INDEX, 
						LOGIC_HIGH
#if PRSIM_WEAK_RULES
						, NORMAL_RULE
#endif
						);
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
State::enforce_excllo(cause_arg_type c) {
	/***
		If n.excllo and n is 1, check if any of the nodes
		in n's excllo rings are enabled.  
		If so, insert them into the excllo-queue.
	***/
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE_STEP;
	const node_index_type& ni(c.node);
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
				// strong rules only
			const pull_enum pei =
#if PRSIM_INDIRECT_EXPRESSION_MAP
				er.pull_dn_state STR_INDEX(NORMAL_RULE).pull();
#else
				get_pull(er.pull_dn_index STR_INDEX(NORMAL_RULE));
#endif
			if (!er.pending_event() &&
				// er->n->dn->val == PRS_VAL_T
				// what if is pulling weakly?
				pei == PULL_ON) {
			DEBUG_STEP_PRINT("enqueuing pull-dn event" << endl);
				const event_index_type ne =
					// same comment as enforce_exclhi
					__allocate_event(er, eri, c, 
						// er.pull_dn_index, 
						INVALID_RULE_INDEX,
						LOGIC_LOW
#if PRSIM_WEAK_RULES
						, NORMAL_RULE
#endif
						);
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
		const value_enum prev, const value_enum next) {
	typedef	check_excl_ring_map_type::const_iterator	const_iterator;
	typedef	lock_index_list_type::const_iterator	lock_index_iterator;
	typedef	check_excl_lock_pool_type::reference	lock_reference;
	if (n.has_check_exclhi()) {
	if (next == LOGIC_HIGH) {
		// need to lock
		const const_iterator f(check_exhi.find(ni));
		ISE_INVARIANT(f != check_exhi.end());
		// make sure all locks are free
		lock_index_iterator
			i(f->second.begin()), e(f->second.end());
		for ( ; i!=e; ++i) {
			const lock_index_type li = *i;
			ISE_INVARIANT(li);
			ISE_INVARIANT(li < check_exhi_ring_pool.size());
			lock_reference l(check_exhi_ring_pool[li]);
			// set all locks
			if (l) {
				// identify lock ID, and node ID
				return excl_exception(true, li, ni);
			} else {
				l = true;	// lock ring
			}
		}
	} else if (prev == LOGIC_HIGH && next != prev) {
		// need to unlock
		const const_iterator f(check_exhi.find(ni));
		ISE_INVARIANT(f != check_exhi.end());
		// make sure all locks are free
		lock_index_iterator
			i(f->second.begin()), e(f->second.end());
		for ( ; i!=e; ++i) {
			const lock_index_type li = *i;
			ISE_INVARIANT(li);
			ISE_INVARIANT(li < check_exhi_ring_pool.size());
			// unlock ring
			check_exhi_ring_pool[li] = false;
		}
	}
	}	// end if n.has_check_exclhi()
	if (n.has_check_excllo()) {
	if (next == LOGIC_LOW) {
		// need to lock
		const const_iterator f(check_exlo.find(ni));
		ISE_INVARIANT(f != check_exlo.end());
		// make sure all locks are free
		lock_index_iterator
			i(f->second.begin()), e(f->second.end());
		for ( ; i!=e; ++i) {
			const lock_index_type li = *i;
			ISE_INVARIANT(li);
			ISE_INVARIANT(li < check_exlo_ring_pool.size());
			lock_reference l(check_exlo_ring_pool[li]);
			// set all locks
			if (l) {
				// identify lock ID, and node ID
				return excl_exception(false, li, ni);
			} else {
				l = true;	// lock ring
			}
		}
	} else if (prev == LOGIC_LOW && next != prev) {
		// need to unlock
		const const_iterator f(check_exlo.find(ni));
		ISE_INVARIANT(f != check_exlo.end());
		// make sure all locks are free
		lock_index_iterator
			i(f->second.begin()), e(f->second.end());
		for ( ; i!=e; ++i) {
			const lock_index_type li = *i;
			ISE_INVARIANT(li);
			ISE_INVARIANT(li < check_exlo_ring_pool.size());
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
State::inspect_exception(const step_exception& ex, ostream& o) const {
if (IS_A(const excl_exception*, &ex)) {
	typedef	check_excl_ring_map_type::const_iterator	const_iterator;
	const excl_exception& exex(AS_A(const excl_exception&, ex));
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
	ISE_INVARIANT(ring.size() > 1);
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
	o << "The simulator\'s excl-check-lock state is no longer coherent; "
		"do not bother trying to continue the simulation, "
		"but you may further inspect the state." << endl;
	o << "You probably want to disable excl-checking with `nocheckexcl\' "
		"if you wish to continue the simulation." << endl;
#if PRSIM_CHANNEL_SUPPORT
} else if (IS_A(const channel_exception*, &ex)) {
	const channel_exception& exex(AS_A(const channel_exception&, ex));
	o << "ERROR: value assertion failed on channel `" <<
		exex.name << "\'." << endl;
	o << "\texpected: " << exex.expect << ", got: " << exex.got << endl;
#endif
} else {
	o << "Unkonwn step_exception." << endl;
}
}	// end method State::inspect_excl_exception

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For the sake of exception safety, 
	Upon destruction, flush intermediate event queues.  
 */
struct State::auto_flush_queues {
	State&		state;

	explicit
	auto_flush_queues(State& s) : state(s) { }

	~auto_flush_queues() {
		// check and flush pending queue, spawn fanout events
		if (UNLIKELY(state.flush_pending_queue())) {
			state.stop();		// set stop flag
		}

		// check and flush pending queue against exclhi/lo events
		state.flush_exclhi_queue();
		state.flush_excllo_queue();
	}
} __ATTRIBUTE_UNUSED__ ;

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
State::step(void) THROWS_STEP_EXCEPTION {
	typedef	State::step_return_type		return_type;
	STACKTRACE_VERBOSE;
	ISE_INVARIANT(pending_queue.empty());
	ISE_INVARIANT(exclhi_queue.empty());
	ISE_INVARIANT(excllo_queue.empty());

	const auto_flush_queues __auto_flush(*this);
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
	const value_enum prev = n.current_value();
	node_index_type _ci;	// just a copy
{
	const event_cause_type& cause(pe.cause);
	const node_index_type& ci(cause.node);
	_ci = ci;
	DEBUG_STEP_PRINT("examining node: " <<
		get_node_canonical_name(ni) << endl);
	ISE_INVARIANT(n.pending_event());	// must have been pending
	ISE_INVARIANT(n.get_event() == ei);	// must be consistent!
{
	// event-deallocation scope (optional)
	// const event_deallocator __d(*this, n, ei);	// auto-deallocate?
	DEBUG_STEP_PRINT("former value: " <<
		node_type::value_to_char[size_t(prev)] << endl);
	DEBUG_STEP_PRINT("new value: " <<
		node_type::value_to_char[size_t(pe.val)] << endl);
	if (pe.val == LOGIC_OTHER &&
		prev == LOGIC_OTHER) {
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
		ISE_INVARIANT(prev != pe.val || n.is_unstab() || force);
		// FAILED ONCE! (test case?)
		// occurred on 20071214 after adding weak rules
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
	// only set the cause of the node when we change its value
	n.set_value_and_cause(pe.val, cause);
	// count transition only if new value is not X
	if (pe.val != LOGIC_OTHER) {
		++n.tcount;
	}
	__deallocate_event(n, ei);
}
}
	// note: pe is invalid, deallocated beyond this point, could scope it
	// reminder: do not reference pe beyond this point (deallocated)
	// could scope the reference to prevent it...
	const value_enum next = n.current_value();
	// value propagation...
	const event_cause_type new_cause(ni, next);
{
	typedef	node_type::const_fanout_iterator	const_iterator;
	const_iterator i, e;
if (eval_ordering_is_random()) {
	__shuffle_indices.clear();
	std::copy(n.fanout.begin(), n.fanout.end(), 
		std::back_inserter(__shuffle_indices));
	std::random_shuffle(__shuffle_indices.begin(), __shuffle_indices.end());
	i = __shuffle_indices.begin();
	e = __shuffle_indices.end();
} else {
	i = n.fanout.begin();
	e = n.fanout.end();
}
	for ( ; i!=e; ++i) {
		// when evaluating a node as an expression, 
		// is appropriate to interpret node value
		// as a pull-value
		if (UNLIKELY(propagate_evaluation(new_cause, *i, pull_enum(prev)))) {
			stop();
		}
	}
}
#if PRSIM_CHANNEL_SUPPORT
	// Q: is this the best place to handle this?
if (n.in_channel()) {
	// predicate may not filter precisely
	// channel manager should 'ignore' irrelevant nodes
	vector<env_event_type> env_events;
	// the following may throw channel_exception
	_channel_manager.process_node(*this, ni,
			prev, next, env_events);
	// cause of these events must be 'ni', this node
	const event_cause_type c(ni, next);
	if (UNLIKELY(flush_channel_events(env_events, c))) {
		stop();
	}
	// HERE: error status?
}
#endif	// PRSIM_CHANNEL_SUPPORT
	/***
		If an event is forced (say, by user), then check node's own
		guards to determine whether or not a new event needs to
		be registered on this node.  
		FIXME: prs.c checks for !n->queue
	***/
	if (force && n.get_event()) {
		DEBUG_STEP_PRINT("detected a forced event vs. pending event" << endl);
#if PRSIM_WEAK_RULES
	// the opposing strong pull:
#if PRSIM_INDIRECT_EXPRESSION_MAP
	const pull_enum nup = n.pull_up_state[NORMAL_RULE].pull();
	const pull_enum ndn = n.pull_dn_state[NORMAL_RULE].pull();
#else
	const pull_enum nup = get_pull(n.pull_up_index[NORMAL_RULE]);
	const pull_enum ndn = get_pull(n.pull_dn_index[NORMAL_RULE]);
#endif
	size_t w = NORMAL_RULE;		// 0
	do {
#endif
		cout << "THIS CODE HAS NEVER BEEN REACHED?" << endl;
#if 0
		// The following code looks wrong...
		const event_index_type ui = n.pull_up_index STR_INDEX(w);
		const event_index_type di = n.pull_dn_index STR_INDEX(w);
		const event_type* up_rule = ui ? &get_event(ui) : NULL;
		const event_type* dn_rule = di ? &get_event(di) : NULL;
#else
#if PRSIM_INDIRECT_EXPRESSION_MAP
		const pull_enum pup = n.pull_up_state STR_INDEX(w).pull();
		const pull_enum pdn = n.pull_dn_state STR_INDEX(w).pull();
#else
		const pull_enum pup = get_pull(n.pull_up_index STR_INDEX(w));
		const pull_enum pdn = get_pull(n.pull_dn_index STR_INDEX(w));
#endif
#endif
		const bool possible_up = pup == PULL_ON
				&& next != LOGIC_HIGH
#if PRSIM_WEAK_RULES
				// check opposition
				&& (!w || (ndn == PULL_OFF))
#endif
				;
		const bool possible_dn = pdn == PULL_ON
				&& next != LOGIC_LOW
#if PRSIM_WEAK_RULES
				// check opposition
				&& (!w || (nup == PULL_OFF))
#endif
				;
		if (possible_up) {
			DEBUG_STEP_PRINT("force pull-up" << endl);
			const event_index_type _ne =
				__allocate_event(n, ni, EMPTY_CAUSE, 
					INVALID_RULE_INDEX, // ui, // cause?
					LOGIC_HIGH
#if PRSIM_WEAK_RULES
					, w	// rule_strength
#endif
					);
			enqueue_pending(_ne);
#if PRSIM_WEAK_RULES
			break;
#endif
		}
		else if (possible_dn) {
			DEBUG_STEP_PRINT("force pull-dn" << endl);
			const event_index_type _ne =
				__allocate_event(n, ni, EMPTY_CAUSE, 
					INVALID_RULE_INDEX, // di, // cause?
					LOGIC_LOW
#if PRSIM_WEAK_RULES
					, w	// rule_strength
#endif
					);
			enqueue_pending(_ne);
#if PRSIM_WEAK_RULES
			break;
#endif
		}
#if PRSIM_WEAK_RULES
		// above strong pulls take precedence over weak pulls
		++w;
	} while (weak_rules_enabled() && w<2);
#endif
	}	// end if forced && pending event

	// exclhi ring enforcement
	if (n.has_mk_exclhi() && (next == LOGIC_LOW)) {
		enforce_exclhi(new_cause);
	}	// end if (exclhi enforcement)

	// excllo ring enforcement
	if (n.has_mk_excllo() && (next == LOGIC_HIGH)) {
		enforce_excllo(new_cause);
	}	// end if (excllo enforcement)

	// energy estimation?  TODO later for a different sim variant
	// queues automatically flushed by dtor of auto_flush_queues

#if 0
	// very slow, but terrific for debugging!!!
	check_event_queue();
#endif

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
		value_enum prev, value_enum next) {
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
State::evaluate(const node_index_type ni,
#if PRSIM_INDIRECT_EXPRESSION_MAP
		expr_index_type gui, 
#else
		expr_index_type ui, 
#endif
		pull_enum prev, pull_enum next) {
	STACKTRACE_VERBOSE_STEP;
	DEBUG_STEP_PRINT("node " << ni << " from " <<
		node_type::value_to_char[size_t(prev)] << " -> " <<
		node_type::value_to_char[size_t(next)] << endl);
	expr_state_type* u;
	__scratch_expr_trace.clear();
#if PRSIM_INDIRECT_EXPRESSION_MAP
	const expr_struct_type* s;
	process_expr_map_type::const_iterator
		f(process_expr_map.upper_bound(gui));
	INVARIANT(f != process_expr_map.begin());
	--f;
	expr_index_type ui = gui -f->first;
	const process_index_type pid = f->second;
	const size_t pti = process_expr_map.find(pid)->second;
	process_sim_state& ps(process_state_array[pid]);
	unique_process_subgraph& pg(unique_process_pool[pti]);
#define	STRUCT	s
#else
#define	STRUCT	u
#endif
do {
	pull_enum old_pull, new_pull;	// pulling state of the subexpression
#if PRSIM_INDIRECT_EXPRESSION_MAP
	s = &pg.expr_pool[ui];
	u = &ps.expr_states[ui];
#else
	u = &expr_pool[ui];
#endif
	__scratch_expr_trace.push_back(ui);
#if DEBUG_STEP
	DEBUG_STEP_PRINT("examining expression ID: " << ui << endl);
	STRUCT->dump_struct(STACKTRACE_INDENT) << endl;
	u->dump_state(STACKTRACE_INDENT << "before: ") << endl;
#endif
	// trust compiler to effectively perform branch-invariant
	// code-motion
	if (STRUCT->is_disjunctive()) {
		// is disjunctive (or)
		DEBUG_STEP_PRINT("is_or()" << endl);
		// countdown represents the number of 1's
#if PRSIM_INDIRECT_EXPRESSION_MAP
		old_pull = u->or_pull_state(*s);
#else
		old_pull = u->or_pull_state();
#endif
		u->unknowns += (next >> 1) - (prev >> 1);
		u->countdown += (next & LOGIC_VALUE)
			- (prev & LOGIC_VALUE);
#if PRSIM_INDIRECT_EXPRESSION_MAP
		new_pull = u->or_pull_state(*s);
#else
		new_pull = u->or_pull_state();
#endif
	} else {
		DEBUG_STEP_PRINT("is_and()" << endl);
		// is conjunctive (and)
#if PRSIM_INDIRECT_EXPRESSION_MAP
		old_pull = u->and_pull_state(*s);
#else
		old_pull = u->and_pull_state();
#endif
		// countdown represents the number of 0's
		u->unknowns += (next >> 1) - (prev >> 1);
		u->countdown += !next - !prev;
#if PRSIM_INDIRECT_EXPRESSION_MAP
		new_pull = u->and_pull_state(*s);
#else
		new_pull = u->and_pull_state();
#endif
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
	ui = STRUCT->parent;
} while (!STRUCT->is_root());
#undef	STRUCT
	// made it to root
	// negation already accounted for
#if PRSIM_INDIRECT_EXPRESSION_MAP
	return evaluate_return_type(gui, u, next);
#else
	return evaluate_return_type(ui, u, next);
#endif
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
		NOW, THIS IS A LOCAL EXPRESSION INDEX, relative to pid
	\param pid process index.
	\param prev the former value of the node/subexpression
	\param next the new value of the node/subexpression.
		Locally, this is used as index of the root expression, 
		in the event that an evaluation propagates to root.  
	NOTE: the action table here depends on the expression-type's
		subtype encoding.  For now, we use the Expr's encoding.  
 */
State::break_type
State::propagate_evaluation(
		cause_arg_type c, 
		const expr_index_type exi, 
		pull_enum prev
		) {
	STACKTRACE_VERBOSE_STEP;
	const node_index_type& ni(c.node);
	pull_enum next;	// just local variable
	// when evaluating node as expression, interpret value as pull
	const evaluate_return_type
		ev_result(evaluate(ni, exi, prev, pull_enum(c.val)));
	if (!ev_result.node_index) {
		return false;
	}
	next = ev_result.root_pull;
	const node_index_type ui = ev_result.node_index;
#if PRSIM_INDIRECT_EXPRESSION_MAP
	process_expr_map_type::const_iterator
		f(process_expr_map.upper_bound(exi));
	INVARIANT(f != process_expr_map.begin());
	--f;
	const expr_index_type lei = exi -f->first;
	const process_index_type pid = f->second;
	const size_t pti = process_expr_map.find(pid)->second;
	// process_sim_state& ps(process_state_array[pid]);
	unique_process_subgraph& pg(unique_process_pool[pti]);
	const expr_struct_type* const u = &pg.expr_pool[lei];
#else
	const expr_state_type* const u(ev_result.root_ex);
#endif
	// we delay the root rule search until here to reduce the amount
	// of searching required to find the responsible rule expression.  
	rule_index_type root_rule;
{
	typedef	expr_trace_type::const_reverse_iterator	trace_iterator;
	trace_iterator ri(__scratch_expr_trace.rbegin()),
		re(__scratch_expr_trace.rend());
	// search from root down, find the first valid rule expr visited
	while (ri!=re && !is_rule_expr(*ri)) { ++ri; }
	root_rule = *ri;
}
	ISE_INVARIANT(root_rule);
#if PRSIM_WEAK_RULES
	const size_t is_weak = lookup_rule(root_rule)->is_weak();
#endif
// propagation made it to the root node, indexed by ui (now node_index_type)
	node_type& n(get_node(ui));
	DEBUG_STEP_PRINT("propagated to output node: " <<
		get_node_canonical_name(ui) << " with pull state " <<
		size_t(next) << endl);
#if PRSIM_WEAK_RULES
	DEBUG_STEP_PRINT("root is " << (is_weak ? "" : "not") << " weak" << endl);
#endif
	const event_index_type ei = n.get_event();
#if DEBUG_STEP
	if (ei) dump_event(cerr << "pending:\t", ei, 0.0) << endl;
#endif
#if PRSIM_INDIRECT_EXPRESSION_MAP
	const pull_enum up_pull = n.pull_up_state STR_INDEX(NORMAL_RULE).pull();
	const pull_enum dn_pull = n.pull_dn_state STR_INDEX(NORMAL_RULE).pull();
#else
	const pull_enum up_pull =
		get_pull(n.pull_up_index STR_INDEX(NORMAL_RULE));
	const pull_enum dn_pull =
		get_pull(n.pull_dn_index STR_INDEX(NORMAL_RULE));
#endif
#if PRSIM_WEAK_RULES
#if PRSIM_INDIRECT_EXPRESSION_MAP
	const pull_enum wdn_pull = weak_rules_enabled() ?
		n.pull_dn_state STR_INDEX(WEAK_RULE).pull() : PULL_OFF;
	const pull_enum wup_pull = weak_rules_enabled() ?
		n.pull_up_state STR_INDEX(WEAK_RULE).pull() : PULL_OFF;
#else
	const pull_enum wdn_pull = weak_rules_enabled() ?
		get_pull(n.pull_dn_index STR_INDEX(WEAK_RULE)) : PULL_OFF;
	const pull_enum wup_pull = weak_rules_enabled() ?
		get_pull(n.pull_up_index STR_INDEX(WEAK_RULE)) : PULL_OFF;
#endif
#endif	// PRSIM_WEAK_RULES
	break_type err = false;
#if PRSIM_WEAK_RULES
	// weak rule pre-filtering
if (weak_rules_enabled()) {
if (n.pending_event()) {
	event_type& e(get_event(ei));	// previous scheduled event
	if (e.is_weak() && !is_weak && next != PULL_OFF) {
		DEBUG_STEP_PRINT("old weak event killed" << endl);
		// it was weak, and should be overtaken
		// what if new event is weak-off?
		if (e.val != LOGIC_OTHER) {
			err |= __report_instability(cout,
				next == PULL_WEAK, 
				e.val == LOGIC_HIGH, e.node, e);
		}
		kill_event(ei, ui);
		// ei = 0;
		// don't return, continue processing non-weak events
	} else if (!e.is_weak() && is_weak
			// && previous-pull != PULL_OFF
			// since vacuous events are dropped, events 
			// must be enqueued by some value-changing pull
			// assert: that pull was not OFF
			) {
		DEBUG_STEP_PRINT("new weak event dropped" << endl);
		// previous event was strong and not off, overriding any
		// new weak events -- just drop weak events
		// HERE: is event already in main event queue? flush?
		return err;	// no error
	}
} else {	// no pending event
	if (is_weak &&
			(up_pull != PULL_OFF ||
			dn_pull != PULL_OFF)) {
		DEBUG_STEP_PRINT("weak event suppressed" << endl);
		// drops weak-firings overpowered by strong on rules
		// regardless of what 'next' weak pull is
		return err;	// no error
	}
}
}	// end if weak_rules_enabled
#endif	// PRSIM_WEAK_RULES
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
	if ((next == PULL_ON &&
			n.current_value() != LOGIC_HIGH) ||
		(next == PULL_WEAK &&
			(n.current_value() == LOGIC_LOW
			|| dn_pull != PULL_OFF))) {
		/***
			if (PULL_ON and wasn't already HIGH ||
				PULL_WEAK and was LOW before ||
				weak-interference)
			then we enqueue the event somewhere.
		***/
		DEBUG_STEP_PRINT("pulling up (on or weak)" << endl);
		const event_index_type pe =
			__allocate_event(n, ui, c,
				root_rule,
				next == PULL_ON ? LOGIC_HIGH : LOGIC_OTHER
#if PRSIM_WEAK_RULES
				, is_weak
#endif
				);
		const event_type& e(get_event(pe));
		// pe->cause = root
		if (n.has_mk_exclhi()) {
			// insert into exclhi queue
			enqueue_exclhi(get_delay_up(e), pe);
		} else {
			// not sure why: checking against non-weak only:
			if (
#if PRSIM_INDIRECT_EXPRESSION_MAP
			n.pull_dn_state STR_INDEX(NORMAL_RULE).any()
#else
			n.pull_dn_index STR_INDEX(NORMAL_RULE)
#endif
#if PRSIM_WEAK_RULES
				|| (weak_rules_enabled() &&
#if PRSIM_INDIRECT_EXPRESSION_MAP
					n.pull_dn_state STR_INDEX(WEAK_RULE).any()
#else
					n.pull_dn_index STR_INDEX(WEAK_RULE)
#endif
					)
#endif
				)
			{
				enqueue_pending(pe);
			} else {
				enqueue_event(get_delay_up(e), pe);
			}
		}
	}
	// "Is this right??" expr_pool[n.pull_dn_index] 
	// might not have been updated yet...
	else if (next == PULL_OFF) {
	DEBUG_STEP_PRINT("pull-up turned off" << endl);
	if (dn_pull == PULL_ON
#if PRSIM_WEAK_RULES
		|| (!is_weak && 
			wdn_pull == PULL_ON &&
			wup_pull == PULL_OFF)
		|| (wdn_pull == PULL_WEAK &&
			n.current_value() != LOGIC_OTHER)
#endif
		) {
		// n->dn->val == PRS_VAL_T
		/***
			if (PULL_OFF and opposing pull-down is ON)
			then enqueue the pull-down event.  
		***/
		DEBUG_STEP_PRINT("yielding to opposing pull-down." << endl);
		const event_index_type pe =
			__allocate_event(n, ui, c,
				root_rule, LOGIC_LOW 
#if PRSIM_WEAK_RULES
				// if cause is the rule that turned off
				// , is_weak
				// if cause is the opposition that was on
				, (dn_pull == PULL_OFF)
				// important for interference checking
#endif
				);
		// pe->cause = root
		if (n.has_mk_excllo()) {
			const event_type& e(get_event(pe));
			enqueue_excllo(get_delay_up(e), pe);
		} else {
			enqueue_pending(pe);
		}
	}
	}	// end if next is PULL_OFF
} else if (!n.in_excl_queue()) {
	DEBUG_STEP_PRINT("pending, but not excl event on this node." << endl);
	// there is a pending event, not in the exclusive queue
	event_type& e(get_event(ei));
	DEBUG_STEP_PRINT("next = " << size_t(next) << endl);
	DEBUG_STEP_PRINT("pull-dn = " << size_t(dn_pull) << endl);
	DEBUG_STEP_PRINT("e.val = " << size_t(e.val) << endl);
	DEBUG_STEP_PRINT("n.val = " << size_t(n.current_value()) << endl);
	if (next == PULL_OFF && 
		((dn_pull == PULL_ON)
#if PRSIM_WEAK_RULES
		|| (wdn_pull == PULL_ON)
#endif
		) &&
		e.val == LOGIC_OTHER &&
		n.current_value() != LOGIC_LOW) {
		/***
			if (pull-up is PULL_OFF, opposing pull-down is ON and
			the pending event's value is X and
			the current node value is NOT LOW)
			The pending X should be cancelled and replaced
			with a pending LOW (keeping the same time).
		***/
		DEBUG_STEP_PRINT("changing pending X to 0 in queue." << endl);
		e.val = LOGIC_LOW;
		e.set_cause_node(ni);
#if PRSIM_WEAK_RULES
		e.set_weak(wdn_pull != PULL_OFF
			&& dn_pull == PULL_OFF);
#endif
#if PRSIM_ALLOW_OVERTAKE_EVENTS
	} else if (dequeue_unstable_events() &&
		next == PULL_ON && 
		dn_pull == PULL_OFF &&
		e.val == LOGIC_OTHER &&
		n.current_value() == LOGIC_LOW
		// n.current_value() != LOGIC_HIGH
		// NOTE: weak rules accounted for by pre-filter above
		) {
		/***
			Terrible overload of the dequeue-unstable mode.
			If (pull-up is ON, opposing pull-dn is OFF, 
			and pending event's value is X, 
			and node's current value is low, monotonic)
			Either:
			1) replace the previous event with new value 1,
				using the same time, or
			2) cancel previous event, and re-insert new event
				with new time.
		***/
		DEBUG_STEP_PRINT("changing pending X to 1 in queue." << endl);
		e.val = LOGIC_HIGH;
		e.set_cause_node(ni);
	} else if (dequeue_unstable_events() &&
		next == PULL_OFF && 
		(dn_pull == PULL_ON ||
			n.current_value() == LOGIC_LOW) &&
		e.val == LOGIC_OTHER) {
		if (n.current_value() == LOGIC_LOW) {
			// already low, just kill pending X
			kill_event(ei, ui);
		} else {
		/***
			Terrible overload of the dequeue-unstable mode.
			If (pull-up is OFF, opposing pull-dn is ON, 
			and pending event's value is X, 
			and node's current value is high, monotonic)
			Either:
			1) replace the previous event with new value 0,
				using the same time, or
			2) cancel previous event, and re-insert new event
				with new time.
		***/
		DEBUG_STEP_PRINT("changing pending X to 0 in queue." << endl);
		e.val = LOGIC_LOW;
		e.set_cause_node(ni);
		}
	} else if (dequeue_unstable_events() && !is_weak &&
		next == PULL_OFF && 
		wup_pull == PULL_OFF &&
		wdn_pull == PULL_ON) {
		/***
			Strong rule turning off, yielding to weak rule 
			pulling in opposite direction.
			TODO: kill pending event and re-enqueue.
			Really doesn't make sense to use the same delay
			if changing to opposite direction.  
		***/
		DEBUG_STEP_PRINT("changing pending 1 to 0 in queue." << endl);
		// for now, out of laziness, overwrite the pending event
		err |= __report_instability(cout, false, true, e.node, e);
		e.val = LOGIC_LOW;
		e.set_cause_node(ni);
#if PRSIM_WEAK_RULES
		e.set_weak(true);
#endif
#if 1
	} else if (next == PULL_OFF && 
		dn_pull == PULL_OFF &&
		wdn_pull == PULL_OFF &&
		wup_pull == PULL_ON &&
		e.val == LOGIC_HIGH) {
		// technically, is this unstable?
		// instability is masked because weak-rule continues to pull...
		// everything but weak pull-up is off
		// then keep event in queue
		// change node cause?
		e.set_cause_node(ni);
#if PRSIM_WEAK_RULES
		e.set_weak(true);
#endif
#endif
#endif	// PRSIM_ALLOW_OVERTAKE_EVENTS
	} else {
		DEBUG_STEP_PRINT("checking for upguard anomaly: guard=" <<
			size_t(next) << ", val=" << size_t(e.val) << endl);
		err |= __diagnose_violation(cout, next, ei, e, ui, n, 
			c, u->direction()
#if PRSIM_WEAK_RULES
			, is_weak
#endif
			);
	}	// end if diagnostic
}	// end if (!n.ex_queue)
} else {
	// pull-dn
if (!n.pending_event()) {
	DEBUG_STEP_PRINT("no pending event on this node being pulled down."
		<< endl);
	// no former event pending, ok to enqueue
	if ((next == PULL_ON &&
			n.current_value() != LOGIC_LOW) ||
		(next == PULL_WEAK &&
			(n.current_value() == LOGIC_HIGH
			|| up_pull != PULL_OFF))) {
		/***
			if (PULL_ON and wasn't already LOW ||
				PULL_WEAK and was HIGH before ||
				weak-interference)
			then we enqueue the event somewhere.
		***/
		DEBUG_STEP_PRINT("pulling down (on or weak)" << endl);
		const event_index_type pe =
			__allocate_event(n, ui, c, 
				root_rule, 
				next == PULL_ON ? LOGIC_LOW : LOGIC_OTHER
#if PRSIM_WEAK_RULES
				, is_weak
#endif
				);
		const event_type& e(get_event(pe));
		if (n.has_mk_excllo()) {
			// insert into exclhi queue
			enqueue_excllo(get_delay_dn(e), pe);
		} else {
			if (
#if PRSIM_INDIRECT_EXPRESSION_MAP
				n.pull_up_state STR_INDEX(NORMAL_RULE).any()
#else
				n.pull_up_index STR_INDEX(NORMAL_RULE)
#endif
#if PRSIM_WEAK_RULES
				|| (weak_rules_enabled() &&
#if PRSIM_INDIRECT_EXPRESSION_MAP
					n.pull_up_state STR_INDEX(WEAK_RULE).any()
#else
					n.pull_up_index STR_INDEX(WEAK_RULE)
#endif
				)
#endif
			)
			{
				enqueue_pending(pe);
			} else {
				enqueue_event(get_delay_dn(e), pe);
			}
		}
	}
	// "Is this right??" expr_pool[n.pull_dn_index] 
	// might not have been updated yet...
	else if (next == PULL_OFF) {
	DEBUG_STEP_PRINT("pull-down turned off" << endl);
	if (up_pull == PULL_ON
#if PRSIM_WEAK_RULES
		|| (!is_weak &&
			wup_pull == PULL_ON &&
			wdn_pull == PULL_OFF)
		|| (wup_pull == PULL_WEAK && 
			n.current_value() != LOGIC_OTHER)
#endif
		) {
		// n->up->val == PRS_VAL_T
		/***
			if (PULL_OFF and opposing pull-up is ON)
			then enqueue the pull-up event.  
		***/
		DEBUG_STEP_PRINT("yielding to opposing pull-up." << endl);
		const event_index_type pe =
			__allocate_event(n, ui, c,
				root_rule, LOGIC_HIGH
#if PRSIM_WEAK_RULES
				// if cause is the rule that turned off
				// , is_weak
				// if cause is the opposition that was on
				, (up_pull == PULL_OFF)
				// important for interference checking
#endif
				);
		// pe->cause = root
		if (n.has_mk_exclhi()) {
			const event_type& e(get_event(pe));
			enqueue_exclhi(get_delay_dn(e), pe);
		} else {
			enqueue_pending(pe);
		}
	}
	}	// end if next is PULL_OFF
} else if (!n.in_excl_queue()) {
	DEBUG_STEP_PRINT("pending, but not excl event on this node." << endl);
	// there is a pending event, not in an exclusive queue
	event_type& e(get_event(ei));
	DEBUG_STEP_PRINT("next = " << size_t(next) << endl);
	DEBUG_STEP_PRINT("pull-up = " << size_t(up_pull) << endl);
	DEBUG_STEP_PRINT("e.val = " << size_t(e.val) << endl);
	DEBUG_STEP_PRINT("n.val = " << size_t(n.current_value()) << endl);
	if (next == PULL_OFF && 
		((up_pull == PULL_ON)
#if PRSIM_WEAK_RULES
		|| (wup_pull == PULL_ON)
#endif
		) &&
		e.val == LOGIC_OTHER &&
		n.current_value() != LOGIC_HIGH) {
		/***
			if (pull-dn is PULL_OFF, opposing pull-up is ON and
			the pending event's value is X and
			the current node value is NOT HIGH)
			The pending X should be cancelled and replaced
			with a pending HIGH (keeping the same time).
		***/
		DEBUG_STEP_PRINT("changing pending X to 1 in queue." << endl);
		e.val = LOGIC_HIGH;
		e.set_cause_node(ni);
#if PRSIM_WEAK_RULES
		e.set_weak(wup_pull != PULL_OFF
			&& up_pull == PULL_OFF);
#endif
#if PRSIM_ALLOW_OVERTAKE_EVENTS
	} else if (dequeue_unstable_events() &&
		next == PULL_ON &&
		up_pull == PULL_OFF &&
		e.val == LOGIC_OTHER &&
		n.current_value() == LOGIC_HIGH
		// n.current_value() != LOGIC_LOW
		// NOTE: weak rules accounted for by pre-filter above
		) {
		/***
			Terrible overload of the dequeue-unstable mode.
			If (pull-dn is ON, opposing pull-up is OFF, 
			and pending event's value is X, 
			and node's current value is HIGH, monotonic)
			Either:
			1) replace the previous event with new value 0,
				using the same time, or
			2) cancel previous event, and re-insert new event
				with new time.
		***/
		DEBUG_STEP_PRINT("changing pending X to 0 in queue." << endl);
		e.val = LOGIC_LOW;
		e.set_cause_node(ni);
	} else if (dequeue_unstable_events() &&
		next == PULL_OFF &&
		(up_pull == PULL_ON ||
			n.current_value() == LOGIC_HIGH) &&
		e.val == LOGIC_OTHER) {
		if (n.current_value() == LOGIC_HIGH) {
			// kill pending X, node is already high
			kill_event(ei, ui);
		} else {
		/***
			Terrible overload of the dequeue-unstable mode.
			If (pull-dn is ON, opposing pull-up is OFF, 
			and pending event's value is X, 
			and node's current value is HIGH, monotonic)
			Either:
			1) replace the previous event with new value 0,
				using the same time, or
			2) cancel previous event, and re-insert new event
				with new time.
		***/
		DEBUG_STEP_PRINT("changing pending X to 1 in queue." << endl);
		e.val = LOGIC_HIGH;
		e.set_cause_node(ni);
		}
	} else if (dequeue_unstable_events() && !is_weak &&
		next == PULL_OFF && 
		wdn_pull == PULL_OFF &&
		wup_pull == PULL_ON) {
		/***
			Strong rule turning off, yielding to weak rule 
			pulling in opposite direction.
			TODO: kill pending event and re-enqueue.
			Really doesn't make sense to use the same delay
			if changing to opposite direction.  
		***/
		DEBUG_STEP_PRINT("changing pending 0 to 1 in queue." << endl);
		// for now, out of laziness, overwrite the pending event
		err |= __report_instability(cout, false, false, e.node, e);
		e.val = LOGIC_HIGH;
		e.set_cause_node(ni);
#if PRSIM_WEAK_RULES
		e.set_weak(true);
#endif
#if 1
	} else if (next == PULL_OFF && 
		up_pull == PULL_OFF &&
		wup_pull == PULL_OFF &&
		wdn_pull == PULL_ON &&
		e.val == LOGIC_LOW) {
		// technically, is this unstable?
		// instability is masked because weak-rule continues to pull...
		// everything but weak pull-up is off
		// then keep event in queue
		// change node cause?
		e.set_cause_node(ni);
#if PRSIM_WEAK_RULES
		e.set_weak(true);
#endif
#endif
#endif	// PRSIM_ALLOW_OVERTAKE_EVENTS
	} else {
		DEBUG_STEP_PRINT("checking for dnguard anomaly: guard=" <<
			size_t(next) << ", val=" << size_t(e.val) << endl);
		err |= __diagnose_violation(cout, next, ei, e, ui, n, 
			c, u->direction()
#if PRSIM_WEAK_RULES
			, is_weak
#endif
			);
	}	// end if diagonstic
}	// end if (!n.ex_queue)
}	// end if (u->direction())
	return err;
}	// end method propagate_evaluation

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::__report_cause(ostream& o, const event_type& ev) const {
	const node_index_type& ni(ev.cause.node);
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
	if (weak_interference_policy != ERROR_IGNORE) {
		o << "WARNING: weak-interference `" <<
			get_node_canonical_name(_ni) << "\'" << endl;
		__report_cause(o, ev);
		return weak_interference_policy == ERROR_BREAK;
	}	// endif weak_interference_policy
	} else {	// !weak
	if (interference_policy != ERROR_IGNORE) {
		o << "WARNING: interference `" <<
			get_node_canonical_name(_ni) << "\'" << endl;
		__report_cause(o, ev);
		return interference_policy == ERROR_BREAK;
	}	// endif interference_policy
	}	// endif weak
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param weak is true if unstable was *possible*, i.e. caused by X
	\param dir the direction of the unstable firing
	\param _ni affected node (ev.node?)
	\param ev the unstable event
	\return true if error causes break in events.  
	If node is flagged unstable, 
 */
State::break_type
State::__report_instability(ostream& o, const bool weak, const bool dir, 
		const node_index_type _ni, const event_type& ev) const {
	if (weak) {
	if (weak_unstable_policy != ERROR_IGNORE) {
		o << "WARNING: weak-unstable `" <<
			get_node_canonical_name(_ni) << "\'" <<
				(dir ? '+' : '-') << endl;
		__report_cause(o, ev);
		return weak_unstable_policy == ERROR_BREAK;
	}	// endif weak_unstable_policy
	} else {	// !weak
	if (unstable_policy != ERROR_IGNORE) {
		o << "WARNING: unstable `" <<
			get_node_canonical_name(_ni) << "\'" <<
				(dir ? '+' : '-') << endl;
		__report_cause(o, ev);
		return unstable_policy == ERROR_BREAK;
	}	// endif unstable_policy
	}	// endif weak
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function for repetitive diagnostic code.  
	\param o error output stream
	\param next the next value of *pull* this node
	\param ei index of the event in question
	\param e the event in question
	\param ui index of the node that fired
	\param n the node that fired
	\param ni the node involved in event e
	\param dir the direction of pull of the causing rule
	\param weak true if rule was pulling rule is weak
	\return true if error causes break.
 */
State::break_type
State::__diagnose_violation(ostream& o, const pull_enum next, 
		const event_index_type ei, event_type& e, 
		const node_index_type ui, node_type& n, 
		cause_arg_type c, 
		const bool dir
#if PRSIM_WEAK_RULES
		, const bool weak
#endif
		) {
	STACKTRACE_VERBOSE;
#if PRSIM_WEAK_RULES
	DEBUG_STEP_PRINT("is " << (weak ? "" : "not") << " weak" << endl);
#endif
	const node_index_type& ni(c.node);
	break_type err = false;
	// something is amiss!
	const uchar eu = dir ?
		event_type::upguard[size_t(next)][size_t(e.val)] :
		event_type::dnguard[size_t(next)][size_t(e.val)];
	DEBUG_STEP_PRINT("event_update = " << size_t(eu) << endl);
	const bool vacuous = eu & event_type::EVENT_VACUOUS;
	if (!vacuous) {
		// then must be unstable or interfering (exclusive)
		const bool instability =
			(eu & event_type::EVENT_UNSTABLE) &&
			!n.is_unstab()
#if PRSIM_WEAK_RULES
			&& !(weak && !e.is_weak())
			// is not instability if original event was strong
			// and this new event is weak
#endif
			;
		/***
			This last condition !unstab violates exact exclusion 
			between unstable and interference!
			Do not use this undocumented feature, it is not 
			expected to work as presently coded.  
		***/
		const bool interference =
			eu & event_type::EVENT_INTERFERENCE
#if PRSIM_WEAK_RULES
			&& !(weak && !e.is_weak())
			// is not interference if original event was strong
			// and this new event is weak
#endif
			;
		const string cause_name(get_node_canonical_name(ni));
		const string out_name(get_node_canonical_name(ui));

			// causing rule only used by propagate
			// don't care about value, 
			// event is for sake of checking
			// check for conflicting/redundant events 
			// on pending queue (result of instability)
		if (instability) {
			e.set_cause_node(ni);
			if (dequeue_unstable_events() &&
				(next == PULL_OFF ||
				n.current_value() == LOGIC_OTHER)) {
				// let dequeuer deallocate killed events
				// weak-unstable should leave X in queue
				const size_t pe = n.get_event();
				DEBUG_STEP_PRINT("dequeuing unstable event " << pe << endl);
				// instability will be reported below
				kill_event(pe, ui);
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
				DEBUG_STEP_PRINT("changing event to X" << endl);
				e.val = LOGIC_OTHER;
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
					n, ui, c, 
					dir ? LOGIC_HIGH :
						LOGIC_LOW
#if PRSIM_WEAK_RULES
						, NORMAL_RULE	// not weak
#endif
						);
			enqueue_pending(pe);
		}
		// diagnostic message
		// suppress message for interferences until pending queue
		if (instability) {
#if PRSIM_WEAK_RULES
			bool b = !weak;
			if (!b) {
			if (dir) {
				const pull_enum up =
#if PRSIM_INDIRECT_EXPRESSION_MAP
					n.pull_up_state STR_INDEX(NORMAL_RULE).pull();
#else
					get_pull(n.pull_up_index STR_INDEX(NORMAL_RULE));
#endif
				b = (up != PULL_ON);
			} else {
				const pull_enum dn =
#if PRSIM_INDIRECT_EXPRESSION_MAP
					n.pull_dn_state STR_INDEX(NORMAL_RULE).pull();
#else
					get_pull(n.pull_dn_index STR_INDEX(NORMAL_RULE));
#endif
				b = (dn != PULL_ON);
			}
			}
			if (b) {
#endif
			err |=
			__report_instability(o, eu & event_type::EVENT_WEAK, 
				dir, ui, e);
#if PRSIM_WEAK_RULES
			}
#endif
		}	// end if unstable
	}	// end if !vacuous
	// else vacuous is OK
	return err;
}	// end method __diagnose_violation

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Run until event queue is empty or breakpoint reached.  
	\return null node index if queue is emptied, else
		the ID of the node that tripped a breakpoint.  
 */
State::step_return_type
State::cycle(void) THROWS_STEP_EXCEPTION {
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
	TODO: show values
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
static
bool
node_is_0(const State::node_type& n) {
	return n.current_value() == LOGIC_LOW;
}

static
bool
node_is_1(const State::node_type& n) {
	return n.current_value() == LOGIC_HIGH;
}

static
bool
node_is_X(const State::node_type& n) {
	return n.current_value() == LOGIC_OTHER;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param val LOGIC_{LOW,HIGH,OTHER}.  
	\param nl use newline delimiter instead of space.
 */
ostream&
State::status_nodes(ostream& o, const value_enum val, const bool nl) const {
	ISE_INVARIANT(node_type::is_valid_value(val));
	o << node_type::value_to_char[size_t(val)] << " nodes:" << endl;
	bool (*f)(const node_type&) = &node_is_X;
	switch (val) {
	case LOGIC_LOW: f = &node_is_0; break;
	case LOGIC_HIGH: f = &node_is_1; break;
	default: break;
	}
	vector<node_index_type> nodes;
	find_nodes(nodes, f);
	print_nodes(o, nodes, nl ? "\n" : " ");
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Verify event-to-node consistency.
	invariant: every node can be referenced at most once 
		by events in the event queue.  
	invariant: every node referenced in the event queue
		points back to the event which affects it.  
 */
void
State::check_event_queue(void) const {
	typedef	temp_queue_type::const_iterator		const_iterator;
	typedef	std::map<node_index_type, event_index_type>	node_map_type;
	typedef	node_map_type::value_type			value_type;
	typedef	std::pair<node_map_type::iterator, bool>	check_type;
	node_map_type unique_nodes;
	temp_queue_type temp;
	event_queue.copy_to(temp);
	const_iterator qi(temp.begin()), qe(temp.end());
	for ( ; qi!=qe; ++qi) {
		const event_type& e(get_event(qi->event_index));
		const node_type& n(get_node(e.node));
		const event_index_type ne = n.get_event();
	if (e.killed()) {
		ISE_INVARIANT(!ne || (ne != qi->event_index));
		// should have been dissociated when it was killed
	} else {
		const check_type p(unique_nodes.insert(
			value_type(e.node, qi->event_index)));
		const bool tru = (ne &&		// is linked back
			ne == qi->event_index &&	// is consistent
			p.second);	// was inserted uniquely
		if (!tru) {
			cerr << "Event queue corrupted!" << endl;
			cerr << "n.event_index = " << ne << endl;
			cerr << "queue:event   = " << qi->event_index << endl;
			cerr << "insert unique = " << p.second << endl;
			if (!p.second) {
				cerr << "\talready set by: " <<
					unique_nodes[e.node] << endl;
			}
			ISE_INVARIANT(tru);
		}
	}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_INDIRECT_EXPRESSION_MAP
void
unique_process_subgraph::check_structure(void) const {
	const expr_index_type exprs = expr_pool.size();
	ISE_INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_EXPR;
	for ( ; i<exprs; ++i) {
		DEBUG_CHECK_PRINT("checking Expr " << i << ":" << endl);
		check_expr(i);
	}
}
#endif
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Structural assertions.  
	TODO: run-time flag to enable/disable calls to this.  
 */
void
State::check_structure(void) const {
	STACKTRACE_VERBOSE;
#if PRSIM_INDIRECT_EXPRESSION_MAP
	for_each(unique_process_pool.begin(), unique_process_pool.end(),
		mem_fun_ref(&unique_process_subgraph::check_structure));
#else
{
	const expr_index_type exprs = expr_pool.size();
	ISE_INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_EXPR;
	for ( ; i<exprs; ++i) {
		DEBUG_CHECK_PRINT("checking Expr " << i << ":" << endl);
		check_expr(i);
	}
}
#endif
{
	const node_index_type nodes = node_pool.size();
	node_index_type j = FIRST_VALID_NODE;
	for ( ; j<nodes; ++j) {
		DEBUG_CHECK_PRINT("checking Node " << j << ":" << endl);
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
	ISE_INVARIANT(i);
	ISE_INVARIANT(i < node_pool.size());
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
#if PRSIM_INDIRECT_EXPRESSION_MAP
{
	o << "Unique processes: {" << endl;
	process_index_type p = 0;	// 0 is top-level type
	unique_process_pool_type::const_iterator
		i(unique_process_pool.begin()), e(unique_process_pool.end());
	for ( ; i!=e; ++i, ++p) {
		o << "type[" << p << "] ";	// << endl;
		i->dump_struct(o);
	}
	o << "}" << endl;
}
	return o;
}

ostream&
unique_process_subgraph::dump_struct(ostream& o) const {
#endif
{
	o << "Expressions: " << endl;
	const expr_index_type exprs = expr_pool.size();
	ISE_INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_EXPR;
	// is 0 valid? process-local?
	for ( ; i<exprs; ++i) {
#if PRSIM_INDIRECT_EXPRESSION_MAP
		const expr_struct_type&
#else
		const expr_state_type&
#endif
			e(expr_pool[i]);
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
	ISE_INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_EXPR;
	for ( ; i<exprs; ++i) {
		o << "EXPR_" << i << "\t[label=\"" << i << "\", shape=";
		const expr_state_type& e(expr_pool[i]);
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
	Internal event printer.
	\param force pass true to show killed events.
 */
ostream&
State::dump_event_force(ostream& o, const event_index_type ei, 
		const time_type t, const bool force) const {
	DEBUG_STEP_PRINT(ei);
	const event_type& ev(get_event(ei));
#if 0
	o << '[' << ei << ']';		// for debugging
#endif
	if (!ev.killed() || force) {
		o << '\t' << t << '\t' <<
			get_node_canonical_name(ev.node) << " : " <<
			node_type::value_to_char[ev.val];
		if (ev.cause.node) {
			o << '\t' << "[from " <<
			get_node_canonical_name(ev.cause.node) << ":=" <<
			node_type::value_to_char[ev.cause.val] << "]";
		}
#if PRSIM_WEAK_RULES
		if (ev.is_weak()) { o << '\t' << "(weak)"; }
#endif
		if (ev.killed()) {
			o << '\t' << "(killed)";
		}
		o << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print a single event.  
 */
ostream&
State::dump_event(ostream& o, const event_index_type ei, 
		const time_type t) const {
#if DEBUG_STEP
	return dump_event_force(o, ei, t, true);
#else
	return dump_event_force(o, ei, t, false);
#endif
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
		dump_event(o, i->event_index, i->time);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Shows pending event on node if any.  
	\param ni node index.  
	\param dbg whether or not to show internal event index.
 */
ostream&
State::dump_node_pending(ostream& o, const node_index_type ni, 
		const bool dbg) const {
	const node_type& n(get_node(ni));
	const event_index_type pending = n.get_event();
	if (pending) {
		if (dbg) {
			o << "pending event index = " << pending << endl;
		}
		// find event in queue: O(N) search
		typedef	temp_queue_type::const_iterator		const_iterator;
		temp_queue_type temp;
		event_queue.copy_to(temp);
		const_iterator i(temp.begin()), e(temp.end());
		// this only works for signed types, such as floating point
		time_type t = delay_policy<time_type>::invalid_value;
		while (i!=e) {
			if (i->event_index == pending) {
				t = i->time;
				break;
			}
			++i;
		}
		o << "queue:";
		dump_event(o, pending, t);
		if (t == delay_policy<time_type>::invalid_value) {
			cerr << "Internal error: pending event was not found "
				"in event queue!" << endl;
		}
	} else {
		o << "No event pending on `" << get_node_canonical_name(ni)
			<< "\'." << endl;
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
	TODO: including all OR combinations, or just the paths that
		include this node directly?  (just the paths that include)
	\param v true if literals should be printed with current values.
 */
ostream&
State::dump_node_fanout(ostream& o, const node_index_type ni, 
		const bool v) const {
	typedef	fanout_array_type::const_iterator	const_iterator;
	typedef	std::set<expr_index_type>		rule_set_type;
#if DEBUG_FANOUT
	STACKTRACE_VERBOSE;
	DEBUG_FANOUT_PRINT("ni = " << ni << endl);
#endif
	const node_type& n(get_node(ni));
	const fanout_array_type& foa(n.fanout);
	rule_set_type fanout_rules;
	const_iterator fi(foa.begin()), fe(foa.end());
	for ( ; fi!=fe; ++fi) {
		// for each leaf expression in the fanout list, 
		// trace up the propagation path to find the affected node.
		expr_index_type ei = *fi;
		const expr_state_type* e = &expr_pool[ei];
		while (!e->is_root()) {
			DEBUG_FANOUT_PRINT("ei = " << ei << endl);
			ei = e->parent;
			e = &expr_pool[ei];
		}
		DEBUG_FANOUT_PRINT("ei = " << ei << endl);
		fanout_rules.insert(ei);	// ignore duplicates
	}
	typedef	rule_set_type::const_iterator		rule_iterator;
	rule_iterator ri(fanout_rules.begin()), re(fanout_rules.end());
	for ( ; ri!=re; ++ri) {
		const expr_state_type* const e = &expr_pool[*ri];
		// ei (*ri) is index to expression whose parent is *node*.
		const node_index_type nr = e->parent;
		// nr is an index to the root *node*.
		DEBUG_FANOUT_PRINT("nr = " << nr << endl);
		const node_type& no(get_node(nr));
		// track the direction of propagation (pull-up/dn)
		const bool dir = e->direction();
		// then print the *entire* fanin rule for that node, 
#if PRSIM_WEAK_RULES
	size_t w = NORMAL_RULE;
	do {
#endif
		const expr_index_type pi =
			(dir ? no.pull_up_index STR_INDEX(w) : 
				no.pull_dn_index STR_INDEX(w));
		DEBUG_FANOUT_PRINT("pi = " << pi << endl);
#if PRSIM_WEAK_RULES
		if (pi) {
			// account for empty weak-rules
#endif
		dump_subexpr(o, pi, v) << " -> ";
		o << get_node_canonical_name(nr) << (dir ? '+' : '-');
		if (v) {
			n.dump_value(o << ':');
		}
		o << endl;
#if PRSIM_WEAK_RULES
		}	// end if
#endif
#if PRSIM_WEAK_RULES
		++w;
	} while (w<2);	// even if !weak_rules_enabled()
#endif
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: OR-combinations on separate lines?
	TODO: Rajit's prsim suppreses weak rule fanins (copy?)
		For now, we print those as well.
	\param v true if literal should be printed with its value.  
		also print expression with pull-state.
 */
ostream&
State::dump_node_fanin(ostream& o, const node_index_type ni, 
		const bool v) const {
	const node_type& n(get_node(ni));
	const string cn(get_node_canonical_name(ni));
#if PRSIM_WEAK_RULES
	size_t w = NORMAL_RULE;
do {
#endif
	const expr_index_type ui = n.pull_up_index STR_INDEX(w);
	const expr_index_type di = n.pull_dn_index STR_INDEX(w);
	if (ui) {
		dump_subexpr(o, ui, v)
			<< " -> " << cn << '+';
		if (v) {
			n.dump_value(o << ':');
		}
		o << endl;
	}
	if (di) {
		dump_subexpr(o, di, v)
			<< " -> " << cn << '-';
		if (v) {
			n.dump_value(o << ':');
		}
		o << endl;
	}
#if PRSIM_WEAK_RULES
	++w;
} while (w<2);		// even if !weak_rules_enabled()
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints why a node is X.
 */
ostream&
State::dump_node_why_X(ostream& o, const node_index_type ni, 
		const size_t limit, const bool verbose) const {
	// unique set to terminate cyclic recursion
	const node_type& n(get_node(ni));
if (n.current_value() == LOGIC_OTHER) {
	node_set_type u, v;	// cycle-detect set, globally-visited set
	return __node_why_X(o, ni, limit, verbose, u, v);
} else {
	o << get_node_canonical_name(ni) << " is not X." << endl;
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Why is a node NOT at a value.  
	\param dir true for 1, false for 0.
 */
ostream&
State::dump_node_why_not(ostream& o, const node_index_type ni, 
		const size_t limit,
		const bool dir, const bool why_not, const bool verbose) const {
	const node_type& n(get_node(ni));
	node_set_type u, v;
switch (n.current_value()) {
case LOGIC_LOW:
	if (dir ^ !why_not) {
		return __node_why_not(o, ni, limit, 
			dir, why_not, verbose, u, v);
	} else {
		o << get_node_canonical_name(ni) << " is 0." << endl;
	}
	break;
case LOGIC_HIGH:
	if (dir ^ !why_not) {
		o << get_node_canonical_name(ni) << " is 1." << endl;
	} else {
		return __node_why_not(o, ni, limit, 
			dir, why_not, verbose, u, v);
	}
	break;
default:
	o << get_node_canonical_name(ni) << " is X." << endl;
	// recommend try 'why-x'
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param u the current stack of visited nodes, for cycle detection, 
		is pushed and popped like a stack.
	\param v the set of all visited nodes, for cross-referencing
		already visited sub-trees, accumulatess without popping.  
 */
ostream&
State::__node_why_X(ostream& o, const node_index_type ni, 
		const size_t limit, const bool verbose, 
		node_set_type& u, node_set_type& v) const {
	const std::pair<node_set_type::iterator, bool>
		p(u.insert(ni)), y(v.insert(ni));
	const string nn(get_node_canonical_name(ni));
	o << auto_indent << nn << ":X";
if (p.second) {
if (y.second) {
	// inserted uniquely
	const node_type& n(get_node(ni));
	INVARIANT(n.current_value() == LOGIC_OTHER);
	const bool from_channel =
#if PRSIM_CHANNEL_SUPPORT
		node_is_driven_by_channel(ni);
#else
		false;
#endif
	if (from_channel) {
		o << ", from-channel";
		// channel may be in stopped state...
	}
	// inspect pull state (and event queue)
	const event_index_type pe = n.get_event();
	if (pe) {
		o << ", pending event -> " <<
			node_type::value_to_char[size_t(get_event(pe).val)]
			<< endl;
	} else {
#if 0
	node_set_type xs;
#endif
#if PRSIM_WEAK_RULES
	size_t w = NORMAL_RULE;
do {
#endif
	const expr_index_type ui = n.pull_up_index STR_INDEX(w);
	const expr_index_type di = n.pull_dn_index STR_INDEX(w);
	const pull_enum up = get_pull(ui);
	const pull_enum dp = get_pull(di);
	const size_t ux = (up == PULL_WEAK);
	const size_t dx = (dp == PULL_WEAK);
	switch (ux +dx) {
	case 0:
		if (up == PULL_ON &&
				dp == PULL_ON) {
			o << ", pull up/dn interfere";
		} else
		if (
#if PRSIM_WEAK_RULES
			w &&
#endif
				up == PULL_OFF &&
				dp == PULL_OFF) {
			o << ", pull up/dn undriven";
			if (!n.has_fanin() && !from_channel) {
				o << ", no fanin";
			}
		} else if (w) {
			o << endl;
		}
		break;
	case 1: {	// one pull is X
		// recursively, find X nodes that are exposed
		const expr_index_type xi = (ux ? ui : di);
		const expr_index_type oi = (ux ? di : ui);	// other pull
		if (get_pull(oi) == PULL_ON) {
			o << ", weak-interference vs. " << (ux ? "dn" : "up");
		} else {	// is OFF
			o << ", unknown-pull " << (ux ? "up" : "dn");
		}
		INVARIANT(xi);
#if 0
		__get_X_fanins(xi, xs);
#else
		o << endl;
		const string __ind_str(verbose ? (ux ? "+" : "-") : "  ");
		const indent __indent(o, __ind_str);
		__expr_why_X(o, xi, limit, verbose, u, v);
#endif
		break;
	}
	case 2:
		o << ", pull up/dn are both X";
		if (u.size() <= limit) {
#if 0
		__get_X_fanins(ui, xs);
		__get_X_fanins(di, xs);
#else
		o << endl;
		{
			const indent __indent(o, verbose ? "+" : "  ");
			__expr_why_X(o, ui, limit, verbose, u, v);
		}{
			const indent __indent(o, verbose ? "-" : "  ");
			__expr_why_X(o, di, limit, verbose, u, v);
		}
#endif
		} else {	// recursion limit
			o << " (more ...)" << endl;
		}
		break;
	default:
		DIE;
	}	// end switch
#if PRSIM_WEAK_RULES
	if (up != PULL_OFF ||
			dp != PULL_OFF) {
#endif
#if 0
		o << endl;
#endif
#if PRSIM_WEAK_RULES
		break;
	} else if (w) {
		o << endl;
	}
	++w;
} while (w<2);		// even if !weak_rules_enabled()
#endif
#if PRSIM_CHANNEL_SUPPORT
	if (n.in_channel()) {
		// if node is part of source or sink
#if 0
		_channel_manager.__get_X_fanins(*this, ni, xs);
#else
		_channel_manager.__node_why_X(*this, o, ni, 
			limit, verbose, u, v);
#endif
	}
#endif
#if 0
	INDENT_SECTION(o);
	node_set_type::const_iterator ii(xs.begin()), ee(xs.end());
	for ( ; ii!=ee; ++ii) {
		__node_why_X(o, *ii, limint, verbose, u, v);	// recursion
	}
#endif
	}
} else {
	// don't print the same subtree twice, just cross-reference
	o << ", (visited before, see above)" << endl;
}	// end if visited
	u.erase(ni);
	return o;
} else {
	INVARIANT(!y.second);
	return o << ", cycle reached" << endl;
}
	return o;
}	// end __node_why_X

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Why is a node not a certain value?
	Q: X nodes are not followed?
	\param d if true, ask why node isn't pulled up, else ... why not down
	\param u the current stack of visited nodes, for cycle detection, 
		is pushed and popped like a stack.
	\param v the set of all visited nodes, for cross-referencing
		already visited sub-trees, accumulatess without popping.  
 */
ostream&
State::__node_why_not(ostream& o, const node_index_type ni, 
		const size_t limit, const bool dir,
		const bool why_not, const bool verbose, 
		node_set_type& u, node_set_type& v) const {
	const std::pair<node_set_type::iterator, bool>
		p(u.insert(ni)), y(v.insert(ni));
	const node_type& n(get_node(ni));
	const string nn(get_node_canonical_name(ni));
	n.dump_value(o << auto_indent << nn << ":");
if (p.second) {
if (y.second) {
	// inserted uniquely
	// inspect pull state (and event queue)
	const event_index_type pe = n.get_event();
	if (pe) {
		// if there is pending event, don't recurse
		o << ", pending event -> " <<
			node_type::value_to_char[size_t(get_event(pe).val)]
			<< endl;
		// check that pending event's value matches
	} else {
		const pull_enum pull_query = why_not ? 
			PULL_OFF : PULL_ON;
		const indent __ind_nd(o, verbose ? "." : "  ");
		// only check for the side that is off
		const expr_index_type pi = dir ?
			n.pull_up_index STR_INDEX(NORMAL_RULE) :
			n.pull_dn_index STR_INDEX(NORMAL_RULE);
		const pull_enum ps = get_pull(pi);
#if PRSIM_WEAK_RULES
		// skip this
		const expr_index_type wpi = dir ?
			n.pull_up_index STR_INDEX(WEAK_RULE) :
			n.pull_dn_index STR_INDEX(WEAK_RULE);
		const pull_enum wps = get_pull(wpi);
#endif
		if ((ps == PULL_OFF)
#if PRSIM_WEAK_RULES
			&& (wps == PULL_OFF)
#endif
			) {
			const bool from_channel =
				node_is_driven_by_channel(ni);
			if (n.has_fanin() || from_channel) {
			if (!why_not && !from_channel) {
				o << ", state-holding";
			}
			} else {
				o << ", input";
			}
		}
		if (u.size() <= limit) {
		o << endl;
		// INDENT_SCOPE(o);
		if (pi && (ps == pull_query)) {
			__expr_why_not(o, pi, limit, why_not, verbose, u, v);
		}
#if PRSIM_WEAK_RULES
		if (wpi && (wps == pull_query)) {
			__expr_why_not(o, wpi, limit, why_not, verbose, u, v);
		}
#endif
#if PRSIM_CHANNEL_SUPPORT
		if (n.in_channel()) {
			// ask channel why it has not driven the node
			_channel_manager.__node_why_not(*this, o, 
				ni, limit, dir, why_not, verbose, u, v);
		}
#endif
		} else {	// recursion limit
		o << ", (more ...)" << endl;
		}
	}	// end if pending event
} else {
	// don't print the same subtree twice, just cross-reference
	o << ", (visited before, see above)" << endl;
}	// end if visited
	u.erase(ni);
	return o;
} else {
	INVARIANT(!y.second);
	o << ", cycle";
	if (why_not) {
		o << ": possible deadlock";
	}
	o << endl;
}
	return o;
}	// end __node_why_not

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In an expression tree, find nodes that are exposed X's.
	Cut-off X nodes will not be visited.  
	1s and 0s are not visited.  
	Should follow similar flow to dump_subexpr.
	\param u set of X node to accumulate
	TODO: generalize this to take any pull-state value!!!
 */
void
State::__get_X_fanins(const expr_index_type xi, node_set_type& u) const {
	ISE_INVARIANT(xi);
	ISE_INVARIANT(xi < expr_pool.size());
	const expr_state_type& x(expr_pool[xi]);
	ISE_INVARIANT(x.pull_state() == PULL_WEAK);
	const graph_node_type& g(expr_graph_node_pool[xi]);
	typedef	graph_node_type::const_iterator		const_iterator;
	const_iterator ci(g.begin()), ce(g.end());
	for ( ; ci!=ce; ++ci) {
		INVARIANT(ci->second);
		if (ci->first) {
			// is a leaf node, visit if value is X
			if (get_node(ci->second).current_value()
					== LOGIC_OTHER) {
				u.insert(ci->second);
			}
		} else {
			// is a sub-expresion, recurse if pull is X
			if (expr_pool[ci->second].pull_state()
					== PULL_WEAK) {
				__get_X_fanins(ci->second, u);
			}
		}
	}	// end for
}	// end __get_X_fanins

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In an expression tree, find nodes that are cutting expressions off.
	now also finds subexpressions that are actively pulling.  
	Xs are not visited.  
	Should follow similar flow to dump_subexpr.
	\param off_on true asks why a node is/not on, false asks ... off
	\param why_not is true if asking why-not? (negative-query), 
		else is asking 'why'? (positive-query)
	\param u anti-cycle stack
	\param v globally visited stack
 */
void
State::__expr_why_not(ostream& o, const expr_index_type xi, const size_t limit,
		// const bool off_on, 
		const bool why_not, const bool verbose,
		node_set_type& u, node_set_type& v) const {
	ISE_INVARIANT(xi);
	ISE_INVARIANT(xi < expr_pool.size());
	const expr_state_type& x(expr_pool[xi]);
	const graph_node_type& g(expr_graph_node_pool[xi]);
	const pull_enum xp(x.pull_state());
	INVARIANT(xp != PULL_WEAK);
	typedef	graph_node_type::const_iterator		const_iterator;
	const_iterator ci(g.begin()), ce(g.end());
	string ind_str;
	if (verbose) {
		ind_str += " ";
		if (x.is_not()) ind_str += "~";
		if (g.children.size() > 1) {
			ind_str += x.is_conjunctive() ? "&" : "|";
			o << auto_indent << "-+" << endl;
		}
		// ind_str += " ";
	}
	const indent __ind_ex(o, ind_str);	// INDENT_SCOPE(o);
	for ( ; ci!=ce; ++ci) {
		INVARIANT(ci->second);
		if (ci->first) {
			// is a leaf node, visit if value is not X
			switch (get_node(ci->second).current_value()) {
			case LOGIC_LOW:
				__node_why_not(o, ci->second, limit, why_not,
					why_not, verbose, u, v);
			break;
			case LOGIC_HIGH:
				__node_why_not(o, ci->second, limit, !why_not,
					why_not, verbose, u, v);
			break;
			default:
				break;
			}
		} else {
			// is a sub-expresion, recurse if pull is off
#if 0
			o << auto_indent << "examining expr..." << endl;
			dump_subexpr(o, ci->second, false,
				expr_struct_type::EXPR_ROOT, false) << endl;
#endif
			const expr_state_type& s(expr_pool[ci->second]);
			const pull_enum match_pull = x.is_not() ?
				expr_state_type::negate_pull(xp) : xp;
		// maintain same (positive/negative) query type recursively
			if (s.pull_state() == match_pull) {
				__expr_why_not(o, ci->second, limit, 
					why_not, verbose, u, v);
			}
		}
	}	// end for
}	// end expr_why_not

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In an expression tree, find nodes that are X's that are not cut off
	(by and-0) nor overtaken (or or-1).  
	Ripped from __node_why_not, above.
	\param off_on true asks why a node is/not on, false asks ... off
	\param why_not is true if asking why-not? (negative-query), 
		else is asking 'why'? (positive-query)
	\param u anti-cycle stack
	\param v globally visited stack
 */
void
State::__expr_why_X(ostream& o, const expr_index_type xi, const size_t limit, 
		const bool verbose, node_set_type& u, node_set_type& v) const {
	ISE_INVARIANT(xi);
	ISE_INVARIANT(xi < expr_pool.size());
	const expr_state_type& x(expr_pool[xi]);
	const graph_node_type& g(expr_graph_node_pool[xi]);
	const pull_enum xp(x.pull_state());
	INVARIANT(xp == PULL_WEAK);
	typedef	graph_node_type::const_iterator		const_iterator;
	const_iterator ci(g.begin()), ce(g.end());
	string ind_str;
	if (verbose) {
		ind_str += " ";
		if (x.is_not()) ind_str += "~";
		if (g.children.size() > 1) {
			ind_str += x.is_conjunctive() ? "&" : "|";
			o << auto_indent << "-+" << endl;
		}
		// ind_str += " ";
	}
	const indent __ind_ex(o, ind_str);	// INDENT_SCOPE(o);
	for ( ; ci!=ce; ++ci) {
		INVARIANT(ci->second);
		if (ci->first) {
			// is a leaf node, visit if value is X
			if (get_node(ci->second).current_value()
				== LOGIC_OTHER) {
				__node_why_X(o, ci->second, 
					limit, verbose, u, v);
			}
		} else {
			// is a sub-expresion, recurse if pull is X
#if 0
			o << auto_indent << "examining expr..." << endl;
			dump_subexpr(o, ci->second, false,
				expr_struct_type::EXPR_ROOT, false) << endl;
#endif
			const expr_state_type& s(expr_pool[ci->second]);
			if (s.pull_state() == PULL_WEAK) {
				__expr_why_X(o, ci->second, 
					limit, verbose, u, v);
			}
		}
	}	// end for
}	// end expr_why_X

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic traversal to find all nodes meeting a certain criterion.  
 */
void
State::find_nodes(vector<node_index_type>& ret, 
		bool (*pred)(const node_type&)) const {
	const node_index_type ns = node_pool.size();
	node_index_type i = INVALID_NODE_INDEX +1;
	for ( ; i<ns; ++i) {
		const node_type& n(node_pool[i]);
		if ((*pred)(n)) {
			ret.push_back(i);
		}
	}
}

// pointer-to-member-function variant overload
// suitable for queries that don't require the State object
void
State::find_nodes(vector<node_index_type>& ret, 
		bool (node_type::*predmf)(void) const) const {
	const node_index_type ns = node_pool.size();
	node_index_type i = INVALID_NODE_INDEX +1;
	for ( ; i<ns; ++i) {
		const node_type& n(node_pool[i]);
		if ((n.*predmf)()) {
			ret.push_back(i);
		}
	}
}

// pointer-to-state member function
void
State::find_nodes(vector<node_index_type>& ret, 
		bool (this_type::*predmf)(const node_index_type) const) const {
	const node_index_type ns = node_pool.size();
	node_index_type i = INVALID_NODE_INDEX +1;
	for ( ; i<ns; ++i) {
		if ((this->*predmf)(i)) {
			ret.push_back(i);
		}
	}
}

// pointer to function
void
State::find_nodes(vector<node_index_type>& ret, 
		bool (*p)(const this_type&, const node_index_type)) const {
	const node_index_type ns = node_pool.size();
	node_index_type i = INVALID_NODE_INDEX +1;
	for ( ; i<ns; ++i) {
		if ((*p)(*this, i)) {
			ret.push_back(i);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	printing functor, like output_iterator
	Caller should call a flush after this.  
 */
struct node_printer_base {
	const State&			state;
	ostream&			os;
	const char*			delim;

	explicit
	node_printer_base(const State& s, ostream& o, const char* d) :
			state(s), os(o), delim(d) {
		NEVER_NULL(delim);
	}

};

struct node_printer : public node_printer_base {
	node_printer(const State& s, ostream& o, const char* d) :
			node_printer_base(s, o, d) { }

	void
	operator () (const node_index_type ni) const {
		os << state.get_node_canonical_name(ni) << delim;
	}
};

struct node_printer_prefix : public node_printer_base {
	node_printer_prefix(const State& s, ostream& o, const char* d) :
			node_printer_base(s, o, d) { }

	void
	operator () (const node_index_type ni) const {
		os << delim << state.get_node_canonical_name(ni);
	}
};

template <typename Iter>
ostream&
State::__print_nodes(ostream& o, Iter b, Iter e, const char* delim) const {
	for_each(b, e, node_printer(*this, o, delim));
	return o << std::flush;
}

template <typename Iter>
ostream&
State::__print_nodes_infix(ostream& o, Iter b, Iter e,
		const char* delim) const {
if (b != e) {
	node_printer(*this, o, "")(*b);
	++b;
	for_each(b, e, node_printer_prefix(*this, o, delim));
}
	return o << std::flush;
}

ostream&
State::print_nodes(ostream& o, const vector<node_index_type>& nodes, 
		const char* delim) const {
	return __print_nodes(o, nodes.begin(), nodes.end(), delim);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
static
bool
node_is_X_no_fanin(const State::node_type& n) {
	return node_is_X(n) && !n.has_fanin();
}

static
bool
node_is_X_no_fanin_with_fanout(const State::node_type& n) {
	return node_is_X_no_fanin(n) && n.fanout.size();
		// FIXME: node_is_used
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 	No driver means also not driven by channel
 */
static
bool
node_is_X_no_driver(const State& s, const node_index_type ni) {
	return node_is_X(s.get_node(ni)) && !s.node_is_driven(ni);
}

/**
	Nodes that are X and undriven, and used outputs.  
 */
static
bool
node_is_X_no_driver_with_fanout(const State& s, const node_index_type ni) {
	return node_is_X_no_driver(s, ni) && s.node_is_used(ni);
}

/**
	Find nodes that are X's and unused outputs.  
 */
static
bool
node_is_X_not_used(const State& s, const node_index_type ni) {
	return node_is_X(s.get_node(ni)) && !s.node_is_used(ni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print all X nodes with no fanin.  
	This won't find X cycles, however.  
	\param b true to include node with no fanout
 */
ostream&
State::dump_dangling_unknown_nodes(ostream& o, const bool b) const {
	o << "X nodes with no fanin";
	if (!b) {
		o << ", with fanout";
	}
	o << ":" << endl;
	vector<node_index_type> nodes;
	find_nodes(nodes,
		b ? &node_is_X_no_driver : &node_is_X_no_driver_with_fanout);
	print_nodes(o, nodes, "\n");
	return o << std::flush;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print all nodes with no fanout, presumed to be outputs.
 */
ostream&
State::dump_output_nodes(ostream& o) const {
	o << "nodes with no fanout (unused): " << endl;
	vector<node_index_type> nodes;
	find_nodes(nodes, &this_type::node_is_not_used);
	print_nodes(o, nodes, "\n");
	return o << std::flush;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Status X intersected with nodes with no fanout (not used).
 */
ostream&
State::dump_output_unknown_nodes(ostream& o) const {
	o << "X nodes with no fanout (unused): " << endl;
	vector<node_index_type> nodes;
	find_nodes(nodes, &node_is_X_not_used);
	print_nodes(o, nodes, "\n");
	return o << std::flush;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_INDIRECT_EXPRESSION_MAP
ostream&
State::dump_subexpr(ostream& o, const expr_index_type ei, 
		const bool v, 
		const uchar ptype, const bool pr) const {
	process_expr_map_type::const_iterator
		f(process_expr_map.upper_bound(ei));
	INVARIANT(f != process_expr_map.begin());
	--f;
	// f->first is the lower bound of the global expr range for process...
	// f->second is the global process index, used to translate names
	// ei - f->first is the local expr id within the indexed process
	return dump_local_subexpr(o, ei -f->first, f->second, v, ptype, pr);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive expression printer.  
	Should be modeled after cflat's expression printer.  
	\param ei is the *LOCAL* expression index within the owning process.  
	\param pi is the global process index to which expression belongs.
	\param v true if literal should be printed with its value.  
	\param ptype the parent's expression type, only used if cp is true.
	\param pr whether or not parent is root
		(if so, ignore type comparison for parenthesization).  
 */
#if PRSIM_INDIRECT_EXPRESSION_MAP
#define	DUMP_RECURSIVE			dump_local_subexpr
#define	CALL_DUMP_RECURSIVE(a,b,c,d,e)	dump_local_subexpr(a,b,c,d,e)
#else
#define	DUMP_RECURSIVE			dump_subexpr
#define	CALL_DUMP_RECURSIVE(a,b,c,d,e)	dump_subexpr(a,b,d,e)
#endif
ostream&
State::DUMP_RECURSIVE
	(ostream& o, const expr_index_type ei, 
#if PRSIM_INDIRECT_EXPRESSION_MAP
		const process_index_type pi, 
#endif
		const bool v, 
		const uchar ptype, const bool pr) const {
#if DEBUG_FANOUT
	STACKTRACE_VERBOSE;
#endif
	ISE_INVARIANT(ei);
	ISE_INVARIANT(ei < expr_pool.size());
	const expr_state_type& e(expr_pool[ei]);
	const graph_node_type& g(expr_graph_node_pool[ei]);
	// can elaborate more on when parens are needed
	const bool need_parens = e.parenthesize(ptype, pr);
	const uchar _type = e.type;
	// check if this sub-expression is a root expression by looking
	// up the expression index in the rule_map.  
	const rule_type* ri(lookup_rule(ei));
	if (ri) {
		// then we can print out its attributes
		ri->dump(o << '[') << "]\t";
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
		if (v) {
			get_node(ci->second).dump_value(o << ':');
		}
	} else {
		CALL_DUMP_RECURSIVE(o, ci->second, pi, v, _type);
	}
	if (g.children.size() >= 1) {
	for (++ci; ci!=ce; ++ci) {
		o << op;
		if (ci->first) {
			o << get_node_canonical_name(ci->second);
			if (v) {
				get_node(ci->second).dump_value(o << ':');
			}
		} else {
			if (e.is_or() && is_rule_expr(ci->second)) {
				// to place each 'rule' on its own line
				o << endl;
			}
			CALL_DUMP_RECURSIVE(o, ci->second, pi, v, _type);
		}
	}
	}
	if (need_parens) {
		o << ')';
	}
	if (v && (e.size > 1)) {
		// if verbose, and expression has more than one subexpr
		// print pull-state
		o << '<' << node_type::value_to_char[size_t(e.pull_state())]
			<< '>';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_mk_excl_ring(ostream& o, const ring_set_type& r) const {
	typedef	ring_set_type::const_iterator	const_iterator;
	ISE_INVARIANT(r.size() > 1);
	return __print_nodes_infix(o << "{ ", r.begin(), r.end(), ", ") << " }";
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
	ISE_INVARIANT(r.size() > 1);
	return __print_nodes_infix(o << "{ ", r.begin(), r.end(), ", ") << " }";
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
#if 1
/**
	Got lambda?
 */
template <class E, class M, M E::*member>
static
size_t
member_size_plus(const size_t sum, const E& n) {
	return sum +n.*member.size();
}

template <class E>
static
size_t
add_size(const size_t sum, const E& s) {
	return sum +s.size();
}

template <class P>
static
size_t
add_second_capacity(const size_t sum, const P& s) {
	return sum +s.second.capacity();
}
#endif

/**
	Print memory usage statistics.
 */
ostream&
State::dump_memory_usage(ostream& o) const {
#ifdef HAVE_STL_TREE
#define	sizeof_tree_node(type)	sizeof(std::_Rb_tree_node<type>)
#else
	// assume tree/set/map nodes have 3 pointers +enum color
	static const size_t tree_node_base_size = (3*(sizeof(void*)) +1);
#define	sizeof_tree_node(type)	(sizeof(type) +tree_node_base_size)
#endif

#ifdef	HAVE_EXT_HASHTABLE_H
#define	sizeof_hashtable_node(type)	sizeof(HASH_MAP_NAMESPACE::_Hashtable_node<type>)
#else
	// assume hashtable nodes have 1 pointer (next, singly-linked list)
	static const size_t hashtable_node_base_size = (sizeof(void*));
#define	sizeof_hashtable_node(type)	(sizeof(type) +hashtable_node_base_size)
#endif
{
	const size_t ns = node_pool.size();
	o << "node-state: ("  << ns << " * " << sizeof(node_type) <<
		" B/node) = " << ns * sizeof(node_type) << " B" << endl;
	// give me boost::lambda!!!
	const size_t fo = std::accumulate(
		node_pool.begin(), node_pool.end(), size_t(0),
		&node_type::add_fanout_size);
	o << "node::fanout: (" << fo << " * " << sizeof(expr_index_type) <<
		" B/FO) = " << fo * sizeof(expr_index_type) << " B" << endl;
}
#if PRSIM_INDIRECT_EXPRESSION_MAP
#else
{
	const size_t es = expr_pool.size();
	o << "expr-state: ("  << es << " * " << sizeof(expr_state_type) <<
		" B/expr) = " << es * sizeof(expr_state_type) << " B" << endl;
}{
	const size_t es = expr_graph_node_pool.size();
	o << "expr-graph: ("  << es << " * " << sizeof(graph_node_type) <<
		" B/expr) = " << es * sizeof(graph_node_type) << " B" << endl;
	const size_t gs = std::accumulate(
		node_pool.begin(), node_pool.end(), size_t(0),
		&node_type::add_fanout_size);
	o << "expr::children: (" << gs << " * " << sizeof(expr_index_type) <<
		" B/child) = " << gs * sizeof(expr_index_type) << " B" << endl;
}{
	// hashtable iterator value-types
	typedef	rule_map_type::const_iterator::value_type	value_type;
	const size_t rs = rule_map.size();
	o << "rule-map: (" << rs << " * " << sizeof_hashtable_node(value_type)
		<< " B/rule) = " << rs * sizeof_hashtable_node(value_type)
		<< " B" << endl;
}
#endif	// PRSIM_INDIRECT_EXPRESSION_MAP
	event_pool.dump_memory_usage(o);
{
	const size_t es = event_queue.size();
	o << "event-queue: ("  << es << " * " << sizeof(event_placeholder_type)
		<< " B/event) = " << es * sizeof(event_placeholder_type)
		<< " B" << endl;
	// may be bigger due to reserved capacity
}
{
	typedef	mk_excl_queue_type::const_iterator::value_type	value_type;
	const size_t hs = exclhi_queue.size();
	o << "exclhi-queue: ("  << hs << " * " << sizeof_tree_node(value_type)
		<< " B/event) = " << hs * sizeof_tree_node(value_type)
		<< " B" << endl;
	const size_t ls = excllo_queue.size();
	o << "excllo-queue: ("  << ls << " * " << sizeof_tree_node(value_type)
		<< " B/event) = " << ls * sizeof_tree_node(value_type)
		<< " B" << endl;
#if UNIQUE_PENDING_QUEUE
	const size_t ps = pending_queue.size();
	o << "pending-queue: ("  << ps << " * " <<
		sizeof_tree_node(event_index_type) << " B/event) = " <<
		ps * sizeof_tree_node(event_index_type) << " B" << endl;
#else
	const size_t ps = pending_queue.capacity();	// reserved
	o << "pending-queue: ("  << ps << " * " << sizeof(event_index_type) <<
		" B/event) = " << ps * sizeof(event_index_type)
		<< " B" << endl;
#endif
}
{
	// rings
	const size_t rs = mk_exhi.size();
	o << "mk-exclhi-rings: ("  << rs << " * " << sizeof(ring_set_type) <<
		" B/ring) = " << rs * sizeof(ring_set_type) << " B" << endl;
	const size_t rr = std::accumulate(mk_exhi.begin(), mk_exhi.end(), 
		size_t(0), &add_size<ring_set_type>);
	typedef	ring_set_type::iterator::value_type	value_type;
	o << "mk-exclhi::nodes: (" << rr << " * " <<
		sizeof_tree_node(value_type) <<
		" B/node) = " << rr * sizeof_tree_node(value_type)
		<< " B" << endl;
	// alternative to set: sorted valarray/vector
}{
	// rings
	const size_t rs = mk_exlo.size();
	o << "mk-excllo-rings: ("  << rs << " * " << sizeof(ring_set_type) <<
		" B/ring) = " << rs * sizeof(ring_set_type) << " B" << endl;
	const size_t rr = std::accumulate(mk_exlo.begin(), mk_exlo.end(), 
		size_t(0), &add_size<ring_set_type>);
	typedef	ring_set_type::iterator::value_type	value_type;
	o << "mk-excllo::nodes: (" << rr << " * " <<
		sizeof_tree_node(value_type) <<
		" B/node) = " << rr * sizeof_tree_node(value_type)
		<< " B" << endl;
	// alternative to set: sorted valarray/vector
}{
	const size_t ch = check_exhi_ring_pool.capacity();
	o << "chk-exclhi-locks: ("  << ch << " / 8 lock/B) >= "
		<< ((ch +7)>>3) << " B" << endl;	// round-up
	const size_t cl = check_exlo_ring_pool.capacity();
	o << "chk-excllo-locks: ("  << cl << " / 8 lock/B) >= "
		<< ((cl +7)>>3) << " B" << endl;	// round-up
}{
	typedef	check_excl_ring_map_type::const_iterator::value_type
						value_type;
	const size_t hs = check_exhi.size();
	o << "chk-exclhi-rings: ("  << hs << " * " <<
		sizeof_tree_node(value_type) << " B/ring) = " <<
		hs * sizeof_tree_node(value_type) << " B" << endl;
	const size_t hc = std::accumulate(check_exhi.begin(), check_exhi.end(), 
		size_t(0), &add_second_capacity<value_type>);
	o << "chk-exclhi::nodes: (" << hc << " * " << sizeof(node_index_type) <<
		" B/node) = " << hc * sizeof(node_index_type) << " B" << endl;
	const size_t ls = check_exhi.size();
	o << "chk-excllo-rings: ("  << ls << " * " <<
		sizeof_tree_node(value_type) << " B/ring) = " <<
		ls * sizeof_tree_node(value_type) << " B" << endl;
	const size_t lc = std::accumulate(check_exhi.begin(), check_exhi.end(), 
		size_t(0), &add_second_capacity<value_type>);
	o << "chk-excllo::nodes: (" << lc << " * " << sizeof(node_index_type) <<
		" B/node) = " << lc * sizeof(node_index_type) << " B" << endl;
}{
	typedef	watch_list_type::value_type		value_type;
	const size_t ws = watch_list.size();
	o << "watch-list: ("  << ws << " * " <<
		sizeof_tree_node(value_type) << " B/node) = " <<
		ws * sizeof_tree_node(value_type) << " B" << endl;
}{
	// smaller stuff
	// expr_trace_type __scratch_expr_trace
	// fanout_array_type __shuffle_indices
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
#if EXTRA_ALIGN_MARKERS
	static const short sep = 0xFFFF;
#define	WRITE_ALIGN_MARKER		write_value(o, sep);
#else
#define	WRITE_ALIGN_MARKER		{}
#endif
	check_event_queue();		// internal structure consistency
	write_value(o, magic_string);
{
	// save the random seed
	ushort seed[3] = {0, 0, 0};
	const ushort* old_seed = seed48(seed);	// libc
	seed[0] = old_seed[0];
	seed[1] = old_seed[1];
	seed[2] = old_seed[2];
	// put it back
	seed48(seed);
	write_value(o, seed[0]);
	write_value(o, seed[1]);
	write_value(o, seed[2]);
}
{
	// node_pool
	write_value(o, node_pool.size());
	for_each(node_pool.begin() +1, node_pool.end(), 
		bind2nd_argval(mem_fun_ref(&node_type::save_state), o)
	);
}
	WRITE_ALIGN_MARKER
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
		get_event(i->event_index).save_state(o);
	}
}
	WRITE_ALIGN_MARKER
	// excl_rings -- structural only
	// excl and pending queues should be empty!
	ISE_INVARIANT(exclhi_queue.empty());
	ISE_INVARIANT(excllo_queue.empty());
	ISE_INVARIANT(pending_queue.empty());
	write_value(o, current_time);
	write_value(o, uniform_delay);
	WRITE_ALIGN_MARKER
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
	WRITE_ALIGN_MARKER
	write_value(o, flags);
	write_value(o, unstable_policy);
	write_value(o, weak_unstable_policy);
	write_value(o, interference_policy);
	write_value(o, weak_interference_policy);
	write_value(o, timing_mode);
#if PRSIM_CHANNEL_SUPPORT
	if (_channel_manager.save_checkpoint(o)) return true;
#endif
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
#if EXTRA_ALIGN_MARKERS
	static const short sep = 0xFFFF;
#define	READ_ALIGN_MARKER						\
	{ short x; read_value(i, x); INVARIANT(x == sep); }
#else
#define	READ_ALIGN_MARKER		{}
#endif
	static const char bad_ckpt[] = 
		"ERROR: not a valid hackt prsim checkpoint file.";
	initialize();		// start by initializing everything
	// or reset(); ?
try {
	string header_check;
	read_value(i, header_check);
	if (header_check != magic_string) {
		cerr << bad_ckpt << endl;
		return true;
	}
} catch (...) {
	cerr << bad_ckpt << endl;
	return true;
}{
	// restore random seed
	ushort seed[3];
	read_value(i, seed[0]);
	read_value(i, seed[1]);
	read_value(i, seed[2]);
	seed48(seed);
}{
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
	READ_ALIGN_MARKER		// sanity alignment check
}{
	// to reconstruct from nodes only, we perform propagation evaluation
	// on every node, as if it had just fired out of X state.  
	typedef node_pool_type::const_iterator	const_iterator;
	const const_iterator nb(node_pool.begin()), ne(node_pool.end());
	const_iterator ni(nb);
	const value_enum prev = LOGIC_OTHER;
	for (++ni; ni!=ne; ++ni) {
		typedef	node_type::const_fanout_iterator
					const_fanout_iterator;
		const node_type& n(*ni);
		const_fanout_iterator fi(n.fanout.begin()), fe(n.fanout.end());
		const value_enum next = n.current_value();
		if (next != prev) {
			const expr_index_type nj(distance(nb, ni));
			for ( ; fi!=fe; ++fi) {
				// interpret node value as pull-value
				evaluate(nj, *fi,
					pull_enum(prev), pull_enum(next));
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
		// can enqueue a killed event
		load_enqueue_event(t, __load_allocate_event(ev));
	}
	// nodes and events should be consistent by now
	check_event_queue();
}
	READ_ALIGN_MARKER		// sanity alignment check
	// excl_rings -- structural only
	// excl and pending queues should be empty!
	ISE_INVARIANT(exclhi_queue.empty());
	ISE_INVARIANT(excllo_queue.empty());
	ISE_INVARIANT(pending_queue.empty());
	read_value(i, current_time);
	read_value(i, uniform_delay);
	READ_ALIGN_MARKER
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
	READ_ALIGN_MARKER		// sanity alignment check
	read_value(i, flags);
	read_value(i, unstable_policy);
	read_value(i, weak_unstable_policy);
	read_value(i, interference_policy);
	read_value(i, weak_interference_policy);
	read_value(i, timing_mode);
	// interrupted flag, just ignore
	// ifstreams? don't bother managing input stream stack.
	// __scratch_expr_trace -- never needed, ignore
#if PRSIM_CHANNEL_SUPPORT
	if (_channel_manager.load_checkpoint(i)) return true;
#endif

	// this must be run *after* mode flags are loaded
if (checking_excl()) {
	typedef node_pool_type::const_iterator	const_iterator;
	const const_iterator nb(node_pool.begin()), ne(node_pool.end());
	const_iterator ni(nb);
	const value_enum prev = LOGIC_OTHER;
	for (++ni; ni!=ne; ++ni) {
	// lock exclusive check rings
		const node_type& n(*ni);
		const value_enum next = n.current_value();
		const excl_exception
			e(check_excl_rings(distance(nb, ni), n, prev, next));
		if (e.lock_id) {
			inspect_exception(e, cerr);
			// don't bother throwing
		}
	}
}
{
	string temp;
	read_value(i, temp);
	if (temp != magic_string) {
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
#if EXTRA_ALIGN_MARKERS
	static const short sep = 0xFFFF;
#endif
	string header_check;
	read_value(i, header_check);
	o << "header string: " << header_check << endl;
{
	// show random seed
	ushort seed[3];
	read_value(i, seed[0]);
	read_value(i, seed[1]);
	read_value(i, seed[2]);
	o << "seed48: " << seed[0] << ' ' << seed[1] << ' ' << seed[2] << endl;
}{
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
	READ_ALIGN_MARKER		// sanity alignment check
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
	READ_ALIGN_MARKER		// sanity alignment check
	time_type current_time, uniform_delay;
	read_value(i, current_time);
	read_value(i, uniform_delay);
	o << "current time: " << current_time << endl;
	o << "uniform delay: " << uniform_delay << endl;
	READ_ALIGN_MARKER
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
	READ_ALIGN_MARKER		// sanity alignment check
	flags_type flags;
	read_value(i, flags);
	o << "flags: " << size_t(flags) << endl;
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
	char timing_mode;
	read_value(i, timing_mode);
	o << "timing mode: " << size_t(timing_mode) << endl;
#if PRSIM_CHANNEL_SUPPORT
{
	channel_manager tmp;
	tmp.load_checkpoint(i);
	tmp.dump_checkpoint_state(o) << endl;
}
#endif
	read_value(i, header_check);
	o << "footer string: " << header_check << endl;
	return o;
}	// end State::dump_checkpoint

#undef	READ_ALIGN_MARKER

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
}	// end namespace PRSIM

// explicit template instantiation of signal handler class
template class signal_handler<PRSIM::State>;
// template class EventQueue<EventPlaceholder<real_time> >;

}	// end namespace SIM
}	// end namespace HAC

