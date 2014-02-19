/**
	\file "sim/prsim/State-prsim.hh"
	The state of the prsim simulator.  
	$Id: State-prsim.hh,v 1.50 2011/06/02 01:19:06 fang Exp $

	This file was renamed from:
	Id: State.h,v 1.17 2007/01/21 06:01:02 fang Exp
	to avoid object name collision in shared libraries.  
 */

#ifndef	__HAC_SIM_PRSIM_STATE_HH__
#define	__HAC_SIM_PRSIM_STATE_HH__

// enable additional sanity checking in "sim/event.hh" for event queue
// can be disabled when not debugging
#define	CHECK_UNIQUE_EVENTS			0

#include "sim/time.hh"
#include "sim/state_base.hh"
#include "sim/signal_handler.hh"
#include "sim/event.hh"
#include "sim/prsim/ExprAllocFlags.hh"
#include "sim/prsim/Exception.hh"
#include "sim/prsim/Event-prsim.hh"
#include "sim/prsim/Node.hh"
#include "sim/prsim/Channel-prsim.hh"	// for channels support
#include "sim/prsim/process_state.hh"
#include "sim/command_error_codes.hh"
#include "Object/lang/PRS_enum.hh"	// for expression parenthesization
#include "Object/expr/types.hh"		// for preal_value_type
#include "Object/common/dump_flags.hh"
#include "util/string_fwd.hh"
#include "util/named_ifstream_manager.hh"
#include "util/tokenize_fwd.hh"
#include "util/numformat.hh"
#include "Object/devel_switches.hh"
#if PRSIM_SETUP_HOLD
#include "sim/prsim/TimingChecker.hh"
#endif

/**
	First-come-first-serve ordering of updated nodes.
	Define to 1 for best-attempt to preserve original event
	ordering among equal-timed events to minimize false-positive
	differences in the regression test-suite.
	This is accomplished by preserving the order in-which 
	fanouts were propagated.
 */
#define	PRSIM_FCFS_UPDATED_NODES			1

#include "util/memory/excl_ptr.hh"
#include "util/memory/count_ptr.hh"

/**
	Define to 1 to use a binary-searchable sorted linear array
	for the global expression -> process-id map.
	Rationale: lookup performance, reduces memory footprint slightly
	Goal: 1
	Measurement: is actually a 1-2% slower? 
		so leave disabled until further analysis
 */
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
#define PRSIM_PROCESS_EXPR_MAP_ARRAY		0
#endif

/**
	Define 1 to inline and disable bounds checking on
	get_node() and __get_node().
 */
#define	PRSIM_FAST_GET_NODE			1

/**
	This gives about 3% speedup.
 */
#define	PRSIM_SET_FAST_ALLOCATOR		1
/**
	This slows down runs by 12%, even after using sentinel entry?
	TODO: investigate further.
 */
#define	PRSIM_MAP_FAST_ALLOCATOR		0

#if PRSIM_SET_FAST_ALLOCATOR || PRSIM_MAP_FAST_ALLOCATOR
#include "util/STL/functional_fwd.hh"		// for std::less
#include "util/memory/chunk_map_pool.hh"
#include "util/memory/allocator_adaptor.hh"
#endif

namespace HAC {
namespace entity {
	class footprint;
}
namespace SIM {
namespace PRSIM {
class ExprAlloc;
class TraceManager;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using SIM::INVALID_TRACE_INDEX;
class VCDManager;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using SIM::INVALID_TRACE_INDEX;
using util::memory::count_ptr;
using std::map;
using entity::dump_flags;
using entity::preal_value_type;

//=============================================================================
/**
	The prsim simulation state.
		(modeled after old prsim's struct Prs)
	This structure shall contain no pointers!
	This state should be trivially saveable and restorable.  
	This will even be duplicable for scenario testing!
	TODO: make a CompactState from this State that uses plain
		vector instead of list_vectors for constant-time access.  
		For now, only the expr_graph_node_pool is log(N) access, 
		but it's not accessed during simulation, so HA!
 */
class State : public module_state_base {
	// too lazy to write public mutator methods for the moment.  
	friend class ExprAlloc;
	friend class channel_manager;
	typedef	State				this_type;
public:
	// these typedefs will make it convenient to template this
	// class in the future...
	typedef	rule_time_type			time_type;
	typedef	delay_policy<time_type>		time_traits;
	typedef	NodeState			node_type;
	typedef	Event				event_type;
	typedef	EventPool			event_pool_type;
	typedef	event_type::cause_type		event_cause_type;
	typedef	node_type::event_cause_type	node_cause_type;
	/**
		NOTE: pass by event_cause_type by reference.  
	 */
	typedef	const event_cause_type&		cause_arg_type;
	typedef	ExprState			expr_state_type;
	typedef	unique_process_subgraph::expr_struct_type
						expr_struct_type;
	typedef	unique_process_subgraph::rule_type
						rule_type;
	typedef	size_t				trace_index_type;
	typedef	TraceManager			trace_manager_type;
	typedef	VCDManager			vcd_manager_type;
	typedef	EventPlaceholder<time_type>	event_placeholder_type;
	typedef	EventQueue<event_placeholder_type>	event_queue_type;
	typedef	vector<node_type>		node_pool_type;
	/**
		The first node index is the one that just changed, 
		the second index refers to the node that caused it, 
		deduced from some event queue.  
	 */
	typedef	std::pair<node_index_type, node_index_type>
							step_return_type;
	typedef	size_t				lock_index_type;
	/**
		Exception thrown when there is a violation of 
		exclusion among exclhi or excllo checked rings.  
	 */
	struct excl_exception : public step_exception {
		/// true for exclhi, false for excllo
		bool				type;
		/// index of the mutex lock triggered
		lock_index_type			lock_id;
		/// node that failed to set lock
		node_index_type			node_id;

		excl_exception(const bool b, const lock_index_type li, 
			const node_index_type ni) : type(b), 
			lock_id(li), node_id(ni) { }

		error_policy_enum
		inspect(const State&, ostream&) const;
	};	// end struct excl_exception

	// generic_exception relocated to sim/prsim/Exception.hh

	/**
		Exception type thrown when there is an invariant
		violation and the error policy is set to fatal.
	 */
	struct invariant_exception : public generic_exception {

		invariant_exception(const node_index_type n, 
			const error_policy_enum e) : 
			generic_exception(n, e) { }

	};	// end struct invariant_exception

#if PRSIM_SETUP_HOLD
	typedef	TimingChecker::timing_exception		timing_exception;
#endif

	typedef	generic_exception	interference_exception;
	typedef	generic_exception	instability_exception;
	typedef	generic_exception	keeper_fail_exception;

	typedef	count_ptr<step_exception>	exception_ptr_type;
#define	THROWS_STEP_EXCEPTION
	// should be nothrow, or C++11 noexcept
private:
	struct evaluate_return_type;

	/**
		Return codes for set_node_time.  
	 */
	enum enqueue_status {
		ENQUEUE_ACCEPT = 0,
		ENQUEUE_WARNING = 1,
		ENQUEUE_REJECT = 2,
		ENQUEUE_FATAL = 3
	};

