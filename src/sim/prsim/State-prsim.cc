/**
	\file "sim/prsim/State-prsim.cc"
	Implementation of prsim simulator state.  
	$Id: State-prsim.cc,v 1.87 2011/06/22 19:38:30 fang Exp $

	This module was renamed from:
	Id: State.cc,v 1.32 2007/02/05 06:39:55 fang Exp
	to avoid object base name collisions.
	A sad discontinuity in revision history...
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_FANOUT			(0 && ENABLE_STACKTRACE)
#define	DEBUG_STEP			(0 && ENABLE_STACKTRACE)
#define	DEBUG_LOOKUP			(0 && ENABLE_STACKTRACE)
#define	DEBUG_CHECK			(0 && ENABLE_STACKTRACE)
#define	DEBUG_WHY			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <algorithm>
#include <functional>
#include <string>
#include <set>
#include "sim/prsim/State-prsim.tcc"
#include "sim/prsim/Command-prsim-export.hh"
#include "sim/prsim/ExprAlloc.hh"
#include "sim/prsim/Trace-prsim.hh"
#include "sim/prsim/VCDManager.hh"
#include "sim/event.tcc"
#include "sim/prsim/util.tcc"
#include "sim/random_time.hh"
#include "sim/signal_handler.tcc"
#include "Object/module.hh"
#include "Object/global_entry.tcc"	// for extract_parent_formal_...
#include "Object/traits/classification_tags.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/lang/PRS_footprint.hh"
#include "Object/lang/SPEC_footprint.hh"
#include "Object/inst/instance_alias_info.hh"	// for bool_connect_policy
#include "Object/inst/alias_empty.hh"
#include "Object/traits/proc_traits.hh"	// for diagnostic
#include "Object/global_entry.hh"
#include "parser/instref.hh"
#include "parser/type.hh"
#include "sim/ISE.hh"
#include "common/TODO.hh"
#include "util/attributes.h"
#include "util/sstream.hh"
#include "util/stacktrace.hh"
#include "util/memory/array_pool.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/likely.h"
#include "util/iterator_more.hh"
#include "util/string.tcc"
#include "util/IO_utils.tcc"
#include "util/binders.hh"
#include "util/utypes.h"
#include "util/indent.hh"
#include "util/swap_saver.hh"
#include "util/tokenize.hh"
#include "util/numformat.tcc"
#if PRSIM_SET_FAST_ALLOCATOR
#include "util/memory/allocator_adaptor.tcc"	// for fast allocators
#include "util/memory/chunk_map_pool.tcc"	// for fast allocators
#endif

// re-define to be more readable
#undef	STACKTRACE_VERBOSE
#define	STACKTRACE_VERBOSE		STACKTRACE_BRIEF

#if	DEBUG_STEP
#define	DEBUG_STEP_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_STEP		STACKTRACE_VERBOSE
#else
#define	DEBUG_STEP_PRINT(x)
#define	STACKTRACE_VERBOSE_STEP
#endif

#if	DEBUG_LOOKUP
#define	DEBUG_LOOKUP_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_LOOKUP	STACKTRACE_VERBOSE
#else
#define	DEBUG_LOOKUP_PRINT(x)
#define	STACKTRACE_VERBOSE_LOOKUP
#endif

#if	DEBUG_CHECK
#define	DEBUG_CHECK_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_CHECK	STACKTRACE_VERBOSE
#else
#define	DEBUG_CHECK_PRINT(x)
#define	STACKTRACE_VERBOSE_CHECK
#endif

#if	DEBUG_WHY
#define	DEBUG_WHY_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_WHY		STACKTRACE_VERBOSE
#else
#define	DEBUG_WHY_PRINT(x)
#define	STACKTRACE_VERBOSE_WHY
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

#define	GET_CONTEXT_CACHE		get_module().get_context_cache().

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
using std::bind2nd;
using std::make_pair;
using util::set_inserter;
using util::strings::string_to_num;
using util::read_value;
using util::write_value;
using util::bind2nd_argval;
using util::bind2nd_argval_void;
using util::auto_indent;
using util::indent;
using util::tokenize_char;
using util::format_ostream_ref;
using util::swap_saver;
using entity::state_manager;
using entity::global_entry_pool;
using entity::bool_tag;
using entity::process_tag;
using entity::footprint_frame_map_type;
#include "util/using_ostream.hh"
using parser::parse_node_to_index;

//=============================================================================
/**
	Passing around information from expression evaluation.  
 */
struct State::evaluate_return_type {
	typedef	expr_struct_type	root_ex_type;
	/// the node affected by propagation
	node_index_type			node_index;
	/// state of the root expression
	const root_ex_type*		root_ex;
	/// new pull state
	pull_enum			root_pull;
	/// root rule struct
	const rule_type*		root_rule;
	/// root rule index
	rule_index_type			root_rule_index;
	/// previous pull-state, complete
	pull_set			prev_pull;
	/// true signals that simulation should halt, e.g. if there is error
	error_policy_enum		invariant_break;

	/// other fields may remain uninitialized, we won't use them
	evaluate_return_type() : node_index(INVALID_NODE_INDEX),
		// other fields, don't care
		invariant_break(ERROR_NONE)
		{ }

	explicit
	evaluate_return_type(const error_policy_enum s) : 
		node_index(INVALID_NODE_INDEX), invariant_break(s) { }

	evaluate_return_type(const node_index_type ni,
		const root_ex_type* const e, const pull_enum p, 
		const pull_set& ps, 
		const rule_type* const r, const rule_index_type ri) :
		node_index(ni), root_ex(e), root_pull(p)
			, root_rule(r), root_rule_index(ri)
			, prev_pull(ps)
			, invariant_break(ERROR_NONE)
			{ }
};	// end struct evaluate_return_type

//=============================================================================
// process_sim_state member struct definitions

struct process_sim_state::dumper_base {
	ostream&			os;
	const State&			st;
	const bool			verbose;

	dumper_base(ostream& o, const State& s, const bool v) :
		os(o), st(s), verbose(v) { }

};	// end struct invariant_checker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct process_sim_state::rules_dumper : public dumper_base {

	rules_dumper(ostream& o, const State& s, const bool v) :
		dumper_base(o, s, v) { }

	ostream&
	operator () (const process_sim_state& s) {
		// skip empty processes
		const unique_process_subgraph& pg(s.type());
		// TODO: count number of non-invariant rules
	if (pg.rule_pool.size()) {
		st.dump_process_canonical_name(os << "process: ", s) << endl;
		// TODO: print type?
		return s.dump_rules(os, st, verbose);
	} else {
		return os;
	}
	}
};	// end struct invariant_checker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenient accumulator functor.
 */
struct process_sim_state::invariant_checker {
	ostream&			os;
	const State&			st;

	invariant_checker(ostream& o, const State& s) : os(o), st(s) { }

	bool
	operator () (const bool e, const process_sim_state& s) {
		return s.check_invariants(os, st) || e;
	}
};	// end struct invariant_checker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct process_sim_state::invariant_dumper : public dumper_base {

	invariant_dumper(ostream& o, const State& s, const bool v) :
		dumper_base(o, s, v) { }

	ostream&
	operator () (const process_sim_state& s) {
		const unique_process_subgraph& pg(s.type());
		// TODO: count number of invariant rules
	if (pg.rule_pool.size()) {
		st.dump_process_canonical_name(os << "process: ", s) << endl;
		return s.dump_invariants(os, st, verbose);
	} else {
		// skip empty processes
		return os;
	}
	}
};	// end struct invariant_checker

//=============================================================================
// class State::event_deallocator definition

/**
	Helper class using destructor semantics to automatically
	deallocate an event upon end-of-life.  
	This helps the State::step method in which there are
	multiple early return cases.  
	Except, there's one case where we don't want to deallocate... 
		(exception), hence the keep member.
 */