	/**
		Simulation flags, bit fields, corresponding the
		the flags member.  
	 */
	enum simulation_flags {
		/**
			Allow unstable events to be dropped off queue
			instead of propagating unknowns.  
			Also dequeues weakly unstable events (involving X).  
		 */
		FLAG_UNSTABLE_DEQUEUE = 0x01,
		/**
			Whether or not the simulation was stopped
			by interrupt or event error/warning.  
			TODO: This could be redundant with the 
			interrupted flag. 
		 */
		FLAG_STOP_SIMULATION = 0x02,
		FLAG_ESTIMATE_ENERGY = 0x04,
		/**
			Print cause information on watched nodes.  
			Causes are always tracked.  
		 */
		FLAG_SHOW_CAUSE = 0x08,
		/**
			Use this to determine whether or not to print
			transition on every node, rather than using the
			sparse watch_list.  
		 */
		FLAG_WATCHALL = 0x10,
		/**
			Whether or not to show transition counts
			when displaying nodes.  
		 */
		FLAG_SHOW_TCOUNTS = 0x20,
		/**
			Set to true to check exclusiveness of nodes.  
		 */
		FLAG_CHECK_EXCL = 0x40, 
		/**
			If true, then evaluate the fanouts in random order, 
			useful for arbitration in certain instances.  
		 */
		FLAG_RANDOM_FANOUT_EVALUATION_ORDERING = 0x80,
		/**
			If true, dump events of watched nodes as they
			are entered into the event queue.  
		 */
		FLAG_WATCH_QUEUE = 0x100,
		/**
			If true, dump every event as it is enqueued in
			the primary event queue.  
		 */
		FLAG_WATCHALL_QUEUE = 0x200,
#if PRSIM_WEAK_RULES
		/**
			Global flag to heed or ignore weak rules.  
			Default off.  
		 */
		FLAG_WEAK_RULES = 0x400,
		/**
			Whether or not to print weak rules in fanin/out.
		 */
		FLAG_HIDE_WEAK_RULES = 0x800,
#endif
		/**
			Autosave on exit.
		 */
		FLAG_AUTOSAVE = 0x1000,
		/**
			Flag used by the interpreter to control
			whether or not successful assertions are 
			confirmed with a message. 
		 */
		FLAG_CONFIRM_ASSERTS = 0x2000,
#if PRSIM_UPSET_NODES
		/**
			When set, print more verbose information
			when interacting with frozen nodes.  
		 */
		FLAG_FROZEN_VERBOSE = 0x4000,
#endif
		/**
			Set to true when events are being 
			recorded to a prsim trace file.
		 */
		FLAG_TRACE_ON = 0x8000,
		/**
			Set to true when events are being traced, 
			and recorded to a vcd file.
		 */
		FLAG_VCD_ON = 0x10000,
#if PRSIM_MK_EXCL_BLOCKING_SET
		/**
			Set to true to treat instabilities that
			are the result of force excl rings as
			dequeued/cancelled events.
			Otherwise, pending event is changed to X.
			By default, is more conservative, -> X.
			The other behavior (dequeue) is consistent
			with legacy prsim.
		 */
		FLAG_EXCL_UNSTABLE_DEQUEUE = 0x20000,
#endif
		/**
			If true, vacuous events are automatically
			dequeued and skipped, and step returns
			when it finds the next non-vacuous event.
			This avoids blank events.
			Default: false
			Want this to be true for cosim,
			so that the next_event_time() reported
			accounts for possibly vacuous events.  
		 */
		FLAG_STOP_ON_VACUOUS = 0x40000,
		/// initial flags
		FLAGS_DEFAULT = FLAG_CHECK_EXCL | FLAG_SHOW_CAUSE,
		/**
			Flags to set upon initialize().
		 */
		FLAGS_INITIALIZE_SET_MASK = 0x00,
		/**
			Flags to clear upon initialize().
			We keep the previous timing modes and watch mode.  
		 */
		FLAGS_INITIALIZE_CLEAR_MASK =
			FLAG_STOP_SIMULATION,
		/**
			Flag states that should NOT be saved. 
		 */
		FLAGS_CHECKPOINT_MASK = ~(FLAG_AUTOSAVE | FLAG_TRACE_ON
			| FLAG_VCD_ON
			)
	};
	/**
		As we add more flags this will have to expand...
	 */
	typedef	size_t				flags_type;

public:
	/**
		Default enumeration values.
	 */
	enum {
		ERROR_DEFAULT_INVARIANT_FAIL = ERROR_BREAK,
		ERROR_DEFAULT_INVARIANT_UNKNOWN = ERROR_WARN,
		ERROR_DEFAULT_UNSTABLE = ERROR_BREAK,
		ERROR_DEFAULT_WEAK_UNSTABLE = ERROR_WARN,
#if PRSIM_MK_EXCL_BLOCKING_SET
		ERROR_DEFAULT_EXCL_UNSTABLE = ERROR_BREAK,
#endif
		ERROR_DEFAULT_INTERFERENCE = ERROR_BREAK,
		ERROR_DEFAULT_WEAK_INTERFERENCE = ERROR_WARN,
		ERROR_DEFAULT_ASSERT_FAIL = ERROR_FATAL,
		ERROR_DEFAULT_CHANNEL_EXPECT_FAIL = ERROR_FATAL,
		ERROR_DEFAULT_EXCL_CHECK_FAIL = ERROR_FATAL,
		ERROR_DEFAULT_KEEPER_CHECK = ERROR_IGNORE,
		ERROR_DEFAULT_GENERIC = ERROR_BREAK	// unused
	};

private:
	/**
		Return type to indicate whether or not to break.  
	 */
	typedef	error_policy_enum			break_type;

	enum timing_enum {
		/**
			Uses per-rule after-delays.  
			These can be manually annotated or
			automatically computed by some model, 
			such as sizing.  
		 */
		TIMING_AFTER = 0,
		/**
			Uses delay 10 units for all events, 
			(except on specially marked rules).
		 */
		TIMING_UNIFORM = 1,
		/**
			Uses random timing for all events, 
			(except on specially marked rules).  
		 */
		TIMING_RANDOM = 2,
		/**
			Mode in which delay is one of two exact values, 
			tests bounded delay assumptions, and 
			scalable delay assumptions.  
			*Ignores* all after/after_min/after_max= delay values. 
		 */
		TIMING_BINARY = 3,
		/**
			Mode in which delay is one of two exact values, 
			tests bounded delay assumptions, and 
			scalable delay assumptions.  
			Overridden by after_min/after_max= delay values. 
		 */
		TIMING_BOUNDED = 4,
		/**
			The mode on start-up.  
		 */
		TIMING_DEFAULT = TIMING_UNIFORM
	};

	/**
		Lookup table for translating pull-up/pull-dn states 
		to next node value.  
		Used when node is re-evaluated.  
		Indexed by [pull-up state][pull-down state].
	 */
	static const value_enum			pull_to_value[3][3];

#if PRSIM_SET_FAST_ALLOCATOR
	typedef util::memory::chunk_map_pool<node_index_type,
			sizeof(size_t) << 3>	// use machine int size
						set_pool_alloc_type;
	typedef util::memory::allocator_adaptor<set_pool_alloc_type>
						set_override_allocator_type;
	typedef set<node_index_type, std::less<node_index_type>,
			set_override_allocator_type>
						index_set_type;
#else
	typedef	set<node_index_type>		index_set_type;
#endif
#if PRSIM_MAP_FAST_ALLOCATOR
	typedef util::memory::chunk_map_pool<node_index_type,
			sizeof(size_t) << 3>	// use machine int size
						map_pool_alloc_type;
	typedef util::memory::allocator_adaptor<map_pool_alloc_type>
						map_override_allocator_type;
#endif
public:
#if !PRSIM_HIERARCHICAL_RINGS
	/**
		Instead of using circular linked lists with pointers, 
		we use a map of (cyclic referenced) indices to represent
		the exclusive rings. 
		Requires half as much memory as equivalent ring of pointers.  
		Another possiblity: fold these next-indices into the 
			Node structure.  
		Alternative: use map for sparser exclusive rings.  
		Alternative: use sorted array (for fast binary search)
	 */
//	typedef	index_set_type			ring_set_type;
	typedef	std::set<node_index_type>	ring_set_type;
#endif
	typedef	unique_process_subgraph::node_set_type
						node_set_type;
protected:
#if PRSIM_MK_EXCL_BLOCKING_SET
	/**
		Shared ring expression state.
		This combines ring structural and state information.
	 */
	struct ring_counter_state {
		/**
			Participating nodes in this ring.  
		 */
		ring_set_type			elems;
		/**
			A 0 count means the ring is unlocked, 
			and that any member may fire freely.
			A non-zero count means that only an unlocking
			transition is permitted, all others must be 
			suppressed while locked.
			Note: char value type limits ring size to 256.
		 */
		char				count;
	
		ring_counter_state() : elems(), count(0) { }
		// default dtor

		size_t
		size(void) const { return elems.size(); }

		ring_set_type::const_iterator
		begin(void) const { return elems.begin(); }

		ring_set_type::const_iterator
		end(void) const { return elems.end(); }

	};	// end struct ring_counter_state
#endif
#if PRSIM_MK_EXCL_BLOCKING_SET
	typedef	vector<ring_counter_state>
#else
	typedef	vector<ring_set_type>
#endif
						mk_excl_ring_map_type;
	/**
		This needs to be a unique forward mapping (injection),
		i.e. no duplicate events.  
		The (key, value) pair is just a transposed event placeholder.
		TODO: pool-allocate?
	 */
	typedef	std::map<event_index_type, time_type>
						mk_excl_queue_type;
protected:
	struct node_update_info {
		rule_index_type			rule_index;
#if EVENT_INCLUDE_RULE_POINTER
		const rule_type*		rule_ptr;
#endif
		// Q: distinguish between rules that turned on/off?
		// should keep around previous pull-state
		// being able to diff previous pull-state against
		// current pull-state may eliminate duplicate
		// interference diagnostics
		pull_set			old_pull_set;
#if PRSIM_MK_EXCL_BLOCKING_SET
		// auxiliary flag to override rule as blocked for excl
		bool				excl_blocked;