class State::event_deallocator {
	State&				_state;
	node_type& 			_node;
	const event_index_type		_event;
public:
	bool				keep;
public:
	event_deallocator(State& s, node_type& n) :
		_state(s), _node(n), _event(n.get_event()), keep(false) { }
	~event_deallocator() {
		if (!keep) {
			_state.__deallocate_event(_node, _event);
		}
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

/**
	Upon library/module loading, initialize the random seed one-time.
 */
static
int
__module_init__(void) {
	util::numeric::seed48_zeros();
	return 1;
}

static
const int __module_init_token__ = __module_init__();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_MAP_FAST_ALLOCATOR
static
State::updated_nodes_type
__updated_nodes_sentinel;

// give this dummy a permanent null entry to prevent allocator from
// repeatedly allocating-first/deallocating-last chunk
static
void
__initialize_updated_nodes_sentinel(void) {
	__updated_nodes_sentinel[__updated_nodes_sentinel.size()] =
		State::node_update_info();
}

static
void
__remove_updated_nodes_sentinel(void) {
	__updated_nodes_sentinel.erase(__updated_nodes_sentinel.size() -1);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates simulation state, given a module.
	\param m the expanded module object.
	\pre m must already be past the allcoate phase.  
	\throw exception if there is an error
 */
State::State(const entity::module& m, const ExprAllocFlags& f) : 
		module_state_base(m, "prsim> "), 
		expr_alloc_flags(f),
		node_pool(),
		process_footprint_map(),
		unique_process_pool(), 
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
		global_expr_process_id_map(), 
#endif
		process_state_array(), 
		event_pool(), event_queue(), 
#if PRSIM_MK_EXCL_BLOCKING_SET
		mk_exhi(1), mk_exlo(1), 
		mk_exhi_counter_map(),
		mk_exlo_counter_map(),
		__mk_excl_blocking_set(),
#else
		mk_exhi(), mk_exlo(), 
		exclhi_queue(), excllo_queue(), 
#endif
		updated_nodes(), 
#if PRSIM_FCFS_UPDATED_NODES
		updated_nodes_queue(),
#endif
		check_exhi_ring_pool(1), check_exlo_ring_pool(1), 
		check_exhi(), check_exlo(), 
#if PRSIM_SETUP_HOLD
		timing_checker(*this),
#endif
#if PRSIM_TIMING_BACKANNOTATE
		delay_annotation_manager(),
#endif
		current_time(0), 
		uniform_delay(time_traits::default_delay), 
		default_after_min(time_traits::zero), 
		default_after_max(time_traits::zero), 
		timing_probability(0.5),
		_channel_manager(), 
		trace_manager(),
		trace_flush_interval(1L<<16),
		vcd_manager(),
		vcd_timescale(1.0),
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
		cache_half_life(1024),
		cache_countdown(cache_half_life),
#endif
		flags(FLAGS_DEFAULT),
#define	E(e)	error_policy_enum(ERROR_DEFAULT_##e)
		invariant_fail_policy(E(INVARIANT_FAIL)),
		invariant_unknown_policy(E(INVARIANT_UNKNOWN)),
		unstable_policy(E(UNSTABLE)),
		weak_unstable_policy(E(WEAK_UNSTABLE)),
#if PRSIM_MK_EXCL_BLOCKING_SET
		excl_unstable_policy(E(EXCL_UNSTABLE)),
#endif
		interference_policy(E(INTERFERENCE)),
		weak_interference_policy(E(WEAK_INTERFERENCE)),
		assert_fail_policy(E(ASSERT_FAIL)),
		channel_expect_fail_policy(E(CHANNEL_EXPECT_FAIL)),
		excl_check_fail_policy(E(EXCL_CHECK_FAIL)),
		keeper_check_fail_policy(E(KEEPER_CHECK)),
#if 0 && PRSIM_SETUP_HOLD
		setup_violation_policy(E(SETUP_VIOLATION)),
		hold_violation_policy(E(HOLD_VIOLATION)),
#endif
#undef	E
		autosave_name("autosave.prsimckpt"),
		timing_mode(TIMING_DEFAULT),
		_dump_flags(dump_flags::no_owners), 
		time_fmt(cout),
		__atomic_updated_nodes(),
		recent_exceptions(),
		__shuffle_indices(0) {
	STACKTRACE_VERBOSE;
#if PRSIM_MAP_FAST_ALLOCATOR
	__initialize_updated_nodes_sentinel();
#endif
//	time_fmt.fmt |= std::ios_base::fixed;
//	time_fmt.fmt &= ~std::ios_base::scientific;
//	time_fmt.precision = 3;		// default is 6
	const footprint& topfp(mod.get_footprint());
	const size_t s = topfp.get_instance_pool<bool_tag>().total_entries() +1;
	// let 0th slot be dummy, so we can use 1-based global indexing
	head_sentinel();
	node_pool.resize(s);
	// let ExprAlloc take care of this during visit

	// not expect expression-trees deeper than 8, but is growable
	__shuffle_indices.reserve(32);
	// then go through all processes to generate expressions

	// use a cflat-prs-like pass to construct the expression netlist
	// got a walker? and prs_expr_visitor?
	// see "ExprAlloc.h"

	m.get_context_cache();		// reference checks for NULL
	// NOTE: we're referencing 'this' during construction, however, we 
	// are done constructing this State's members at this point.  
	const entity::global_process_context gpc(topfp);
	ExprAlloc v(*this, gpc, f);
	// pre-allocate array of process states
	// use 0th slot for top-level
	process_state_array.resize(
		topfp.get_instance_pool<process_tag>().total_entries() +1);
	// unique_process_pool.reserve() ?
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
	// first valid global expression ID is 1, 0 is reserved as NULL
#if PRSIM_PROCESS_EXPR_MAP_ARRAY
	global_expr_process_id_map.reserve(1024);
	global_expr_process_id_map.push_back(
		expr_process_entry_type(FIRST_VALID_GLOBAL_EXPR, 0));
#else
	global_expr_process_id_map[FIRST_VALID_GLOBAL_EXPR] = 0;
#endif
	// for top-level
	// if top-level process is empty, will need to replace this entry!
#endif
	// top-level prs in the module, pid=0
try {
	STACKTRACE_INDENT_PRINT("instantiated processes ..." << endl);
	v();	// GO!!!
} catch (const entity::instance_exception<process_tag>& e) {
	cerr << "Error with process instance: ";
	// ALERT: could be top-level
	topfp.dump_canonical_name<process_tag>(cerr, e.pid, 
		_dump_flags) << endl;
	THROW_EXIT;	// re-throw
}
	// recommend caller to invoke ::initialize() immediately after ctor
}	// end State::State(const module&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: possibly run some checks?
	TODO: auto-checkpoint here if desired, even if state incoherent
 */
State::~State() {
	STACKTRACE_VERBOSE;
	if ((flags & FLAG_AUTOSAVE) && autosave_name.size()) {
		// always clear some flags before the automatic save?
		// close traces before checkpointing
		close_trace();
		close_vcd();
		ofstream o(autosave_name.c_str());
		if (o) {
		try {
			save_checkpoint(o);
		} catch (...) {
			cerr << "Fatal error during checkpoint save." << endl;
		}
		} else {
			cerr << "Error opening \'" << autosave_name <<
				"\' for saving checkpoint." << endl;
		}
	}
	// dequeue all events and check consistency with event pool 
	// upon its destruction.
	while (!event_queue.empty()) {
		const event_placeholder_type next(event_queue.pop());
		event_pool.deallocate(next.event_index);
	}
#if PRSIM_MAP_FAST_ALLOCATOR
	__remove_updated_nodes_sentinel();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Destroys the simulator state, releasing all of its memory too.  
 */
void
State::destroy(void) {
	node_pool.clear();
	// clear process_sim_state array?
	// unique_process_pool.clear();	// necessary?
	process_state_array.clear();
	event_pool.clear();
	event_queue.clear();
	head_sentinel();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restarts simulation at time 0.
	This requires closing trace files so they remain coherent, 
	and don't jump backwards in time.
 */
void
State::__initialize_time(void) {
	current_time = 0;
	// autosave? OK to keep
	// trace file?
	close_trace();	// close trace, else trace will be incoherent
	close_vcd();	// close trace, else trace will be incoherent
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Clears internal data structures.
	Resets nodes to X and clears event queues.  
	However, keeps time unchanged.
	Procedure is common to initialize() and reset().
 */
void
State::__initialize_state(const bool startup, const bool reset_count) {
	STACKTRACE_VERBOSE;
	for_each(node_pool.begin(), node_pool.end(), 
		mem_fun_ref(startup ? &node_type::reset :
			(reset_count ? &node_type::initialize
				: &node_type::x_value_and_cause)));
	for_each(process_state_array.begin(), process_state_array.end(), 
		mem_fun_ref(&process_sim_state::initialize));
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

	// initialize implicit port supplies
	const footprint& fp(mod.get_footprint());
	const node_index_type gi = parse_node_to_index("!GND", fp).index;
	const node_index_type vi = parse_node_to_index("!Vdd", fp).index;
	INVARIANT(gi);
	INVARIANT(vi);
	// Q: should this be done with set_node(), in case setting globals
	// triggers events to be enqueued?
	// for now, the implciit globals cannot be referenced directly
	// in the source language, so we'll punt.
	const event_cause_type null;
#if PRSIM_MODEL_POWER_SUPPLIES
	// TODO: these need to trigger fanout evaluations!
	set_node(gi, LOGIC_LOW, false);		// force?
	set_node(vi, LOGIC_HIGH, false);	// force?
	cycle();
	// make sure time has not advanced!
#else
	node_pool[gi].set_value_and_cause(LOGIC_LOW, null, current_time);
	node_pool[vi].set_value_and_cause(LOGIC_HIGH, null, current_time);
#endif
	// keep connected channels up-to-date with X values of nodes
	// but this closes channel-log streams
	_channel_manager.initialize();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::__initialize(const bool startup) {
	STACKTRACE_VERBOSE;
	__initialize_time();
	__initialize_state(startup, true);
	assert(time_traits::is_zero(current_time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	X-out all node values, clear causes.  
	Retain channel setups, modes, etc.
 */
void
State::x_all(void) {
	STACKTRACE_VERBOSE;
	__initialize_state(false, false);
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
	__initialize(false);
	flags |= FLAGS_INITIALIZE_SET_MASK;
	flags &= ~FLAGS_INITIALIZE_CLEAR_MASK;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pair<size_t, bool>
State::allocate_unique_process_graph(const footprint* f) {
	const pair<process_footprint_map_type::iterator, bool>
		p(process_footprint_map.insert(
			std::make_pair(f, unique_process_pool.size())));
	if (p.second) {
		unique_process_pool.push_back(
			unique_process_subgraph(f));
	}
	return std::make_pair(p.first->second, p.second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const footprint*
State::parse_to_footprint(const string& t) const {
	return parser::parse_to_footprint(t.c_str(), get_module());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A node is driven if it has fanin rule, or can be driven from channel.
	Has nothing to do with the state of the node.
 */
bool
State::node_is_driven(const node_index_type ni) const {
	return get_node(ni).has_fanin() || node_is_driven_by_channel(ni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A node is used if it has fanout or can cause a channel event.  
	Invariants should NOT count as fanout, thus we have to inspect.
	Always count weak rules as fanout.
 */
bool
State::node_is_used(const node_index_type ni) const {
	typedef	fanout_array_type::const_iterator	const_iterator;
	const node_type& n(get_node(ni));
	const fanout_array_type& foa(n.fanout);
	const_iterator fi(foa.begin()), fe(foa.end());
	for ( ; fi!=fe; ++fi) {
		const expr_index_type& gei = *fi;
		const process_sim_state& ps(lookup_global_expr_process(gei));
		const expr_index_type lei = ps.local_expr_index(gei);
		const unique_process_subgraph& pg(ps.type());
		const rule_type* r = pg.lookup_rule(pg.local_root_expr(lei));
		if (!r->is_invariant()) {
			return true;
		}
	}	// end for
	if (node_drives_any_channel(ni)) {
		return true;
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
State::node_is_driven_by_channel(const node_index_type ni) const {
	return get_node(ni).in_channel() &&
		_channel_manager.node_has_fanin(ni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
State::node_drives_any_channel(const node_index_type ni) const {
	return get_node(ni).in_channel() &&
		_channel_manager.node_has_fanout(ni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Q: is this the best place to handle this?
/**
	Q: should node activity coming from channels honor the
	node's frozen state?
	A: I don't think so, that's what channel-stop is for.
	\return true if there is an event exception
 */
State::break_type
State::flush_channel_events(const vector<env_event_type>& env_events, 
		cause_arg_type c) {
	STACKTRACE_VERBOSE;
	break_type err = ERROR_NONE;
	// cause of these events must be 'ni', this node
	vector<env_event_type>::const_iterator
		i(env_events.begin()), e(env_events.end());
	// const event_cause_type c(ni, next);
	for ( ; i!=e; ++i) {
#if 0
		dump_node_canonical_name(cerr << "channel event on node: ",
			i->first) << endl;
#endif
#if PRSIM_CHANNEL_TIMING
		const node_index_type& ni(i->node_index);
		const value_enum _v = i->value;
		const time_type& t = i->delay;
#else
		const node_index_type& ni(i->first);
		const value_enum _v = i->second;
#endif
		node_type& _n(__get_node(ni));
		// frozen nodes should never enter the temporary queues
		ISE_INVARIANT(!_n.is_frozen());
		const event_index_type pe = _n.get_event();
		// check for pending events regardless of value-change
		// to detect cancelled events and instabilities.
		if (pe) {
	// interaction with other enqueued events? anomalies?
	// for now, give up if there are conflicting events in queue
			// instability!?
			event_type& ev(get_event(pe));
			const break_type E =
			__report_instability(cout,
				_v == LOGIC_OTHER, 
				ev.val == LOGIC_HIGH,
#if PRSIM_MK_EXCL_BLOCKING_SET
				false,	// unrelated to excl
#endif
				ev);
#if 0
			if (UNLIKELY(E >= ERROR_BREAK)) {
				stop();
			if (UNLIKELY(E >= ERROR_INTERACTIVE)) {
				// now catches FATAL and INTERACTIVE
				record_exception(exception_ptr_type(
					new instability_exception(ni, E)));
			}
			}
#endif
			if (E > err) err = E;
			if (dequeue_unstable_events()) {
				// overtake
				kill_event(pe, ev.node);
			} else {
				ev.val = LOGIC_OTHER;
				continue;
			}
		}

		if (_n.current_value() != _v) {
		const event_index_type pn =
			__allocate_event(_n, ni, c,
				INVALID_RULE_INDEX,
#if EVENT_INCLUDE_RULE_POINTER
				NULL,
#endif
				_v
#if PRSIM_WEAK_RULES
				, false	// environment never weak
#endif
			);
			// enqueue_event
			const event_type& ev(get_event(pn));
#if PRSIM_CHANNEL_TIMING
		if (!i->use_global) {
			enqueue_event(current_time +t, pn);
		} else {
#endif
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
#if PRSIM_CHANNEL_TIMING
		}
#endif
		} // else filter out vacuous events
	}
	return err;
}	// end method State::flush_channel_events

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Set a single channel into reset state.  
	FIXME:
	NB: flush_channel_events may result in instabilities!
	\return ?
 */
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
bool
State::reset_channel(channel& chan) {
	vector<env_event_type> temp;
	chan.reset(temp);
#if PRSIM_TRACK_CAUSE_TIME
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER, current_time);
#else
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER);
#endif
	flush_channel_events(temp, c);
	return false;
}
#else
bool
State::reset_channel(const string& cn) {
	vector<env_event_type> temp;
	if (_channel_manager.reset_channel(cn, temp))	return true;
#if PRSIM_TRACK_CAUSE_TIME
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER, current_time);
#else
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER);
#endif
	flush_channel_events(temp, c);
	return false;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reset all channels.  
	FIXME:
	NB: flush_channel_events may result in instabilities!
 */
void
State::reset_all_channels(void) {
	vector<env_event_type> temp;
	_channel_manager.reset_all_channels(temp);
#if PRSIM_TRACK_CAUSE_TIME
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER, current_time);
#else
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER);
#endif
	flush_channel_events(temp, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Set a single channel into reset state.  
	NB: flush_channel_events may result in instabilities!
 */
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
bool
State::resume_channel(channel& chan) {
	// TODO: could make this static local, and clear() to re-use space
	vector<env_event_type> temp;
	chan.resume(*this, temp);
#if PRSIM_TRACK_CAUSE_TIME
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER, current_time);
#else
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER);
#endif
	flush_channel_events(temp, c);
	return false;
}
#else
bool
State::resume_channel(const string& cn) {
	// TODO: could make this static local, and clear() to re-use space
	vector<env_event_type> temp;
	if (_channel_manager.resume_channel(*this, cn, temp))	return true;
#if PRSIM_TRACK_CAUSE_TIME
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER, current_time);
#else
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER);
#endif
	flush_channel_events(temp, c);
	return false;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reset all channels.  
	NB: flush_channel_events may result in instabilities!
 */
void
State::resume_all_channels(void) {
	// TODO: could make this static local, and clear() to re-use space
	vector<env_event_type> temp;
	_channel_manager.resume_all_channels(*this, temp);
#if PRSIM_TRACK_CAUSE_TIME
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER, current_time);
#else
	const event_cause_type c(INVALID_NODE_INDEX, LOGIC_OTHER);
#endif
	flush_channel_events(temp, c);
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
	This unfortunately still preserves interpreter aliases.  
		The 'unalias-all' command should clear all aliases.
 */
void
State::reset(void) {
	STACKTRACE_VERBOSE;
	__initialize(true);
	// this also closes trace files
	flags = FLAGS_DEFAULT;
#define	E(e)	error_policy_enum(ERROR_DEFAULT_##e)
	unstable_policy = E(UNSTABLE);
	weak_unstable_policy = E(WEAK_UNSTABLE);
#if PRSIM_MK_EXCL_BLOCKING_SET
	excl_unstable_policy = E(EXCL_UNSTABLE);
#endif
	interference_policy = E(INTERFERENCE);
	weak_interference_policy = E(WEAK_INTERFERENCE);
	invariant_fail_policy = E(INVARIANT_FAIL),
	invariant_unknown_policy = E(INVARIANT_UNKNOWN),
	assert_fail_policy = E(ASSERT_FAIL);
	channel_expect_fail_policy = E(CHANNEL_EXPECT_FAIL);
	excl_check_fail_policy = E(EXCL_CHECK_FAIL);
	keeper_check_fail_policy = E(KEEPER_CHECK);
#if PRSIM_SETUP_HOLD
	timing_checker.reset();
#endif
#undef	E
	timing_mode = TIMING_DEFAULT;
	unwatch_all_nodes();
	clear_all_breakpoints();
	uniform_delay = time_traits::default_delay;
	_channel_manager.clobber_all();
	// reset seed
	util::numeric::seed48_zeros();
	// FIXME, ALERT: not every libc resets with the same 0-seed!!!
	// one option is to set upon State construction, but this
	// would only safely accomodate one state in any program...
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::set_mode_fatal(void) {
	unstable_policy = ERROR_FATAL;
	weak_unstable_policy = ERROR_FATAL;
#if PRSIM_MK_EXCL_BLOCKING_SET
	excl_unstable_policy = ERROR_FATAL;
#endif
	interference_policy = ERROR_FATAL;
	weak_interference_policy = ERROR_FATAL;
// additional fatalities:
	invariant_fail_policy = ERROR_FATAL;
	invariant_unknown_policy = ERROR_FATAL;
	assert_fail_policy = ERROR_FATAL;
	channel_expect_fail_policy = ERROR_FATAL;
	excl_check_fail_policy = ERROR_FATAL;
	if (expr_alloc_flags.fast_weak_keepers && weak_rules_enabled()) {
		keeper_check_fail_policy = ERROR_FATAL;
	} else {
	// otherwise there are no weak keepers to check
		cerr << "Warning: [mode fatal, keeper-check-fail]" << endl;
	if (!expr_alloc_flags.fast_weak_keepers) {
	cerr << "  simulator did not start with -f fast-weak-keepers" << endl;
	}
	if (!weak_rules_enabled()) {
		cerr << "  weak-rules are currently disabled." << endl;
	}
		cerr << "  keeper-check-fail policy unmodified" << endl;
	}
#if PRSIM_SETUP_HOLD
	timing_checker.set_mode_fatal();
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
	node_pool.resize(FIRST_VALID_GLOBAL_NODE);
	check_exhi_ring_pool.resize(FIRST_VALID_LOCK);
	check_exlo_ring_pool.resize(FIRST_VALID_LOCK);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !PRSIM_FAST_GET_NODE
/**
	Node accessor, const.
	Can remove bounds checks if we're really confident.
 */
const State::node_type&
State::get_node(const node_index_type i) const {
	ISE_INVARIANT(i);
	ISE_INVARIANT(i < node_pool.size());
	// the following line ICEs gcc-3.3.4 (suse-linux), WTF!?
	// internal compiler error: in build_base_path, at cp/class.c:283
	return node_pool[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Node accessor, mutable.  
	Can remove bounds checks if we're really confident.
 */
State::node_type&
State::__get_node(const node_index_type i) {
	ISE_INVARIANT(i);
	ISE_INVARIANT(i < node_pool.size());
	return node_pool[i];
}
#endif	// PRSIM_FAST_GET_NODE

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Traces backwards in event history until repeat (cycle) found.  
	\param v the node transition value to start query from.
 */
void
State::backtrace_node(ostream& o, const node_index_type ni, 
		const value_enum v) const {
	typedef	set<node_cause_type>		event_set_type;
	// start from the current value of the referenced node
	const node_type* n(&get_node(ni));
#if PRSIM_TRACK_CAUSE_TIME
	node_cause_type e(ni, v, current_time);	// or last_edge_time
	time_type min = e.time;
#elif PRSIM_TRACK_LAST_EDGE_TIME
	// last_edge_time may not be the time of the cause
	bool decr = true;
	time_type tt = n->get_last_edge_time(v);
	node_cause_type e(ni, v);
#else
	node_cause_type e(ni, v);
#endif
	// TODO: could look at critical event index if tracing...
	dump_node_canonical_name(o << "event    : `", ni) <<
		"\' : " << node_type::translate_value_to_char(v);
#if PRSIM_TRACK_LAST_EDGE_TIME
	if (tt >= 0.0) {
		o << " @ " << tt;
	}
#endif
	o << endl;
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
#if PRSIM_TRACK_LAST_EDGE_TIME
			const node_type& cn(get_node(e.node));
			const time_type ntt = cn.get_last_edge_time(e.val);
			const bool vt = (ntt >= 0.0);	// valid time
			// test for monotinic decrease in time, going backwards
			if (vt) {
				if (ntt > tt) { decr = false; }
				tt = ntt;
			} else {
				decr = false;
			}
#endif
			dump_node_canonical_name(o << "caused by: `", e.node)
				<< "\' : " <<
				node_type::translate_value_to_char(e.val);
#if PRSIM_TRACK_CAUSE_TIME
			if (e.time <= min) {
			// only show event times as they monotonically decrease
				o << " @ " << e.time;
				min = e.time;
			}
#elif PRSIM_TRACK_LAST_EDGE_TIME
			if (decr && vt) {
				o << " @ " << ntt;
			}
#endif
			o << endl;
			cyc = l.insert(e).second;
		} else {
			cyc = false;		// break
		}
	} while (cyc);
	if (e.node) {
		// seen same transition twice
		o << "(cycle reached)" << endl;
	} else {
		// cause not found (likely from environment or user)
		o << "(no cycle)" << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overload defaults to passing current value of node.  
 */
void
State::backtrace_node(ostream& o, const node_index_type ni) const {
	backtrace_node(o, ni, get_node(ni).current_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CACHE_GLOBAL_FOOTPRINT_FRAMES
footprint_frame_map_type
#else
const footprint_frame_map_type&
#endif
State::get_footprint_frame_map(const process_index_type pid) const {
	return GET_CONTEXT_CACHE
		get_global_context(pid).value.frame.get_frame_map<bool_tag>();
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
	STACKTRACE_VERBOSE;
	typedef	ring_set_type::const_iterator	const_iterator;
#if PRSIM_MK_EXCL_BLOCKING_SET
	const lock_index_type j = mk_exhi.size();
#endif
	const_iterator i(r.begin()), e(r.end());
	for ( ; i!=e; ++i) {
		const node_index_type ni = *i;
		node_type& n(__get_node(ni));
		if (n.is_atomic()) {
			// this check occurs too early, never caught
			// Q: could this be checked earlier, at create-time?
			cerr << "Error: Atomic nodes may not participate in mk_excl rings.  ("
				<< get_node_canonical_name(ni) << ')' << endl;
			THROW_EXIT;
		}
		n.make_exclhi();
#if PRSIM_MK_EXCL_BLOCKING_SET
		mk_exhi_counter_map[ni].push_back(j);
#endif
	}
#if PRSIM_MK_EXCL_BLOCKING_SET
	mk_exhi.push_back(ring_counter_state());
	mk_exhi.back().elems.swap(r);
#else
	mk_exhi.push_back(ring_set_type());
	mk_exhi.back().swap(r);
#endif
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
	STACKTRACE_VERBOSE;
	typedef	ring_set_type::const_iterator	const_iterator;
#if PRSIM_MK_EXCL_BLOCKING_SET
	const lock_index_type j = mk_exlo.size();
#endif
	const_iterator i(r.begin()), e(r.end());
	for ( ; i!=e; ++i) {
		const node_index_type ni = *i;
		node_type& n(__get_node(ni));
		if (n.is_atomic()) {
			// this check occurs too early, never caught
			// Q: could this be checked earlier, at create-time?
			cerr << "Error: Atomic nodes may not participate in mk_excl rings.  ("
				<< get_node_canonical_name(ni) << ')' << endl;
			THROW_EXIT;
		}
		n.make_excllo();
#if PRSIM_MK_EXCL_BLOCKING_SET
		mk_exlo_counter_map[ni].push_back(j);
#endif
	}
#if PRSIM_MK_EXCL_BLOCKING_SET
	mk_exlo.push_back(ring_counter_state());
	mk_exlo.back().elems.swap(r);
#else
	mk_exlo.push_back(ring_set_type());
	mk_exlo.back().swap(r);
#endif
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
		__get_node(ni).check_exclhi();
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
		__get_node(ni).check_excllo();
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
	\param force to indicate that event was caused by user intervention
	\pre n must not already have a pending event.
	\pre n must be the node corresponding to node index ni
	\return index into event pool for newly created event.
 */
// inline
event_index_type
State::__allocate_event(node_type& n,
		const node_index_type ni,
		cause_arg_type c, 
		const rule_index_type ri,
#if EVENT_INCLUDE_RULE_POINTER
		const rule_type* r,
#endif
		const value_enum val,
#if PRSIM_WEAK_RULES
		const bool weak,
#endif
		const bool force
		) {
	STACKTRACE_VERBOSE;
	ISE_INVARIANT(!n.pending_event());
	event_type e(ni, c, ri, 
#if EVENT_INCLUDE_RULE_POINTER
		r,
#endif
		val
#if PRSIM_WEAK_RULES
		, weak
#endif
		);
	if (force) e.force();
#if DEBUG_STEP
	e.dump_raw(STACKTRACE_INDENT_PRINT("")) << endl;
#endif
#if 0
	ISE_INVARIANT(!n.is_frozen() || force);
	n.set_event(event_pool.allocate(e));
	// n.set_cause_node(ci);	// now assign *after* dequeue_event
	return n.get_event();
#else
	return __allocate_event(n, e);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	With this overload, the event type has already been constructed
	and just needs to be copied.  
 */
inline
event_index_type
State::__allocate_event(node_type& n, const event_type& e) {
	STACKTRACE_VERBOSE;
	ISE_INVARIANT(!n.pending_event());
	ISE_INVARIANT(!n.is_frozen() || e.forced());
	n.set_event(event_pool.allocate(e));
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
		event_type(ni, c, INVALID_RULE_INDEX,
#if EVENT_INCLUDE_RULE_POINTER
		NULL,
#endif
		next
#if PRSIM_WEAK_RULES
		, weak
#endif
		));
	event_type& e(get_event(ne));
	e.pending_interference(true);
	// not yet because hasn't been committed to event queue yet
	// n.set_event(ne);		// for consistency?
	// n.set_cause_node(ci);	// now assign *after* dequeue_event
#if DEBUG_STEP
	e.dump_raw(STACKTRACE_INDENT_PRINT("")) << endl;
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
	node_type& n(__get_node(ev.node));
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
	\return true if there are any liev (not-killed) events in the queue.
 */
bool
State::pending_live_events(void) const {
	if (event_queue.empty()) {
		return false;
	}
	event_queue_type::const_iterator
		i(event_queue.begin()), e(event_queue.end());
	for ( ; i!=e; ++i) {
		if (!event_pool[i->event_index].killed())
			return true;
	}
	return false;
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
	DEBUG_STEP_PRINT("dequeuing event ID " << ei << " on node " << ni << endl);
	if (UNLIKELY(watching_all_event_queue() ||
			(watching_event_queue() && is_watching_node(ni)))) {
		dump_event_force(cout << "killed  :", ei,
			delay_policy<time_type>::zero, true);
		// flush?
	}
	get_event(ei).kill();
	__get_node(ni).clear_event();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers event in the primary event queue.  
	Only this is allowed to load killed events.  
	\return index of the node whose event was just enqueued
 */
// inline
node_index_type
State::load_enqueue_event(const time_type t, const event_index_type ei) {
	INVARIANT(ei);
	const event_type& e(get_event(ei));
	const node_index_type ni = e.node;
	// keep this on for now b/c VPI co-sim seems to trip this a lot
	if (UNLIKELY(!(t >= current_time))) {
		dump_node_canonical_name(cerr <<
		"FATAL: attempt to schedule event in the past on node: ", ni)
			<< endl;
		format_ostream_ref(cerr << "\tnew: ", time_fmt)
			<< t << " vs. now: ";
		format_ostream_ref(cerr, time_fmt) << current_time << endl;
		dump_event(cout << "event:", ei, t);
		// just to keep event_queue consistent for termination
		event_queue.push(event_placeholder_type(t, ei));
		ISE_INVARIANT(t >= current_time);	// noreturn
	}
	DEBUG_STEP_PRINT("enqueuing event ID " << ei <<
		" on node " << ni <<
		" at time " << t << endl);
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
//	DEBUG_STEP_PRINT("enqueuing event ID " << ei << " at time " << t << endl);
	const node_index_type ni = load_enqueue_event(t, ei);
	const node_type& n(get_node(ni));
	ISE_INVARIANT(n.pending_event());
	ISE_INVARIANT(n.get_event() == ei);
	// atomic node updates never go through event queue
	ISE_INVARIANT(!n.is_atomic());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	User action to explicitly kill an event from the queue, 
	referenced by node.
 */
bool
State::deschedule_event(const node_index_type ni) {
	node_type& n(__get_node(ni));
	const event_index_type ei = n.get_event();
	if (ei) {
		kill_event(ei, ni);
		return false;
	} else {
		dump_node_canonical_name(
			cerr << "There is no pending event on node `", 
			ni) << "\'" << endl;
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Move event to front of queue.
	Tricky: breaking ties at front of queue!
 */
bool
State::reschedule_event_now(const node_index_type ni) {
	node_type& n(__get_node(ni));
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
		dump_node_canonical_name(
			cerr << "Error: there is no pending event on node `", 
			ni) << "\'" << endl;
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
	node_type& n(__get_node(ni));
	const event_index_type ei = n.get_event();
	if (ei) {
		const event_index_type ne = __copy_event(get_event(ei));
		kill_event(ei, ni);
		n.set_event(ne);
		enqueue_event(t, ne);
		return false;
	} else {
		dump_node_canonical_name(
			cerr << "Error: there is no pending event on node `",
			ni) << "\'" << endl;
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param dt is relative time to previously scheduled event.  
 */
bool
State::reschedule_event_relative(const node_index_type ni, const time_type dt) {
	node_type& n(__get_node(ni));
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
		dump_node_canonical_name(
			cerr << "Error: there is no pending event on node `",
			ni) << "\'" << endl;
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
#if !PRSIM_MK_EXCL_BLOCKING_SET
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
	__get_node(get_event(ei).node).set_excl_queue();
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
	__get_node(get_event(ei).node).set_excl_queue();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Flush killed events that are at the head of the event queue.
 */
void
State::flush_killed_events(void) {
	STACKTRACE_VERBOSE_STEP;
	while (!event_queue.empty() &&
			get_event(peek_next_event().event_index).killed()) {
		__deallocate_killed_event(event_queue.pop().event_index);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Fetches next event from the priority queue.  
	Automatically skips and deallocates killed events.  
	NOTE: possible that last event in queue is killed, 
		in which case, need to return a NULL placeholder.  
	\pre event queue is not empty
	\return next event (placeholder) if there is one, else a NULL event.
 */
State::event_placeholder_type
State::dequeue_event(void) {
	STACKTRACE_VERBOSE_STEP;
	flush_killed_events();
	if (event_queue.empty()) {
		return event_placeholder_type(
			current_time, INVALID_EVENT_INDEX);
	} else {
#if PRSIM_TIMING_BACKANNOTATE
		const event_placeholder_type& ep(peek_next_event());
		const event_type& ne(get_event(ep.event_index));
		if (ne.val != LOGIC_OTHER) {
		// do not apply min-delays to X transitions (conservative)
		const applied_min_delay_constraint
			c(node_event_min_delay(ne.node, ne.val));
		if (c.delayed && (c.time > ep.time)) {
			INVARIANT(c.time > current_time);
			// then need to reschedule this next event for later
			const bool err = reschedule_event(ne.node, c.time);
			INVARIANT(!err);
			if (verbose_min_delays()) {
				cout << "note: event on ";
				dump_node_canonical_name(cout, ne.node);
				cout << (ne.val == LOGIC_HIGH ? '+' : '-') <<
					" was delayed from " <<
					ep.time << " until " <<
					c.time << " by ";
				dump_node_canonical_name(cout, c.ref);
				get_node(c.ref).dump_value(cout << ':') << endl;
			}
			current_time = ep.time;	// advance
#if 0
			// update critical event?
			// problem: don't have trace index of the last
			// event on the c.ref node
			const node_type& r(get_node(c.ref));
			ne.cause = event_type::cause_type(
				c.ref, r.current_value(),
				r.get_last_transition_time());
				// critical_trace_event index?
#endif
			// or is it better to return NULL event? like killed event?
			return dequeue_event();	// tail recursion, FIXME: rewrite
		}
		}
#endif
		return event_queue.pop();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre There must be at least one event in queue before this is called.  
	\return the scheduled time of the next event.  
	This should return the next *non-killed* event time.  
 */
State::time_type
State::next_event_time(void) const {
#if 0
	// includes killed events, which we don't want
	return event_queue.top().time;
#else
	// fixed.
	event_queue_type::const_iterator
		i(event_queue.begin()), e(event_queue.end());
	for ( ; i!=e; ++i) {
		if (!get_event(i->event_index).killed())
			return i->time;
	}
	// else no-live events left, issue warning?
	return delay_policy<time_type>::max();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allows '~' to be interpreted as "opposite of current value".
	Allows '?' to be interpreted as 0 or 1 (random).
 */
value_enum
State::node_to_value(const string& v, const node_index_type ni) const {
	const rand48<long> r;
	return (v == "~") ?
		node_type::invert_value[get_node(ni).current_value()] :
		node_type::string_to_value(v);
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
		" to " << node_type::translate_value_to_char(val) <<
		" at " << t << endl);
	// we have ni = the canonically allocated index of the bool node
	// just look it up in the node_pool
	node_type& n(__get_node(ni));
	if (n.is_atomic()) {
		const string objname(get_node_canonical_name(ni));
		cout << "Warning: ignoring attempt to set an atomic node `"
			<< objname << "\'." << endl;
		return ENQUEUE_REJECT;
	}
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
	const event_type& pe(get_event(pending));
	const value_enum pval = pe.val;
	const char pc = node_type::translate_value_to_char(pval);
	const char nc = node_type::translate_value_to_char(val);
	if (f) {
		// doesn't matter what what last_val was, override it
		// even if value is the same, reschedule it
		// cancel former event, but don't deallocate it until dequeued
		cout << "WARNING: pending event for node `" << objname <<
			"\' -> " << pc << " was overridden to " << nc <<
			'.' << endl;
		kill_event(pending, ni);
	} else if (!unchanged) {
		// not forcing: if new value is different, issue warning
		cout << "WARNING: pending value for node `" << objname <<
			"\' is already " << pc <<
			"; ignoring new request" << endl;
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
if (!n.is_frozen() || f) {
	const event_index_type ei =
		// node cause to assign, since this is externally set
		__allocate_event(n, ni, 
			EMPTY_CAUSE,
			INVALID_RULE_INDEX,
#if EVENT_INCLUDE_RULE_POINTER
			NULL,
#endif
			val,
#if PRSIM_WEAK_RULES
			NORMAL_RULE,	// normal strength of 'set'
#endif
			f);	// forced events can only come from user
#if 0
	const event_type& e(get_event(ei));
	STACKTRACE_INDENT_PRINT("new event: (node,val)" << endl);
#endif
#if 0
	if (f) {	// what if nothing was pending?
		// mark event as forced
		get_event(ei).force();
	}
#endif
	enqueue_event(t, ei);
	return ENQUEUE_ACCEPT;
} else {
	// is frozen and not forced
	const string objname(get_node_canonical_name(ni));
	cout << "WARNING: Attempt to set frozen node `" << objname <<
		"\' ignored.\n"
		"To override, either use setf or thaw the node first."
		<< endl;
	return ENQUEUE_WARNING;
}
}	// end State::set_node_time

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Without going through the event queue, set the value of the node now.
	Cause will be null, b/c comes from user.
	Note: this can now be called for atomic node updates.
	FIXME: needs to be re-entrant!
 */
State::step_return_type
State::set_node_immediately(const node_index_type ni, const value_enum val,
		const bool force) {
	STACKTRACE_BRIEF;
	event_type e;	// default construct
	e.node = ni;
	e.val = val;
	if (force) e.force();
	ISE_INVARIANT(!get_node(ni).get_event());
	return execute_immediately(e, current_time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_UPSET_NODES
/**
	Flags a node as frozen, preventing further activity.
	Already pending events will stay in queue, and eventually fire.
 */
void
State::freeze_node(const node_index_type ni) {
	// Q: should nodes driven by channels be allowed to freeze?
	node_type& n(__get_node(ni));
	if (!n.is_atomic()) {
		n.freeze();
	} else {
		cerr << "Warning: atomic nodes cannot be frozen.  Ignoring."
			<< endl;
	}
}
#endif

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
	node_type& n(__get_node(ni));
	DEBUG_STEP_PRINT("node: " << get_node_canonical_name(ni) << endl);
#if PRSIM_UPSET_NODES
	n.restore();	// unstuck the node
	INVARIANT(!n.is_frozen());
#endif
	const event_index_type pending = n.get_event();
	// strong events take precedence over weak ones
	// evaluate node's pull-up and pull-down
	const pull_set p(n, weak_rules_enabled());
if (!p.state_holding()) {
	// to Hell with #if PRSIM_WEAK_RULES...
	const bool weak_wins = p.weak_wins_any();
#if 0
	const value_enum new_val = pull_to_value[size_t(pu)][size_t(pd)];
#else
	const value_enum new_val =
		p.pull_up_wins_any() ? LOGIC_HIGH :
		p.pull_dn_wins_any() ? LOGIC_LOW : LOGIC_OTHER;
		// is not state-holding, must be some interference
#endif
	if (pending) {
		DEBUG_STEP_PRINT("already pending event" << endl);
		event_type& e(get_event(pending));
		if (e.val != new_val) {
			dump_node_canonical_name(
			cerr << "Overriding pending event\'s value on node `",
				ni) << "\' from " <<
				node_type::translate_value_to_char(e.val) << " to " <<
				node_type::translate_value_to_char(new_val) <<
				", keeping the same event time." << endl;
			e.val = new_val;
		}
#if PRSIM_WEAK_RULES
		e.set_weak(weak_wins);
#endif
		e.unforce();
		// else do nothing, correct value already pending
	} else {
		DEBUG_STEP_PRINT("no pending event" << endl);
		// no event pending, can make one
		const value_enum current = n.current_value();
		if (current == LOGIC_LOW &&
				p.up == PULL_OFF) {
			// nothing pulling up, no change
			return;
		} else if (current == LOGIC_HIGH &&
				p.dn == PULL_OFF) {
			// nothing pulling down, no change
			return;
		} else if (current != new_val) {
		// TODO: find minimum delay of ON fanin rules (*slow*)
		// there must be at least one rule ON to pull
		// get_delay will fail because INVALID_RULE_INDEX
			const rule_index_type ri = INVALID_RULE_INDEX;
			// a real rule index would help determine the delay
			// but we don't know which delay in a multi-delay
			// fanin to use!  Passing INVALID_RULE_INDEX
			// will use a delay of 0.  
			const event_index_type ei = __allocate_event(
				n, ni, EMPTY_CAUSE, ri, 
#if EVENT_INCLUDE_RULE_POINTER
				NULL,
#endif
				new_val
#if PRSIM_WEAK_RULES
				, weak_wins
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
}	// else neither side is pulling, leave node as is
}	// end State::unset_node

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Applies unset_node to all nodes.  
 */
void
State::unset_all_nodes(void) {
	node_index_type ni = FIRST_VALID_GLOBAL_NODE;
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
	__get_node(ni).set_breakpoint();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If this isn't self-documenting enough, I don't know what is.
 */
void
State::clear_node_breakpoint(const node_index_type ni) {
	__get_node(ni).clear_breakpoint();
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
		if (i->is_breakpoint()) {
			dump_node_canonical_name(o, ni) << ' ';
		}
	}	// end if is_breakpoint
	}	// end for-all nodes
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print the current mode: mostly checking and error-handling policies.  
 */
ostream&
State::dump_mode(ostream& o) const {
	o << "mode: " << endl;
	o << "\tweak rules: " << (weak_rules_enabled() ? "on" : "off") << endl;
	o << "\tunstable events " <<
		(dequeue_unstable_events() ? "are dequeued" : "propagate Xs")
			<< endl;
#if PRSIM_MK_EXCL_BLOCKING_SET
	o << "\tforce-excl caused unstable events " <<
		(dequeue_excl_unstable_events() ?
			"are dequeued" : "propagate Xs") << endl;
#endif
	o << "\ton unstable: " <<
		error_policy_string(unstable_policy) << endl;
	o << "\ton weak-unstable: " <<
		error_policy_string(weak_unstable_policy) << endl;
#if PRSIM_MK_EXCL_BLOCKING_SET
	o << "\ton excl-unstable: " <<
		error_policy_string(excl_unstable_policy) << endl;
#endif
	o << "\ton interference: " <<
		error_policy_string(interference_policy) << endl;
	o << "\ton weak-interference: " <<
		error_policy_string(weak_interference_policy) << endl;
	o << "\tchecking exclusions: " <<
		(checking_excl() ? "on" : "off") << endl;
	o << "\ton exclusion-fail: " <<
		error_policy_string(excl_check_fail_policy) << endl;
	o << "\ton invariant-fail: " <<
		error_policy_string(invariant_fail_policy) << endl;
	o << "\ton invariant-unknown: " <<
		error_policy_string(invariant_unknown_policy) << endl;
	o << "\ton assert-fail: " <<
		error_policy_string(assert_fail_policy) << endl;
	o << "\ton channel-expect-fail: " <<
		error_policy_string(channel_expect_fail_policy) << endl;
	o << "\ton keeper-check-fail: " <<
		error_policy_string(keeper_check_fail_policy) << endl;
#if PRSIM_SETUP_HOLD
	o << "\ton setup-timing-violation: " <<
		error_policy_string(timing_checker.setup_violation_policy) << endl;
	o << "\ton hold-timing-violation: " <<
		error_policy_string(timing_checker.hold_violation_policy) << endl;
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Show current timing mode.
	Show random seed?
	TODO: use time_fmt
 */
ostream&
State::dump_timing(ostream& o) const {
	o << "timing: ";
switch (timing_mode) {
	case TIMING_RANDOM:	o << "random [" << default_after_min << ",";
		if (time_traits::is_zero(default_after_max))
			o << "+INF";
		else	o << default_after_max;
		o << "]";
		break;
	case TIMING_UNIFORM:
		o << "uniform (" << uniform_delay << ")";
		break;
	case TIMING_AFTER:	o << "after";	break;
	case TIMING_BINARY:	o << "binary [" << default_after_min << ','
			<< default_after_max << "] ("
			<< timing_probability << ')';
		break;
	case TIMING_BOUNDED:	o << "bounded, default:["
			<< default_after_min << ','
			<< default_after_max << "] ("
			<< timing_probability << ')';
		break;
	default:		o << "unknown";
}
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parses delay-pair parameter of the form: [MIN]:[MAX]
	\return true on syntax error.
 */
bool
State::parse_min_max_delay(const string& d,
		time_type& min_delay, time_type& max_delay) {
	time_type new_min = min_delay;
	time_type new_max = max_delay;
	string_list mm;
	tokenize_char(d, mm, ':');
	// there should be one ':'
	// substrings before and after may be empty (set to 0)
	if (mm.size() != 2)
		return true;
	if (mm.front().length()) {
		if (string_to_num(mm.front(), new_min))
			return true;
		else if (new_min < time_traits::zero) {
			cerr << "min delay must be >= 0" << endl;
			return true;
		}
	} else {
		new_min = time_traits::zero;
	}
	if (mm.back().length()) {
		if (string_to_num(mm.back(), new_max))
			return true;
		else if (new_max < time_traits::zero) {
			cerr << "max delay must be >= 0" << endl;
			return true;
		}
	} else {
		new_max = time_traits::zero;
	}
	if (time_traits::is_zero(new_max) || 
			new_max > new_min) {
		min_delay = new_min;
		max_delay = new_max;
		return false;
	} else {
		cerr << "if (max>0) min <= max" << endl;
		// leave min_delay and max_delay unchanged
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if there is a syntax error.  
	TODO: use a map to parsers.  
	TODO: use time_fmt
 */
bool
State::set_timing(const string& m, const string_list& a) {
	static const string __random("random");
	static const string __uniform("uniform");
	static const string __after("after");
	static const string __binary("binary");
	static const string __bounded__("bounded");
	if (m == __random) {
		timing_mode = TIMING_RANDOM;
		switch (a.size()) {
		case 0:
			cout << "default after min: " << default_after_min
				<< endl;
			cout << "default after max: ";
			if (time_traits::is_zero(default_after_max))
				cout << "+INF" << endl;
			else	cout << default_after_max << endl;
			return false;
		case 1: {
			return parse_min_max_delay(a.back(),
				default_after_min, default_after_max);
		}
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
	} else if (m == __binary || m == __bounded__) {
		// syntax: [min]:[max] prob
		timing_mode = (m == __binary) ? TIMING_BINARY : TIMING_BOUNDED;
		if (a.size() == 2) {
			if (parse_min_max_delay(a.front(),
				default_after_min, default_after_max)) {
				cerr << "Error parsing min:max." << endl;
					return true;
			}
			if (string_to_num(a.back(), timing_probability)) {
				cerr << "Error lexing probability." << endl;
				return true;
			}
			return false;
		} else {
			cerr << "Expecting min:max prob arguments." << endl;
		}
	}
	// else
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::help_timing(ostream& o) {
	o << "available timing modes:" << endl;
	o << "\trandom [[min]:[max]]" << endl;
	o << "\tbinary [min]:[max] prob" << endl;
	o << "\tbounded [min]:[max] prob" << endl;
	o << "\tuniform [delay]" << endl;
	o << "\tafter" << endl;
	o <<
"Random mode uses a heavy-tailed distribution random-variable for delay, "
"*except* where a delay is marked with [after=0].\n"
"Default min and max delay bounds can be set with an additional MIN:MAX\n"
"argument.  A blank value will clear the bound to 0.\n"
"A max value of 0 is interpreted as being unbounded.\n"
"Uniform mode ignores all after-delay annotations and uses a fixed delay "
"for all events, which can be used to count transitions.\n"
"After mode uses fixed after-annotated delays for timing, and assumes\n"
"default delays where none are given.\n"
"Binary mode chooses between the min and max values only with the given\n"
"probability p of choosing min, and also *ignores* all specified rule delays,\n"
"including after=0 delays.\n"
"Bounded mode selects the min or max delay value with the given probability,\n"
"whose delay values are *overridden* by after_min and after_max attributes.\n"
"Use \'seed48\' to set a random number seed." << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
State::time_type
State::exponential_random_delay(void) {
	typedef	random_time<random_time_limit<time_type>::type>
				random_generator_type;
	return random_generator_type()();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\returns a value [0,1)
 */
State::time_type
State::uniform_random_delay(void) {
	typedef	random_time<random_time_limit<time_type>::type>
				random_generator_type;
	return rand48<time_type>()();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return override value o if it is not zero, else return default def.
 */
inline
static
const State::time_type&
__get_delay(const State::rule_type* r, State::time_type State::rule_type::* m,
		const State::time_type& def) {
	return (r && !State::time_traits::is_zero(r->*m)) ? r->*m : def;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return absolute time of scheduled pull-up event.
	NOTE: possible reasons for null e.cause_rule:
		due to exclhi/exclo ring enforcement?
	NOTE: event's cause_rule is not checkpointed.  
	TODO: if rule not found, infer delay from fanin-get state...
 */
// inline
State::time_type
State::get_delay_up(const event_type& e) const {
#if EVENT_INCLUDE_RULE_POINTER
	const rule_type* r = e.cause_rule_ptr;
#else
	const rule_type* r = NULL;
if (e.cause_rule) {
	r = lookup_rule(e.cause_rule);
	NEVER_NULL(r);
}
#endif
	time_type delta;
	if (timing_mode == TIMING_UNIFORM) {
		delta = uniform_delay;
	} else if (timing_mode == TIMING_RANDOM ||
			(r && r->is_always_random())) {
		const bool after_zero = r && time_traits::is_zero(r->after);
		// after=0 always takes precedence in random mode
		if (after_zero) {
			delta = time_traits::zero;
		} else {
#if PRSIM_AFTER_RANGE
		const time_type min_val =
			__get_delay(r, &rule_type::after_min, default_after_min);
		const time_type max_val =
			__get_delay(r, &rule_type::after_max, default_after_max);
		const bool have_min = r && !time_traits::is_zero(min_val);
		const bool have_max = r && !time_traits::is_zero(max_val);
		if (have_max) {
			if (have_min) {
				delta = min_val +
					(max_val -min_val)
					* uniform_random_delay();
			} else {
				delta = max_val * uniform_random_delay();
			}
		} else {
#endif
			delta = ((0x01 << 11) * exponential_random_delay());
#if PRSIM_AFTER_RANGE
			if (have_min) delta += min_val;
		}
#endif
		}
	} else if (timing_mode == TIMING_BINARY) {
		// timing binary does NOT honor after=0
		// b/c want to verify race ratios
		delta = (uniform_random_delay() < timing_probability)
			? default_after_min : default_after_max;
	} else if (timing_mode == TIMING_BOUNDED) {
#if PRSIM_AFTER_RANGE
		const time_type min_val =
			__get_delay(r, &rule_type::after_min, default_after_min);
		const time_type max_val =
			__get_delay(r, &rule_type::after_max, default_after_max);
		const bool have_min = r && !time_traits::is_zero(min_val);
		const bool have_max = r && !time_traits::is_zero(max_val);
		INVARIANT(have_min && have_max);
		INVARIANT(max_val >= min_val);
		delta = (uniform_random_delay() < timing_probability)
			? min_val : max_val;
#endif
	} else {
	// timing_mode == TIMING_AFTER
		delta = r ? r->after : time_traits::zero;
	}
	return current_time +delta;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: event's cause_rule, cause_rule_ptr is not checkpointed.  
	\return absolute time of scheduled pull-down event.
 */
// inline
State::time_type
State::get_delay_dn(const event_type& e) const {
	return get_delay_up(e);		// is identical, actually
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_TIMING_BACKANNOTATE
/**
	Wipes entire state of delay-back annotation.
 */
void
State::reset_min_delays(void) {
	// reset all back-annotation bits on nodes
	for_each(node_pool.begin(), node_pool.end(),
		std::mem_fun_ref(&node_type::reset_min_delay_target));
	// reset timing-fanin on all nodes
	delay_annotation_manager.reset_timing_fanin();
	// reset timing-constraints on all unique_process_graphs
	for_each(unique_process_pool.begin(), unique_process_pool.end(),
		std::mem_fun_ref(
			&unique_process_subgraph::reset_delay_constraints));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Traverse every process instance, and apply per-type timing constraints,
	flagging target nodes as having annotated delays applied and 
	noting from which processes those delays have been applied 
	(timing fanin).
 */
void
State::apply_all_min_delays(void) {
	process_index_type pid = 0;	// start at top-level pid, 0
	for ( ; pid < process_state_array.size(); ++pid) {
		const process_sim_state& s(process_state_array[pid]);
		const unique_process_subgraph& g(s.type());
		const footprint_frame_map_type&
			bfm(get_footprint_frame_map(pid));
		const unique_process_subgraph::min_delay_set_type&
			md(g.min_delays);
		unique_process_subgraph::min_delay_set_type::const_iterator
			mdi(md.begin()), mde(md.end());
		for ( ; mdi != mde; ++mdi) {
			const node_index_type lt = mdi->first;
			INVARIANT(lt);
			const node_index_type gt = bfm[lt -1];
			__get_node(gt).flag_min_delay_target();
// don't actually need to iterate through individual constraints
// just need to flag node and note process timing fanin in database
			delay_annotation_manager.add_timing_fanin(gt, pid);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
State::list_min_delays_type(ostream& o, const string& s) const {
	const unique_process_subgraph* g = lookup_unique_process_graph(s);
	if (g) {
		g->dump_backannotated_delays(o);
		return false;
	} else {
		o << "Error parsing type: " << s << endl;
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print delays per unique type.
 */
void
State::list_all_min_delays(ostream& o) const {
	vector<unique_process_subgraph>::const_iterator
		i(unique_process_pool.begin()),
		e(unique_process_pool.end());
	for ( ; i!=e; ++i) {
		i->dump_backannotated_delays(o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::min_delay_fanin(ostream& o, const node_index_type ni) const {
	o << "Timing arcs whose head is ";
	dump_node_canonical_name(o, ni) << ": " << endl;
	const process_timing_fanin_type* ptf =
		delay_annotation_manager.lookup_process_timing_fanin(ni);
if (ptf) {
	process_timing_fanin_type::const_iterator
		i(ptf->begin()), e(ptf->end());
	for ( ; i!=e; ++i) {
		const process_index_type pid = *i;
		// for each process:
		// find all the local nodes that map to
		// the global node ni as a target.
		// Possible that more than one local node
		// maps to the same global node, due to 
		// external connections.
		// This requires an O(n) search.
		const process_sim_state& s(process_state_array[pid]);
		const unique_process_subgraph& g(s.type());
		const footprint_frame_map_type&
			bfm(get_footprint_frame_map(pid));
		vector<node_index_type> m;
		// m.reserve(8);
		// find all local offsets that map to the global
		size_t j = 0;
		for ( ; j<bfm.size(); ++j) {
		if (bfm[j] == ni) {
			m.push_back(j+1);
		}
		}
		// m is sorted
		g.dump_backannotated_delays_targeting(o, m);
	}
}
	// else no fanin found
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	DEBUG_MIN_DELAY			0
/**
	Determine whether or not event on node ni should be further 
	delayed by any of the min-delay constraints.
	\param ni node that is about to switch.
	\param nval the next value for the transitioning node.
 */
State::applied_min_delay_constraint
State::node_event_min_delay(const node_index_type ni, 
		const value_enum nval) const {
	STACKTRACE_BRIEF;
	applied_min_delay_constraint ret(current_time);
	const node_type& n(get_node(ni));
// ignore min delay constraints when target node is -> X
if (n.is_min_delay_target() && nval != LOGIC_OTHER) {
	const bool tgt_dir = (nval == LOGIC_HIGH);
#if DEBUG_MIN_DELAY
	dump_node_canonical_name(cerr << "target: ", ni)
		<< (tgt_dir ? '+' : '-') << endl;
#endif
	const process_timing_fanin_type* ptf =
		delay_annotation_manager.lookup_process_timing_fanin(ni);
	NEVER_NULL(ptf);
	process_timing_fanin_type::const_iterator
		i(ptf->begin()), e(ptf->end());
for ( ; i!=e; ++i) {
	const process_index_type pid = *i;
	const process_sim_state& s(process_state_array[pid]);
	const unique_process_subgraph& g(s.type());
	const footprint_frame_map_type& bfm(get_footprint_frame_map(pid));
	typedef	unique_process_subgraph::min_delay_set_type
				min_delay_set_type;
	typedef	min_delay_set_type::mapped_type
				delay_fanin_set_type;
	// indentify process-local aliases to the target node
	// TODO: speedup lookup by caching global fanins
	// otherwise this scan can be really slow
	size_t j = 0;	// local index
	for ( ; j<bfm.size(); ++j) {
	if (bfm[j] == ni) {
		const min_delay_set_type::const_iterator
			f(g.min_delays.find(j+1));
		INVARIANT(f != g.min_delays.end());
		// by construction
		delay_fanin_set_type::const_iterator
			di(f->second.begin()), de(f->second.end());
		for ( ; di!=de; ++di) {
			const node_index_type ref_node = di->first;
			const min_delay_entry& md(di->second);
#if DEBUG_MIN_DELAY
			md.dump_raw(cerr << "md: ") << endl;
#endif
			const node_index_type grr = bfm[ref_node -1];
			const node_type& r(get_node(grr));
			bool ref_dir = false;
			const value_enum rv = r.current_value();
			switch (rv) {
			case LOGIC_LOW: ref_dir = false; break;
			case LOGIC_HIGH: ref_dir = true; break;
			// ignore min-delay constraint when ref node is X
			default: continue;
			}
#if DEBUG_MIN_DELAY
			dump_node_canonical_name(cerr << "reference: ", grr)
				<< (ref_dir ? '+' : '-') << endl;
#endif
			// suppress if predicate is false
			bool apply = true;
			const node_index_type pni = md.predicate[ref_dir][tgt_dir];
			if (pni) {
				const node_index_type gpr = bfm[pni -1];
				if (get_node(gpr).current_value() == LOGIC_LOW) {
					apply = false;
#if DEBUG_MIN_DELAY
					cerr << "predicate false." << endl;
#endif
				}
			}
			if (apply) {
				// right now, don't care about value
				const time_type past = r.get_last_edge_time(rv);
				const time_type min_time = past +md.time[ref_dir][tgt_dir];
#if DEBUG_MIN_DELAY
				cerr << "comparing times... past = " << past <<
					", min_time = " << min_time << endl;
#endif
				// take argmax(t_ref +t_min_delay)
				// ignore uninitialized transition times < 0
				if (past >= 0.0 && min_time > ret.time) {
					ret.set(grr, min_time);
				}
			}
		}
	}
	}	// end for
}	// end for each process-timing-fanin
}	// end if
	return ret;
}

#endif	// PRSIM_TIMING_BACKANNOTATE

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This makes sure that next event does NOT fall behind specified time.
 */
void
State::safe_fast_forward(const time_type& t) {
	ISE_INVARIANT(t >= current_time);
	if (t > current_time) {
		if (pending_events()) {
			const time_type next = next_event_time();
			if (t > next) {
				cerr << "attempt to fast-forward to " << t
					<< ", but next event at " << next
					<< endl;
				dump_event_queue(cerr);
				ISE_INVARIANT(t <= next);
			}
		}
		update_time(t);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if expression is a root-level expression
	(and hence, has attributes)
 */
bool
State::is_rule_expr(const expr_index_type ei) const {
	return lookup_rule(ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenience function, instead of calling rule_map.find() directly.
	\param ei global expression index corresponding to rule.  
	\return pointer to rule struct, non-modifiable.
 */
const State::rule_type*
State::lookup_rule(const expr_index_type ei) const {
	STACKTRACE_VERBOSE_LOOKUP;
	DEBUG_LOOKUP_PRINT("global-rule: " << ei << endl);
if (ei) {
	// translate global expression index to 
	// global process index with local expression index,
	const process_sim_state& s(lookup_global_expr_process(ei));
	return s.lookup_rule(ei);
}
	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenience routine for flush_updated_nodes() enqueuing of new events.
 */
void
State::enqueue_new_event(const event_type& newevent) {
	STACKTRACE_VERBOSE;
	const value_enum& pull_val(newevent.val);
	node_type& n(__get_node(newevent.node));
	const event_index_type ei = __allocate_event(n, newevent);
	enqueue_event(pull_val == LOGIC_LOW ?
		get_delay_dn(newevent) : get_delay_up(newevent),
		ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_MK_EXCL_BLOCKING_SET
/**
	Takes all node in the force-excl blocked set and 
	overwrites their pull-state to reflect a virtual exclusive cut-off.
	This should be called after all fanouts have been evaluated, 
	no more additions to updated_nodes, and before flush_updated_nodes().
 */
void
State::flush_blocked_excl_nodes(const value_enum val) {
	STACKTRACE_VERBOSE_STEP;
	set<node_index_type>::const_iterator
		i(__mk_excl_blocking_set.begin()),
		e(__mk_excl_blocking_set.end());
for ( ; i!=e; ++i) {
	const node_index_type& ni(*i);
	updated_nodes_type::iterator f(updated_nodes.find(ni));
	if (f != updated_nodes.end()) {
		DEBUG_STEP_PRINT("flagging blocked node " << ni << endl);
		f->second.excl_blocked = true;
	} else {
		// if event is pending, force a re-evaluation that will
		// result in cancelling it (instability)
		const node_type& n(get_node(ni));
		const event_index_type ei(n.get_event());
		if (ei) {
			DEBUG_STEP_PRINT("blocking pending event on node "
				<< ni << endl);
			if (get_event(ei).val == val) {
				node_update_info nui;
				nui.excl_blocked = true;
				updated_nodes.insert(make_pair(ni, nui));
				// assert: insertion successful, was not found
#if PRSIM_FCFS_UPDATED_NODES
				updated_nodes_queue.push_back(ni);
#endif
			}
			// TODO: minor opt: use insert instead of find above
			// becomes a single log-time access
		}
	}
}
	__mk_excl_blocking_set.clear();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	updated_nodes contains set of all nodes whose pull-states were
	updated by expression propagation.
	updated_nodes_queue preserves order of fanout evaluation
	This procedure evaluates the pull-state of all affected nodes
	and determines a course of action, possibly enqueuing or
	dequeuing events.  
 */
State::break_type
State::flush_updated_nodes(cause_arg_type c
#if PRSIM_TRACK_LAST_EDGE_TIME && !PRSIM_TRACK_CAUSE_TIME
		, const time_type& et
#endif
		) {
	STACKTRACE_VERBOSE_STEP;
#if PRSIM_MK_EXCL_BLOCKING_SET
	if (c.val != LOGIC_OTHER) {
		flush_blocked_excl_nodes(c.val);
	}
#endif
	break_type err = ERROR_NONE;
#if PRSIM_FCFS_UPDATED_NODES
	typedef	updated_nodes_queue_type::const_iterator	const_iterator;
	const_iterator i(updated_nodes_queue.begin()),
		e(updated_nodes_queue.end());
	ISE_INVARIANT(updated_nodes.size() == updated_nodes_queue.size());
#else
	typedef	updated_nodes_type::const_iterator	const_iterator;
	const_iterator i(updated_nodes.begin()), e(updated_nodes.end());
	// TODO: mind the ordering of processing nodes, numerical order by ID?
#endif
for ( ; i!=e; ++i) {
#if PRSIM_FCFS_UPDATED_NODES
	const node_index_type& ni(*i);
#else
	const node_index_type& ni(i->first);
#endif
	node_type& n(__get_node(ni));
	DEBUG_STEP_PRINT("considering updated node " <<
		get_node_canonical_name(ni) << endl);
	const event_index_type prevevent = n.get_event();
	event_type newevent;
	newevent.node = ni;
	newevent.cause = c;
#if PRSIM_FCFS_UPDATED_NODES
	const updated_nodes_type::iterator f(updated_nodes.find(*i));
	ISE_INVARIANT(f != updated_nodes.end());
	newevent.cause_rule = f->second.rule_index;
#if EVENT_INCLUDE_RULE_POINTER
	newevent.cause_rule_ptr = f->second.rule_ptr;
#endif
	const pull_set& ops(f->second.old_pull_set);
#else
	newevent.cause_rule = i->second;
#endif
	DEBUG_STEP_PRINT("by rule #" << newevent.cause_rule << endl);
	// flags?
	// can we use why-analysis for identify short paths?
	// TODO: why-interfere (why is pull 1 or X in both directions)?
	value_enum& pull_val(newevent.val);	// yes, *reference*
	const value_enum old_val = n.current_value();
	pull_val = old_val;	// start with current value
	bool have_interference = false;	// to be updated

	// interference and instability are actually independent
	// check for interference first
	// create event first, but don't tie it to the node until
	// after checking for instability and queue conflicts
#if PRSIM_MK_EXCL_BLOCKING_SET
	// pull states are overridden by excl forcing
	const bool excl_blocked = f->second.excl_blocked;
	pull_set p(n, weak_rules_enabled());
	if (excl_blocked) {
		// note: this also masks interference diagnostics!
		DEBUG_STEP_PRINT("blocked by force excl" << endl);
		// nullify the pull in the direction that is blocked
		switch (c.val) {
		case LOGIC_HIGH:	p.cut_off_pull_up(); break;
		case LOGIC_LOW:		p.cut_off_pull_dn(); break;
		default: break;
		}
	}
#else
	const pull_set p(n, weak_rules_enabled());
#endif
	const bool weak_wins = p.weak_wins_any();
	bool possible_interference = false;	// used to be named pending_weak
#if DEBUG_STEP
	p.dump(DEBUG_STEP_PRINT("")) << endl;
#endif
{
	// compute the future value based on pull-state
	const pull_set::interference_info int_info(p);
	have_interference = int_info.have_interference;
	possible_interference = int_info.possible_interference;
	if (have_interference) {
		if (int_info.weak_rule_involved) {
			newevent.set_weak(true);
		}
		// TODO: move this up above
		pull_val = LOGIC_OTHER;
	/**
		What if previous state was already interfering
		should a duplicate interference diagnostic be issued?
		Only if weakness (possibility) of interference changed.
		Since everything starts out as X, this will reduce the
		number of transient weak-interference diagnostics
		as signals transition out of X.  
	**/
		const pull_set::interference_info old_interf(ops);
		if (!old_interf.have_interference
			|| (old_interf.val_type != int_info.val_type)
			// redundant
//			|| (old_interf.possible_interference != possible_interference)
//			|| (old_interf.rule_type != int_info.rule_type)
			) {
		const break_type E =
			__report_interference(cout,
				possible_interference, ni, c);
		if (E > err) err = E;
		}	// else suppress non-changed interference status
	} else {
		// is a non-interfering rule firing
		newevent.set_weak(weak_wins);
		if (p.pull_up_wins_any()) {
			DEBUG_STEP_PRINT("normal pull-up" << endl);
			pull_val = LOGIC_HIGH;
		} else if (p.pull_dn_wins_any()) {
			DEBUG_STEP_PRINT("normal pull-dn" << endl);
			pull_val = LOGIC_LOW;
		} else if (p.pull_up_x_wins_any()) {
			if (pull_val == LOGIC_LOW) {
			DEBUG_STEP_PRINT("non-interfering X-pull-up" << endl);
				pull_val = LOGIC_OTHER;
			} else {
			// otherwise, X-pulling up a high node is vacuous
			DEBUG_STEP_PRINT("vacuous X-pull-up" << endl);
			}
		} else if (p.pull_dn_x_wins_any()) {
			if (pull_val == LOGIC_HIGH) {
			DEBUG_STEP_PRINT("non-interfering X-pull-dn" << endl);
				pull_val = LOGIC_OTHER;
			} else {
			// otherwise, X-pulling dn a low node is vacuous
			DEBUG_STEP_PRINT("vacuous X-pull-dn" << endl);
			}
		} else {
			ISE_INVARIANT(p.state_holding());
			DEBUG_STEP_PRINT("state-holding" << endl);
			// keep current_value
		}
		// keeper-check is done at the end of all this
	}
	// schedule event below, after instability check
}{
	bool have_instability = false;
	// check for instability
	if (prevevent) {
		DEBUG_STEP_PRINT("there is pending event" << endl);
		// there is a pending event in queue already
		event_type& pe(get_event(prevevent));
		const value_enum pval = pe.val;	// save away previous pull
		// * compare event values
		// * check pull-state based on prev.val
		//	if pull is no longer active, then is unstable
		if (have_interference) {
			DEBUG_STEP_PRINT("had some interference" << endl);
			// previous event should be clobbered to X
			// regardless of the instability state
			// BUT NOT if it was set by user (forced)
			// see test case interference-07.prsimrc
		if (pe.forced()) {
			DEBUG_STEP_PRINT("yielding to user-set value" << endl);
		} else if (old_val == LOGIC_OTHER) {
			DEBUG_STEP_PRINT("already X (cancel)" << endl);
			kill_event(prevevent, ni);
			// update cause, even though X is vacuous
#if PRSIM_TRACK_LAST_EDGE_TIME && !PRSIM_TRACK_CAUSE_TIME
			n.set_value_and_cause(LOGIC_OTHER, c, et);
#else
			n.set_value_and_cause(LOGIC_OTHER, c);
#endif
		} else {
			DEBUG_STEP_PRINT("overwrite to X." << endl);
			pe.val = LOGIC_OTHER;
		// cause: most recent transition participating in interference
			// pe.set_cause_node()?
			pe.cause = c;
		}
			// check for stability below
		} else {
			DEBUG_STEP_PRINT("no interference" << endl);
		}	// end if !have_interference
		// even if interfering, also check for instability
		switch (pval) {	// based on previously scheduled value
			// conditionals here can't just use pull_val
			// because state-holding node will retain old value
		case LOGIC_LOW: {
			// Q: are we still pulling down?
			if (p.pulling_dn()) {
				DEBUG_STEP_PRINT("active pull-dn" << endl);
				// the rule re-firing is vacuous
				// the old event stays in queue
			} else if (p.__pulling_dn_x()) {
				DEBUG_STEP_PRINT("X pull-dn" << endl);
				// possible instability
				have_instability = true;
			} else if (pull_val != pval) {
				// TODO: opposite-pulls? new event below
				have_instability = true;
			} else {
				// pull turned off -- instability
				have_instability = true;
			}
			break;
		}
		case LOGIC_HIGH: {
			// Q: are we still pulling up?
			if (p.pulling_up()) {
				DEBUG_STEP_PRINT("active pull-up" << endl);
				// the rule re-firing is vacuous
				// the old event stays in queue
			} else if (p.__pulling_up_x()) {
				DEBUG_STEP_PRINT("X pull-up" << endl);
				// possible instability
				have_instability = true;
			} else if (pull_val != pval) {
				// TODO: opposite-pulls? new event below
				have_instability = true;
			} else {
				// pull turned off -- instability
				have_instability = true;
			}
			break;
		}
		case LOGIC_OTHER:
		default:
			// we don't know what the nature of the scheduled X is
			// so we might have to deduce from cause-rule
			break;
		}	// end switch
		// report instability before acting on it
		if (have_instability) {
			// update cause of instability for diagnostic
			// if marked X? if dequeued?
			// event_type pe_diag(pe);		// diagnostic
//			if (!dequeue_unstable_events()) {
				pe.cause.node = c.node;
				pe.cause.val = c.val;
//			}
			// pe.cause = c;
			// retain original cause.rule
			const break_type E =
			__report_instability(cout, c.val == LOGIC_OTHER, 
				pval == LOGIC_HIGH,
#if PRSIM_MK_EXCL_BLOCKING_SET
				excl_blocked, 
#endif
				pe);
			if (E > err) err = E;
		}
		// interference takes precedence over instability, outcome-wise
		if (have_interference) {
			// rewrite event's value to X, done with pull_val
			DEBUG_STEP_PRINT("interference precedence" << endl);
		} else if (have_instability) {
			DEBUG_STEP_PRINT("some instability" << endl);
			// we either need to cancel, overwrite with X,
			// or kill and reschedule entirely new event
			// diagnostic should be issued here
			INVARIANT(pull_val != pval);	// vacuous => stable
			// hope the compiler is smart enough to optimize away
			// lookup_rule when it is not needed (code motion)
			const rule_type* const r = lookup_rule(pe.cause_rule);
			const bool r_unstable = r && r->is_unstable();
			if (dequeue_unstable_events() || r_unstable
#if PRSIM_MK_EXCL_BLOCKING_SET
				// user-controllable policy
				|| ( excl_blocked && dequeue_excl_unstable_events())
#endif
				) {
				// remove from queue or replace?
				kill_event(prevevent, ni);
				if (pull_val != old_val) {
					enqueue_new_event(newevent);
				}
			} else if (old_val == LOGIC_OTHER) {
				DEBUG_STEP_PRINT("already X (cancel)" << endl);
				kill_event(prevevent, ni);
				// update cause, even though X is vacuous
#if PRSIM_TRACK_LAST_EDGE_TIME && !PRSIM_TRACK_CAUSE_TIME
				n.set_value_and_cause(LOGIC_OTHER, c, et);
#else
				n.set_value_and_cause(LOGIC_OTHER, c);
#endif
			} else {
				DEBUG_STEP_PRINT("overwrite to X." << endl);
				pe.val = LOGIC_OTHER;
				pe.set_cause_node(c.node);
				pe.set_weak(weak_wins);
				// no need to set cause::val?
			}
		} else {
			DEBUG_STEP_PRINT("neither interference/instability" << endl);
			// no change to event queue?
			// graduate event to main queue?
			// check for overtaking:
			if ((p.pulling_dn() || p.pulling_up()) &&
					pull_val != pval &&
					pe.val == LOGIC_OTHER
//					dequeue_unstable_events()
					) {
				DEBUG_STEP_PRINT("overtaking X with [01]" << endl);
				INVARIANT(pull_val != LOGIC_OTHER);
				kill_event(prevevent, ni);
				if (pull_val != old_val) {
					enqueue_new_event(newevent);
				} else {
					DEBUG_STEP_PRINT("vacuous, not re-queued" << endl);
				}
			}
		}
	} else {
		DEBUG_STEP_PRINT("no pending event" << endl);
		// no event in queue, then just enqueue new one
		if (pull_val != old_val) {
			enqueue_new_event(newevent);
		} else {
			DEBUG_STEP_PRINT("dropping vacuous event" << endl);
		}
	}
}
#if PRSIM_FCFS_UPDATED_NODES
	// remove as each node is processed
	updated_nodes.erase(f);
#endif
}	// end for all updated nodes
#if PRSIM_FCFS_UPDATED_NODES
	ISE_INVARIANT(updated_nodes.empty());
	updated_nodes_queue.clear();
#else
	updated_nodes.clear();
#endif
	return err;
}	// end flush_updated_nodes()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !PRSIM_MK_EXCL_BLOCKING_SET
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
	node_type& epn(__get_node(epni));
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
	node_type& epn(__get_node(epni));
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
#endif	// PRSIM_MK_EXCL_BLOCKING_SET

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enforces exclusive high rings by enqueue necessary events 
	into the exclusive high queue.  
	\param ni index of the node that changed that affects exclhi rings.  
 */
void
State::enforce_exclhi(cause_arg_type c
#if PRSIM_MK_EXCL_BLOCKING_SET
		, const value_enum prev
#endif
		) {
	/***
		If n.exclhi and n is 0, check if any of the nodes
		in n's exclhi rings are enabled.  
		If so, insert them into the exclhi-queue.
	***/
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE_STEP;
	const node_index_type& ni(c.node);
#if PRSIM_MK_EXCL_BLOCKING_SET
	const value_enum v(c.val);
	mk_excl_counter_map_type::const_iterator
		f(mk_exhi_counter_map.find(ni));
	ISE_INVARIANT(f != mk_exhi_counter_map.end());
	const lock_index_list_type& l(f->second);
	DEBUG_STEP_PRINT("exclhi ring memberships: " << l.size() << endl);
	lock_index_list_type::const_iterator i(l.begin()), e(l.end());
for ( ; i!=e; ++i) {	// for each ring
	// update counter value too
	ring_counter_state& r(mk_exhi[*i]);
	ring_set_type::const_iterator
		ri(r.elems.begin()), re(r.elems.end());
	// Xs count as inactive (low), but 0 <-> X should not count
	if (v == LOGIC_HIGH && prev != LOGIC_HIGH) {
		DEBUG_STEP_PRINT("exclhi ring blocking" << endl);
		++r.count;
		// lock-out: add to blocking set for suppression/cancellation
		for ( ; ri!=re; ++ri) {
			if (*ri != ni) {	// exclude self
				DEBUG_STEP_PRINT("blocking node " << *ri << endl);
				__mk_excl_blocking_set.insert(*ri);
			}
		}
	} else if (v != LOGIC_HIGH && prev == LOGIC_HIGH) {
		DEBUG_STEP_PRINT("exclhi ring releasing" << endl);
		--r.count;
		// un-lock: add to standard re-evaluation queue
		if (!r.count) {		// in case count > 1
		for ( ; ri!=re; ++ri) {
			if (*ri != ni) {	// exclude self
				const pair<updated_nodes_type::iterator, bool>
					p(updated_nodes.insert(
						updated_nodes_type::value_type(
						*ri, node_update_info())));
				// node_update_info is blank: no associated rule
#if PRSIM_FCFS_UPDATED_NODES
				if (p.second) {
					// new entry
					updated_nodes_queue.push_back(*ri);
				}
				// else keep existing entry's node_update_info
#endif
			}
		}
		}
	} else {
		// else no change (should never have to add to blocking_set?)
		DEBUG_STEP_PRINT("exclhi ring no change" << endl);
	}
}	// end for each ring
#else
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
			node_type& er(__get_node(eri));
			// frozen nodes are not considered
			if (er.is_frozen()) {
				continue;
			}
			// strong rules only
			const pull_set p(er, weak_rules_enabled());
			if (!er.pending_event() &&
				// er->n->up->val == PRS_VAL_T
				// what if is pulling weakly?
				p.up == PULL_ON) {
			DEBUG_STEP_PRINT("enqueuing pull-up event" << endl);
				const event_index_type ne =
					// the pull-up index may not necessarily
					// correspond to the causing expression!
					__allocate_event(er, eri, c,
						// not sure...
						// er.pull_up_index, 
						INVALID_RULE_INDEX, 
#if EVENT_INCLUDE_RULE_POINTER
						NULL,
#endif
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
#endif
}	// end method enforce_exclhi()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enforces exclusive low rings by enqueue necessary events 
	into the exclusive low queue.  
	\param ni index of the node that changed that affects excllo rings.  
 */
void
State::enforce_excllo(cause_arg_type c
#if PRSIM_MK_EXCL_BLOCKING_SET
		, const value_enum prev
#endif
		) {
	/***
		If n.excllo and n is 1, check if any of the nodes
		in n's excllo rings are enabled.  
		If so, insert them into the excllo-queue.
	***/
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE_STEP;
	const node_index_type& ni(c.node);
#if PRSIM_MK_EXCL_BLOCKING_SET
	const value_enum v(c.val);
	mk_excl_counter_map_type::const_iterator
		f(mk_exlo_counter_map.find(ni));
	ISE_INVARIANT(f != mk_exlo_counter_map.end());
	const lock_index_list_type& l(f->second);
	DEBUG_STEP_PRINT("excllo ring memberships: " << l.size() << endl);
	lock_index_list_type::const_iterator i(l.begin()), e(l.end());
for ( ; i!=e; ++i) {	// for each ring
	// update counter value too
	ring_counter_state& r(mk_exlo[*i]);
	ring_set_type::const_iterator
		ri(r.elems.begin()), re(r.elems.end());
	// Xs count as inactive (low), but 0 <-> X should not count
	if (v == LOGIC_LOW && prev != LOGIC_LOW) {
		DEBUG_STEP_PRINT("excllo ring blocking" << endl);
		++r.count;
		// lock-out: add to blocking set for suppression/cancellation
		for ( ; ri!=re; ++ri) {
			if (*ri != ni) {	// exclude self
				DEBUG_STEP_PRINT("blocking node " << *ri << endl);
				__mk_excl_blocking_set.insert(*ri);
			}
		}
	} else if (v != LOGIC_LOW && prev == LOGIC_LOW) {
		DEBUG_STEP_PRINT("excllo ring releasing" << endl);
		--r.count;
		// un-lock: add to standard re-evaluation queue
		if (!r.count) {		// in case count > 1
		for ( ; ri!=re; ++ri) {
			if (*ri != ni) {	// exclude self
				const pair<updated_nodes_type::iterator, bool>
					p(updated_nodes.insert(
						updated_nodes_type::value_type(
						*ri, node_update_info())));
				// node_update_info is blank: no associated rule
#if PRSIM_FCFS_UPDATED_NODES
				if (p.second) {
					// new entry
					updated_nodes_queue.push_back(*ri);
				}
				// else keep existing entry's node_update_info
#endif
			}
		}
		}
	} else {
		// else no change (should never have to add to blocking_set?)
		DEBUG_STEP_PRINT("excllo ring no change" << endl);
	}
}	// end for each ring
#else
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
			node_type& er(__get_node(eri));
			// frozen nodes are not considered
			if (er.is_frozen()) {
				continue;
			}
			// strong rules only
			const pull_set p(er, weak_rules_enabled());
			if (!er.pending_event() &&
				// er->n->dn->val == PRS_VAL_T
				// what if is pulling weakly?
				p.dn == PULL_ON) {
			DEBUG_STEP_PRINT("enqueuing pull-dn event" << endl);
				const event_index_type ne =
					// same comment as enforce_exclhi
					__allocate_event(er, eri, c, 
						// er.pull_dn_index, 
						INVALID_RULE_INDEX,
#if EVENT_INCLUDE_RULE_POINTER
						NULL,
#endif
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
#endif
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
error_policy_enum
State::excl_exception::inspect(const State& s, ostream& o) const {
	typedef	check_excl_ring_map_type::const_iterator	const_iterator;
	ring_set_type ring;
	const_iterator i, e;
	if (type) {
		i = s.check_exhi.begin();
		e = s.check_exhi.end();
	} else {
		i = s.check_exlo.begin();
		e = s.check_exlo.end();
	}
	// find all nodes that contain the lock index
	for ( ; i!=e; ++i) {
		const lock_index_list_type::const_iterator
			le(i->second.end()), 
			lf(find(i->second.begin(), le, lock_id));
		if (lf != le) {
			// then this node_index belongs to this ring
			ring.insert(i->first);
		}
	}
	ISE_INVARIANT(ring.size() > 1);
	o << "ERROR: excl" << (type ? "hi" : "lo") << 
		" violation detected!" << endl;
	ring_set_type::const_iterator ri(ring.begin()), re(ring.end());
	o << "ring-state:" << endl;
	for (; ri!=re; ++ri) {
		s.dump_node_canonical_name(o << "\t", *ri) << " : ";
		s.get_node(*ri).dump_value(o) << endl;
	}
	s.dump_node_canonical_name(o << "but node `", node_id) <<
		"\' tried to become " << (type ? 1 : 0) << "." << endl;
	o << "The simulator\'s excl-check-lock state is no longer coherent; "
		"do not bother trying to continue the simulation, "
		"but you may further inspect the state." << endl;
	o << "You probably want to disable excl-checking with `nocheckexcl\' "
		"if you wish to continue the simulation." << endl;
	return s.get_excl_check_fail_policy();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For the sake of exception safety, 
	Upon destruction, flush intermediate event queues.  
 */
struct State::auto_flush_queues {
	State&				state;
	const event_cause_type&		cause;
#if PRSIM_TRACK_LAST_EDGE_TIME && !PRSIM_TRACK_CAUSE_TIME
	const event_time_type		time;
#endif

	explicit
	auto_flush_queues(State& s, const event_cause_type& c
#if PRSIM_TRACK_LAST_EDGE_TIME && !PRSIM_TRACK_CAUSE_TIME
		, const event_time_type& t
#endif
		) :
		state(s), cause(c)
#if PRSIM_TRACK_LAST_EDGE_TIME && !PRSIM_TRACK_CAUSE_TIME
		, time(t)
#endif
		{ }

	~auto_flush_queues() {
		// check and flush pending queue, spawn fanout events
#if PRSIM_TRACK_LAST_EDGE_TIME && !PRSIM_TRACK_CAUSE_TIME
		const break_type E(state.flush_updated_nodes(cause, time));
#else
		const break_type E(state.flush_updated_nodes(cause));
#endif
#if !PRSIM_MK_EXCL_BLOCKING_SET
		// can anything go wrong here?
		// check and flush pending queue against exclhi/lo events
		state.flush_exclhi_queue();
		state.flush_excllo_queue();
#endif
		if (UNLIKELY(E >= ERROR_BREAK)) {
			state.stop();		// set stop flag
			if (UNLIKELY(E >= ERROR_INTERACTIVE)) {
				state.record_exception(exception_ptr_type(
					new interference_exception(
						cause.node, E)));
			}
		}
	}
} __ATTRIBUTE_UNUSED__ ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Modeled after prs_step() from the original prsim.  
	TODO: possible add arguments later. 
	\return index of the affected node, 
		INVALID_NODE_INDEX if nothing happened.  
	\throw excl_exception if there is a logical exclusion violation, 
		leaving violating event in queue.  
 */
State::step_return_type
State::step(void) THROWS_STEP_EXCEPTION {
	typedef	State::step_return_type		return_type;
	STACKTRACE_VERBOSE;
	__atomic_updated_nodes.clear();
	if (event_queue.empty()) {
		return return_type(INVALID_NODE_INDEX, INVALID_NODE_INDEX);
	}
	const event_placeholder_type ep(dequeue_event());
	current_time = ep.time;
	return execute_immediately(ep.event_index, ep.time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This executes a specific event (passed by placeholder)
	which need not be the next event in the queue.
	This can be useful for instantly executing an event 
	without going through the event queue, or advancing
	an event that is already in queue.
	Critical path through here.  
	NOTE: this does NOT update the current time to that of the event.
	\pre the exclhi, excllo, and pending queues are empty.
	\post the exclhi, excllo, and pending queues are empty.
	\return index of the affected node, 
		INVALID_NODE_INDEX if nothing happened.  
	\throw excl_exception if there is a logical exclusion violation, 
		leaving violating event in queue.  
 */
// inline
State::step_return_type
State::execute_immediately(
		// const event_placeholder_type& ep,
		const event_index_type ei,
		const time_type& ept) THROWS_STEP_EXCEPTION {
	typedef	State::step_return_type		return_type;
//	STACKTRACE_VERBOSE;
	if (!ei) {
		// possible in the queue that last events are killed
		return return_type(INVALID_NODE_INDEX, INVALID_NODE_INDEX);
	}
	const event_type& pe(get_event(ei));
	const node_index_type ni = pe.node;
	node_type& n(__get_node(ni));
	ISE_INVARIANT(n.pending_event());	// must have been pending
	ISE_INVARIANT(n.get_event() == ei);	// must be consistent!
	DEBUG_STEP_PRINT("time = " << current_time << endl);
	DEBUG_STEP_PRINT("event_index = " << ei << endl);
	return execute_immediately(pe, ept);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This execute a raw event that may not necessarily be in queue.
	This changes the value of a node now, bypassing the event_queue.
 */
State::step_return_type
State::execute_immediately(
		const event_type& pe,
		const time_type& ept) THROWS_STEP_EXCEPTION {
	// FIXME: this needs to be re-entrant, due to atomic updates!
	typedef	State::step_return_type		return_type;
	STACKTRACE_VERBOSE;
	ISE_INVARIANT(updated_nodes.empty());	// b/c of auto_flush_queues
#if PRSIM_FCFS_UPDATED_NODES
	ISE_INVARIANT(updated_nodes_queue.empty());
#endif
#if PRSIM_MK_EXCL_BLOCKING_SET
	ISE_INVARIANT(__mk_excl_blocking_set.empty());
#else
	ISE_INVARIANT(exclhi_queue.empty());
	ISE_INVARIANT(excllo_queue.empty());
#endif
#if PRSIM_FWD_POST_TIMING_CHECKS
	timing_checker.expire_timing_checks();
#endif
	clear_exceptions();
//	const event_index_type& ei(ep.event_index);
#if 0
	if (!ei) {
		// possible in the queue that last events are killed
		return return_type(INVALID_NODE_INDEX, INVALID_NODE_INDEX);
	}
#endif
//	const event_type& pe(get_event(ei));
	const bool force = pe.forced();
	const node_index_type ni = pe.node;
	node_type& n(__get_node(ni));
if (!n.is_atomic()) {
	// FIXME: don't want atomic node propagation to clear exceptions
	// caused by non-atomic nodes
	recent_exceptions.clear();
}
//	ISE_INVARIANT(n.pending_event());	// must have been pending
//	ISE_INVARIANT(n.get_event() == ei);	// must be consistent!
	const value_enum prev = n.current_value();
	node_index_type _ci;	// just a copy
	trace_index_type critical = INVALID_TRACE_INDEX;
	if (is_tracing()) {
		critical = trace_manager->push_back_event(
			state_trace_point(current_time, pe.cause_rule, 
#if 0 && EVENT_INCLUDE_RULE_POINTER
			pe.cause_rule_ptr,	// cannot checkpoint pointer
#endif
				pe.cause.critical_trace_event, 
				ni, pe.val, prev));
		DEBUG_STEP_PRINT("prsim trace event # " << critical << endl);
		if (trace_manager->current_event_count() >=
				trace_flush_interval) {
			trace_manager->flush();
		}
	}
	// if both traces are on, assert that critical (event count) is the same
	if (is_tracing_vcd()) {
		critical = vcd_manager->record_event(current_time, ni, pe.val);
		DEBUG_STEP_PRINT("prsim vcd event # " << critical << endl);
		// no need to manage flushing
	}
{
	const event_cause_type& cause(pe.cause);
	const node_index_type& ci(cause.node);
	_ci = ci;
	DEBUG_STEP_PRINT("examining node: " <<
		get_node_canonical_name(ni) << endl);
{
	// event-deallocation scope
	event_deallocator ed(*this, n);
	const event_index_type pei = n.get_event();
		// pending event index, may be 0 if queue was bypassed!
		// if bypassed, then event was not allocated from pool
	if (!pei) {
		// if event was not from queue, then it did not point into pool
		// so never try to de-allocate it
		ed.keep = true;
	}
	DEBUG_STEP_PRINT("former value: " <<
		node_type::translate_value_to_char(prev) << endl);
	DEBUG_STEP_PRINT("new value: " <<
		node_type::translate_value_to_char(pe.val) << endl);
	if (pe.val == LOGIC_OTHER &&
		prev == LOGIC_OTHER) {
		// node being set to X, but is already X, this could occur
		// b/c there are other causes of X besides guards going X.
		DEBUG_STEP_PRINT("X: returning node index " << ni << endl);
		return return_type(ni, ci);
	}
	// assert: vacuous firings on the event queue, 
	// but ONLY if unstable events don't cause vacuous events to
	// be resheduled, e.g. pulse
	if (dequeue_unstable_events()) {
		if (UNLIKELY(prev == pe.val)) {
			DEBUG_STEP_PRINT("vacuous event dequeued" << endl);
			// Q: or is it better to catch this before enqueuing?
			// then just silence this event
#if 0
			cout << "vacuous event on node " <<
				get_node_canonical_name(ni) << endl;
			// might need a mode to return control to caller
			// after vacuous firings
#endif
			if (stopping_on_vacuous_events()) {
				return return_type(INVALID_NODE_INDEX,
					INVALID_NODE_INDEX);
			} else {
			// slow head-recursion, but infrequent
				return step();
			}
		}
		// else proceed
	} else {
		// vacuous event is allowed if set was forced by user
		ISE_INVARIANT(prev != pe.val || force);
		// FAILED ONCE! (test case?)
		// occurred on 20071214 after adding weak rules
	}
	// saved previous value above already
	if (checking_excl()) {
		const excl_exception
			exex(check_excl_rings(ni, n, prev, pe.val));
		if (UNLIKELY(exex.lock_id)) {
		switch (excl_check_fail_policy) {
		case ERROR_BREAK:
			stop();				// fall-through
		case ERROR_WARN:
			exex.inspect(*this, cerr);	// fall-through
		case ERROR_IGNORE:
			cerr <<
"WARNING: detected exclusion violation, but because the simulation is\n"
"continuing (current policy), further exclusion checking is automatically\n"
"disabled because the checking structures would be incoherent otherwise.\n";
			nocheck_excl();
			break;
		case ERROR_INTERACTIVE:	// fall-through
		case ERROR_FATAL:
		default:
			// to keep event queue coherent, re-enqueue event
			// because event will not be deallocated!
			// next attempt to step will hit same exception
			// forcing simulation to be stuck (intentional)
			if (pei) {
				// then event came from queue
				ed.keep = true;
				// enqueue_event(ep.time, ei);
				enqueue_event(ept, pei);
			}	// otherwise was not from queue, don't enqueue
			record_exception(
				exception_ptr_type(new excl_exception(exex)));
			// return immediately to keep event from executing
			// early abort, keeping event in queue
			return return_type(INVALID_NODE_INDEX, INVALID_NODE_INDEX);
		}	// end switch
		}
	}
#if PRSIM_SETUP_HOLD
#if PRSIM_FWD_POST_TIMING_CHECKS
	// forward-post timing checks if this is node participates
	// as a reference node in some timing constraint.
	if (UNLIKELY(n.has_setup_check())) {
		timing_checker.post_setup_check(ni, pe.val);
	}
	if (UNLIKELY(n.has_hold_check())) {
		timing_checker.post_hold_check(ni, pe.val);
	}
	timing_checker.check_active_timing_constraints(*this, ni, pe.val);
#else
	// TODO: decide what to do if node is X
	if (UNLIKELY(n.has_setup_check())) {
		timing_checker.do_setup_check(*this, ni, pe.val);
	}
	if (UNLIKELY(n.has_hold_check())) {
		timing_checker.do_hold_check(*this, ni, pe.val);
	}
#endif
#endif	// PRSIM_SETUP_HOLD
	// only set the cause of the node when we change its value
	DEBUG_STEP_PRINT("committing value change to node" << endl);
#if PRSIM_TRACK_LAST_EDGE_TIME && !PRSIM_TRACK_CAUSE_TIME
	n.set_value_and_cause(pe.val, cause, ept);
#else
	n.set_value_and_cause(pe.val, cause);
#endif
	// count transition only if new value is not X
	if (pe.val != LOGIC_OTHER) {
		++n.tcount;
	}
}
}
	DEBUG_STEP_PRINT("evaluating fanouts" << endl);
	// note: pe is invalid, deallocated beyond this point, could scope it
	// reminder: do not reference pe beyond this point (deallocated)
	// could scope the reference to prevent it...
	const value_enum next = n.current_value();
	// value propagation...
#if PRSIM_TRACK_CAUSE_TIME
	const event_cause_type new_cause(ni, next, ept, critical);
#else
	const event_cause_type new_cause(ni, next, critical);
#endif
{	// scoping for auto-flush
#if PRSIM_TRACK_LAST_EDGE_TIME && !PRSIM_TRACK_CAUSE_TIME
	const auto_flush_queues __auto_flush(*this, new_cause, ept);
#else
	const auto_flush_queues __auto_flush(*this, new_cause);
#endif
{
	// evaluate fanout in some order
	typedef	node_type::const_fanout_iterator	const_iterator;
	const_iterator i, e;
	DEBUG_STEP_PRINT("#fanouts: " << n.fanout.size() << endl);
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
{
	// propagate_evaluation() may populate some structures for
	// later re-evaluation.  These should start empty.
	__keeper_check_candidates.clear();	// look for turned off rules
	// atomic expressions never participte in keeper-checks
	// however, they do participate in invariants, so the following 
	const swap_saver<invariant_update_map_type> tmp(__invariant_update_map);
	__invariant_update_map.clear();
	for ( ; i!=e; ++i) {
		// when evaluating a node as an expression, 
		// is appropriate to interpret node value
		// as a pull-value
		const break_type E =
		propagate_evaluation(new_cause, *i, pull_enum(prev));
		if (UNLIKELY(E >= ERROR_BREAK)) {
			stop();
		}
	}
	handle_invariants(new_cause);
}
}
	// Q: is this the best place to handle this?
if (n.in_channel()) {
	DEBUG_STEP_PRINT("channels reacting to node" << endl);
	// predicate may not filter precisely
	// channel manager should 'ignore' irrelevant nodes
	vector<env_event_type> env_events;
	// the following may throw channel_exception
	_channel_manager.process_node(*this, ni,
			prev, next, env_events);
	// cause of these events must be 'ni', this node
//	const event_cause_type c(ni, next);	// same as new_cause
	const break_type E = flush_channel_events(env_events, new_cause);
	if (UNLIKELY(E >= ERROR_BREAK)) {
		stop();
	if (UNLIKELY(E >= ERROR_INTERACTIVE)) {
		// now catches FATAL and INTERACTIVE
		record_exception(exception_ptr_type(
			new instability_exception(ni, E)));
	}
	}
}
#if PRSIM_MK_EXCL_BLOCKING_SET
	// update excl lock counters
	if (n.has_mk_exclhi()) {
		enforce_exclhi(new_cause, prev);
	}
	if (n.has_mk_excllo()) {
		enforce_excllo(new_cause, prev);
	}
#else
	// exclhi ring enforcement
	if (n.has_mk_exclhi() && (next == LOGIC_LOW)) {
		enforce_exclhi(new_cause);
	}	// end if (exclhi enforcement)

	// excllo ring enforcement
	if (n.has_mk_excllo() && (next == LOGIC_HIGH)) {
		enforce_excllo(new_cause);
	}	// end if (excllo enforcement)
#endif
	// energy estimation?  TODO later for a different sim variant
	// queues automatically flushed by dtor of auto_flush_queues

#if 0
	// very slow, but terrific for debugging!!!
	check_event_queue();
#endif
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
	// periodically age the cache, evict old entries
	if (LIKELY(cache_countdown)) {
		--cache_countdown;
	} else {
		cache_countdown = cache_half_life;
		GET_CONTEXT_CACHE halve_cache();
	}
#endif

	// return the affected node's index
	DEBUG_STEP_PRINT("returning node index " << ni << endl);
}	// auto-flush of pending queue happens here
	// optional keeper check
	if (keeper_check_fail_policy > ERROR_IGNORE) {
		handle_keeper_checks(ni);
	}
	return return_type(ni, _ci);
}	// end method execute_immediately()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::handle_keeper_checks(const node_index_type ni) {
//		cout << "checking floating..." << endl;
		size_t first_thrown_node = INVALID_NODE_INDEX;
		set<node_index_type>::const_iterator
			i(__keeper_check_candidates.begin()),
			e(__keeper_check_candidates.end());
#define	E	keeper_check_fail_policy
		for ( ; i!=e; ++i) {
		if (get_node(*i).has_fanin()) {
		if (check_floating_node(*i, ni)) {
			if (!first_thrown_node)
				first_thrown_node = *i;
			cout << ((E > ERROR_WARN) ? "Error: " : "Warning: ");
			dump_node_canonical_name(cout << "node ", *i)
				<< " is floating without pending feedback. ";
			if (!expr_alloc_flags.fast_weak_keepers) {
				cout << " (fast-weak-keepers disabled)";
			}
			if (!weak_rules_enabled()) {
				cout << " (weak-rules off)";
			}
			cout << endl;
		}	// else that node is feedback-driven
		}	// else no fanin, assume input
		}	// end for each candidate
		if (first_thrown_node) {
			dump_node_value(cout << "\t[caused by ", ni)
				<< ']' << endl;
			if (UNLIKELY(E >= ERROR_BREAK)) {
				stop();
			if (UNLIKELY(E >= ERROR_INTERACTIVE)) {
				// can only throw one node, pick first one
				record_exception(exception_ptr_type(
					new keeper_fail_exception(ni, E)));
			}
			}
		}
#undef E
		// __keeper_check_candidates.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_SETUP_HOLD
void
State::handle_timing_exception(const timing_exception& tex) {
	const error_policy_enum E = tex.policy;
	if (E >= ERROR_BREAK) {
		stop();
	if (E >= ERROR_INTERACTIVE) {
		record_exception(exception_ptr_type(
			new timing_exception(tex)));
	}
	} else {
		// break or warning, print now
		tex.inspect(*this, cout);
	}
}

#if 0
#if PRSIM_FWD_POST_TIMING_CHECKS
/**
	Registers a single timing exception, keeping related structures consistent.
 */
void
State::register_timing_check(const timing_exception& tex, const time_type& ft) {
	const node_index_type gti = tex.node_id;
	const timing_check_index_type ti = timing_check_pool.allocate(tex);
	// schedule the new check ID at the trigger-node (map)
	active_timing_check_map[gti].insert(ti);
	// insert same ID into check-expiration-queue,
	//	on expiration, should dequeue, and remove from trigger-node
	timing_check_queue.insert(make_pair(ft, make_pair(ti, gti)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Node ni is the reference node of a constraint.
	Post a constraint check on the trigger node that expires
	after an elapsed time dictated by the constraint.  
 */
void
State::post_setup_check(const node_index_type ni, const value_enum nv) {
	STACKTRACE_INDENT_PRINT("there is setup check on node" << endl);
	const timing_constraint_process_map_type::const_iterator
		f(setup_check_map.find(ni));
if (f != setup_check_map.end()) {
	STACKTRACE_INDENT_PRINT("found processes with setup constraints" << endl);
	const map<process_index_type, local_node_ids_type>& m(f->second);
	map<process_index_type, local_node_ids_type>::const_iterator
		i(m.begin()), e(m.end());
for ( ; i!=e; ++i) {
	// processes that own a setup constraint on this node
	const process_index_type pid = i->first;
	STACKTRACE_INDENT_PRINT("  process id " << pid << endl);
	const process_sim_state& ps(get_process_state(pid));
	const unique_process_subgraph& pg(ps.type());
	// possible multiple local occurrences
	local_node_ids_type::const_iterator
		li(i->second.begin()), le(i->second.end());
	for ( ; li!=le; ++li) {
		const node_index_type lni = *li;
		STACKTRACE_INDENT_PRINT("    local node id " << lni << endl);
		const unique_process_subgraph::setup_constraint_key_type
			c = lni;
		const unique_process_subgraph::setup_constraint_set_type::const_iterator
			cf(pg.setup_constraints.find(c));
	if (cf != pg.setup_constraints.end()) {
		STACKTRACE_INDENT_PRINT("      found local setup constraint" << endl);
		const vector<setup_constraint_entry>& sc(cf->second);
		vector<setup_constraint_entry>::const_iterator
			si(sc.begin()), se(sc.end());
		for ( ; si!=se; ++si) {
			// local nodes to check
			STACKTRACE_INDENT_PRINT("        local trigger node "
				<< si->trig_node << endl);
			const node_index_type gti = translate_to_global_node(pid, si->trig_node);

			const time_type ft = current_time +si->time;
			STACKTRACE_INDENT_PRINT("        should change no earlier than "
				<< ft << endl);
			// pool-allocate a check to post
			const timing_exception tex(ni, gti, LOGIC_OTHER, si->dir, true,
				pid, si->time, setup_violation_policy);
#if 0
			const timing_check_index_type ti = timing_check_pool.allocate(tex);
			// schedule the new check ID at the trigger-node (map)
			active_timing_check_map[gti].insert(ti);
			// insert same ID into check-expiration-queue,
			//	on expiration, should dequeue, and remove from trigger-node
			timing_check_queue.insert(make_pair(ft, make_pair(ti, gti)));
#else
			register_timing_check(tex, ft);
#endif
		}	// end for local reference nodes
	}	// end if hold_constraints.find
	}	// end for-all constraints with common reference node
}	// end for-all involved processes
}
}	// end post_setup_check

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Node ni is the reference node of a constraint.
	Post a constraint check on the trigger node that expires
	after an elapsed time dictated by the constraint.  
 */
void
State::post_hold_check(const node_index_type ni, const value_enum nv) {
	STACKTRACE_INDENT_PRINT("there is hold check on node" << endl);
	const timing_constraint_process_map_type::const_iterator
		f(hold_check_map.find(ni));
if (f != hold_check_map.end()) {
	STACKTRACE_INDENT_PRINT("found processes with hold constraints" << endl);
	bool rise_fall[2];
	rise_fall[1] = nv != LOGIC_LOW;	// maybe rising
	rise_fall[0] = nv != LOGIC_HIGH;	// maybe falling
	const map<process_index_type, local_node_ids_type>& m(f->second);
	map<process_index_type, local_node_ids_type>::const_iterator
		i(m.begin()), e(m.end());
for ( ; i!=e; ++i) {
	// processes that own a hold constraint on this node
	const process_index_type pid = i->first;
	STACKTRACE_INDENT_PRINT("  process id " << pid << endl);
	const process_sim_state& ps(get_process_state(pid));
	const unique_process_subgraph& pg(ps.type());
	// possible multiple local occurrences
	local_node_ids_type::const_iterator
		li(i->second.begin()), le(i->second.end());
	for ( ; li!=le; ++li) {
		const node_index_type lni = *li;
		STACKTRACE_INDENT_PRINT("    local node id " << lni << endl);
		bool dir = false;
	do {
		STACKTRACE_INDENT_PRINT("    check dir = " << dir << endl);
	if (rise_fall[dir]) {
		const unique_process_subgraph::hold_constraint_key_type
			c(lni, dir);
		const unique_process_subgraph::hold_constraint_set_type::const_iterator
			cf(pg.hold_constraints.find(c));
	if (cf != pg.hold_constraints.end()) {
		STACKTRACE_INDENT_PRINT("      found local hold constraint" << endl);
		const vector<hold_constraint_entry>& sc(cf->second);
		vector<hold_constraint_entry>::const_iterator
			si(sc.begin()), se(sc.end());
		for ( ; si!=se; ++si) {
			// local nodes to check
			STACKTRACE_INDENT_PRINT("        local trigger node "
				<< si->trig_node << endl);
			const node_index_type gti = translate_to_global_node(pid, si->trig_node);

			const time_type ft = current_time +si->time;
			STACKTRACE_INDENT_PRINT("        should change no earlier than "
				<< ft << endl);
			// pool-allocate a check to post
			const timing_exception tex(ni, gti, LOGIC_OTHER, dir, false,
				pid, si->time, hold_violation_policy);
			const timing_check_index_type ti = timing_check_pool.allocate(tex);
			// schedule the new check ID at the trigger-node (map)
			active_timing_check_map[gti].insert(ti);
			// insert same ID into check-expiration-queue,
			//	on expiration, should dequeue, and remove from trigger-node
			timing_check_queue.insert(make_pair(ft, make_pair(ti, gti)));
		}	// end for local reference nodes
	}	// end if hold_constraints.find
	}	// end if check in this direction of reference node
		dir = !dir;
	} while (dir);	// 2x: once per direction
	}	// end for-all constraints with common reference node
}	// end for-all involved processes
}
}	// end post_hold_check

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expire overdue timing checks (clear of violation window).
 */
void
State::expire_timing_checks(void) {
	STACKTRACE_BRIEF;
	const timing_check_queue_type::const_iterator
		e(timing_check_queue.upper_bound(current_time));
	timing_check_queue_type::iterator i(timing_check_queue.begin());
	for ( ; i!=e; ) {
		STACKTRACE_INDENT_PRINT("expiring timing check at " <<
			i->first << endl);
		const timing_check_index_type tci = i->second.first;
		const node_index_type tni = i->second.second;
		active_timing_check_map[tni].erase(tci);
		timing_check_pool.deallocate(tci);
		const timing_check_queue_type::iterator j(i);
		++i;
		timing_check_queue.erase(j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Destructor-time, return resources.
 */
void
State::destroy_timing_checks(void) {
	STACKTRACE_BRIEF;
	const timing_check_queue_type::const_iterator
		e(timing_check_queue.end());
	timing_check_queue_type::iterator i(timing_check_queue.begin());
	for ( ; i!=e; ) {
		const timing_check_index_type tci = i->second.first;
		const node_index_type tni = i->second.second;
		active_timing_check_map[tni].erase(tci);
		timing_check_pool.deallocate(tci);
		const timing_check_queue_type::iterator j(i);
		++i;
		timing_check_queue.erase(j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: it may be possible to reconstruct this information from 
	the rest of the state, by examining times of last edges
	on reference nodes.  
 */
void
State::timing_exception::save(ostream& o) const {
	generic_exception::save(o);
	write_value(o, tvalue);
	write_value(o, reference);
	write_value(o, dir);
	write_value(o, is_setup);
	write_value(o, pid);
	write_value(o, min_delay);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::timing_exception::load(istream& i) {
	generic_exception::load(i);
	read_value(i, tvalue);
	read_value(i, reference);
	read_value(i, dir);
	read_value(i, is_setup);
	read_value(i, pid);
	read_value(i, min_delay);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// for checkpointing
void
State::save_active_timing_checks(ostream& o) const {
	// all we need is the timing_check_pool
	// timing_check_queue and active_timing_check_map can be reconstructed
// FIXME:
#if 0
	const size_t N = timing_check_queue.size();
	write_value(o, N);
	timing_check_queue_type::const_iterator
		qi(timing_check_queue.begin()), qe(timing_check_queue.end());
	for ( ; qi!=qe; ++qi) {
		write_value(o, qi->first);
//		timing_check_pool[qi->second.first].save(o);
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// for checkpointing
void
State::load_active_timing_checks(istream& i) {
// FIXME:
#if 0
	size_t N;
	read_value(i, N);
	size_t j = 0;
	for ( ; j<N; ++j) {
		time_type ft;
		read_value(i, ft);
#if 0
		timing_exception tex;
		tex.load(i);
		register_timing_check(tex, ft);		// reconstruct
#endif
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param tni trigger node to check for active constraints.
	\param nv new value of trigger node
 */
void
State::check_active_timing_constraints(const node_index_type tni,
		const value_enum nv) {
	timing_check_map_type::const_iterator
		f(active_timing_check_map.find(tni));
if (f != active_timing_check_map.end()) {
	const set<timing_check_index_type>& active(f->second);
	set<timing_check_index_type>::const_iterator
		ai(active.begin()), ae(active.end());
	for ( ; ai!=ae; ++ai) {
		timing_exception tex(timing_check_pool[*ai]);	// yes, copy
		tex.tvalue = nv;	// update trigger value
		if (tex.is_setup) {
			// is setup violation, trigger node is a clock
			// check for direction match on clock edge
			if (tex.dir ? (nv != LOGIC_LOW) : (nv != LOGIC_HIGH)) {
				handle_timing_exception(tex);
			}
		} else {
			// is hold time violation
			// direction of trigger node doesn't matter
			handle_timing_exception(tex);
		}
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#else	// !PRSIM_FWD_POST_TIMING_CHECKS
/**
	Node ni is the trigger node of a constraint.
	Look back at last time of transition of reference nodes.
 */
void
State::do_setup_check(const node_index_type ni, const value_enum nv) {
	STACKTRACE_INDENT_PRINT("there is setup check on node" << endl);
	const timing_constraint_process_map_type::const_iterator
		f(setup_check_map.find(ni));
if (f != setup_check_map.end()) {
	STACKTRACE_INDENT_PRINT("found processes with setup constraints" << endl);
	bool rise_fall[2];
	rise_fall[1] = nv != LOGIC_LOW;	// maybe rising
	rise_fall[0] = nv != LOGIC_HIGH;	// maybe falling
	const map<process_index_type, local_node_ids_type>& m(f->second);
	map<process_index_type, local_node_ids_type>::const_iterator
		i(m.begin()), e(m.end());
for ( ; i!=e; ++i) {
	// processes that own a setup constraint on this node
	const process_index_type pid = i->first;
	STACKTRACE_INDENT_PRINT("  process id " << pid << endl);
	const process_sim_state& ps(get_process_state(pid));
	const unique_process_subgraph& pg(ps.type());
	// possible multiple local occurrences
	local_node_ids_type::const_iterator
		li(i->second.begin()), le(i->second.end());
	for ( ; li!=le; ++li) {
		const node_index_type lni = *li;
		STACKTRACE_INDENT_PRINT("    local node id " << lni << endl);
		bool dir = false;
	do {
		STACKTRACE_INDENT_PRINT("    check dir = " << dir << endl);
	if (rise_fall[dir]) {
		const unique_process_subgraph::setup_constraint_key_type
			c(lni, dir);	// trigger node direction
		const unique_process_subgraph::setup_constraint_set_type::const_iterator
			cf(pg.setup_constraints.find(c));
	if (cf != pg.setup_constraints.end()) {
		STACKTRACE_INDENT_PRINT("      found local setup constraint" << endl);
		const vector<setup_constraint_entry>& sc(cf->second);
		vector<setup_constraint_entry>::const_iterator
			si(sc.begin()), se(sc.end());
		for ( ; si!=se; ++si) {
			// local nodes to check
			STACKTRACE_INDENT_PRINT("        local reference node "
				<< si->ref_node << endl);
			const node_index_type gri = translate_to_global_node(pid, si->ref_node);
			const time_type rt = get_node(gri).get_last_transition_time();
			STACKTRACE_INDENT_PRINT("        changed at time "
				<< rt << endl);
			const time_type d = current_time -rt;
			if (rt >= 0.0 && (d < si->time)) {
#define	E	setup_violation_policy
			if (E > ERROR_IGNORE) {
				const timing_exception
					tex(gri, ni, nv, dir, true, pid, si->time, E);
				handle_timing_exception(tex);
			}
#undef	E
		}	// end for local reference nodes
	}	// end if setup_constraints.find
	}	// end if rise_fall[dir]
		dir = !dir;
	} while (dir);	// 2x: once per direction
	}	// end for local node references
}	// end for processes
}	// end if check_setup_map
}	// end do_setup_check

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::do_hold_check(const node_index_type ni, const value_enum nv) {
	STACKTRACE_INDENT_PRINT("there is hold check on node" << endl);
	const timing_constraint_process_map_type::const_iterator
		f(hold_check_map.find(ni));
if (f != hold_check_map.end()) {
	STACKTRACE_INDENT_PRINT("found processes with hold constraints" << endl);
	const map<process_index_type, local_node_ids_type>& m(f->second);
	map<process_index_type, local_node_ids_type>::const_iterator
		i(m.begin()), e(m.end());
for ( ; i!=e; ++i) {
	// processes that own a hold constraint on this node
	const process_index_type pid = i->first;
	STACKTRACE_INDENT_PRINT("  process id " << pid << endl);
	const process_sim_state& ps(get_process_state(pid));
	const unique_process_subgraph& pg(ps.type());
	// possible multiple local occurrences
	local_node_ids_type::const_iterator
		li(i->second.begin()), le(i->second.end());
	for ( ; li!=le; ++li) {
		const node_index_type lni = *li;
		STACKTRACE_INDENT_PRINT("    local node id " << lni << endl);
		const unique_process_subgraph::hold_constraint_key_type
			c = lni;
		const unique_process_subgraph::hold_constraint_set_type::const_iterator
			cf(pg.hold_constraints.find(c));
	if (cf != pg.hold_constraints.end()) {
		STACKTRACE_INDENT_PRINT("      found local hold constraint" << endl);
		const vector<hold_constraint_entry>& sc(cf->second);
		vector<hold_constraint_entry>::const_iterator
			si(sc.begin()), se(sc.end());
		for ( ; si!=se; ++si) {
			// local nodes to check
			STACKTRACE_INDENT_PRINT("        local reference node "
				<< si->ref_node << endl);
			const node_index_type gri = translate_to_global_node(pid, si->ref_node);
			const time_type rt = get_node(gri).get_last_edge_time(
				si->dir ? LOGIC_HIGH : LOGIC_LOW);
			STACKTRACE_INDENT_PRINT("        changed at time "
				<< rt << endl);
			const time_type d = current_time -rt;
			if (rt >= 0.0 && (d < si->time)) {
#define	E	hold_violation_policy
			if (E > ERROR_IGNORE) {
				const timing_exception
					tex(gri, ni, nv, si->dir, false, pid, si->time, E);
				handle_timing_exception(tex);
			}
#undef	E
		}	// end for local reference nodes
	}	// end if hold_constraints.find
	}	// end for local node references
}	// end for processes
}	// end if check_hold_map
}	// end do_hold_check
#endif	// PRSIM_FWD_POST_TIMING_CHECKS
#endif
#endif	// PRSIM_SETUP_HOLD

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::handle_invariants(const event_cause_type& new_cause) {
	const node_index_type ni = new_cause.node;
	invariant_update_map_type::const_iterator
		ii(__invariant_update_map.begin()),
		ie(__invariant_update_map.end());
	for ( ; ii!=ie; ++ii) {
		const break_type E =
			__diagnose_invariant(cerr,
				ii->first.first, ii->first.second, 
				ii->second, new_cause.node, new_cause.val);
		if (UNLIKELY(E >= ERROR_BREAK)) {
			stop();
			if (UNLIKELY(E >= ERROR_INTERACTIVE)) {
				record_exception(exception_ptr_type(
					new invariant_exception(ni, E)));
			}
		}
	}
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::record_exception(const exception_ptr_type& p) const {
	STACKTRACE_BRIEF;
	recent_exceptions.push_back(p);
}

/**
	Print diagnostic messages for recent exceptions.
	Always use cerr?
 */
error_policy_enum
State::inspect_exceptions(void) const {
	STACKTRACE_BRIEF;
	error_policy_enum ret = ERROR_NONE;
	vector<exception_ptr_type>::const_iterator
		i(recent_exceptions.begin()), e(recent_exceptions.end());
	for ( ; i!=e; ++i) {
		NEVER_NULL(*i);
		error_policy_enum E = (*i)->inspect(*this, cerr);
		if (E > ret)
			ret = E;
	}
	return ret;
}

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
static
inline
void
__debug_print_node_change(const node_index_type ni,
		const pull_enum prev, const pull_enum next) {
	DEBUG_STEP_PRINT("node " << ni << " from " <<
		State::node_type::value_to_char[size_t(prev)] << " -> " <<
		State::node_type::value_to_char[size_t(next)] << endl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct State::eval_info {
	// process state corresponding to global expr index
	process_sim_state*			ps;
	// if this is NULL, then propagation stopped before root reached
	node_index_type				root_node;
	expr_index_type				root_expr;
	const expr_struct_type*			root_struct;
	bool					terminate_propagation;
	eval_info() : ps(NULL), root_node(INVALID_NODE_INDEX), 
		root_expr(INVALID_EXPR_INDEX), root_struct(NULL),
		terminate_propagation(false) { }
};

/**
	When propagation reaches a node,
	root_node refers the updated node,
	root_expr refers the the rule/expr that pulls that node.
 */
// inline
State::eval_info
State::evaluate_kernel(const expr_index_type gui, 
		pull_enum& prev, pull_enum& next) {
//	STACKTRACE_VERBOSE_STEP;
	DEBUG_STEP_PRINT("node " << ni << " from " <<
		node_type::translate_value_to_char(prev) << " -> " <<
		node_type::translate_value_to_char(next) << endl);
	eval_info ret;
	const expr_struct_type*& s(ret.root_struct);
	node_index_type& ui(ret.root_node);
	expr_index_type& ri(ret.root_expr);	// lags behind ui by 1 iteration
	expr_state_type* u;
	process_sim_state& ps(lookup_global_expr_process(gui));
	ret.ps = &ps;
	ui = ps.local_expr_index(gui);
	// evaluation always happens local to a process
	const unique_process_subgraph& pg(ps.type());

#define	STRUCT	s
do {
	pull_enum old_pull, new_pull;	// pulling state of the subexpression
	s = &pg.expr_pool[ui];
	u = &ps.expr_states[ui];
#define	PULL_ARG		*s
#if DEBUG_STEP
	DEBUG_STEP_PRINT("examining expression ID: " << ui << endl);
	STRUCT->dump_struct(STACKTRACE_INDENT, false) << endl;
		// direction doesn't matter, is not root
	u->dump_state(STACKTRACE_INDENT << "before: ", *s) << endl;
#endif
	// trust compiler to effectively perform branch-invariant
	// code-motion
	if (STRUCT->is_disjunctive()) {
		// is disjunctive (or)
		DEBUG_STEP_PRINT("is_or()" << endl);
		// countdown represents the number of 1's
		old_pull = u->or_pull_state(PULL_ARG);
		u->unknowns += (next >> 1) - (prev >> 1);
		u->countdown += (next & LOGIC_VALUE)
			- (prev & LOGIC_VALUE);
		new_pull = u->or_pull_state(PULL_ARG);
	} else {
		DEBUG_STEP_PRINT("is_and()" << endl);
		// is conjunctive (and)
		old_pull = u->and_pull_state(PULL_ARG);
		// countdown represents the number of 0's
		u->unknowns += (next >> 1) - (prev >> 1);
		u->countdown += !next - !prev;
		new_pull = u->and_pull_state(PULL_ARG);
	}	// end if
#if DEBUG_STEP
	u->dump_state(STACKTRACE_INDENT << "after : ", *s) << endl;
#endif
#undef	PULL_ARG
	if (old_pull == new_pull) {
		// then the pull-state did not change.
		DEBUG_STEP_PRINT("end of propagation." << endl);
		ret.terminate_propagation = true;
		return ret;	// break
	}
	// already accounted for negation in pull_state()
	// NOTE: cannot equate pull with value!
	prev = old_pull;
	next = new_pull;
	ri = ui;		// save previous index
	ui = STRUCT->parent;
} while (!STRUCT->is_root());
	// if this is reached, we made it to a root rule
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluates expression changes without propagating/generating events.  
	Useful for expression state reconstruction from checkpoint.  
	\return pair(root expression, new pull value) if event propagated
		to the root, else (INVALID_NODE_INDEX, whatever)
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
State::evaluate(
		// const node_index_type ni,
		const expr_index_type gui, 
		pull_enum prev, pull_enum next) {
	STACKTRACE_VERBOSE_STEP;
	evaluate_return_type ret;	// intended for only non-atomic nodes
	// note: prev, next are modified by *reference*
	const eval_info ei(evaluate_kernel(gui, prev, next));
	const node_index_type ui = ei.root_node;
	const bool terminate_propagation = ei.terminate_propagation;
if (!terminate_propagation) {
	const process_sim_state& ps(*ei.ps);
	const unique_process_subgraph& pg(ps.type());
	const expr_index_type ri = ei.root_expr;
	const expr_struct_type* s = ei.root_struct;
	// need to check because of 'break' above
	DEBUG_STEP_PRINT("propagated to root rule" << endl);
	// made it to root
	// negation already accounted for
	const rule_index_type lri = pg.rule_map.find(ri)->second;
		// expr -> rule
	const rule_type& r(pg.rule_pool[lri]);
	const process_index_type pid = lookup_process_index(ps);
if (!r.is_invariant()) {
	// new: remember to update node-fanin state structure!
	const node_index_type oni = translate_to_global_node(pid, ui);
	// local -> global node
	node_type& n(__get_node(oni));
	const pull_set ops(n, weak_rules_enabled());
	const bool dir = r.direction();
#if PRSIM_WEAK_RULES
	const bool is_weak = r.is_weak();
#endif
if (UNLIKELY(n.is_atomic())) {
	// atomic nodes are only defined by pull-up
	INVARIANT(dir);
#if PRSIM_WEAK_RULES
	INVARIANT(!is_weak);
#endif
	// the (nonexistent) pull-dn is effectively the opposite of pull-up
	// when one is on, the other is off, and vice versa
	// iterate over fanout here and enqueue into worklist
	// update this node's value immediately and evaluate its fanouts
	// yes, interpret pull_enum as value_enum
	__atomic_updated_nodes.insert(std::make_pair(oni, n.current_value()));
	// ignore return value, first old-value is the correct one,
	// in case multiple paths lead to updating the same node.
}
	fanin_state_type& fs(n.get_pull_struct(dir
#if PRSIM_WEAK_RULES
		, is_weak
#endif
		));
	// root rules of a node are disjunctive (OR-combination)
#if DEBUG_STEP
	fs.dump_state(STACKTRACE_INDENT << "before : ") << endl;
#endif
	const pull_enum old_pull = fs.pull();
	// because 2nd bit (bit[1]) represents X
	// and 1st bit (bit[1]) represents 0,1 value
	fs.unknowns += (next >> 1) - (prev >> 1);
	fs.countdown += (next & LOGIC_VALUE) - (prev & LOGIC_VALUE);
	const pull_enum new_pull = fs.pull();
#if DEBUG_STEP
	fs.dump_state(STACKTRACE_INDENT << "after : ") << endl;
#endif
	if (old_pull == new_pull) {
		// then the pull-state did not change.
		DEBUG_STEP_PRINT("end of propagation." << endl);
	} else {
		next = fs.pull();
		ret = evaluate_return_type(oni, STRUCT, next,
			ops, &r, ps.global_expr_index(ri));
	}
} else {	// else this is an invariant
	DEBUG_STEP_PRINT("participating in invariant" << endl);
	// then this rule doesn't actually pull a node, is an invariant
	// aggregate updates before diagnosing invariant violation
	const rule_reference_type rr(pid, ri);
	__invariant_update_map[rr] = next;
	// only the last one will take effect
}
#undef	STRUCT
}	// end if !terminate_propagation
// else propagation did not reach root, just stop

	return ret;
}	// end State::evaluate()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param pi global process index
	\param ri local rule index
	\param next new state of invariant expression pull
	\param ni the causing node
	\param nval the causing node value
	\return error status of evaluated invariant expression
 */
error_policy_enum
State::__diagnose_invariant(ostream& o, const process_index_type pid,
		const rule_index_type ri, const pull_enum next,
		const node_index_type ni, const value_enum nval) const {
	const bool fail = (next == PULL_OFF);
	const bool maybe = (next == PULL_WEAK);
	const error_policy_enum err = fail ? invariant_fail_policy :
		maybe ? invariant_unknown_policy : ERROR_NONE;
	if (err != ERROR_IGNORE) {
		const bool halt = (err >= ERROR_BREAK);
		const string pn(get_process_canonical_name(pid));
		o << (halt ? "Error: " : "Warning: ") <<
			(maybe ? "possible " : "" ) <<
			"invariant violation in " << pn << ": (";
		const process_sim_state& ps(get_process_state(pid));
		const unique_process_subgraph& pg(ps.type());
		ps.dump_subexpr(o, ri, *this, true);	// always verbose
		dump_node_canonical_name(o << ") by node ", ni) << ':' <<
			node_type::translate_value_to_char(nval);
		pg.dump_invariant_message(o, ri, ", \"", "\"") << endl;
	}
	return err;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Finalizes pointer links to unique process types, 
	because allocation only set back links as indices.  
 */
void
State::finish_process_type_map(void) {
	STACKTRACE_VERBOSE;
	process_state_array_type::iterator
		i(process_state_array.begin()), e(process_state_array.end());
	// replaces index union member with pointer union member
	for ( ; i!=e; ++i) {
		const size_t j = i->get_index();
		const unique_process_subgraph& g(unique_process_pool[j]);
		STACKTRACE_INDENT_PRINT("type " << j << " @ " << &g << endl);
		i->set_ptr(g);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Computes index based on address.  
	\param s must be an element of the process_state_array.
 */
process_index_type
State::lookup_process_index(const process_sim_state& s) const {
	return std::distance(&process_state_array[0], &s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct process_sim_state_base::offset_comparator {
	bool
	operator () (const process_index_type l, 
			const process_sim_state_base& r) {
		return l < r.global_expr_offset;
	}
};	// end struct offset_comparator

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
#if PRSIM_PROCESS_EXPR_MAP_ARRAY
struct State::expr_id_key_compare {
	bool
	operator () (const expr_index_type e, 
			const expr_process_entry_type& p) const {
		return e < p.first;
	}
};
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param gei global expression index
	\return pair: local process expression index offset, process ID
 */
// inline
const process_sim_state&
State::lookup_global_expr_process(const expr_index_type gei) const {
	STACKTRACE_VERBOSE_LOOKUP;
	DEBUG_LOOKUP_PRINT("global-expr: " << gei << endl);
	INVARIANT(gei);
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
#if PRSIM_PROCESS_EXPR_MAP_ARRAY
	global_expr_process_id_map_type::const_iterator
		f(upper_bound(global_expr_process_id_map.begin(), 
			global_expr_process_id_map.end(), gei, 
			expr_id_key_compare()));
#else
	global_expr_process_id_map_type::const_iterator
		f(global_expr_process_id_map.upper_bound(gei));
#endif
	INVARIANT(f != global_expr_process_id_map.begin());
	--f;
	DEBUG_LOOKUP_PRINT("pid = " << f->second << endl);
	return process_state_array[f->second];
#else
	process_state_array_type::const_iterator
		e(process_state_array.end()),
		f(std::upper_bound(process_state_array.begin(), e, gei, 
			process_sim_state_base::offset_comparator()));
	INVARIANT(f != e);
	--f;
	return *f;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Modifiable version.
 */
process_sim_state&
State::lookup_global_expr_process(const expr_index_type gei) {
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
#if PRSIM_PROCESS_EXPR_MAP_ARRAY
	global_expr_process_id_map_type::const_iterator
		f(upper_bound(global_expr_process_id_map.begin(), 
			global_expr_process_id_map.end(), gei, 
			expr_id_key_compare()));
#else
	global_expr_process_id_map_type::const_iterator
		f(global_expr_process_id_map.upper_bound(gei));
#endif
	INVARIANT(f != global_expr_process_id_map.begin());
	--f;
	return process_state_array[f->second];
#else
	process_state_array_type::iterator
		e(process_state_array.end()),
		f(std::upper_bound(process_state_array.begin(), e, gei, 
			process_sim_state_base::offset_comparator()));
	INVARIANT(f != e);
	--f;
	return *f;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Given a reference to a process state object, translate a local node
	index to the global node index using corresponding footprint frame.  
	\param ps process-state, must belong to the process_state_array member
	\param lni local node index, local to the unique_process_subgraph,
		1-based
	\return global node index, 1-based
 */
node_index_type
State::translate_to_global_node(const process_sim_state& ps, 
		const node_index_type lni) const {
	// HACK: poor style, using pointer arithmetic to deduce index! :(
	const process_index_type pid =
		std::distance(&process_state_array[0], &ps);
	return translate_to_global_node(pid, lni);
}

node_index_type
State::translate_to_global_node(const process_index_type pid, 
		const node_index_type lni) const {
	STACKTRACE_VERBOSE_LOOKUP;
	DEBUG_LOOKUP_PRINT("lni = " << lni << endl);
	// HACK: poor style, using pointer arithmetic to deduce index
	ISE_INVARIANT(pid < process_state_array.size());
	// no longer need special case for pid=0, b/c frame is identity
	const footprint_frame_map_type& bfm(get_footprint_frame_map(pid));
#if DEBUG_LOOKUP
	copy(bfm.begin(), bfm.end(),
		std::ostream_iterator<size_t>(
			DEBUG_LOOKUP_PRINT("translate bfm: "), ","));
	STACKTRACE_STREAM << endl;
#endif
	INVARIANT(lni < bfm.size());
	const size_t ret = bfm[lni];
	DEBUG_LOOKUP_PRINT("gni = " << ret << endl);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main expression evaluation method, ripped off of
	old prsim's propagate_up.  
	As expression/rule updates reach root nodes, events are spawned
	and enqueued to one of the event queues, and eventually
	graduate to the primary event queue.
	\param c is the event that triggered these expressions re-evaluation.  
	\param ni the index of the node causing this propagation (root),
		only used for diagnostic purposes.
	\param ui the index of the sub expression being evaluated, 
		this is already a parent expression of the causing node, 
		unlike original prsim.  
		Locally, this is used as the index of the affected node.  
		NOW, THIS IS A LOCAL EXPRESSION INDEX, relative to pid
	\param exi is the global expression index to update.  
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
		const pull_enum prev
		) {
	STACKTRACE_VERBOSE_STEP;
	const node_index_type& ni(c.node);
	// when evaluating node as expression, interpret value as pull
	const pull_enum nextval = pull_enum(c.val);
	__debug_print_node_change(ni, prev, nextval);
	const evaluate_return_type
		ev_result(evaluate(exi, prev, nextval));
	if (ev_result.invariant_break >= ERROR_BREAK) {
		// then violation is not a result of a real rule
		// thus, there can be no change or addition of events
		// return invariant_fail_policy;
		return ev_result.invariant_break;
		// return true;
	}
	if (!ev_result.node_index) {
		return ERROR_NONE;
		// return false;
	}
	const pull_set& ops(ev_result.prev_pull);
	const pull_enum next = ev_result.root_pull;
	const node_index_type ui = ev_result.node_index;
	// we delay the root rule search until here to reduce the amount
	// of searching required to find the responsible rule expression.  
	rule_index_type root_rule;
	root_rule = ev_result.root_rule_index;
	DEBUG_STEP_PRINT("root_rule: " << root_rule << endl);
	ISE_INVARIANT(root_rule);
	const bool dir = ev_result.root_rule->direction();
// propagation made it to the root node, indexed by ui (now node_index_type)
	node_type& n(__get_node(ui));
	DEBUG_STEP_PRINT("propagated to output node: " <<
		get_node_canonical_name(ui) << " with pull state " <<
		size_t(next) << endl);
	// frozen nodes will not switch when expressions propagate to their root
	break_type err = ERROR_NONE;
	if (n.is_atomic()) {
		// possible that updated_nodes/queue is !empty, due to fanout
		// so we must save it aside, restore it after done with atomic update
		const swap_saver<updated_nodes_type> swp1(updated_nodes);
#if PRSIM_FCFS_UPDATED_NODES
		const swap_saver<updated_nodes_queue_type> swp2(updated_nodes_queue);
#endif
		// yes, interpret pull_enum as value_enum
		// const step_return_type sr = // ignore return value
		set_node_immediately(ui, value_enum(next), false);
		// TODO: how to handle multiple atomic updates in a single step?
		// especially for watching nodes?
		return err;
	}
#if PRSIM_UPSET_NODES
	if (n.is_frozen()) {
		// even if new pull_state is off
		if (is_frozen_verbose() && (next != PULL_OFF)) {
			cout << "Firing for frozen node `" <<
				get_node_canonical_name(ui) <<
				"\' suppressed." << endl;
		}
		return err;
	}
#endif
	const pull_set p(n, weak_rules_enabled());
	if (keeper_check_fail_policy > ERROR_IGNORE && p.state_holding()) {
		__keeper_check_candidates.insert(ui);
	}
#if PRSIM_WEAK_RULES
	const bool is_weak = ev_result.root_rule->is_weak();
	DEBUG_STEP_PRINT("updated root is" << (is_weak ? " " : " not") << " weak" << endl);
	const event_index_type ei = n.get_event();
#if DEBUG_STEP
	if (ei) {
		dump_event(cerr << "pending:\t", ei, 0.0) << endl;
		const event_type& e(get_event(ei));
		STACKTRACE_INDENT_PRINT("e.node = " << e.node <<
			" vs. ui = " << ui << endl);
		INVARIANT(e.node == ui);
	}
#endif
	// weak rule pre-filtering
if (weak_rules_enabled()) {
if (n.pending_event()) {
	event_type& e(get_event(ei));	// previous scheduled event
	if (e.is_weak() && !is_weak && next != PULL_OFF) {
		DEBUG_STEP_PRINT("old weak event killed" << endl);
		// it was weak, and should be overtaken
		// what if new event is weak-off?
		if (e.val != LOGIC_OTHER) {
			const break_type E =
			__report_instability(cout,
				next == PULL_WEAK, 
				e.val == LOGIC_HIGH,
#if PRSIM_MK_EXCL_BLOCKING_SET
				false,	// unrelated to excl
#endif
				e);
			if (E > err) err = E;
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
		// Q: is event already in main event queue? flush?
		return err;	// no error
	} else {
		DEBUG_STEP_PRINT("old weak event kept" << endl);
	}
} else {	// no pending event
	if (is_weak &&
			// old (broken) -- wrongly suppressed firing
			!p.normal_rules_off()
		) {
		// also account for case where weak-1 turns off and yields
		// to a strong-X in same direction
		if ((dir &&
			((p.cutoff_up() && !p.cutoff_dn()) ||
			(p.weak_pulling_up_x() && p.dn == PULL_WEAK)))
		|| (!dir &&
			((p.cutoff_dn() && !p.cutoff_up()) ||
			(p.weak_pulling_dn_x() && p.up == PULL_WEAK)))
		) {
		// check for yielding to opposition
		DEBUG_STEP_PRINT("weak event preserved" << endl);
		} else {
		DEBUG_STEP_PRINT("weak event suppressed" << endl);
		// drops weak-firings overpowered by strong on rules
		// regardless of what 'next' weak pull is
		return err;	// no error
		}
	} else {
		DEBUG_STEP_PRINT("weak event preserved" << endl);
	}
}
} else if (is_weak) {
	DEBUG_STEP_PRINT("weak rules off" << endl);
	return ERROR_NONE;
}	// end if weak_rules_enabled
#endif	// PRSIM_WEAK_RULES
if (dir) {
	// pull-up
	// re-evaluate queue
	node_update_info nui;
	nui.rule_index = root_rule;
#if EVENT_INCLUDE_RULE_POINTER
	nui.rule_ptr = ev_result.root_rule;
#endif
	const pair<updated_nodes_type::iterator, bool>
		pp(updated_nodes.insert(
			updated_nodes_type::value_type(ui, nui)));
	if (!pp.second) {
		if ((!is_weak && p.up != PULL_OFF && ops.up != PULL_ON) ||
			(is_weak && p.wup != PULL_OFF && ops.wup != PULL_ON)) {
			pp.first->second.rule_index = root_rule;
#if EVENT_INCLUDE_RULE_POINTER
			pp.first->second.rule_ptr = ev_result.root_rule;
#endif
		// TODO: determine rule precedence for causality
		// whichever turned on first? strength?
		// the root rule that is assigned is the one whose
		// delay value will be used!
		// In the event of weak-interference, favor the pull
		// rule side that *increased* in strength, 0->X, 0->1, X->1,
		// that turned partially or fully on.
		}
	}
#if PRSIM_FCFS_UPDATED_NODES
	else {
		// first time added
		updated_nodes_queue.push_back(ui);
		pp.first->second.old_pull_set = ops;
	}
#endif
} else {
	// pull-dn
	// re-evaluate queue
	node_update_info nui;
	nui.rule_index = root_rule;
#if EVENT_INCLUDE_RULE_POINTER
	nui.rule_ptr = ev_result.root_rule;
#endif
	const pair<updated_nodes_type::iterator, bool>
		pp(updated_nodes.insert(
			updated_nodes_type::value_type(ui, nui)));
	if (!pp.second) {
		if ((!is_weak && p.dn != PULL_OFF && ops.dn != PULL_ON) ||
			(is_weak && p.wdn != PULL_OFF && ops.wdn != PULL_ON)) {
			pp.first->second.rule_index = root_rule;
#if EVENT_INCLUDE_RULE_POINTER
			pp.first->second.rule_ptr = ev_result.root_rule;
#endif
		// In the event of weak-interference, favor the pull
		// rule side that *increased* in strength, 0->X, 0->1, X->1,
		// that turned partially or fully on.
		}
	}
#if PRSIM_FCFS_UPDATED_NODES
	else {
		// first time added
		updated_nodes_queue.push_back(ui);
		pp.first->second.old_pull_set = ops;
	}
#endif
}	// end if (u->direction())
	return err;
}	// end method propagate_evaluation

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::__report_cause(ostream& o, cause_arg_type cause) const {
	const node_index_type& ni(cause.node);
	if (ni) {
		dump_node_canonical_name(o << ">> cause: `", ni)
			<< "\' (val: ";
		get_node(ni).dump_value(o) << ')' << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Report a node with interfering fanin rules.  
	Diagnostics may be suppresed by the may-interfere, and 
	may-weak-interfere node attributes.
	\param weak is true if interference is only *possible*, but not definite
	\param _ni the affected output node.
	\return true if error causes break in events.  
 */
State::break_type
State::__report_interference(ostream& o, const bool weak, 
		const node_index_type _ni, cause_arg_type c) const {
	const node_type& _n(get_node(_ni));
	if (weak) {
	if (weak_interference_policy != ERROR_IGNORE &&
			!_n.may_weak_interfere()) {
		dump_node_canonical_name(o << "WARNING: weak-interference `", 
			_ni) << "\'" << endl;
		__report_cause(o, c);
		return weak_interference_policy;	// >= ERROR_BREAK;
	}	// endif weak_interference_policy
	} else {	// !weak
	if (interference_policy != ERROR_IGNORE &&
			!_n.may_interfere()) {
		dump_node_canonical_name(o << "WARNING: interference `", _ni)
			<< "\'" << endl;
		__report_cause(o, c);
		return interference_policy;		// >= ERROR_BREAK;
	}	// endif interference_policy
	}	// endif weak
	return ERROR_NONE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param weak is true if unstable was *possible*, i.e. caused by X
	\param dir the direction of the unstable firing
	\param excl is true if instabilities was caused by force-exclusion.
	\param ev the unstable event
	\return true if error causes break in events.  
	If node is flagged unstable, 
 */
State::break_type
State::__report_instability(ostream& o, const bool weak, const bool dir, 
#if PRSIM_MK_EXCL_BLOCKING_SET
		const bool excl,
#endif
		const event_type& ev) const {
#if EVENT_INCLUDE_RULE_POINTER
	const rule_type* const r = ev.cause_rule_ptr;
#else
	const rule_type* const r = lookup_rule(ev.cause_rule);
#endif
	const node_index_type& _ni(ev.node);	// the scheduled node
//	INVARIANT(ev.val != LOGIC_OTHER);		// not true
//	INVARIANT(dir == (ev.val == LOGIC_HIGH));	// not true
if (!r || !r->is_unstable()) {
#if PRSIM_MK_EXCL_BLOCKING_SET
	if (excl) {
	if (excl_unstable_policy != ERROR_IGNORE) {
		dump_node_canonical_name(o << "WARNING: excl-unstable `",
			_ni) << "\'" << (dir ? '+' : '-') << endl;
		// TODO: print ring(s) that were involved
	if (r) dump_rule(o << "rule: ", ev.cause_rule, true, false) << endl;
		__report_cause(o, ev.cause);
		return excl_unstable_policy;
	}
	} else
#endif
	if (weak) {
	if (weak_unstable_policy != ERROR_IGNORE) {
		dump_node_canonical_name(o << "WARNING: weak-unstable `",
			_ni) << "\'" << (dir ? '+' : '-') << endl;
	if (r) dump_rule(o << "rule: ", ev.cause_rule, true, false) << endl;
		__report_cause(o, ev.cause);
		return weak_unstable_policy;		// >= ERROR_BREAK;
	}	// endif weak_unstable_policy
	} else {	// !weak
	if (unstable_policy != ERROR_IGNORE) {
		dump_node_canonical_name(o << "WARNING: unstable `", _ni)
			<< "\'" << (dir ? '+' : '-') << endl;
	if (r) dump_rule(o << "rule: ", ev.cause_rule, true, false) << endl;
		__report_cause(o, ev.cause);
		return unstable_policy;			// >= ERROR_BREAK;
	}	// endif unstable_policy
	}	// endif weak
}
	return ERROR_NONE;
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
	\param ui the node involved in event e
	\param c the node/event that just fired and caused this violation.
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
	break_type err = ERROR_NONE;
	// something is amiss!
	// these tables are probably wrong when involving weak-rules
	const uchar eu = dir ?
		event_type::upguard[size_t(next)][size_t(e.val)] :
		event_type::dnguard[size_t(next)][size_t(e.val)];
	DEBUG_STEP_PRINT("event_update = " << size_t(eu) << endl);
	const bool vacuous = eu & event_type::EVENT_VACUOUS;
	if (!vacuous) {
		DEBUG_STEP_PRINT("not vacuous" << endl);
		// then must be unstable or interfering (exclusive)
		const bool instability =
			(eu & event_type::EVENT_UNSTABLE)
#if PRSIM_WEAK_RULES
			&& !(weak && !e.is_weak())
			// is not instability if original event was strong
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
			DEBUG_STEP_PRINT("instablity" << endl);
			e.set_cause_node(ni);
#if EVENT_INCLUDE_RULE_POINTER
			const rule_type* const r = e.cause_rule_ptr;
#else
			const rule_type* const r = lookup_rule(e.cause_rule);
#endif
			if ((dequeue_unstable_events() ||
					(r && r->is_unstable())) &&
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
				const error_policy_enum E = next == PULL_WEAK ?
					weak_unstable_policy : unstable_policy;
				if (E >= ERROR_INTERACTIVE) {
					record_exception(exception_ptr_type(
						new instability_exception(ni, E)));
				}
			}
		}
		// check for interference when processing updated nodes
		// diagnostic message
		// suppress message for interferences until pending queue
		if (instability) {
#if PRSIM_WEAK_RULES
			bool b = !weak;
			if (!b) {
				const pull_set p(n, weak_rules_enabled());
			if (dir) {
				b = (p.up != PULL_ON);
			} else {
				b = (p.dn != PULL_ON);
			}
			}
			if (b) {
#endif
			const break_type E =
			__report_instability(o, eu & event_type::EVENT_WEAK, 
				dir,
#if PRSIM_MK_EXCL_BLOCKING_SET
				false,	// unrelated to excl
#endif
				e);
			if (E > err) err = E;
#if PRSIM_WEAK_RULES
			}
#endif
		}	// end if unstable
	} else {
		// end if !vacuous
		DEBUG_STEP_PRINT("vacuous" << endl);
		// check for interference when processing updated nodes
	}
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
	STACKTRACE_VERBOSE;
	DEBUG_STEP_PRINT("flags: " << std::hex << flags << endl);
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
	node_type& n(__get_node(ni));
	n.set_watchpoint();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Removes node from the watchlist.  
	Q: if node is set as a breakpoint while it is watched, 
		but then unwatched, will it undo the effect of the breakpoint?
 */
void
State::unwatch_node(const node_index_type ni) {
	node_type& n(__get_node(ni));
	n.clear_watchpoint();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
State::is_watching_node(const node_index_type ni) const {
	return get_node(ni).is_watchpoint();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Clears the watch-list, restoring nodes' former breakpoint states.  
 */
void
State::unwatch_all_nodes(void) {
	typedef	node_pool_type::iterator		iterator;
	iterator i(node_pool.begin()), e(node_pool.end());
	for (++i; i!=e; ++i) {
		i->clear_watchpoint();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints list of explicitly watched nodes.  
	Doesn't count watchall flag.  
	TODO: show values
 */
ostream&
State::dump_watched_nodes(ostream& o) const {
	o << "watched nodes: ";
	typedef	node_pool_type::const_iterator		const_iterator;
	const_iterator i(node_pool.begin()), e(node_pool.end());
	for (++i; i!=e; ++i) {
	if (i->is_watchpoint()) {
		dump_node_canonical_name(o, *i) << ' ';
	}
	}
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param val LOGIC_{LOW,HIGH,OTHER}.  
	\param nl use newline delimiter instead of space.
 */
void
State::status_nodes(const value_enum val, 
		vector<node_index_type>& nodes) const {
	ISE_INVARIANT(node_type::is_valid_value(val));
	find_nodes(nodes, bind2nd(mem_fun_ref(&node_type::match_value), val));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::print_status_nodes(ostream& o, const value_enum val,
		const bool nl) const {
	vector<node_index_type> nodes;
	o << node_type::translate_value_to_char(val) << " nodes:" << endl;
	status_nodes(val, nodes);
	print_nodes(o, nodes, false, nl ? "\n" : " ");
	return o << endl;	// TODO: only if !nl, else flush
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::status_interference(const bool w, 
		vector<node_index_type>& nodes) const {
	find_nodes(nodes,
		mem_fun_ref(w ?
			&node_type::weak_interfering :
			&node_type::interfering));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param w if true, report weak-interference, else only report
		strong interference.  
 */
ostream&
State::print_status_interference(ostream& o, const bool w) const {
	vector<node_index_type> nodes;
	o << "Nodes with " << (w ? "weak-" : "") << "interference:" << endl;
	status_interference(w, nodes);
	print_nodes(o, nodes, false, "\n");
	return o << std::flush;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks at the state of pull up/dn, not current value.
	Interfering nodes are driven.
	\param p the pull_state to match.
	\param fanin_only true to only report nodes with fanin.
 */
void
State::status_driven(const pull_enum p,
		const bool fanin_only, 
		vector<node_index_type>& nodes) const {
	find_nodes(nodes,
		mem_fun_ref(
		(p == PULL_OFF) ? &node_type::undriven :
		(p == PULL_WEAK) ? &node_type::x_driven :
			&node_type::driven));
	if (fanin_only) {
		filter_nodes(nodes, mem_fun_ref(&node_type::has_fanin));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if asserting empty and assertion failed.
 */
bool
State::print_status_driven(ostream& o, const pull_enum p,
		const bool fanin_only, const bool assert_empty) const {
	bool err = false;
	vector<node_index_type> nodes;
	status_driven(p, fanin_only, nodes);
	if (assert_empty && !nodes.empty()) {
		o << "Assertion failed: there are ";
		err = true;
	}
switch (p) {
case PULL_OFF:
	o << "Nodes undriven (state-holding):" << endl;
	break;
case PULL_WEAK:
	o << "Nodes driven by X:" << endl;
	break;
// case PULL_ON:
default:
	o << "Nodes driven:" << endl;
	break;
}
	print_nodes(o, nodes, false, "\n");
	o << std::flush;
	return err;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_UPSET_NODES

void
State::status_frozen(vector<node_index_type>& nodes) const {
	find_nodes(nodes, mem_fun_ref(&node_type::is_frozen));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print all nodes that are frozen (no activity).
 */
ostream&
State::print_status_frozen(ostream& o, const bool v) const {
	vector<node_index_type> nodes;
	status_frozen(nodes);
	o << "Nodes frozen:" << endl;
	print_nodes(o, nodes, v, "\n");
	return o << std::flush;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::print_watched_atomic_updated_nodes(ostream& o) const {
	atomic_updated_nodes_type::const_iterator
		i(__atomic_updated_nodes.begin()),
		e(__atomic_updated_nodes.end());
for ( ; i!=e; ++i) {
	const node_type& n(get_node(i->first));
	if (n.is_watchpoint() || watching_all_nodes()) {
	if (n.current_value() != i->second) {
		const string nodename(get_node_canonical_name(i->first));
		format_ostream_ref(o << '\t', time_fmt) << time() << '\t';
		n.dump_value(o << nodename << " : ");
	// FIXME: causality tracking for atomic nodes and expressions
#if 0
		const node_index_type ci = GET_CAUSE(r);
		if (ci && s.show_cause()) {
			const string causename(s.get_node_canonical_name(ci));
			const State::node_type& c(s.get_node(ci));
			c.dump_value(o << "\t[by " << causename << ":=") << ']';
		}
		if (s.show_tcounts()) {
			o << "\t(" << n.tcount << " T)";
		}
#else
		o << "\t<atomic>";
#endif
		o << endl;
	}
	}
}
	return o;
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
/**
	Report all violations of invariants.  
	\return true if there are any invariant violations.  
 */
bool
State::check_all_invariants(ostream& o) const {
	return std::accumulate(process_state_array.begin(),
		process_state_array.end(), false,
		process_sim_state::invariant_checker(o, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_invariants(ostream& o, const process_index_type pid, 
		const bool v) const {
	return process_state_array[pid].dump_invariants(o, *this, v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_all_invariants(ostream& o, const bool v) const {
	for_each(process_state_array.begin(),
		process_state_array.end(), 
		process_sim_state::invariant_dumper(o, *this, v));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Structural assertions.  
	TODO: run-time flag to enable/disable calls to this.  
 */
void
State::check_structure(void) const {
	STACKTRACE_VERBOSE;
	for_each(unique_process_pool.begin(), unique_process_pool.end(),
		mem_fun_ref(&unique_process_subgraph::check_structure));
	// assert size consistency between each process_sim_state and its type
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i global node index.  
	\return string of the canonical node name.  
 */
ostream&
State::dump_node_canonical_name(ostream& o, const node_index_type i) const {
	return mod.get_footprint().dump_canonical_name<bool_tag>(o, i-1, 
		_dump_flags);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_node_canonical_name(ostream& o, const node_type& n) const {
	return dump_node_canonical_name(o,
		distance(&node_pool[0], &n));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i global node index.  
	\return string of the canonical node name.  
 */
string
State::get_node_canonical_name(const node_index_type i) const {
	ostringstream oss;
	dump_node_canonical_name(oss, i);
	return oss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i 1-based global process index.
 */
ostream&
State::dump_process_canonical_name(ostream& o, 
		const process_index_type i) const {
if (i) {
	ISE_INVARIANT(i < process_state_array.size());
	// footprint::dump_canonical_name expects 0-based index.
	return mod.get_footprint().dump_canonical_name<process_tag>(o, i-1, 
		_dump_flags);
} else {
	return o << "[top-level]";
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_process_canonical_name(ostream& o, 
		const process_sim_state& s) const {
	return dump_process_canonical_name(o, lookup_process_index(s));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
State::get_process_canonical_name(const process_index_type i) const {
if (i) {
	ostringstream oss;
	dump_process_canonical_name(oss, i);
	return oss.str();
} else {
	// pid=0 corresponds to top-level
	static const string top("[top-level]");
	return top;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_struct(ostream& o) const {
{
	o << "Nodes: " << endl;
	const node_index_type nodes = node_pool.size();
	node_index_type i = FIRST_VALID_GLOBAL_NODE;
	for ( ; i<nodes; ++i) {
		o << "node[" << i << "]: \"";
		dump_node_canonical_name(o, i);
		node_pool[i].dump_struct(o << "\" ") << endl;
	}
}
{
	o << "Unique processes: {" << endl;
	process_index_type p = FIRST_VALID_PROCESS;	// 0 is top-level type
	unique_process_pool_type::const_iterator
		i(unique_process_pool.begin()), e(unique_process_pool.end());
	for ( ; i!=e; ++i, ++p) {
		o << "type[" << p << "]: {" << endl;
		i->dump_struct(o) << "}" << endl;
	}
	o << "}" << endl;
}
	// print maps (debug only?)
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
	o << "map: global-expr-id -> process-id" << endl;
	dump_pair_map(o, global_expr_process_id_map);
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This prints out the netlist of nodes and expressions
	in dot-form for visualization.  
	Good for visualizing a decent 2D cell/wire/transistor placement.
 */
ostream&
State::dump_struct_dot(ostream& o) const {
	STACKTRACE_VERBOSE;
	o << "digraph G {" << endl;
{
	o << "# nodes: " << endl;
	const node_index_type nodes = node_pool.size();
	// box or plaintext
	o << "node [shape=box, fillcolor=white];" << endl;
	node_index_type i = FIRST_VALID_GLOBAL_NODE;
	for ( ; i<nodes; ++i) {
		ostringstream oss;
		oss << "NODE_" << i;
		const string& s(oss.str());
		o << s;
		o << "\t[label=\"";
		dump_node_canonical_name(o, i)
			<< "\"];" << endl;
		node_pool[i].dump_fanout_dot(o, s) << endl;
	}
}
{
	o << "# Processes: " << endl;
	process_state_array_type::const_iterator
		i(process_state_array.begin()), e(process_state_array.end());
	size_t pid = 0;
	for ( ; i!=e; ++i, ++pid) {
		const unique_process_subgraph& pg(i->type());
		if (pg.expr_pool.size()) {
			const footprint_frame_map_type&
				bfm(get_footprint_frame_map(pid));
			dump_process_canonical_name(o << "# [" << pid << "]: ",
				pid) << endl;
			pg.dump_struct_dot(o, i->get_offset(), bfm);
		}
	}
}
	return o << "}" << endl;
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
//	o << '[' << ei << ']';		// for debugging
	if (!ev.killed() || force) {
		format_ostream_ref(o << '\t', time_fmt) << t << '\t';
		dump_node_canonical_name(o, ev.node) <<
			" : " << node_type::translate_value_to_char(ev.val);
		if (ev.cause.node) {
			dump_node_canonical_name(o << '\t' << "[from ",
				ev.cause.node) << ":=" <<
			node_type::translate_value_to_char(ev.cause.val) << "]";
		}
#if PRSIM_WEAK_RULES
		if (ev.is_weak()) { o << '\t' << "(weak)"; }
#endif
		if (ev.killed()) {
			o << '\t' << "(killed)";
		}
#if DEBUG_STEP
		o << "\t#" << ev.cause_rule;
#endif
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
		dump_node_canonical_name(o << "No event pending on `", ni)
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
	n.dump_value(dump_node_canonical_name(o, ni) << " : ");
	return o;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints a rule by expression/rule index.
	\param ri global expression index, that corresponds to a rule.
	\param v true for verbose printing.  
 */
ostream&
State::dump_rule(ostream& o, const expr_index_type ri, const bool v, 
		const bool multi_fanin) const {
	const process_sim_state& ps(lookup_global_expr_process(ri));
#if DEBUG_STEP
	o << '#' << ri << ' ';	// global rule ID
#endif
	ps.dump_rule(o, ps.local_expr_index(ri), *this, v, multi_fanin);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out nodes affected by the node argument.  
	Note: this only requires structural information, no stateful info.  
	TODO: including all OR combinations, or just the paths that
		include this node directly?  (just the paths that include)
	\param ni global index of the 
	\param sr if normal rules should be printed.
	\param si if invariant rules should be printed.
	\param v true if literals should be printed with current values.
 */
ostream&
State::dump_node_fanout(ostream& o, const node_index_type ni, 
		const bool sr, const bool si, const bool v) const {
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
		const expr_index_type& gei = *fi;
		const process_sim_state& ps(lookup_global_expr_process(gei));
		const expr_index_type ei = ps.global_expr_to_root(gei);
		// adding offset translates back to global expression id
		DEBUG_FANOUT_PRINT("ei = " << ei << endl);
		fanout_rules.insert(ei);	// ignore duplicates
	}
	typedef	rule_set_type::const_iterator		rule_iterator;
	rule_iterator ri(fanout_rules.begin()), re(fanout_rules.end());
	// index-sorted expressions *should* be sorted by process!
	const process_sim_state* last = NULL;
	for ( ; ri!=re; ++ri) {
		const process_sim_state& ps(lookup_global_expr_process(*ri));
		if (last != &ps) {
			last = &ps;
			dump_process_canonical_name(o << "(to ", ps) << "):\n";
		}
		const rule_type* r = ps.lookup_rule(*ri);
#if PRSIM_WEAK_RULES
		const bool w = !r->is_weak() || weak_rules_shown();
#else
		const bool w = true;
#endif
	// if we want invariants printed
	if ((r->is_invariant())) {
	if (si) {
		const expr_index_type& gei = *ri;
		const unique_process_subgraph& pg(ps.type());
		const expr_index_type lei = ps.local_expr_index(gei);
#if 0
		const expr_index_type ei =
			ps.global_expr_index(pg.local_root_expr(lei));
		// adding offset translates back to global expression id
#endif
		dump_subexpr(o << "$(", gei, v);
		pg.dump_invariant_message(o, lei, ", \"", "\"");
		o << ')' << endl;
	}
	} else if (w) {
	if (sr) {
		dump_rule(o, *ri, v, (n.fanin.size() > 1)) << endl;
	}
	}
	}	// end for
	if (node_drives_any_channel(ni)) {
		_channel_manager.dump_node_fanout(o << "to channel(s):", ni)
			<< endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Counts number of pulling rules in each category.
	Used only for diagnostics.  
 */
struct faninout_struct_type::counter {
#if PRSIM_WEAK_RULES
	rule_index_type			up[2];
	rule_index_type			dn[2];
#else
	rule_index_type			up;
	rule_index_type			dn;
#endif
	counter() {
#if PRSIM_WEAK_RULES
		up[NORMAL_RULE] = 0;
		up[WEAK_RULE] = 0;
		dn[NORMAL_RULE] = 0;
		dn[WEAK_RULE] = 0;
#else
		up = 0;
		dn = 0;
#endif
	}

	// defauilt copy-ctor

	size_t
	sum(void) const {
#if PRSIM_WEAK_RULES
		return up[NORMAL_RULE] +up[WEAK_RULE]
			+dn[NORMAL_RULE] +dn[WEAK_RULE];
#else
		return up +dn;
#endif
	}

	counter&
	operator += (const faninout_struct_type& r) {
#if PRSIM_WEAK_RULES
		up[NORMAL_RULE] += r.pull_up[NORMAL_RULE].size();
		up[WEAK_RULE] += r.pull_up[WEAK_RULE].size();
		dn[NORMAL_RULE] += r.pull_dn[NORMAL_RULE].size();
		dn[WEAK_RULE] += r.pull_dn[WEAK_RULE].size();
#else
		up += r.pull_up.size();
		dn += r.pull_dn.size();
#endif
		return *this;
	}
};	// end struct faninout_struct_type::counter

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if node has more than one rule that pulls it in
		a particular direction.
 */
faninout_struct_type::counter
State::count_node_fanins(const node_index_type ni) const {
	typedef	faninout_struct_type::counter		counter_type;
	const node_type& n(get_node(ni));
#if VECTOR_NODE_FANIN
	process_fanin_type::const_iterator i(n.fanin.begin()), e(n.fanin.end());
#else
	const process_index_type* i(&n.fanin[0]), *e(&n.fanin[n.fanin.size()]);
#endif
	counter_type ret;
for ( ; i!=e; ++i) {
	const process_index_type& pid = *i;
	const process_sim_state& ps(process_state_array[pid]);
	const unique_process_subgraph& pg(ps.type());
	// find the local node index that corresponds to global node
	const footprint_frame_map_type& bfm(get_footprint_frame_map(pid));
	// note: many local nodes may map to the same global node
	// linear search to find them all
	typedef	footprint_frame_map_type::const_iterator frame_iter;
	const frame_iter b(bfm.begin()), fe(bfm.end());
	frame_iter f = find(b, fe, ni);
	while (f != fe) {
		// iterate over local node's fanin expressions!
		const node_index_type lni = std::distance(b, f);
		ret += pg.local_faninout_map[lni];
		f = find(f+1, fe, ni);
	}
}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Since each node now only lists which processes can drive it, 
	we need to visit each process and expand the list of global 
	expressions/rules (by index) drive a node.  
	\param ni global node index.
 */
ostream&
State::dump_node_fanin(ostream& o, const node_index_type ni, 
		const bool v) const {
	// typedef	process_fanin_type::const_iterator	const_iterator;
	STACKTRACE_INDENT_PRINT("node_fanin:ni = " << ni << endl);
	const node_type& n(get_node(ni));
#if VECTOR_NODE_FANIN
	process_fanin_type::const_iterator i(n.fanin.begin()), e(n.fanin.end());
#else
	const process_index_type* i(&n.fanin[0]), *e(&n.fanin[n.fanin.size()]);
#endif
for ( ; i!=e; ++i) {
	const process_index_type& pid = *i;
	dump_process_canonical_name(o << "(from ", pid) << "):\n";
	const process_sim_state& ps(process_state_array[pid]);
	// find the local node index that corresponds to global node
	const footprint_frame_map_type& bfm(get_footprint_frame_map(pid));
	// note: many local nodes may map to the same global node
	// linear search to find them all
	typedef	footprint_frame_map_type::const_iterator frame_iter;
	const frame_iter b(bfm.begin()), fe(bfm.end());
	frame_iter f = find(b, fe, ni);
	while (f != fe) {
		// iterate over local node's fanin expressions!
		const node_index_type lni = std::distance(b, f);
		ps.dump_node_fanin(o, lni, *this, v);
		f = find(f+1, fe, ni);
	}
}
	if (node_is_driven_by_channel(ni)) {
		_channel_manager.dump_node_fanin(o << "from channel(s):", ni)
			<< endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Takes unsorted list with dupes, cleans it up, sorting, and remove dupes.
 */
static
void
unique_sort_nodes(vector<node_index_type>& ret) {
	std::sort(ret.begin(), ret.end());		// in-place
	vector<node_index_type> tmp;
	tmp.reserve(ret.size());
	std::unique_copy(ret.begin(), ret.end(), back_inserter(tmp));
	ret.swap(tmp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Show explicit feedback rules where applicable.  
	Prints nodes that are both in the fanout and fanin set.
	First identify feedback node(s).  (There should really be only 1.)
	TODO: cache results
 */
void
State::node_fanin(const node_index_type ni,
		vector<node_index_type>& ret) const {
	// fanins
	const node_type& n(get_node(ni));
#if VECTOR_NODE_FANIN
	process_fanin_type::const_iterator i(n.fanin.begin()), e(n.fanin.end());
#else
	const process_index_type* i(&n.fanin[0]), *e(&n.fanin[n.fanin.size()]);
#endif
for ( ; i!=e; ++i) {
	const process_index_type& pid = *i;
	const process_sim_state& ps(process_state_array[pid]);
	// find the local node index that corresponds to global node
	const footprint_frame_map_type& bfm(get_footprint_frame_map(pid));
	// note: many local nodes may map to the same global node
	// linear search to find them all
	typedef	footprint_frame_map_type::const_iterator frame_iter;
	const frame_iter b(bfm.begin()), fe(bfm.end());
	frame_iter f = find(b, fe, ni);
	while (f != fe) {
		// iterate over local node's fanin expressions!
		const node_index_type lni = std::distance(b, f);
		ps.collect_node_fanin(lni, *this, ret);
		f = find(f+1, fe, ni);
	}
}
	unique_sort_nodes(ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reduced from dump_node_fanout.
 */
void
State::node_fanout(const node_index_type ni,
		vector<node_index_type>& ret) const {
	// fanouts
	typedef	fanout_array_type::const_iterator	const_iterator;
	typedef	std::set<expr_index_type>		rule_set_type;
	const node_type& n(get_node(ni));
	const fanout_array_type& foa(n.fanout);
	rule_set_type fanout_rules;
	const_iterator fi(foa.begin()), fe(foa.end());
	for ( ; fi!=fe; ++fi) {
		// for each leaf expression in the fanout list, 
		// trace up the propagation path to find the affected node.
		const expr_index_type& gei = *fi;
		const process_sim_state& ps(lookup_global_expr_process(gei));
		const expr_index_type ei = ps.global_expr_to_root(gei);
		fanout_rules.insert(ei);	// ignore duplicates
	}
	typedef	rule_set_type::const_iterator		rule_iterator;
	rule_iterator ri(fanout_rules.begin()), re(fanout_rules.end());
	// index-sorted expressions *should* be sorted by process!
	for ( ; ri!=re; ++ri) {
		const process_sim_state& ps(lookup_global_expr_process(*ri));
		const rule_type* r = ps.lookup_rule(*ri);
		NEVER_NULL(r);
	// invariants do not apply here
	if (!r->is_invariant()) {
		// always consider weak-rules in fanout
		const rule_index_type gr = ps.local_expr_index(*ri);
		ret.push_back(ps.rule_fanout(gr, *this));
	}	// end for
	}
	// don't consider channels
	unique_sort_nodes(ret);
}	// end State::node_feedback

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Intersects the fanin of node with fanout of same node.
 */
void
State::node_feedback(const node_index_type ni,
		vector<node_index_type>& ret) const {
	vector<node_index_type> fanin_set, fanout_set;
	node_fanin(ni, fanin_set);
	node_fanout(ni, fanout_set);
	// fanin_set and fanout_set are sorted
	std::set_intersection(fanin_set.begin(), fanin_set.end(), 
		fanout_set.begin(), fanout_set.end(), back_inserter(ret));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Intersects the fanout of node a with the fanin of node b.
	This finds nodes in common along a 2-transition path.  
 */
void
State::node_feedthrough(const node_index_type a, const node_index_type b,
		vector<node_index_type>& ret) const {
	vector<node_index_type> fanin_set, fanout_set;
	node_fanout(a, fanout_set);
	node_fanin(b, fanin_set);
	// fanin_set and fanout_set are sorted
	std::set_intersection(fanin_set.begin(), fanin_set.end(), 
		fanout_set.begin(), fanout_set.end(), back_inserter(ret));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Integrates the node_feedback and node_feedthrough, but for
	efficiency, only calls node_fanin once.  
	\param a is the input node that just switched
	\param b is the dynamic node in Z state
 */
void
State::node_feedback_feedthrough(const node_index_type a, 
		const node_index_type b,
		vector<node_index_type>& ret) const {
	STACKTRACE_VERBOSE;
	vector<node_index_type> tmpa, tmpb;
	node_fanout(a, tmpa);
	node_fanout(b, tmpb);
	std::set_union(tmpa.begin(), tmpa.end(), 
		tmpb.begin(), tmpb.end(), back_inserter(ret));
	ret.swap(tmpb);
	tmpa.clear();
	node_fanin(b, tmpa);
	std::set_intersection(tmpa.begin(), tmpa.end(), 
		tmpb.begin(), tmpb.end(), back_inserter(ret));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_node_feedback(ostream& o, const node_index_type ni, 
		const bool v) const {
//	dump_node_canonical_name(o << "feedback nodes of ", ni) << ':' << endl;
	vector<node_index_type> fb;
	node_feedback(ni, fb);
	return print_nodes(o, fb, v, "\n");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks that if node is floating, feedback node is already pending.
	Not const, because may call stop().
	\param ni the output node that just became state-holding
	\param in the input node that just switched to cause node ni
		to become state-holding
	\return true if keeper check fails
 */
bool
State::check_floating_node(const node_index_type ni,
		const node_index_type in) const {
	const node_type& n(get_node(ni));
	// check if node is floating
	// skip this check if node has no fanin, as it is probably an input
	const pull_set p(n, weak_rules_enabled());
	if (p.state_holding()) {
		// check and see if there is a feedback rule pending
		// in the event queue
		vector<node_index_type> fb;
#if 0
		node_feedback(ni, fb);
#else
		// allow non-feedback single transition delays
		node_feedback_feedthrough(in, ni, fb);
#endif
		bool have_pending = false;
		vector<node_index_type>::const_iterator
			i(fb.begin()), e(fb.end());
		for ( ; i!=e; ++i) {
//		dump_node_canonical_name(cout << "considering: ", *i) << endl;
		if (get_node(*i).pending_event()) {
			have_pending = true;
			break;
		}
		}
		if (!have_pending) {
			return true;
		}
	}	// else is driven
	return false;
}	// end check_floating_node

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_rules(ostream& o, const process_index_type pid, 
		const bool v) const {
	return process_state_array[pid].dump_rules(o, *this, v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_all_rules(ostream& o, const bool v) const {
	for_each(process_state_array.begin(), process_state_array.end(), 
		process_sim_state::rules_dumper(o, *this, v));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_SETUP_HOLD
ostream&
State::dump_timing_constraints(ostream& o, const process_index_type pid) const {
	return process_state_array[pid].dump_timing_constraints(o, *this);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints why a node is X.
 */
ostream&
State::dump_node_why_X(ostream& o, const node_index_type ni, 
		const size_t limit, const bool verbose) const {
	STACKTRACE_VERBOSE_WHY;
	// unique set to terminate cyclic recursion
	const node_type& n(get_node(ni));
if (n.current_value() == LOGIC_OTHER) {
	node_set_type u, v;	// cycle-detect set, globally-visited set
	return __node_why_X(o, ni, limit, verbose, u, v);
} else {
	dump_node_canonical_name(o, ni) << " is not X." << endl;
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
	STACKTRACE_VERBOSE_WHY;
	const node_type& n(get_node(ni));
	node_set_type u, v;
switch (n.current_value()) {
case LOGIC_LOW:
	if (dir ^ !why_not) {
		return __node_why_not(o, ni, limit, 
			dir, why_not, verbose, u, v);
	} else {
		dump_node_canonical_name(o, ni) << " is 0." << endl;
	}
	break;
case LOGIC_HIGH:
	if (dir ^ !why_not) {
		dump_node_canonical_name(o, ni) << " is 1." << endl;
	} else {
		return __node_why_not(o, ni, limit, 
			dir, why_not, verbose, u, v);
	}
	break;
default:
	dump_node_canonical_name(o, ni) << " is X." << endl;
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
	STACKTRACE_VERBOSE_WHY;
	const std::pair<node_set_type::iterator, bool>
		p(u.insert(ni)), y(v.insert(ni));
	const string nn(get_node_canonical_name(ni));
	o << auto_indent << nn << ":X";
if (p.second) {
if (y.second) {
	// inserted uniquely
	const node_type& n(get_node(ni));
	INVARIANT(n.current_value() == LOGIC_OTHER);
	const bool from_channel = node_is_driven_by_channel(ni);
	if (from_channel) {
		o << ", from-channel";
		// channel may be in stopped state...
	}
	// inspect pull state (and event queue)
	const event_index_type pe = n.get_event();
	if (pe) {
		o << ", pending event -> " <<
			node_type::translate_value_to_char(get_event(pe).val)
			<< endl;
	} else {
#if PRSIM_WEAK_RULES
	size_t w = NORMAL_RULE;
do {
#endif
	const pull_enum up = n.pull_up_state STR_INDEX(w).pull();
	const pull_enum dp = n.pull_dn_state STR_INDEX(w).pull();
	const size_t ux = (up == PULL_WEAK);
	const size_t dx = (dp == PULL_WEAK);
	switch (ux +dx) {
	case 0:
		if (up == PULL_ON &&
				dp == PULL_ON) {
			o << ", pull up/dn interfere\n";
		} else if (
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
		// const pull_enum xp = (ux ? up : dp);	// unknown pull
		const pull_enum op = (ux ? dp : up);	// other pull
		if (op == PULL_ON)
		{
			o << ", weak-interference vs. " << (ux ? "dn" : "up");
		} else {	// is OFF
			o << ", unknown-pull " << (ux ? "up" : "dn");
		}
		o << endl;
		const string __ind_str(verbose ? (ux ? "+" : "-") : "  ");
		const indent __indent(o, __ind_str);
		// iterate over OR-combination all X-state rules
		__root_expr_why_X(o, ni, ux, w, limit, verbose, u, v);
		break;
	}
	case 2:
		o << ", pull up/dn are both X";
		if (u.size() <= limit) {
		o << endl;
		// unroll: iterate over OR-combination all X-state rules
		{
			const indent __indent(o, verbose ? "+" : "  ");
			__root_expr_why_X(o, ni, true, w, limit, verbose, u, v);
		}{
			const indent __indent(o, verbose ? "-" : "  ");
			__root_expr_why_X(o, ni, false, w, limit, verbose, u, v);
		}
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
	if (n.in_channel()) {
		// if node is part of source or sink
		_channel_manager.__node_why_X(*this, o, ni, 
			limit, verbose, u, v);
	}
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
	\param dir is value.
		if true, ask why node isn't pulled up, else ... why not down
		This is irrelevant to atomic nodes.
	\param why_not is true if query is negated (why *isn't* node at value?)
		This is irrelevant to atomic nodes.
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
	STACKTRACE_VERBOSE_WHY;
	const std::pair<node_set_type::iterator, bool>
		p(u.insert(ni)), y(v.insert(ni));
	const node_type& n(get_node(ni));
	const string nn(get_node_canonical_name(ni));
	n.dump_value(o << auto_indent << nn << ":");
if (p.second) {
if (y.second) {
	// only consider pull-ups for atomics
//	const bool dir = _dir || n.is_atomic();
	// inserted uniquely
	// inspect pull state (and event queue)
	const event_index_type pe = n.get_event();
	const bool frozen = n.is_frozen();
	if (frozen) {
		o << ", frozen";
	}
	if (pe) {
		// if there is pending event, don't recurse
		o << ", pending event -> " <<
			node_type::translate_value_to_char(get_event(pe).val)
			<< endl;
		// check that pending event's value matches
	} else {
		const indent __ind_nd(o, verbose ? "." : "  ");
		// only check for the side that is off
		const pull_set pp(n, weak_rules_enabled());
		const pull_enum ps = (dir ? pp.up : pp.dn);
#if PRSIM_WEAK_RULES
		// skip this
		const pull_enum wps = (dir ? pp.wup : pp.wdn);
#endif
		if ((ps == PULL_OFF)
#if PRSIM_WEAK_RULES
			&& (wps == PULL_OFF)
#endif
			) {
			const bool from_channel =
				node_is_driven_by_channel(ni);
			if (n.has_fanin() || from_channel) {
			if (n.is_atomic()) {
				o << ", atomic";
			} else if (!why_not && !from_channel) {
				o << ", state-holding";
			}
			} else {
				o << ", input";
			}
		}
		// TODO: optional interactive-mode hook here!
		// if not descending, remove from set v
		if (u.size() <= limit) {
		o << endl;
		// INDENT_SCOPE(o);
		// can't use pi, wpi
		// unroll fanin rules: iterate over all relevant rules
		if (!frozen) {
		const pull_enum pull_query = why_not ? PULL_OFF : PULL_ON;
		if (n.is_atomic()) {
		if (ps == pull_query) {
			// always query the pull-up
			// for pull-dn query, negate why_not
			__root_expr_why_not(o, ni, true, NORMAL_RULE, limit,
				why_not ^ !dir, verbose, u, v);
		}
		} else {
		if (ps == pull_query) {
			__root_expr_why_not(o, ni, dir, NORMAL_RULE, limit, why_not, verbose, u, v);
		}
#if PRSIM_WEAK_RULES
		if (wps == pull_query) {
			__root_expr_why_not(o, ni, dir, WEAK_RULE, limit, why_not, verbose, u, v);
		}
#endif
		}	// end if n.is_atomic()
		if (n.in_channel()) {
			// ask channel why it has not driven the node
			_channel_manager.__node_why_not(*this, o, 
				ni, limit, dir, why_not, verbose, u, v);
		}
		}	// end if !frozen
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
	\param xi index of expression whose pull state is unknown
	\param u set of X node to accumulate
	TODO: generalize this to take any pull-state value!!!
 */
void
State::__get_X_fanins(const expr_index_type xi, node_set_type& u) const {
	STACKTRACE_VERBOSE;
	ISE_INVARIANT(xi);
	const process_sim_state& ps(lookup_global_expr_process(xi));
	ps.__get_local_X_fanins(ps.local_expr_index(xi), *this, u);
}

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
		const bool why_not, const bool verbose,
		node_set_type& u, node_set_type& v) const {
	STACKTRACE_VERBOSE_WHY;
	// translate global to local
	const process_sim_state& ps(lookup_global_expr_process(xi));
	ps.__local_expr_why_not(o, ps.local_expr_index(xi), *this, 
		limit, why_not, verbose, u, v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::__root_expr_why_not(ostream& o, const node_index_type ni,
                const bool dir,
#if PRSIM_WEAK_RULES
                const bool wk,
#endif          
                const size_t limit, const bool why_not, const bool verbose, 
                node_set_type& u, node_set_type& v) const {
	STACKTRACE_VERBOSE_WHY;
	DEBUG_WHY_PRINT("querying node: " << ni << endl);
	const node_type& n(get_node(ni));
	const process_fanin_type& fanin(n.fanin);
	string ind_str;
	if (verbose) {
		// root is OR-combination of rules
		const faninout_struct_type::counter c(count_node_fanins(ni));
		if ((dir ? c.up : c.dn) STR_INDEX(wk) > 1) {
			ind_str += " |";
			o << auto_indent << "-+" << endl;
		}
		// ind_str += " ";
	}
	const indent __ind_ex(o, ind_str);	// INDENT_SCOPE(o);

	const fanin_state_type&
		nf((dir ? n.pull_up_state : n.pull_dn_state) STR_INDEX(wk));
	const pull_enum match_pull = nf.pull();	// never negate OR
	// iterate over processes, collect root expressions/rules
#if VECTOR_NODE_FANIN
	process_fanin_type::const_iterator i(fanin.begin()), e(fanin.end());
#else
	const process_index_type* i(&fanin[0]), *e(&fanin[fanin.size()]);
#endif
for ( ; i!=e; ++i) {		// for all processes
	const process_index_type& pid = *i;
	const process_sim_state& ps(process_state_array[pid]);
	const unique_process_subgraph& pg(ps.type());
	// find local node indices that corresponds to global node
	const footprint_frame_map_type& bfm(get_footprint_frame_map(pid));
	// note: many local nodes may map to the same global node
	// so linear search to find them all
	typedef	footprint_frame_map_type::const_iterator frame_iter;
	const frame_iter b(bfm.begin()), fe(bfm.end());
	frame_iter f = find(b, fe, ni);
	while (f != fe) {	// for all local nodes that reference node ni
		// collect local node's fanin expressions!
		const node_index_type lni = std::distance(b, f);
		DEBUG_WHY_PRINT("local node: " << lni << endl);
		const faninout_struct_type&
			fm(pg.local_faninout_map[lni]);
		const fanin_array_type&
			fia(dir ? fm.pull_up STR_INDEX(wk)
				: fm.pull_dn STR_INDEX(wk));
		fanin_array_type::const_iterator ci(fia.begin()), ce(fia.end());
		for ( ; ci!=ce; ++ci) {
			const expr_index_type& lei(*ci);
			DEBUG_WHY_PRINT("local expr: " << lei << endl);
			// ISE_INVARIANT(lei);	// locally 0-indexed
			ps.__recurse_expr_why_not(o, lei, match_pull, 
				*this, limit, why_not, verbose, u, v);
		}	// end for
		f = find(f+1, fe, ni);
	}
}	// end for
}

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
	STACKTRACE_VERBOSE_WHY;
	ISE_INVARIANT(xi);
	const process_sim_state& ps(lookup_global_expr_process(xi));
	ps.__local_expr_why_X(o, ps.local_expr_index(xi), 
		*this, limit, verbose, u, v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need this specialization because root expressions are now structured
	separate as part of nodes fanin structures.  
	This should behave like __local_expr_why_X.
 */
void
State::__root_expr_why_X(ostream& o, const node_index_type ni, 
		const bool dir, 
#if PRSIM_WEAK_RULES
		const bool wk, 
#endif
		const size_t limit, const bool verbose, 
		node_set_type& u, node_set_type& v) const {
	STACKTRACE_VERBOSE_WHY;
	string ind_str;
	const process_fanin_type& fanin(get_node(ni).fanin);
	if (verbose) {
		// remember, fanin.size() is number of *processes*
		const faninout_struct_type::counter c(count_node_fanins(ni));
		if ((dir ? c.up : c.dn) STR_INDEX(wk) > 1) {
			ind_str += " |";
			o << auto_indent << "-+" << endl;
		}
	}
	const indent __ind_ex(o, ind_str);	// INDENT_SCOPE(o);

	// iterate over processes, collect root expressions/rules
#if VECTOR_NODE_FANIN
	process_fanin_type::const_iterator i(fanin.begin()), e(fanin.end());
#else
	const process_index_type* i(&fanin[0]), *e(&fanin[fanin.size()]);
#endif
for ( ; i!=e; ++i) {		// for all processes
	const process_index_type& pid = *i;
	const process_sim_state& ps = process_state_array[pid];
	const unique_process_subgraph& pg(ps.type());
	// find local node indices that corresponds to global node
	const footprint_frame_map_type& bfm(get_footprint_frame_map(pid));
	// note: many local nodes may map to the same global node
	// so linear search to find them all
	typedef	footprint_frame_map_type::const_iterator frame_iter;
	const frame_iter b(bfm.begin()), fe(bfm.end());
	frame_iter f = find(b, fe, ni);
	while (f != fe) {	// for all local nodes that reference node ni
		// collect local node's fanin expressions!
		const node_index_type lni = std::distance(b, f);
		const faninout_struct_type&
			fm(pg.local_faninout_map[lni]);
		const fanin_array_type&
			fia(dir ? fm.pull_up STR_INDEX(wk)
				: fm.pull_dn STR_INDEX(wk));
		fanin_array_type::const_iterator ci(fia.begin()), ce(fia.end());
		for ( ; ci!=ce; ++ci) {
			const expr_index_type& lei(*ci);
			// ISE_INVARIANT(lei);	// locally 0-indexed
			ps.__recurse_expr_why_X(o, lei, *this, 
				limit, verbose, u, v);
		}	// end for
		f = find(f+1, fe, ni);
	}
}	// end for
}	// end __root_expr_why_X

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#else
// use std::ptr_fun or std::mem_fun_ref, embrace functional-style
#endif

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
	const bool			verbose;
	const char*			delim;

	explicit
	node_printer_base(const State& s, ostream& o, 
			const bool v, const char* d) :
			state(s), os(o), verbose(v), delim(d) {
		NEVER_NULL(delim);
	}

};

struct node_printer : public node_printer_base {
	node_printer(const State& s, ostream& o, const bool v, const char* d) :
			node_printer_base(s, o, v, d) { }

	void
	operator () (const node_index_type ni) const {
		state.dump_node_canonical_name(os, ni);
		if (verbose) {
			state.get_node(ni).dump_value(os << ':');
		}
		os << delim;
	}
};

struct node_printer_prefix : public node_printer_base {
	node_printer_prefix(const State& s, ostream& o, 
			const bool v, const char* d) :
			node_printer_base(s, o, v, d) { }

	void
	operator () (const node_index_type ni) const {
		state.dump_node_canonical_name(os << delim, ni);
		if (verbose) {
			state.get_node(ni).dump_value(os << ':');
		}
	}
};

template <typename Iter>
ostream&
State::__print_nodes(ostream& o, Iter b, Iter e, const bool v, 
		const char* delim) const {
	for_each(b, e, node_printer(*this, o, v, delim));
	return o << std::flush;
}

template <typename Iter>
ostream&
State::__print_nodes_infix(ostream& o, Iter b, Iter e,
		const bool v, const char* delim) const {
if (b != e) {
	node_printer(*this, o, v, "")(*b);
	++b;
	for_each(b, e, node_printer_prefix(*this, o, v, delim));
}
	return o << std::flush;
}

ostream&
State::print_nodes(ostream& o, const vector<node_index_type>& nodes, 
		const bool v, const char* delim) const {
	return __print_nodes(o, nodes.begin(), nodes.end(), v, delim);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 	No driver means also not driven by channel
 */
static
bool
node_is_X_no_driver(const State& s, const node_index_type ni) {
	return s.get_node(ni).is_X() && !s.node_is_driven(ni);
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
node_is_unused(const State& s, const node_index_type ni) {
	return !s.node_is_driven(ni) && !s.node_is_used(ni);
}

/**
	Find nodes that are X's and unused outputs.  
 */
static
bool
node_is_X_not_used(const State& s, const node_index_type ni) {
	return s.get_node(ni).is_X() && !s.node_is_used(ni);
}

/**
	Find nodes that are X's and used (have fanout).  
 */
static
bool
node_is_X_used(const State& s, const node_index_type ni) {
	return s.get_node(ni).is_X() && s.node_is_used(ni);
}

/**
	Find nodes that are X's and have fanin, 
	and is NOT being pulled.
 */
static
bool
node_is_X_has_fanin_off(const State& s, const node_index_type ni) {
	const State::node_type& n(s.get_node(ni));
if (n.is_X() && s.node_is_driven(ni)) {
bool w = false;
do {
bool d = false;
do {
	if (n.get_pull_struct(d
#if PRSIM_WEAK_RULES
		, w
#endif
		).pull() != PULL_OFF) {
		return false;
	}
	d = !d;
} while (d);
	w = !w;
} while (w);
	return true;
} else {
	return false;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::unused_nodes(vector<node_index_type>& nodes) const {
	find_nodes(nodes, &node_is_unused);
}

ostream&
State::dump_unused_nodes(ostream& o, const bool v) const {
	o << "Nodes with no fanin, no fanout:" << endl;
	vector<node_index_type> nodes;
	unused_nodes(nodes);
	print_nodes(o, nodes, v, "\n");
	return o << std::flush;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print all X nodes with no fanin.  
	This won't find X cycles, however.  
	\param b true to include node with no fanout
 */
void
State::dangling_unknown_nodes(const bool b, 
		vector<node_index_type>& nodes) const {
	find_nodes(nodes,
		b ? &node_is_X_no_driver : &node_is_X_no_driver_with_fanout);
}

ostream&
State::dump_dangling_unknown_nodes(ostream& o, const bool b) const {
	o << "X nodes with no fanin";
	if (!b) {
		o << ", with fanout";
	}
	o << ":" << endl;
	vector<node_index_type> nodes;
	dangling_unknown_nodes(b, nodes);
	print_nodes(o, nodes, false, "\n");
	return o << std::flush;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print all nodes with no fanout, presumed to be outputs.
 */
void
State::output_nodes(vector<node_index_type>& nodes) const {
	find_nodes(nodes, &this_type::node_is_not_used);
}

ostream&
State::dump_output_nodes(ostream& o, const bool v) const {
	o << "nodes with no fanout (unused): " << endl;
	vector<node_index_type> nodes;
	output_nodes(nodes);
	print_nodes(o, nodes, v, "\n");
	return o << std::flush;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Status X intersected with nodes with no fanout (not used).
 */
void
State::output_unknown_nodes(vector<node_index_type>& nodes) const {
	find_nodes(nodes, &node_is_X_not_used);
}

ostream&
State::dump_output_unknown_nodes(ostream& o) const {
	o << "X nodes with no fanout (unused): " << endl;
	vector<node_index_type> nodes;
	output_unknown_nodes(nodes);
	print_nodes(o, nodes, false, "\n");
	return o << std::flush;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Status X intersected with nodes with fanout (used).
 */
void
State::unknown_nodes_fanout(vector<node_index_type>& nodes) const {
	find_nodes(nodes, &node_is_X_used);
}

ostream&
State::dump_unknown_nodes_fanout(ostream& o) const {
	o << "X nodes with fanout: " << endl;
	vector<node_index_type> nodes;
	unknown_nodes_fanout(nodes);
	print_nodes(o, nodes, false, "\n");
	return o << std::flush;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Status X node with fanin, whose pull-up/dn state is off.
 */
void
State::unknown_nodes_fanin_off(vector<node_index_type>& nodes) const {
	find_nodes(nodes, &node_is_X_has_fanin_off);
}

ostream&
State::dump_unknown_nodes_fanin_off(ostream& o) const {
	o << "X nodes with fanins pull-off: " << endl;
	vector<node_index_type> nodes;
	unknown_nodes_fanin_off(nodes);
	print_nodes(o, nodes, false, "\n");
	return o << std::flush;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ostream&
State::dump_subexpr(ostream& o, const expr_index_type ei, 
		const bool v, 
		const uchar ptype, const bool pr) const {
	const process_sim_state& ps(lookup_global_expr_process(ei));
	return ps.dump_subexpr(o, ps.local_expr_index(ei), *this, v, ptype, pr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_mk_excl_ring(ostream& o, const ring_set_type& r,
		const bool v) const {
	typedef	ring_set_type::const_iterator	const_iterator;
	ISE_INVARIANT(r.size() > 1);
	return __print_nodes_infix(o << "{ ", r.begin(), r.end(), v, ", ")
		<< " }";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_mk_exclhi_rings(ostream& o, const bool v) const {
	o << "forced exclhi rings:" << endl;
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	const_iterator i(mk_exhi.begin()), e(mk_exhi.end());
#if PRSIM_MK_EXCL_BLOCKING_SET
	++i;
#endif
	for ( ; i!=e; ++i) {
#if PRSIM_MK_EXCL_BLOCKING_SET
		dump_mk_excl_ring(o, i->elems, v) << endl;
#else
		dump_mk_excl_ring(o, *i, v) << endl;
#endif
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_mk_excllo_rings(ostream& o, const bool v) const {
	o << "forced excllo rings:" << endl;
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
	const_iterator i(mk_exlo.begin()), e(mk_exlo.end());
#if PRSIM_MK_EXCL_BLOCKING_SET
	++i;
#endif
	for ( ; i!=e; ++i) {
#if PRSIM_MK_EXCL_BLOCKING_SET
		dump_mk_excl_ring(o, i->elems, v) << endl;
#else
		dump_mk_excl_ring(o, *i, v) << endl;
#endif
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints excl-ring fanout of node.  
	TODO: rewrite using lookup by node id, 
		instead of iterating over all rings duh!
 */
ostream&
State::dump_node_mk_excl_rings(ostream& o, const node_index_type ni, 
		const bool v) const {
#if PRSIM_MK_EXCL_BLOCKING_SET
	typedef	mk_excl_counter_map_type::const_iterator	const_iterator;
#else
	typedef	mk_excl_ring_map_type::const_iterator	const_iterator;
#endif
	const string nn(get_node_canonical_name(ni));
{
	o << "forced exclhi rings of which `" << nn <<
		"\' is a member:" << endl;
#if PRSIM_MK_EXCL_BLOCKING_SET
	const const_iterator f(mk_exhi_counter_map.find(ni));
	if (f != mk_exhi_counter_map.end()) {
		lock_index_list_type::const_iterator
			ri(f->second.begin()), re(f->second.end());
		for ( ; ri!=re; ++ri) {
			dump_mk_excl_ring(o, mk_exhi[*ri].elems, v) << endl;
		}
	}
#else
	const_iterator i(mk_exhi.begin()), e(mk_exhi.end());
	for ( ; i!=e; ++i) {
		if (i->find(ni) != i->end()) {
			dump_mk_excl_ring(o, *i, v) << endl;
		}
	}
#endif
}{
	o << "forced excllo rings of which `" << nn <<
		"\' is a member:" << endl;
#if PRSIM_MK_EXCL_BLOCKING_SET
	const const_iterator f(mk_exlo_counter_map.find(ni));
	if (f != mk_exlo_counter_map.end()) {
		lock_index_list_type::const_iterator
			ri(f->second.begin()), re(f->second.end());
		for ( ; ri!=re; ++ri) {
			dump_mk_excl_ring(o, mk_exlo[*ri].elems, v) << endl;
		}
	}
#else
	const_iterator i(mk_exlo.begin()), e(mk_exlo.end());
	for ( ; i!=e; ++i) {
		if (i->find(ni) != i->end()) {
			dump_mk_excl_ring(o, *i, v) << endl;
		}
	}
#endif
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_check_excl_ring(ostream& o, const lock_index_list_type& r, 
		const bool v) const {
	typedef	lock_index_list_type::const_iterator	const_iterator;
	ISE_INVARIANT(r.size() > 1);
	return __print_nodes_infix(o << "{ ", r.begin(), r.end(), v, ", ")
		<< " }";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dump all checked exclhi rings.  
 */
ostream&
State::dump_check_exclhi_rings(ostream& o, const bool v) const {
	o << "checked exclhi rings:" << endl;
	check_excl_array_type temp(check_exhi_ring_pool.size());
	__collate_check_excl_reverse_map(check_exhi, temp);
	typedef	check_excl_array_type::const_iterator	const_iterator;
	const_iterator i(temp.begin()), e(temp.end());
	// skip first b/c [0] is reserved
	for (++i; i!=e; ++i) {
		dump_mk_excl_ring(o, *i, v) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dump all checked excllo rings.  
 */
ostream&
State::dump_check_excllo_rings(ostream& o, const bool v) const {
	o << "checked excllo rings:" << endl;
	check_excl_array_type temp(check_exlo_ring_pool.size());
	__collate_check_excl_reverse_map(check_exlo, temp);
	typedef	check_excl_array_type::const_iterator	const_iterator;
	const_iterator i(temp.begin()), e(temp.end());
	// skip first b/c [0] is reserved
	for (++i; i!=e; ++i) {
		dump_mk_excl_ring(o, *i, v) << endl;
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
State::dump_node_check_excl_rings(ostream& o, const node_index_type ni, 
		const bool v) const {
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
			dump_mk_excl_ring(o, i->second, v) << endl;
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
			dump_mk_excl_ring(o, i->second, v) << endl;
		}
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper functor for counting memory usage.
 */
struct unique_process_subgraph::memory_accumulator {
	/// counts both expr_pool and expr_graph_node_pool
	expr_index_type			exprs;
	/// counts both rule_pool and rule_map
	rule_index_type			rules;
	/// number of local nodes
	node_index_type			local_nodes;
	/// aggregate count of all fanin and fanouts of all nodes
	expr_index_type			fans;

	memory_accumulator() : exprs(0), rules(0), local_nodes(0), fans(0) { }

	memory_accumulator(const expr_index_type e, const rule_index_type r, 
		const node_index_type n, const expr_index_type f) :
		exprs(e), rules(r), local_nodes(n), fans(f) { }

	static
	inline
	memory_accumulator
	add(const memory_accumulator& l, const unique_process_subgraph& s) {
		INVARIANT(s.expr_pool.size() == s.expr_graph_node_pool.size());
		INVARIANT(s.rule_pool.size() == s.rule_map.size());
		return memory_accumulator(
			l.exprs +s.expr_pool.size(),
			l.rules +s.rule_pool.size(),
			l.local_nodes +s.local_faninout_map.size(),
			l.fans +s.fan_count()
		);
	}

	ostream&
	report(ostream& o) const {
		o << "\texprs+graph_nodes: (" << exprs << " *(" <<
			sizeof(expr_struct_type) << '+' <<
			sizeof(graph_node_type) << ") B/expr) = " <<
			exprs *(sizeof(expr_struct_type)
				+sizeof(graph_node_type)) << " B" << endl;
		typedef	rule_map_type::const_iterator::value_type
					value_type;
		o << "\trules+map_nodes: (" << rules << " *(" <<
			sizeof(rule_type) << '+' <<
			sizeof_hashtable_node(value_type) << ") B/rule) = " <<
			exprs *(sizeof(rule_type)
				+sizeof_hashtable_node(value_type))
			<< " B" << endl;
		o << "\tlocal_nodes: (" << local_nodes << " * " << 
			sizeof(faninout_struct_type) << " B/node) = " <<
			local_nodes *sizeof(faninout_struct_type)
			<< " B" << endl;
		o << "\tfanin/outs: (" << fans << " * " << 
			sizeof(expr_index_type) << " B/fan) = " <<
			fans *sizeof(expr_index_type) << " B" << endl;
		return o;
	}
};	// end struct unique_process_subgraph::memory_accumulator

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct process_sim_state::memory_accumulator {
	/// counts both expr_pool and expr_graph_node_pool
	expr_index_type			exprs;
	/// counts both rule_pool and rule_map
	rule_index_type			rules;

	memory_accumulator() : exprs(0), rules(0) { }

	memory_accumulator(const expr_index_type e, const rule_index_type r) :
		exprs(e), rules(r) { }

	static
	inline
	memory_accumulator
	add(const memory_accumulator& l, const process_sim_state& s) {
		return memory_accumulator(
			l.exprs +s.expr_states.size(),
			l.rules +s.rule_states.size()
		);
	}

	ostream&
	report(ostream& o) const {
		o << "\texpr-state: (" << exprs << " * " << 
			sizeof(expr_state_type) << " B/expr) = " <<
			exprs *sizeof(expr_state_type) << " B" << endl;
		o << "\trule-state: (" << rules << " * " << 
			sizeof(rule_state_type) << " B/rule) = " <<
			rules *sizeof(rule_state_type) << " B" << endl;
		return o;
	}
};	// end struct process_sim_state::memory_accumulator

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print memory usage statistics.
	TODO: write re-usable memory usage report library.
 */
ostream&
State::dump_memory_usage(ostream& o) const {
	GET_CONTEXT_CACHE dump_memory_usage(o);
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
{
	const size_t u = unique_process_pool.size();
	o << "unique-process: (" << u << " * " <<
		sizeof(unique_process_subgraph) << " B/type) = " <<
		u *sizeof(unique_process_subgraph) << " B" << endl;
	const unique_process_subgraph::memory_accumulator
		m(std::accumulate(unique_process_pool.begin(),
			unique_process_pool.end(), 
			unique_process_subgraph::memory_accumulator(), 
			&unique_process_subgraph::memory_accumulator::add));
	m.report(o);
}{
	const size_t u = process_state_array.size();
	o << "process-instances: (" << u << " * "  <<
		sizeof(process_sim_state) << " B/proc) = " <<
		u *sizeof(process_sim_state) << " B" << endl;
	const process_sim_state::memory_accumulator
		m(std::accumulate(process_state_array.begin(),
			process_state_array.end(), 
			process_sim_state::memory_accumulator(), 
			&process_sim_state::memory_accumulator::add));
	m.report(o);
}{
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
	// hashtable iterator value-types
	typedef	global_expr_process_id_map_type::const_iterator::value_type
							value_type;
#if PRSIM_PROCESS_EXPR_MAP_ARRAY
	const size_t es = sizeof(value_type);
#else
	const size_t es = sizeof_tree_node(value_type);
#endif
	const size_t n = global_expr_process_id_map.size();
	o << "expr-process-map: (" << n << " * " << es
		<< " B/proc) = " << n * es
		<< " B" << endl;
#endif
}
	event_pool.dump_memory_usage(o);
{
	const size_t es = event_queue.size();
	o << "event-queue: ("  << es << " * " << sizeof(event_placeholder_type)
		<< " B/event) = " << es * sizeof(event_placeholder_type)
		<< " B" << endl;
	// may be bigger due to reserved capacity
}
{
#if PRSIM_MK_EXCL_BLOCKING_SET
	typedef	mk_excl_counter_map_type::value_type	value_type;
	const size_t hs = mk_exhi_counter_map.size();
	o << "exclhi-counter-map: ("  << hs << " * " << sizeof_tree_node(value_type)
		<< " B/node) = " << hs * sizeof_tree_node(value_type)
		<< " B" << endl;
	const size_t ls = mk_exlo_counter_map.size();
	o << "excllo-counter-map: ("  << ls << " * " << sizeof_tree_node(value_type)
		<< " B/node) = " << ls * sizeof_tree_node(value_type)
		<< " B" << endl;
	// TODO: mapped_type is also a vector, and should be accounted for
#else
	typedef	mk_excl_queue_type::const_iterator::value_type	value_type;
	const size_t hs = exclhi_queue.size();
	o << "exclhi-queue: ("  << hs << " * " << sizeof_tree_node(value_type)
		<< " B/event) = " << hs * sizeof_tree_node(value_type)
		<< " B" << endl;
	const size_t ls = excllo_queue.size();
	o << "excllo-queue: ("  << ls << " * " << sizeof_tree_node(value_type)
		<< " B/event) = " << ls * sizeof_tree_node(value_type)
		<< " B" << endl;
#endif
	const size_t ps = updated_nodes.size();
	o << "updated-nodes: ("  << ps << " * " <<
		sizeof_tree_node(node_index_type) << " B/node) = " <<
		ps * sizeof_tree_node(node_index_type) << " B" << endl;
#if PRSIM_FCFS_UPDATED_NODES
	o << "updated-nodes-queue: ("  << ps << " * " <<
		sizeof(node_index_type) << " B/node) = " <<
		ps * sizeof(node_index_type) << " B" << endl;
#endif
}
{
	// rings
	typedef	mk_excl_ring_map_type::value_type		elem_type;
	const size_t rs = mk_exhi.size();
	o << "mk-exclhi-rings: ("  << rs << " * " << sizeof(elem_type) <<
		" B/ring) = " << rs * sizeof(elem_type) << " B" << endl;
	const size_t rr = std::accumulate(mk_exhi.begin(), mk_exhi.end(), 
		size_t(0), &add_size<elem_type>);
	typedef	ring_set_type::iterator::value_type	value_type;
	o << "mk-exclhi::nodes: (" << rr << " * " <<
		sizeof_tree_node(value_type) <<
		" B/node) = " << rr * sizeof_tree_node(value_type)
		<< " B" << endl;
	// alternative to set: sorted valarray/vector
}{
	// rings
	typedef	mk_excl_ring_map_type::value_type		elem_type;
	const size_t rs = mk_exlo.size();
	o << "mk-excllo-rings: ("  << rs << " * " << sizeof(elem_type) <<
		" B/ring) = " << rs * sizeof(elem_type) << " B" << endl;
	const size_t rr = std::accumulate(mk_exlo.begin(), mk_exlo.end(), 
		size_t(0), &add_size<elem_type>);
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
	// smaller stuff
	// expr_trace_type __scratch_expr_trace
	// fanout_array_type __shuffle_indices
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Set autosave policy and checkpoint name.
 */
void
State::autosave(const bool b, const string& n) {
	if (b)	flags |= FLAG_AUTOSAVE;
	else	flags &= ~FLAG_AUTOSAVE;
	if (n.length()) {
		autosave_name = n;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Bump this when checkpoint binary version changes (compatibility).
	TODO: use library versioning scheme to indicate forward compat
1: initial version
2: removed watch_list in lieu of using simpler watchpoint flag
3: added timing binary support
4: added keeper_check_fail_policy, and expr_alloc_flags
5: added channel timing_probability
6: added auto_precharge_invariant to expr_alloc_flags
7: grew sizeof flags to long, to acommodate VCD tracing
8: added excl-unstable and dequeue-unknown flags
 */
static
const size_t	checkpoint_version = 8;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: need to check consistency with module.  
	Write out a header for safety checks.  
	TODO: save state only? without structure?
	\return true if to signal that an error occurred. 
	NOTE: we do not save the fact the a trace-file was being recorded!
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
	write_value(o, checkpoint_version);
	expr_alloc_flags.write_object(o);
	util::numeric::write_seed48(o);
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
#if PRSIM_MK_EXCL_BLOCKING_SET
	ISE_INVARIANT(__mk_excl_blocking_set.empty());
#else
	ISE_INVARIANT(exclhi_queue.empty());
	ISE_INVARIANT(excllo_queue.empty());
#endif
	ISE_INVARIANT(updated_nodes.empty());
#if PRSIM_FCFS_UPDATED_NODES
	ISE_INVARIANT(updated_nodes_queue.empty());
#endif
	write_value(o, current_time);
	write_value(o, uniform_delay);
	write_value(o, default_after_min);
	write_value(o, default_after_max);
	write_value(o, timing_probability);
	WRITE_ALIGN_MARKER
	WRITE_ALIGN_MARKER
	write_value(o, flags_type(flags & FLAGS_CHECKPOINT_MASK));
	write_value(o, unstable_policy);
	write_value(o, weak_unstable_policy);
#if PRSIM_MK_EXCL_BLOCKING_SET
	write_value(o, excl_unstable_policy);
#endif
	write_value(o, interference_policy);
	write_value(o, weak_interference_policy);
	write_value(o, invariant_fail_policy);
	write_value(o, invariant_unknown_policy);
	write_value(o, assert_fail_policy);
	write_value(o, channel_expect_fail_policy);
	write_value(o, excl_check_fail_policy);
	write_value(o, keeper_check_fail_policy);
#if 0 && PRSIM_SETUP_HOLD
	write_value(o, setup_violation_policy);
	write_value(o, hold_violation_policy);
#endif
//	write_value(o, autosave_name);		// don't preserve
	write_value(o, timing_mode);
	_dump_flags.write_object(o);
	time_fmt.write_object(o);
#if PRSIM_SETUP_HOLD
	timing_checker.save_checkpoint(o);
#endif
	if (_channel_manager.save_checkpoint(o)) return true;
	// interrupted flag, just ignore
	// trace_flush_interval: not saved
	// vcd_timescale: not saved
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
	size_t version_check;
	read_value(i, version_check);
	if (version_check != checkpoint_version) {
		cerr << "Expecting checkpoint compatibility version " <<
			checkpoint_version << ", but got " <<
			version_check << "." << endl;
		return true;
	}
} catch (...) {
	cerr << bad_ckpt << endl;
	return true;
}
{
	// guarantee that same options were used that affect
	// state expansion.
	ExprAllocFlags tmpf;
	tmpf.load_object(i);
	if (!tmpf.assert_equal(expr_alloc_flags))
		return true;
}
	util::numeric::read_seed48(i);
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
	READ_ALIGN_MARKER		// sanity alignment check
}{
	// while loading checkpoint, ignore violations
	invariant_fail_policy = ERROR_IGNORE;
	invariant_unknown_policy = ERROR_IGNORE;
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
				__debug_print_node_change(nj,
					pull_enum(prev), pull_enum(next));
				evaluate(*fi, pull_enum(prev), pull_enum(next));
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
#if PRSIM_MK_EXCL_BLOCKING_SET
	ISE_INVARIANT(__mk_excl_blocking_set.empty());
#else
	ISE_INVARIANT(exclhi_queue.empty());
	ISE_INVARIANT(excllo_queue.empty());
#endif
	ISE_INVARIANT(updated_nodes.empty());
#if PRSIM_FCFS_UPDATED_NODES
	ISE_INVARIANT(updated_nodes_queue.empty());
#endif
	read_value(i, current_time);
	read_value(i, uniform_delay);
	read_value(i, default_after_min);
	read_value(i, default_after_max);
	read_value(i, timing_probability);
	READ_ALIGN_MARKER
	READ_ALIGN_MARKER		// sanity alignment check
{
	if (is_tracing()) {
		close_trace();
		cout << "Closing trace stream while loading checkpoint."
			<< endl;
	}
	if (is_tracing_vcd()) {
		close_vcd();
		cout << "Closing vcd stream while loading checkpoint."
			<< endl;
	}
	flags_type tmp;
	read_value(i, tmp);
	// preserve the auto-checkpointing, but not tracing
	flags = (flags & FLAG_AUTOSAVE) | (tmp & ~FLAG_AUTOSAVE);
}
	// DO NOT auto-checkpoint with the same name!
	// this prevents accidental overwrite due to loading!
	// This is already masked out during saving of checkpoint.
	read_value(i, unstable_policy);
	read_value(i, weak_unstable_policy);
#if PRSIM_MK_EXCL_BLOCKING_SET
	read_value(i, excl_unstable_policy);
#endif
	read_value(i, interference_policy);
	read_value(i, weak_interference_policy);
	read_value(i, invariant_fail_policy);
	read_value(i, invariant_unknown_policy);
	read_value(i, assert_fail_policy);
	read_value(i, channel_expect_fail_policy);
	read_value(i, excl_check_fail_policy);
	read_value(i, keeper_check_fail_policy);
#if 0 && PRSIM_SETUP_HOLD
	read_value(i, setup_violation_policy);
	read_value(i, hold_violation_policy);
#endif
//	read_value(i, autosave_name);	// ignore the name of checkpoint
	read_value(i, timing_mode);
	_dump_flags.load_object(i);
	time_fmt.load_object(i);
#if PRSIM_SETUP_HOLD
	timing_checker.load_checkpoint(i);
#endif
	// interrupted flag, just ignore
	// ifstreams? don't bother managing input stream stack.
	// __scratch_expr_trace -- never needed, ignore
	if (_channel_manager.load_checkpoint(i)) return true;

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
			e.inspect(*this, cerr);
			// don't bother throwing
			// TODO: handle error code
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
#if PRSIM_MK_EXCL_BLOCKING_SET
// TODO: reconstruct force excl ring counter values
#endif
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
	size_t version_check;
	read_value(i, version_check);
	o << "checkpoint version: " << version_check << endl;
	if (version_check != checkpoint_version) {
		cerr << "WARNING: checkpoint version mismatch!\n"
		"Attempting to continue, but this could get ugly..." << endl;
	}
}{
	ExprAllocFlags tmpf;
	tmpf.load_object(i);
	// TODO: print options?
}{
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
	time_type current_time, uniform_delay,
		default_after_min, default_after_max;
	preal_value_type timing_probability;
	read_value(i, current_time);
	read_value(i, uniform_delay);
	read_value(i, default_after_min);
	read_value(i, default_after_max);
	read_value(i, timing_probability);
	o << "current time: " << current_time << endl;
	o << "uniform delay: " << uniform_delay << endl;
	o << "default after min: " << default_after_min << endl;
	o << "default after max: " << default_after_max << endl;
	o << "binary timing probability: " << timing_probability << endl;
	READ_ALIGN_MARKER
	READ_ALIGN_MARKER		// sanity alignment check
	flags_type flags;
	read_value(i, flags);
	const util::save_flags s(o, o.flags());
	o << "flags: 0x" << std::hex << size_t(flags) << endl;
{
	error_policy_enum p;
	read_value(i, p);
	o << "unstable policy: " << error_policy_string(p) << endl;
	read_value(i, p);
	o << "weak-unstable policy: " << error_policy_string(p) << endl;
	read_value(i, p);
#if PRSIM_MK_EXCL_BLOCKING_SET
	o << "excl-unstable policy: " << error_policy_string(p) << endl;
	read_value(i, p);
#endif
	o << "interference policy: " << error_policy_string(p) << endl;
	read_value(i, p);
	o << "weak-interference policy: " << error_policy_string(p) << endl;
	read_value(i, p);
	o << "invariant-fail policy: " << error_policy_string(p) << endl;
	read_value(i, p);
	o << "invariant-unknown policy: " << error_policy_string(p) << endl;
	read_value(i, p);
	o << "assert-fail (bool) policy: " << error_policy_string(p) << endl;
	read_value(i, p);
	o << "channel-expect-fail policy: " << error_policy_string(p) << endl;
	read_value(i, p);
	o << "exclusion-fail policy: " << error_policy_string(p) << endl;
	read_value(i, p);
	o << "keeper-check policy: " << error_policy_string(p) << endl;
#if 0 && PRSIM_SETUP_HOLD
	read_value(i, p);
	o << "setup-violation policy: " << error_policy_string(p) << endl;
	read_value(i, p);
	o << "hold-violation policy: " << error_policy_string(p) << endl;
#endif
}
#if 0
{	// ignore the name of checkpoint
	string s;
	read_value(i, s);
	o << "autosave: " << s << endl;
}
#endif
	char timing_mode;
	read_value(i, timing_mode);
	o << "timing mode: " << size_t(timing_mode) << endl;
{
	dump_flags tmp;
	tmp.load_object(i);
	tmp.dump(o << "dump flags: {\n") << "}" << endl;
}{
	util::numformat tmp(cout);
	tmp.load_object(i);
	tmp.dump(o << "time-fmt flags: { ") << " }" << endl;
}
#if PRSIM_SETUP_HOLD
	TimingChecker::dump_checkpoint(o, i);
#endif
{
	channel_manager tmp;
	tmp.load_checkpoint(i);
	tmp.dump_checkpoint_state(o) << endl;
}
	read_value(i, header_check);
	o << "footer string: " << header_check << endl;
	return o;
}	// end State::dump_checkpoint

#undef	READ_ALIGN_MARKER

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if result is successful/good.  
 */
bool
State::open_trace(const string& tfn) {
	if (trace_manager) {
cerr << "Error: trace stream already open.  (command ignored)" << endl;
		return true;
	}
	trace_manager =
		excl_ptr<TraceManager>(new TraceManager(tfn, node_pool));
	NEVER_NULL(trace_manager);
	if (trace_manager->good()) {
		flags |= FLAG_TRACE_ON;
		return true;
	} else {
		stop_trace();
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Invoke this if you want tracing to end early.  
 */
void
State::close_trace(void) {
if (trace_manager) {
	// destroying the trace manager should cause it to finish writing out.
	trace_manager = excl_ptr<TraceManager>(NULL);
}
	stop_trace();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if result is successful/good.  
 */
bool
State::open_vcd(const string& tfn) {
	if (vcd_manager) {
cerr << "Error: vcd stream already open.  (command ignored)" << endl;
		return true;
	}
	vcd_manager = excl_ptr<VCDManager>(
		new VCDManager(tfn, mod, current_time, vcd_timescale));
	NEVER_NULL(vcd_manager);
	if (vcd_manager->good()) {
		flags |= FLAG_VCD_ON;
		vcd_manager->dumpvars(*this);
		return true;
	} else {
		stop_vcd();
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Invoke this if you want tracing to end early.  
 */
void
State::close_vcd(void) {
if (vcd_manager) {
	// destroying the vcd manager should cause it to finish writing out.
	vcd_manager = excl_ptr<VCDManager>(NULL);
}
	stop_vcd();
}

//=============================================================================
// explicit class template instantiations
}	// end namespace PRSIM

// explicit template instantiation of signal handler class
template class signal_handler<PRSIM::State>;
// template class EventQueue<EventPlaceholder<real_time> >;

}	// end namespace SIM
}	// end namespace HAC