		node_update_info() :
			rule_index(INVALID_RULE_INDEX),
#if EVENT_INCLUDE_RULE_POINTER
			rule_ptr(NULL),
#endif
			excl_blocked(false) { }
#endif
	};	// end struct node_update_info
#if PRSIM_MAP_FAST_ALLOCATOR
	typedef	map<node_index_type, node_update_info, 
			std::less<node_index_type>, map_override_allocator_type>
#else
	typedef	map<node_index_type, node_update_info>
#endif
						updated_nodes_type;
protected:
	/**
		key: (possibly) updated node index
		value: old value
		If the old value matches new value, don't print.
		This can happen if atomic updates form a DAG.
	 */
	typedef	map<node_index_type, value_enum>	
						atomic_updated_nodes_type;
protected:
#if PRSIM_FCFS_UPDATED_NODES
	/**
		Preserve the order in which fanouts were processed.
	 */
	typedef vector<node_index_type>		updated_nodes_queue_type;
#endif
	typedef	vector<event_queue_type::value_type>
						temp_queue_type;
	typedef	vector<expr_index_type>		expr_trace_type;
	typedef	node_type::fanout_array_type	fanout_array_type;
	/**
		Exclusive checks are implemented as lock flags.  
		reminder: this is specialized in STL :)
		0th index is reserved as a placeholder.  
		Using bools, as soon as exclusion is violated, 
		cannot continue because lock will be incoherent.  
		For continuability, use semaphore counters.  
	 */
	typedef	vector<bool>			check_excl_lock_pool_type;
	enum {	INVALID_LOCK_INDEX = 0,
		FIRST_VALID_LOCK = INVALID_LOCK_INDEX +1
	};
	typedef	vector<lock_index_type>		lock_index_list_type;
#if !PRSIM_HIERARCHICAL_RINGS
	/**
		Sparse map of nodes to their check-exclusive rings.  
		Could also use std::multimap, and look up using equal_range.
	 */
	typedef	map<node_index_type, lock_index_list_type>
						check_excl_ring_map_type;
#if PRSIM_MK_EXCL_BLOCKING_SET
	/**
		Sparse map of nodes to their force-exclusive rings.  
		Could also use std::multimap, and look up using equal_range.
	 */
	typedef	map<node_index_type, lock_index_list_type>
						mk_excl_counter_map_type;
#endif
#endif
	/**
		Useful for collating check-excl rings sparsely, 
		a reverse map of lock_index to set of nodes.  
		Not generated upon allocation, only upon dumping.  
	 */
	typedef	map<lock_index_type, ring_set_type>
						check_excl_reverse_map_type;
	/**
		Useful for reverse-mapping all check-excl rings.  
	 */
	typedef	vector<ring_set_type>		check_excl_array_type;
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
	/**
		TODO: use a hierarchical local_private_entry map scheme!
		Analogous to global_entry_context's id lookups.

		Translates a global expression ID to the process ID
		to which the expression belongs.  
		key is the *lowest* expression-index 'owned' by
		the value-indexed process.  
		Basically each process owns a contiguous 
		range of expr indices.  
	 */
#if PRSIM_PROCESS_EXPR_MAP_ARRAY
	typedef	pair<expr_index_type, process_index_type>
					expr_process_entry_type;
	struct expr_id_key_compare;
	typedef	vector<expr_process_entry_type>
					global_expr_process_id_map_type;
#else
	typedef	map<expr_index_type, process_index_type>
					global_expr_process_id_map_type;
#endif
#endif
	typedef map<const entity::footprint*, process_index_type>
						process_footprint_map_type;
	/**
		Collection of unique process footprints.
	 */
	typedef	vector<unique_process_subgraph>	unique_process_pool_type;
	/**
		Where each process's state is kept.  
		Array is indexed by process IDs from the above
		global_expr_process_id_map (and process_rule_map).  
	 */
	typedef	vector<process_sim_state>	process_state_array_type;
public:
	/**
		Options with which global prsim state was constructed.
	 */
	const ExprAllocFlags			expr_alloc_flags;
private:
	/**
		Collection of node states.
	 */
	node_pool_type				node_pool;
	/**
		Translates unique prs_footprint to unique process index.  
	 */
	process_footprint_map_type		process_footprint_map;
	// TODO: per process instance attributes!
	/**
		Collection of unique process footprints.  
		This is contructed and populated by ExprAlloc,
		but then preserved for lookup.
	 */
	unique_process_pool_type		unique_process_pool;
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
	/**
		Maps global expression ID to owner process.  
		Tradeoff: this map will be smaller to search than
		the entire process_state_array because many processes
		in the hierarchy that would be empty and omitted
		would be included in the search tree.  
	 */
	global_expr_process_id_map_type		global_expr_process_id_map;
#endif
	/**
		Per-process state is kept in an array, indexed
		by process index.  0 is valid, but reserved for 
		the top-level process. 
		Alternative idea is to just use one contiguous array 
		for all state, but still be able to extract out
		ranges based on indexed mapping.  
	 */
	process_state_array_type		process_state_array;
	event_pool_type				event_pool;
	event_queue_type			event_queue;
	/// coerce exclusive-hi ring
	mk_excl_ring_map_type			mk_exhi;
	/// coerce exclusive-low ring
	mk_excl_ring_map_type			mk_exlo;
#if PRSIM_MK_EXCL_BLOCKING_SET
	/// map of exclusive-hi force rings
	mk_excl_counter_map_type		mk_exhi_counter_map;
	/// map of exclusive-low force rings
	mk_excl_counter_map_type		mk_exlo_counter_map;
	/**
		The set of nodes whose events should be suppressed
		or cancelled due to force-excl rings (locked).  
	 */
	std::set<node_index_type>		__mk_excl_blocking_set;
#else
	/// coerced exclusive-hi logic queue
	mk_excl_queue_type			exclhi_queue;
	/// coerced exclusive-low logic queue
	mk_excl_queue_type			excllo_queue;
#endif
	updated_nodes_type			updated_nodes;
#if PRSIM_FCFS_UPDATED_NODES
	updated_nodes_queue_type		updated_nodes_queue;
#endif
	/// pool of exclusive-hi checking locks
	check_excl_lock_pool_type		check_exhi_ring_pool;
	/// pool of exclusive-low checking locks
	check_excl_lock_pool_type		check_exlo_ring_pool;
	/// sparse set of node-associated lock sets
	check_excl_ring_map_type		check_exhi;
	/// sparse set of node-associated lock sets
	check_excl_ring_map_type		check_exlo;
#if PRSIM_SETUP_HOLD
	/// timing constraint checking subsystem
	TimingChecker				timing_checker;
#endif	// PRSIM_SETUP_HOLD
	// current time, etc...
	time_type				current_time;
	time_type				uniform_delay;
public:	// too lazy to write accessors
	// these values are also used for timing binary
	// for random timing only, default lower bound of delay
	time_type				default_after_min;
	// for random timing only, default upper bound of delay
	time_type				default_after_max;
	/**
		For timing binary only, sets the probability of
		choosing the first of two delay values.  
	 */
	preal_value_type			timing_probability;
private:
	// vectors
	/**
		Extension to manage channel environments and actions. 
	 */
	channel_manager				_channel_manager;
	/// responsible for recording trace file
	excl_ptr<trace_manager_type>		trace_manager;
	/// controls frequency of trace flushing
	trace_index_type			trace_flush_interval;
	/// responsible for recording vcd trace file
	excl_ptr<vcd_manager_type>		vcd_manager;
	/// scale factor for vcd timestamps
	double					vcd_timescale;
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
public:
	// parameters for managing module_state_base's footprint_frame cache
	// period at which cache decays, half-life
	size_t					cache_half_life;
private:
	// when counter hits zero, age the cache
	size_t					cache_countdown;
#endif
	// mode of operation
	// operation flags
	flags_type				flags;
	/// poicy when invariants are violated
	error_policy_enum			invariant_fail_policy;
	/// poicy when invariants are *possibly* violated, due to 'X' pull
	error_policy_enum			invariant_unknown_policy;
	/// controls the simulation behavior upon instability
	error_policy_enum			unstable_policy;
	/// controls the simulation behavior upon weak-instability
	error_policy_enum			weak_unstable_policy;
#if PRSIM_MK_EXCL_BLOCKING_SET
	error_policy_enum			excl_unstable_policy;
#endif
	/// controls the simulation behavior upon interference
	error_policy_enum			interference_policy;
	/// controls the simulation behavior upon weak-interference
	error_policy_enum			weak_interference_policy;
	/// control affect of assertion fail
	error_policy_enum			assert_fail_policy;
	/// control handling of channel expect failures
	error_policy_enum			channel_expect_fail_policy;
	/// control handling of exclusion failures
	error_policy_enum			excl_check_fail_policy;
	/// control handling/checking of keeper invariants
	error_policy_enum			keeper_check_fail_policy;
	/// name of automatically taken checkpoint
	string					autosave_name;
	/// timing mode
	uchar					timing_mode;
	// loadable random seed?
public:
	// save flags used for printing
	dump_flags				_dump_flags;
	// for formatting timestamps
	util::numformat				time_fmt;
private:
	/**
		For aggregating multiple atomic updated nodes b/c
		a single step may result in multiple atomic updates.
		This is really intended for printing watched nodes.
		FIXME: this is currently un-ordered.
		To reconstruct a causality-preserving ordering,
		we'd need to traverse the update-DAG.
	 */
	atomic_updated_nodes_type		__atomic_updated_nodes;
	/**
		Multiple exceptions are kept here.
		This is cleared every time a step() is begun.
	 */
	mutable vector<exception_ptr_type>	recent_exceptions;
	/**
		set by the SIGINT signal handler
		(is this redundant with the STOP flag?)
	 */
	volatile bool				interrupted;
	/**
		Auxiliary array for in-place random reordering
		of fanout indices for evaluation.  
		Never maintained for checkpointing.  
	 */
	fanout_array_type			__shuffle_indices;
	/**
		Auxiliary array of nodes whose pull (in either direction)
		turned OFF, and are thus candidates for checking for 
		missing keepers.
	 */
	typedef	index_set_type			keeper_check_set_type;
	keeper_check_set_type			__keeper_check_candidates;

// for lazy invariants:
	// using pair for built-in < comparison operator
	typedef	pair<process_index_type, rule_index_type>
						rule_reference_type;
	typedef	map<rule_reference_type, pull_enum>
						invariant_update_map_type;
	/**
		This map is used to accumulate invariant expression updates.  
		When one node fans out to multiple sub-expressions of the
		same invariant expression, only the final state is
		taken; transient values are ignored.  
		The contents of this map are short-lived are short-lived.
	 */
	invariant_update_map_type		__invariant_update_map;
public:
	/**
		Signal handler class that binds the State reference
		for the duration of the scope in which it is declared.  
	 */
	typedef	signal_handler<this_type>	signal_handler_type;
public:
	explicit
	State(const module&, const ExprAllocFlags&);
private:
	// inaccessible undefined copy-constructor ... for now
	State(const State&);

public:
	~State();

	/// initializes the simulator state, all nodes and exprs X
	void
	initialize(void);

	/// Wipes the simulator state AND modes as if fresh started.  
	void
	reset(void);

	/// wipes the simulation state (like destructor)
	void
	destroy(void);

	void
	x_all(void);

private:
	void
	__initialize_state(const bool, const bool);

	void
	__initialize_time(void);

	void
	__initialize(const bool);

	node_type&
	__get_node(const node_index_type i)
#if PRSIM_FAST_GET_NODE
		{ return node_pool[i]; }
#else
		;
#endif

public:

	const node_pool_type&
	get_node_pool(void) const { return node_pool; }

	const node_type&
	get_node(const node_index_type i) const
#if PRSIM_FAST_GET_NODE
		{ return node_pool[i]; }
#else
		;
#endif

	node_index_type
	get_node_index(const node_type& n) const {
		return node_index_type(std::distance(&node_pool[0], &n));
	}

public:
	ostream&
	dump_node_canonical_name(ostream&, const node_index_type) const;

	ostream&
	dump_node_canonical_name(ostream&, const node_type&) const;

	string
	get_node_canonical_name(const node_index_type) const;

	void
	backtrace_node(ostream&, const node_index_type) const;

	void
	backtrace_node(ostream&, const node_index_type, const value_enum) const;

	process_index_type
	get_num_processes(void) const { return process_state_array.size(); }

	ostream&
	dump_process_canonical_name(ostream&, const process_sim_state&) const;

	ostream&
	dump_process_canonical_name(ostream&, const process_index_type) const;

	string
	get_process_canonical_name(const process_index_type) const;

#if !CACHE_GLOBAL_FOOTPRINT_FRAMES
	footprint_frame_map_type
#else
	const footprint_frame_map_type&
#endif
	get_footprint_frame_map(const process_index_type pid) const;

#if CACHE_GLOBAL_FOOTPRINT_FRAMES
	const size_t&
	get_cache_countdown(void) const { return cache_countdown; }
#endif

	const process_sim_state&
	get_process_state(const process_index_type pid) const {
		return process_state_array[pid];
	}

	faninout_struct_type::counter
	count_node_fanins(const node_index_type) const;

	void
	finish_process_type_map(void);

	process_index_type
	lookup_process_index(const process_sim_state&) const;

	const process_sim_state&
	lookup_global_expr_process(const expr_index_type) const;

private:
	process_sim_state&
	lookup_global_expr_process(const expr_index_type);

public:
	node_index_type
	translate_to_global_node(const process_sim_state&, 
		const node_index_type) const;

	node_index_type
	translate_to_global_node(const process_index_type, 
		const node_index_type) const;

	bool
	is_rule_expr(const expr_index_type) const;

	const rule_type*
	lookup_rule(const expr_index_type) const;

	// this fast-forwards, but should make sure event-queue is safe
	void
	update_time(const time_type t) {
		current_time = t;
	}

	void
	safe_fast_forward(const time_type&);

	void
	advance_time(const time_type t) {
		current_time += t;
	}

	const time_type&
	time(void) const { return current_time; }

	ostream&
	dump_timing(ostream&) const;

	static
	bool
	parse_min_max_delay(const string&, time_type&, time_type&);

	bool
	set_timing(const string&, const string_list&);

	bool
	timing_is_randomized(void) const {
		return !(timing_mode == TIMING_UNIFORM ||
			timing_mode == TIMING_AFTER);
		// everything else is randomized
	}

	static
	ostream&
	help_timing(ostream&);

	void
	randomize(void) { timing_mode = TIMING_RANDOM; }

	void
	norandom(void) { timing_mode = TIMING_UNIFORM; }

	bool
	show_tcounts(void) const { return flags & FLAG_SHOW_TCOUNTS; }

	void
	set_show_tcounts(void) { flags |= FLAG_SHOW_TCOUNTS; }

	void
	clear_show_tcounts(void) { flags &= ~FLAG_SHOW_TCOUNTS; }

	bool
	show_cause(void) const { return flags & FLAG_SHOW_CAUSE; }

	void
	set_show_cause(void) { flags |= FLAG_SHOW_CAUSE; }

	void
	clear_show_cause(void) { flags &= ~FLAG_SHOW_CAUSE; }

	bool
	eval_ordering_is_random(void) const {
		return flags & FLAG_RANDOM_FANOUT_EVALUATION_ORDERING;
	}

	void
	set_eval_ordering_random(void) {
		flags |= FLAG_RANDOM_FANOUT_EVALUATION_ORDERING;
	}

	void
	set_eval_ordering_inorder(void) {
		flags &= ~FLAG_RANDOM_FANOUT_EVALUATION_ORDERING;
	}

#if PRSIM_WEAK_RULES
	void
	enable_weak_rules(void) { flags |= FLAG_WEAK_RULES; }

	void
	disable_weak_rules(void) { flags &= ~FLAG_WEAK_RULES; }

	bool
	weak_rules_enabled(void) const { return flags & FLAG_WEAK_RULES; }

	void
	show_weak_rules(void) { flags &= ~FLAG_HIDE_WEAK_RULES; }

	void
	hide_weak_rules(void) { flags |= FLAG_HIDE_WEAK_RULES; }

	bool
	weak_rules_shown(void) const { return !(flags & FLAG_HIDE_WEAK_RULES); }
#endif	// PRSIM_WEAK_RULES

	bool
	confirm_asserts(void) const {
		return flags & FLAG_CONFIRM_ASSERTS;
	}

	void
	confirm_asserts(const bool b) {
		if (b)	flags |= FLAG_CONFIRM_ASSERTS;
		else	flags &= ~FLAG_CONFIRM_ASSERTS;
	}

	/**
		Extension to manage channel environments and actions. 
	 */
	channel_manager&
	get_channel_manager(void) { return _channel_manager; }

private:
	break_type
	flush_channel_events(const vector<env_event_type>&, cause_arg_type);

public:
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	bool
	reset_channel(channel&);

	bool
	resume_channel(channel&);
#else
	bool
	reset_channel(const string&);

	bool
	resume_channel(const string&);
#endif

	void
	reset_all_channels(void);

	void
	resume_all_channels(void);

	void
	reset_tcounts(void);

	ostream&
	dump_mode(ostream&) const;

// TODO: document these modes in table format in texinfo
	void
	set_mode_reset(void) {
		unstable_policy = ERROR_BREAK;
		weak_unstable_policy = ERROR_WARN;
		interference_policy = ERROR_BREAK;
		weak_interference_policy = ERROR_IGNORE;
	}

	void
	set_mode_run(void) {
		unstable_policy = ERROR_BREAK;
		weak_unstable_policy = ERROR_WARN;
		interference_policy = ERROR_BREAK;
		weak_interference_policy = ERROR_WARN;
	}

	void
	set_mode_breakall(void) {
		unstable_policy = ERROR_BREAK;
		weak_unstable_policy = ERROR_BREAK;
		interference_policy = ERROR_BREAK;
		weak_interference_policy = ERROR_BREAK;
	}

	void
	set_mode_fatal(void); 

#define	DEFINE_POLICY_CONTROL_SET(name)				\
	void							\
	set_##name##_policy(const error_policy_enum e) {	\
		name##_policy = e;				\
	}

#define	DEFINE_POLICY_CONTROL_GET(name)				\
	error_policy_enum					\
	get_##name##_policy(void) const {			\
		return name##_policy;				\
	}

#define	DEFINE_POLICY_CONTROL_SET_MEM(mem, name)		\
	void							\
	set_##name##_policy(const error_policy_enum e) {	\
		mem. name##_policy = e;				\
	}

#define	DEFINE_POLICY_CONTROL_GET_MEM(mem, name)		\
	error_policy_enum					\
	get_##name##_policy(void) const {			\
		return mem. name##_policy;			\
	}

	bool
	check_all_invariants(ostream&) const;

	ostream&
	dump_invariants(ostream&, const process_index_type, const bool) const;

	ostream&
	dump_all_invariants(ostream&, const bool) const;

	DEFINE_POLICY_CONTROL_SET(invariant_fail)
	DEFINE_POLICY_CONTROL_GET(invariant_fail)
	DEFINE_POLICY_CONTROL_SET(invariant_unknown)
	DEFINE_POLICY_CONTROL_GET(invariant_unknown)

	DEFINE_POLICY_CONTROL_SET(unstable)
	DEFINE_POLICY_CONTROL_SET(weak_unstable)
	DEFINE_POLICY_CONTROL_SET(interference)
	DEFINE_POLICY_CONTROL_SET(weak_interference)
	DEFINE_POLICY_CONTROL_GET(unstable)
	DEFINE_POLICY_CONTROL_GET(weak_unstable)
	DEFINE_POLICY_CONTROL_GET(interference)
	DEFINE_POLICY_CONTROL_GET(weak_interference)

#if PRSIM_MK_EXCL_BLOCKING_SET
	DEFINE_POLICY_CONTROL_GET(excl_unstable)
	DEFINE_POLICY_CONTROL_SET(excl_unstable)
#endif

	DEFINE_POLICY_CONTROL_SET(assert_fail)
	DEFINE_POLICY_CONTROL_SET(channel_expect_fail)
	DEFINE_POLICY_CONTROL_SET(excl_check_fail)
	DEFINE_POLICY_CONTROL_SET(keeper_check_fail)
	DEFINE_POLICY_CONTROL_GET(assert_fail)
	DEFINE_POLICY_CONTROL_GET(channel_expect_fail)
	DEFINE_POLICY_CONTROL_GET(excl_check_fail)
	DEFINE_POLICY_CONTROL_GET(keeper_check_fail)
#if PRSIM_SETUP_HOLD
	DEFINE_POLICY_CONTROL_GET_MEM(timing_checker, setup_violation)
	DEFINE_POLICY_CONTROL_GET_MEM(timing_checker, hold_violation)
	DEFINE_POLICY_CONTROL_SET_MEM(timing_checker, setup_violation)
	DEFINE_POLICY_CONTROL_SET_MEM(timing_checker, hold_violation)
#endif

#undef	DEFINE_POLICY_CONTROL_SET
#undef	DEFINE_POLICY_CONTROL_GET

	/// \return true if any events are in queue, *** even if killed ***
	bool
	pending_events(void) const { return !event_queue.empty(); }

	/// \return true if any non-killed (live) events remain in queue
	bool
	pending_live_events(void) const;

	static
	time_type
	uniform_random_delay(void);

	static
	time_type
	exponential_random_delay(void);

	time_type
	next_event_time(void) const;

	value_enum
	node_to_value(const string&, const node_index_type) const;

	int
	set_node_time(const node_index_type, const value_enum val, 
		const time_type t, const bool f);

	int
	set_node_after(const node_index_type n, const value_enum val, 
		const time_type t, const bool f) {
		return set_node_time(n, val, this->current_time +t, f);
	}

	int
	set_node(const node_index_type n, const value_enum val,
			const bool f) {
		return set_node_time(n, val, this->current_time, f);
	}

	step_return_type
	set_node_immediately(const node_index_type, const value_enum,
		const bool f);

	void
	unset_node(const node_index_type);

	void
	unset_all_nodes(void);

#if PRSIM_UPSET_NODES
	void
	set_frozen_verbose(const bool b) {
		if (b)	flags |= FLAG_FROZEN_VERBOSE;
		else	flags &= ~FLAG_FROZEN_VERBOSE;
	}

	bool
	is_frozen_verbose(void) const {
		return flags & FLAG_FROZEN_VERBOSE;
	}

	void
	freeze_node(const node_index_type);
#endif

	void
	set_node_breakpoint(const node_index_type);

	void
	clear_node_breakpoint(const node_index_type);

	void
	clear_all_breakpoints(void);

	ostream&
	dump_breakpoints(ostream&) const;

private:
	step_return_type
	execute_immediately(const event_index_type, const time_type&)
		THROWS_STEP_EXCEPTION;

	step_return_type
	execute_immediately(const event_type&, const time_type&)
		THROWS_STEP_EXCEPTION;

	void
	clear_exceptions(void) {
		recent_exceptions.clear();
	}

public:
	step_return_type
	step(void) THROWS_STEP_EXCEPTION;

	step_return_type
	cycle(void) THROWS_STEP_EXCEPTION;

	size_t
	fatal_exceptions(void) const {
		return recent_exceptions.size();
	}

	bool
	is_fatal(void) const {
		return recent_exceptions.size();
	}

	void
	stop(void) {
		flags |= FLAG_STOP_SIMULATION;
		interrupted = true;
	}

	bool
	stopped(void) const {
		return flags & FLAG_STOP_SIMULATION;
		// return interrupted;
	}

	void
	resume(void) {
		flags &= ~FLAG_STOP_SIMULATION;
		interrupted = false;
		clear_exceptions();
	}

	bool
	stopped_or_fatal(void) const {
		return stopped() || is_fatal();
	}

	void
	watch_node(const node_index_type);

	void
	unwatch_node(const node_index_type);

	void
	watch_all_nodes(void) { flags |= FLAG_WATCHALL; }

	void
	nowatch_all_nodes(void) { flags &= ~FLAG_WATCHALL; }

	void
	unwatch_all_nodes(void);

	bool
	watching_all_nodes(void) const {
		return flags & FLAG_WATCHALL;
	}

	bool
	is_watching_node(const node_index_type) const;

	void
	watch_event_queue(void) { flags |= FLAG_WATCH_QUEUE; }

	void
	nowatch_event_queue(void) { flags &= ~FLAG_WATCH_QUEUE; }

	bool
	watching_event_queue(void) const { return flags & FLAG_WATCH_QUEUE; }

	void
	watchall_event_queue(void) { flags |= FLAG_WATCHALL_QUEUE; }

	void
	nowatchall_event_queue(void) { flags &= ~FLAG_WATCHALL_QUEUE; }

	bool
	watching_all_event_queue(void) const {
		return flags & FLAG_WATCHALL_QUEUE;
	}

	/// for any user-defined structures from the .hac
	void
	watch_structure(void);

	/// for any user-defined structures from the .hac
	void
	unwatch_structure(void);

	ostream&
	dump_watched_nodes(ostream&) const;

	void
	status_nodes(const value_enum, vector<node_index_type>&) const;

	void
	status_interference(const bool, vector<node_index_type>&) const;

	void
	status_driven(const pull_enum, const bool, 
		vector<node_index_type>&) const;

#if PRSIM_UPSET_NODES
	void
	status_frozen(vector<node_index_type>&) const;
#endif

	ostream&
	print_status_nodes(ostream&, const value_enum, const bool) const;

	ostream&
	print_status_interference(ostream&, const bool) const;

	bool
	print_status_driven(ostream&, const pull_enum, const bool, 
		const bool) const;

#if PRSIM_UPSET_NODES
	ostream&
	print_status_frozen(ostream&, const bool) const;
#endif

	bool
	have_atomic_updates(void) const {
		return !__atomic_updated_nodes.empty();
	}

	ostream&
	print_watched_atomic_updated_nodes(ostream&) const;

	bool
	dequeue_unstable_events(void) const {
		return flags & FLAG_UNSTABLE_DEQUEUE;
	}

	void
	dequeue_unstable_events(const bool dq) {
		if (dq)	flags |= FLAG_UNSTABLE_DEQUEUE;
		else	flags &= ~FLAG_UNSTABLE_DEQUEUE;
	}

#if PRSIM_MK_EXCL_BLOCKING_SET
	bool
	dequeue_excl_unstable_events(void) const {
		return flags & FLAG_EXCL_UNSTABLE_DEQUEUE;
	}

	void
	dequeue_excl_unstable_events(const bool dq) {
		if (dq)	flags |= FLAG_EXCL_UNSTABLE_DEQUEUE;
		else	flags &= ~FLAG_EXCL_UNSTABLE_DEQUEUE;
	}

#endif

	void
	step_over_vacuous_events(void) {
		flags &= ~FLAG_STOP_ON_VACUOUS;
	}

	void
	stop_on_vacuous_events(void) {
		flags |= FLAG_STOP_ON_VACUOUS;
	}

	bool
	stopping_on_vacuous_events(void) const {
		return flags & FLAG_STOP_ON_VACUOUS;
	}

	void
	append_mk_exclhi_ring(ring_set_type&);

	void
	append_mk_excllo_ring(ring_set_type&);

protected:
	excl_exception
	check_excl_rings(const node_index_type, const node_type&, 
		const value_enum prev, const value_enum next);

public:
	bool
	checking_excl(void) const { return flags & FLAG_CHECK_EXCL; }

	void
	check_excl(void) { flags |= FLAG_CHECK_EXCL; }

	void
	nocheck_excl(void) { flags &= ~FLAG_CHECK_EXCL; }

	void
	append_check_exclhi_ring(const ring_set_type&);

	void
	append_check_excllo_ring(const ring_set_type&);

	ostream&
	dump_mk_excl_ring(ostream&, const ring_set_type&, const bool) const;

	ostream&
	dump_mk_exclhi_rings(ostream&, const bool) const;

	ostream&
	dump_mk_excllo_rings(ostream&, const bool) const;

	ostream&
	dump_mk_excl_rings(ostream& o, const bool v) const {
		dump_mk_exclhi_rings(o, v);
		dump_mk_excllo_rings(o, v);
		return o;
	}

	ostream&
	dump_node_mk_excl_rings(ostream&, const node_index_type,
		const bool) const;

	ostream&
	dump_check_excl_ring(ostream&, const lock_index_list_type&,
		const bool) const;

	ostream&
	dump_check_exclhi_rings(ostream&, const bool) const;

	ostream&
	dump_check_excllo_rings(ostream&, const bool) const;

	ostream&
	dump_check_excl_rings(ostream& o, const bool v) const {
		dump_check_exclhi_rings(o, v);
		dump_check_excllo_rings(o, v);
		return o;
	}

	ostream&
	dump_node_check_excl_rings(ostream&, const node_index_type, 
		const bool) const;

private:

	void
	__collate_check_excl_reverse_map(const check_excl_ring_map_type&, 
		check_excl_array_type&) const;

	void
	__partial_collate_check_excl_reverse_map(
		const check_excl_ring_map_type&, 
		const lock_index_list_type&, 
		check_excl_reverse_map_type&) const;

private:
	event_index_type
	__copy_event(const event_type&);

	event_index_type
	__allocate_event(node_type&, const event_type& e);

	event_index_type
	__allocate_event(node_type&, const node_index_type n,
		cause_arg_type,	// this is the causing node/event
		const rule_index_type, 
#if EVENT_INCLUDE_RULE_POINTER
		const rule_type*,
#endif
		const value_enum,
#if PRSIM_WEAK_RULES
		const bool weak,
#endif
		const bool force = false
		);

	event_index_type
	__allocate_pending_interference_event(
		node_type&, const node_index_type n,
		cause_arg_type,	// this is the causing node/event
		const value_enum
#if PRSIM_WEAK_RULES
		, const bool weak
#endif
		);

	void
	__deallocate_pending_interference_event(const event_index_type);

	event_index_type
	__load_allocate_event(const event_type&);

	void
	__deallocate_event(node_type&, const event_index_type);

	class event_deallocator;

	void
	__deallocate_killed_event(const event_index_type);

	const event_type&
	get_event(const event_index_type) const;

	event_type&
	get_event(const event_index_type);

	void
	kill_event(const event_index_type, const node_index_type);

	node_index_type
	load_enqueue_event(const time_type, const event_index_type);

	void
	enqueue_event(const time_type, const event_index_type);

	void
	enqueue_new_event(const event_type&);

public:
	bool
	deschedule_event(const node_index_type);

	bool
	reschedule_event_now(const node_index_type);

	bool
	reschedule_event(const node_index_type, const time_type);

	bool
	reschedule_event_future(const node_index_type, const time_type);

	bool
	reschedule_event_relative(const node_index_type, const time_type);

private:
#if PRSIM_MK_EXCL_BLOCKING_SET
	void
	enforce_exclhi(cause_arg_type, const value_enum);

	void
	enforce_excllo(cause_arg_type, const value_enum);
#else
	void
	enqueue_exclhi(const time_type, const event_index_type);

	void
	enforce_exclhi(cause_arg_type);

	void
	flush_exclhi_queue(void);

	void
	enqueue_excllo(const time_type, const event_index_type);

	void
	enforce_excllo(cause_arg_type);

	void
	flush_excllo_queue(void);
#endif

#if PRSIM_SETUP_HOLD
public:
	// for TimingChecker
	void
	handle_timing_exception(const timing_exception&);

private:
#endif

#if PRSIM_MK_EXCL_BLOCKING_SET
	void
	flush_blocked_excl_nodes(const value_enum);
#endif

	break_type
	flush_updated_nodes(cause_arg_type
#if PRSIM_TRACK_LAST_EDGE_TIME && !PRSIM_TRACK_CAUSE_TIME
		, const time_type&
#endif
		);

	struct auto_flush_queues;

	event_placeholder_type
	dequeue_event(void);

	time_type
	get_delay_up(const event_type&) const;

	time_type
	get_delay_dn(const event_type&) const;

	pull_enum
	get_pull(const expr_index_type ei) const;	// define in .cc file

	struct eval_info;

	eval_info
	evaluate_kernel(const expr_index_type, pull_enum&, pull_enum&);

	evaluate_return_type
	evaluate(
		// const node_index_type,	// not needed
		const expr_index_type, 
		pull_enum prev, pull_enum next);

	break_type
	propagate_evaluation(cause_arg_type, const expr_index_type, 
		const pull_enum prev);

#if 0
	void
	kill_evaluation(const node_index_type, expr_index_type, 
		value_enum prev, value_enum next);
#endif

	void
	handle_keeper_checks(const node_index_type);

	void
	handle_invariants(const event_cause_type&);

	error_policy_enum
	__diagnose_invariant(ostream&, const process_index_type, 
		const rule_index_type, const pull_enum,
		const node_index_type, const value_enum) const;

	break_type
	__diagnose_violation(ostream&, const pull_enum next, 
		const event_index_type, event_type&, 
		const node_index_type ui, node_type& n, 
		cause_arg_type, const bool dir
#if PRSIM_WEAK_RULES
		, const bool w
#endif
		);

	break_type
	__report_instability(ostream&, const bool wk, const bool dir, 
#if PRSIM_MK_EXCL_BLOCKING_SET
		const bool excl,
#endif
		const event_type&) const;

	break_type
	__report_interference(ostream&, const bool wk, 
		const node_index_type, cause_arg_type) const;

	ostream&
	__report_cause(ostream&, cause_arg_type) const;

public:
// prsim trace functions
	bool
	is_tracing(void) const { return flags & FLAG_TRACE_ON; }

	void
	stop_trace(void) { flags &= ~FLAG_TRACE_ON; }

	never_ptr<trace_manager_type>
	get_trace_manager(void) const {
		return trace_manager;
	}

	never_ptr<trace_manager_type>
	get_trace_manager_if_tracing(void) const {
		return is_tracing() ? trace_manager
			: never_ptr<trace_manager_type>(NULL);
	}

	bool
	open_trace(const string&);

	void
	close_trace(void);

	trace_index_type
	get_trace_flush_interval(void) const {
		return trace_flush_interval;
	}

	void
	set_trace_flush_interval(const trace_index_type i) {
		INVARIANT(i);
		trace_flush_interval = i;
	}

// vcd trace functions
	bool
	is_tracing_vcd(void) const { return flags & FLAG_VCD_ON; }

	void
	stop_vcd(void) { flags &= ~FLAG_VCD_ON; }

	never_ptr<vcd_manager_type>
	get_vcd_manager(void) const {
		return vcd_manager;
	}

	never_ptr<vcd_manager_type>
	get_vcd_manager_if_tracing(void) const {
		return is_tracing_vcd() ? vcd_manager
			: never_ptr<vcd_manager_type>(NULL);
	}

	bool
	open_vcd(const string&);

	void
	close_vcd(void);

	double
	get_vcd_timescale(void) const {
		return vcd_timescale;
	}

	void
	set_vcd_timescale(const double& d) {
		vcd_timescale = d;
	}


	void
	check_expr(const expr_index_type) const;

	void
	check_event_queue(void) const;

	/// run-time check of invariants in Node/Expr structures.  
	void
	check_structure(void) const;

	/// optimizes structure
	void
	optimize_structure(void) const;

	// save

	// restore

	// TODO: dump structure and state information, human-readable

	ostream&
	dump_struct(ostream&) const;

	ostream&
	dump_state(ostream&) const;

	/// prints output in DOT form for visualization (options?)
	ostream&
	dump_struct_dot(ostream&) const;

	ostream&
	dump_event_force(ostream&, const event_index_type,
		const time_type, const bool) const;

	ostream&
	dump_event(ostream&, const event_index_type, const time_type) const;

	ostream&
	dump_event_queue(ostream&) const;

	ostream&
	dump_node_pending(ostream&, const node_index_type, const bool) const;

	ostream&
	dump_node_value(ostream&, const node_index_type) const;

	ostream&
	dump_rule(ostream&, const expr_index_type, const bool, 
		const bool) const;

	ostream&
	dump_node_fanout(ostream&, const node_index_type, 
		const bool r, const bool i, const bool v) const;

	ostream&
	dump_node_fanout_rules(ostream& o, const node_index_type ni, 
			const bool v) const {
		return dump_node_fanout(o, ni, true, false, v);
	}

	ostream&
	dump_node_fanin(ostream&, const node_index_type, const bool) const;

	void
	node_fanin(const node_index_type, vector<node_index_type>&) const;

	void
	node_fanout(const node_index_type, vector<node_index_type>&) const;

	void
	node_feedback(const node_index_type, vector<node_index_type>&) const;

	void
	node_feedthrough(const node_index_type, const node_index_type,
		vector<node_index_type>&) const;

	void
	node_feedback_feedthrough(const node_index_type, const node_index_type,
		vector<node_index_type>&) const;

	bool
	check_floating_node(const node_index_type, const node_index_type) const;

	ostream&
	dump_node_fanin_brief(ostream&, const node_index_type, const bool) const;

	ostream&
	dump_node_fanout_brief(ostream&, const node_index_type, const bool) const;

	ostream&
	dump_node_feedback(ostream&, const node_index_type, const bool) const;

	ostream&
	dump_rules(ostream&, const process_index_type, const bool) const;

	ostream&
	dump_all_rules(ostream&, const bool) const;

#if PRSIM_SETUP_HOLD
	ostream&
	dump_timing_constraints(ostream&, const process_index_type) const;
#endif

	ostream&
	dump_node_why_X(ostream&, const node_index_type, 
		const size_t, const bool) const;

	ostream&
	dump_node_why_not(ostream&, const node_index_type, 
		const size_t, const bool, const bool, const bool) const;

	/**
		\param F function or functor is a unary predicate, 
			whose argument type is const node_type&.
	 */
	template <class F>
	void
	find_nodes(vector<node_index_type>& ret, F f) const {
	const node_index_type ns = node_pool.size();
	node_index_type i = INVALID_NODE_INDEX +1;
	for ( ; i<ns; ++i) {
		const node_type& n(node_pool[i]);
		if (f(n)) {
			ret.push_back(i);
		}
	}
	}

#if 0
	void
	find_nodes(vector<node_index_type>&,
		bool (*)(const node_type&)) const;

	void
	find_nodes(vector<node_index_type>&,
		bool (node_type::*)(void) const) const;
#endif

	void
	find_nodes(vector<node_index_type>&,
		bool (this_type::*)(const node_index_type) const) const;

	void
	find_nodes(vector<node_index_type>&,
		bool (*)(const State&, const node_index_type)) const;

	template <class F>
	void
	filter_nodes(vector<index_type>& ret, F f) const;

	ostream&
	print_nodes(ostream&, const vector<node_index_type>&, const bool,
		const char*) const;

	void
	dangling_unknown_nodes(const bool, vector<node_index_type>&) const;

	ostream&
	dump_dangling_unknown_nodes(ostream&, const bool) const;

	void
	unused_nodes(vector<node_index_type>&) const;

	ostream&
	dump_unused_nodes(ostream&, const bool) const;

	void
	output_nodes(vector<node_index_type>&) const;

	ostream&
	dump_output_nodes(ostream&, const bool) const;

	void
	output_unknown_nodes(vector<node_index_type>&) const;

	ostream&
	dump_output_unknown_nodes(ostream&) const;

	void
	unknown_nodes_fanout(vector<node_index_type>&) const;

	ostream&
	dump_unknown_nodes_fanout(ostream&) const;

	void
	unknown_nodes_fanin_off(vector<node_index_type>&) const;

	ostream&
	dump_unknown_nodes_fanin_off(ostream&) const;

	ostream&
	dump_subexpr(ostream&, const expr_index_type, 
		const bool v, const uchar p, 
		const bool cp = false) const;

	ostream&
	dump_subexpr(ostream& o, const expr_index_type ei, 
		const bool v) const {
		// really don't care what kind of expr, is ignored
		return dump_subexpr(o, ei, v,
			expr_struct_type::EXPR_ROOT, true);
	}

#if !PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	bool
	dump_channel(ostream& o, const string& s) const {
		return _channel_manager.dump_channel(o, *this, s);
	}
#endif

	ostream&
	dump_channels(ostream& o) const {
		return _channel_manager.dump(o, *this);
	}

#if !PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	bool
	dump_channel_state(ostream& o, const string& s) const {
		return _channel_manager.dump_channel_state(o, *this, s);
	}
#endif

	bool
	node_is_driven_by_channel(const node_index_type) const;

	bool
	node_drives_any_channel(const node_index_type) const;

	bool
	node_is_driven(const node_index_type) const;

	bool
	node_is_not_driven(const node_index_type ni) const {
		return !node_is_driven(ni);
	}

	bool
	node_is_used(const node_index_type) const;

	bool
	node_is_not_used(const node_index_type ni) const {
		return !node_is_used(ni);
	}

	void
	record_exception(const exception_ptr_type&) const;

	error_policy_enum
	inspect_exceptions(void) const;

	ostream&
	dump_memory_usage(ostream&) const;

	void
	autosave(const bool, const string&);

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

	static
	ostream&
	dump_checkpoint(ostream&, istream&);

private:
	// not recursive, collect
	void
	__get_X_fanins(const expr_index_type, node_set_type&) const;

	// recursive
	void
	__expr_why_X(ostream&, const expr_index_type, 
		const size_t, const bool, 
		node_set_type&, node_set_type&) const;

	void
	__expr_why_not(ostream&, const expr_index_type, 
		const size_t, const bool, const bool, 
		node_set_type&, node_set_type&) const;

	void
	__root_expr_why_X(ostream&, const node_index_type, 
		const bool d, 
#if PRSIM_WEAK_RULES
		const bool w, 
#endif
		const size_t, const bool, 
		node_set_type&, node_set_type&) const;

	void
	__root_expr_why_not(ostream&, const node_index_type, 
		const bool d,
#if PRSIM_WEAK_RULES
		const bool w, 
#endif
		const size_t, const bool, const bool, 
		node_set_type&, node_set_type&) const;

	template <typename Iter>
	ostream&
	__print_nodes(ostream&, Iter, Iter, const bool, const char*) const;

	template <typename Iter>
	ostream&
	__print_nodes_infix(ostream&, Iter, Iter,
		const bool, const char*) const;

public:
	// so channel_manager has access (or pass callback?)
	ostream&
	__node_why_not(ostream&, const node_index_type, 
		const size_t, const bool, 
		const bool, const bool, node_set_type&, node_set_type&) const;

	ostream&
	__node_why_X(ostream&, const node_index_type, const size_t, const bool, 
		node_set_type&, node_set_type&) const;

private:
	void
	head_sentinel(void);

	static const string		magic_string;

};	// end class State

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_STATE_HH__

