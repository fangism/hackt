/**
	\file "sim/prsim/State-prsim.h"
	The state of the prsim simulator.  
	$Id: State-prsim.h,v 1.22.2.2 2009/01/27 22:16:45 fang Exp $

	This file was renamed from:
	Id: State.h,v 1.17 2007/01/21 06:01:02 fang Exp
	to avoid object name collision in shared libraries.  
 */

#ifndef	__HAC_SIM_PRSIM_STATE_H__
#define	__HAC_SIM_PRSIM_STATE_H__

// enable additional sanity checking in "sim/event.h" for event queue
// can be disabled when not debugging
#define	CHECK_UNIQUE_EVENTS			0

// define to 1 to use a unique-set container for pending queue
// a wee bit slower, but saner
#define	UNIQUE_PENDING_QUEUE			1

#include <iosfwd>
#include <map>
#include <set>
#include "util/STL/hash_map.h"
#include "sim/time.h"
#include "sim/state_base.h"
#include "sim/signal_handler.h"
#include "sim/event.h"
#include "sim/prsim/Exception.h"
#include "sim/prsim/Event-prsim.h"
#include "sim/prsim/Node.h"
#include "sim/prsim/Expr.h"
#include "sim/prsim/Rule.h"
#include "sim/prsim/Channel-prsim.h"	// for channels support
#include "sim/command_error_codes.h"
#include "Object/lang/PRS_enum.h"	// for expression parenthesization
#include "util/string_fwd.h"
#include "util/named_ifstream_manager.h"
#include "util/tokenize_fwd.h"
#if PRSIM_INDIRECT_EXPRESSION_MAP
#include "Object/common/frame_map.h"
#include <valarray>
#else
#include "util/list_vector.h"
#endif
#if PRSIM_TRACE_GENERATION
#include "util/memory/excl_ptr.h"
#endif

namespace HAC {
#if PRSIM_INDIRECT_EXPRESSION_MAP
namespace entity {
	class footprint;
}
#endif
namespace SIM {
namespace PRSIM {
class ExprAlloc;
struct ExprAllocFlags;
#if PRSIM_TRACE_GENERATION
class TraceManager;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using SIM::INVALID_TRACE_INDEX;
#endif
using std::map;
using HASH_MAP_NAMESPACE::hash_map;
#if PRSIM_INDIRECT_EXPRESSION_MAP
using std::valarray;
using entity::footprint;
struct process_sim_state;
using entity::footprint_frame_map_type;
#else
using util::list_vector;
#endif


#if PRSIM_INDIRECT_EXPRESSION_MAP
/**
	Define to 1 to maintain a separate expr->process id map.
	TODO: benchmark the different between performance
	on large hierarchical designs.  
	Whe the map is combined search may need to be modified
	to use lower_bound rather than --upper_bound
 */
#define	PRSIM_SEPARATE_PROCESS_EXPR_MAP		1
#endif

/// can switch between integer and real-valued time
// typedef	discrete_time			rule_time_type;
typedef	real_time			rule_time_type;

//=============================================================================
enum {
	/// index of the first valid global node
	FIRST_VALID_GLOBAL_NODE = SIM::INVALID_NODE_INDEX +1,
	/// index of the first valid global expr/expr_graph_node
	FIRST_VALID_GLOBAL_EXPR = SIM::INVALID_EXPR_INDEX +1,
#if PRSIM_INDIRECT_EXPRESSION_MAP
	/// index of the first valid local node
	FIRST_VALID_LOCAL_NODE = 0, 
	/// index of the first valid local expr/expr_graph_node
	FIRST_VALID_LOCAL_EXPR = 0,
	/// index of first valid process, 0 is the top-level process
	FIRST_VALID_PROCESS = 0,
#endif
	/// index of the first valid event
	FIRST_VALID_EVENT = SIM::INVALID_EVENT_INDEX +1
};

//=============================================================================
/**
	Watch list entry.  
	Node index not included because it will be the first
	value of the mapped pair.  
 */
struct watch_entry {
	/// true if node is also a breakpoint
	char	breakpoint;
	// TODO: if is also a member of vector
	watch_entry() : breakpoint(0) { }

	void
	save_state(ostream&) const;

	void
	load_state(istream&);

	static
	ostream&
	dump_checkpoint_state(ostream&, istream&);

} __ATTRIBUTE_ALIGNED__ ;

//=============================================================================
#if PRSIM_INDIRECT_EXPRESSION_MAP
// structures to account for local fanin contribution
typedef	std::vector<expr_index_type>	fanin_array_type;
typedef	fanin_array_type		fanout_array_type;

/**
	This resembles Node(Struct), but only contains 
	fanin and fanout information.  
 */
struct faninout_struct_type {
#if PRSIM_WEAK_RULES
	fanin_array_type		pull_up[2];
	fanin_array_type		pull_dn[2];
#else
	fanin_array_type		pull_up;
	fanin_array_type		pull_dn;
#endif
	fanout_array_type		fanout;

	struct counter;			// counter
	// default ctor/dtor/copy
	fanin_array_type&
	get_pull_expr(const bool b
#if PRSIM_WEAK_RULES
		, const rule_strength w
#endif
		) {
		return b ? pull_up STR_INDEX(w)
			: pull_dn STR_INDEX(w);
	}

	const fanin_array_type&
	get_pull_expr(const bool b
#if PRSIM_WEAK_RULES
		, const rule_strength w
#endif
		) const {
		return b ? pull_up STR_INDEX(w)
			: pull_dn STR_INDEX(w);
	}

	bool
	has_fanin(void) const;

	bool
	contains_fanout(const expr_index_type) const;

	size_t
	fans(void) const {
		return fanout.size()
#if PRSIM_WEAK_RULES
			+pull_up[NORMAL_RULE].size()
			+pull_up[WEAK_RULE].size()
			+pull_dn[NORMAL_RULE].size()
			+pull_dn[WEAK_RULE].size();
#else
			+pull_up.size() +pull_dn.size();
#endif
	}

	static
	ostream&
	dump_faninout_list(ostream&, const fanin_array_type&);

	ostream&
	dump_struct(ostream&) const;

	static
	expr_index_type
	add_size(const expr_index_type N, const faninout_struct_type& f) {
		return N +f.fans();
	}

};	// end struct faninout_struct_type
#endif	// PRSIM_INDIRECT_EXPRESSION_MAP

//=============================================================================
/**
	shared structures used per process type
	This is where we save memory!
	This extends information that would normally go into
		a process's footprint. 
	TODO: eventually, PRSIM_UNIFY_GRAPH_STRUCTURES to avoid 
		unnecessary data replication, structure from create/unrolling.
	No stateful information should be kept here.  
	TODO: rings for mk_excl and check_excl!
 */
struct unique_process_subgraph {
#if PRSIM_INDIRECT_EXPRESSION_MAP
	typedef	Expr				expr_struct_type;
	typedef	Rule<rule_time_type>		rule_type;
#else
	typedef	ExprState			expr_struct_type;
	typedef	RuleState<rule_time_type>	rule_type;
#endif
	typedef	ExprGraphNode			graph_node_type;
	/**
		Collection of all subexpressions.  
		These expressions, unlike those in the footprint,
		point and propagate from leaf to root, bottom up.  
		Indices are *local* to process (type)!
	 */
	typedef	vector<expr_struct_type>	expr_pool_type;
	/**
		Top-down graph structure.  
		Indices are *local* to process (type)!
		Cannot use list_vector, which is not copy-constructible (yet).
	 */
#if PRSIM_INDIRECT_EXPRESSION_MAP
	typedef	vector<graph_node_type>		expr_graph_node_pool_type;
#else
	typedef	list_vector<graph_node_type>	expr_graph_node_pool_type;
#endif
	/**
		Collection of rule static information, attributes.  
		Indices are *local* to process (type).
	 */
	typedef	vector<rule_type>		rule_pool_type;
	/**
		Sparse map from top-level expressions to rules.
		Can probably use ordered map.  
		TODO: even better, since both keys increase monotonically, 
		just binary search through a vector of pairs, 
		which can support reverse-lookups.
		ALERT: the trick is to keep it sorted by both keys
		in the face of pool-compaction optimizations...
	 */
#if PRSIM_INDIRECT_EXPRESSION_MAP
	typedef	hash_map<expr_index_type, rule_index_type>
#else
	typedef	hash_map<expr_index_type, rule_type>
#endif
						rule_map_type;
#if PRSIM_HIERARCHICAL_RINGS
	/**
		Instead of using circular linked lists with pointers, 
		we use a map of (cyclic referenced) indices to represent
		the exclusive rings. 
		Requires half as much memory as equivalent ring of pointers.  
		Another possiblity: fold these next-indices into the 
			Node structure.  
		Alternative: use map for sparser exclusive rings.  
	 */
	typedef	std::set<node_index_type>	ring_set_type;
#endif
	/**
		Structure for passing around set of node indices.
	 */
	typedef	std::set<node_index_type>	node_set_type;

	expr_pool_type				expr_pool;
	expr_graph_node_pool_type		expr_graph_node_pool;
	rule_pool_type				rule_pool;
	rule_map_type				rule_map;

#if PRSIM_INDIRECT_EXPRESSION_MAP
	/**
		Member functions interpret this as a node for 
		structural purposes.  
	 */
	typedef	faninout_struct_type			node_type;
	/**
		indexed by local node index.
	 */
	typedef	std::vector<faninout_struct_type>	faninout_map_type;
	/**
		This array-size should match number of nodes in unique_process.
		This sort of functions as a local node_pool.
	 */
	faninout_map_type			local_faninout_map;

	struct memory_accumulator;
#else
	// don't bother with flattened global view
#endif	// PRSIM_INDIRECT_EXPRESSION_MAP

	unique_process_subgraph();
	~unique_process_subgraph();

	node_index_type
	local_root_expr(expr_index_type) const;

#if PRSIM_INDIRECT_EXPRESSION_MAP
	void
	void_expr(const expr_index_type);

	void
	check_expr(const expr_index_type) const;

	void
	check_node(const node_index_type) const;

	void
	check_structure(void) const;

	expr_index_type
	fan_count(void) const;

	const rule_type*
	lookup_rule(const expr_index_type) const;

	bool
	is_rule_expr(const expr_index_type) const;

	ostream&
	dump_struct(ostream&) const;

	ostream&
	dump_struct_dot(ostream&, const expr_index_type) const;
#endif
};	// end struct unique_process_subgraph

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_INDIRECT_EXPRESSION_MAP
/**
	This base structure contains simulation-invariant structural data only.
 */
class process_sim_state_base {
protected:
	/**
		While state is being allocated, the index field is valid, 
		after the array is finished, the ptr replaces the index.
	 */
	union {
		process_index_type		index;
		const unique_process_subgraph*	ptr;
	} type_ref;
	/**
		global offset of first expression belonging to this process
		must be non-zero.
		In the global array of processes, these offset values
		must increase monotonically (sorted).
	 */
	expr_index_type				global_expr_offset;

public:
	struct offset_comparator;

	/// only call this after ptr has been set by finish_process...
	const unique_process_subgraph&
	type(void) const {
		NEVER_NULL(type_ref.ptr);
		return *type_ref.ptr;
	}

	const process_index_type&
	get_index(void) const { return type_ref.index; }

	const expr_index_type&
	get_offset(void) const { return global_expr_offset; }

	void
	set_ptr(const unique_process_subgraph& g) {
		type_ref.ptr = &g;
	}

	/**
		\param gei is a global expression index.
		\return local expression index.
	 */
	expr_index_type
	local_expr_index(const expr_index_type gei) const {
		return gei -global_expr_offset;
	}

	/**
		\param lei is a local expression index.
		\return global expression index.
	 */
	expr_index_type
	global_expr_index(const expr_index_type lei) const {
		return lei +global_expr_offset;
	}

	const unique_process_subgraph::rule_type*
	lookup_rule(const expr_index_type gei) const {
		return type().lookup_rule(local_expr_index(gei));
	}

};	// end struct process_sim_state_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	state information per process instance.
	This is memory-intensive, and thus should be kept small.  
	Node state information is kept outside of these structures.  
 */
struct process_sim_state : public process_sim_state_base {
	typedef	ExprState			expr_state_type;
	typedef	RuleState<rule_time_type>	rule_state_type;
	typedef	unique_process_subgraph::rule_type
						rule_type;
	typedef	unique_process_subgraph::node_set_type
						node_set_type;
	typedef	unique_process_subgraph::expr_struct_type
						expr_struct_type;
	typedef	unique_process_subgraph::graph_node_type
						graph_node_type;

	struct memory_accumulator;
//	struct expr_offset_comparator;

	/// array of expression states
	valarray<expr_state_type>		expr_states;
	/// array of rule states
	valarray<rule_state_type>		rule_states;


	void
	allocate_from_type(const unique_process_subgraph&, 
		const process_index_type, const expr_index_type);

	void
	clear(void);

	void
	initialize(void);

	void
	__get_local_X_fanins(const expr_index_type, 
		const State&, node_set_type&) const;

	void
	__local_expr_why_not(ostream&, const expr_index_type, 
		const State&, const size_t, 
		const bool, const bool, 
		node_set_type&, node_set_type&) const;

	void
	__local_expr_why_X(ostream&, const expr_index_type, 
		const State&, const size_t, 
		const bool, node_set_type&, node_set_type&) const;

	void
	__recurse_expr_why_not(ostream&, const expr_index_type, 
		const pull_enum, 
		const State&, const size_t, 
		const bool, const bool, 
		node_set_type&, node_set_type&) const;

	void
	__recurse_expr_why_X(ostream&, const expr_index_type, 
		const State&, const size_t, 
		const bool, node_set_type&, node_set_type&) const;

#if PRSIM_INVARIANT_RULES
	struct invariant_checker;
	struct invariant_dumper;

	bool
	check_invariants(ostream&, const State&) const;

	ostream&
	dump_invariants(ostream&, const State&, const bool) const;
#endif

	ostream&
	dump_subexpr(ostream&, const expr_index_type, 
		const State&, 
		const bool v, const uchar p = expr_struct_type::EXPR_ROOT,
		const bool cp = false) const;

	struct dumper_base;
	struct rules_dumper;

	ostream&
	dump_rules(ostream&, const State&, const bool) const;

	ostream&
	dump_rule(ostream&, const rule_index_type, 
		const State&, const bool, const bool) const;

	ostream&
	dump_node_fanin(ostream&, const node_index_type, 
		const State&, const bool) const;

};	// end struct process_sim_state

#endif	// PRSIM_INDIRECT_EXPRESSION_MAP

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
class State : public state_base
#if !PRSIM_INDIRECT_EXPRESSION_MAP
	, protected unique_process_subgraph	// flattened view
#endif
	{
	// too lazy to write public mutator methods for the moment.  
	friend class ExprAlloc;
	typedef	State				this_type;
public:
	// these typedefs will make it convenient to template this
	// class in the future...
	typedef	rule_time_type			time_type;
	typedef	delay_policy<time_type>		time_traits;
	typedef	NodeState			node_type;
	typedef	Event				event_type;
	typedef	EventPool			event_pool_type;
#if PRSIM_TRACE_GENERATION
	typedef	event_type::cause_type		event_cause_type;
#endif
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
#if PRSIM_TRACE_GENERATION
	typedef	size_t				trace_index_type;
	typedef	TraceManager			trace_manager_type;
#endif
	typedef	EventPlaceholder<time_type>	event_placeholder_type;
	typedef	EventQueue<event_placeholder_type>	event_queue_type;
	typedef	vector<node_type>		node_pool_type;

	typedef	map<node_index_type, watch_entry>	watch_list_type;
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


#define	THROWS_STEP_EXCEPTION	throw (step_exception)
private:
	struct evaluate_return_type;

	/**
		Return codes for set_node_time.  
	 */
	enum {
		ENQUEUE_ACCEPT = 0,
		ENQUEUE_WARNING = 1,
		ENQUEUE_REJECT = 2,
		ENQUEUE_FATAL = 3
	};

	/**
		Simulation flags, bit fields, corresponding the
		the flags member.  
	 */
	enum {
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
#if PRSIM_TRACE_GENERATION
		/**
			Set to true when events are being traced.
		 */
		FLAG_TRACE_ON = 0x8000,
#endif
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
			FLAG_STOP_SIMULATION
	};
	/**
		As we add more flags this will have to expand...
	 */
	typedef	ushort				flags_type;

public:
	/**
		Default enumeration values.
	 */
	enum {
#if 0
		ERROR_IGNORE = 0,
		ERROR_NONE = ERROR_IGNORE,
		ERROR_WARN = 1,
		ERROR_NOTIFY = ERROR_WARN,
		ERROR_BREAK = 2,
		/// return control to user temporarily before resuming script
		ERROR_INTERACTIVE = 3,
		/// halt the simulation immediately
		ERROR_FATAL = 4,
		ERROR_INVALID,
#endif
#if PRSIM_INVARIANT_RULES
		ERROR_DEFAULT_INVARIANT_FAIL = ERROR_BREAK,
		ERROR_DEFAULT_INVARIANT_UNKNOWN = ERROR_WARN,
#endif
		ERROR_DEFAULT_UNSTABLE = ERROR_BREAK,
		ERROR_DEFAULT_WEAK_UNSTABLE = ERROR_WARN,
		ERROR_DEFAULT_INTERFERENCE = ERROR_BREAK,
		ERROR_DEFAULT_WEAK_INTERFERENCE = ERROR_WARN,
		ERROR_DEFAULT_ASSERT_FAIL = ERROR_FATAL,
		ERROR_DEFAULT_CHANNEL_EXPECT_FAIL = ERROR_FATAL,
		ERROR_DEFAULT_EXCL_CHECK_FAIL = ERROR_FATAL
	};

private:
	/**
		Return type to indicate whether or not to break.  
	 */
	typedef	error_policy_enum			break_type;

	enum {
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
	typedef	std::set<node_index_type>	ring_set_type;
#endif
	typedef	unique_process_subgraph::node_set_type
						node_set_type;
protected:
	typedef	vector<ring_set_type>
						mk_excl_ring_map_type;
	/**
		This needs to be a unique forward mapping (injection),
		i.e. no duplicate events.  
		The (key, value) pair is just a transposed event placeholder.
		TODO: pool-allocate?
	 */
	typedef	std::map<event_index_type, time_type>
						mk_excl_queue_type;
	/**
		invariant: no event should be in pending queue more than once.
	 */
#if UNIQUE_PENDING_QUEUE
	typedef	std::set<event_index_type>	pending_queue_type;
#else
	typedef	vector<event_index_type>	pending_queue_type;
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
	 */
	typedef	map<node_index_type, lock_index_list_type>
						check_excl_ring_map_type;
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
#if PRSIM_INDIRECT_EXPRESSION_MAP
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
	/**
		Translates a global expression ID to the process ID
		to which the expression belongs.  
		key is the *lowest* expression-index 'owned' by
		the value-indexed process.  
		Basically each process owns a contiguous 
		range of expr indices.  
	 */
	typedef	map<expr_index_type, process_index_type>
					global_expr_process_id_map_type;
#endif
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
	// TODO: per process instance attributes!
#endif	// PRSIM_INDIRECT_EXPRESSION_MAP
private:
	node_pool_type				node_pool;
#if PRSIM_INDIRECT_EXPRESSION_MAP
	/**
		Collection of unique process footprints.  
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
#endif
	event_pool_type				event_pool;
	event_queue_type			event_queue;
	/// coerce exclusive-hi ring
	mk_excl_ring_map_type			mk_exhi;
	/// coerce exclusive-low ring
	mk_excl_ring_map_type			mk_exlo;
	/// coerced exclusive-hi logic queue
	mk_excl_queue_type			exclhi_queue;
	/// coerced exclusive-low logic queue
	mk_excl_queue_type			excllo_queue;
	/// pending queue
	pending_queue_type			pending_queue;
	/// pool of exclusive-hi checking locks
	check_excl_lock_pool_type		check_exhi_ring_pool;
	/// pool of exclusive-low checking locks
	check_excl_lock_pool_type		check_exlo_ring_pool;
	/// sparse set of node-associated lock sets
	check_excl_ring_map_type		check_exhi;
	/// sparse set of node-associated lock sets
	check_excl_ring_map_type		check_exlo;
	// current time, etc...
	time_type				current_time;
	time_type				uniform_delay;
	// watched nodes
	watch_list_type				watch_list;
	// vectors
	/**
		Extension to manage channel environments and actions. 
	 */
	channel_manager				_channel_manager;
#if PRSIM_TRACE_GENERATION
	excl_ptr<trace_manager_type>		trace_manager;
	trace_index_type			trace_flush_interval;
#endif
	// mode of operation
	// operation flags
	flags_type				flags;
#if PRSIM_INVARIANT_RULES
	/// poicy when invariants are violated
	error_policy_enum			invariant_fail_policy;
	/// poicy when invariants are *possibly* violated, due to 'X' pull
	error_policy_enum			invariant_unknown_policy;
#endif
	/// controls the simulation behavior upon instability
	error_policy_enum			unstable_policy;
	/// controls the simulation behavior upon weak-instability
	error_policy_enum			weak_unstable_policy;
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
	/// name of automatically taken checkpoint
	string					autosave_name;
	/// timing mode
	uchar					timing_mode;
	// loadable random seed?
	/**
		set by the SIGINT signal handler
		(is this redundant with the STOP flag?)
	 */
	volatile bool				interrupted;
#if !PRSIM_INDIRECT_EXPRESSION_MAP
	/**
		For efficient tracing and lookup of root rule expressions.  
		Should not be maintained for state checkpointing.  
	 */
	expr_trace_type				__scratch_expr_trace;
#endif
	/**
		Auxiliary array for in-place random reordering
		of fanout indices for evaluation.  
		Never maintained for checkpointing.  
	 */
	fanout_array_type			__shuffle_indices;
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

private:
	void
	__initialize(void);

public:
#if !PRSIM_INDIRECT_EXPRESSION_MAP
	void
	check_node(const node_index_type) const;
#endif

	const node_type&
	get_node(const node_index_type) const;

	node_type&
	get_node(const node_index_type);

	ostream&
	dump_node_canonical_name(ostream&, const node_index_type) const;

	string
	get_node_canonical_name(const node_index_type) const;

	void
	backtrace_node(ostream&, const node_index_type) const;

#if PRSIM_INDIRECT_EXPRESSION_MAP
	process_index_type
	get_num_processes(void) const { return process_state_array.size(); }

	ostream&
	dump_process_canonical_name(ostream&, const process_sim_state&) const;

	ostream&
	dump_process_canonical_name(ostream&, const process_index_type) const;

	string
	get_process_canonical_name(const process_index_type) const;

	const footprint_frame_map_type&
	get_footprint_frame_map(const process_index_type pid) const;

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
#else
	/// only called by ExprAlloc
	void
	void_expr(const expr_index_type);

	rule_map_type&
	get_rule_map(void) { return rule_map; }

	const rule_map_type&
	get_rule_map(void) const { return rule_map; }
#endif

	bool
	is_rule_expr(const expr_index_type) const;

	const rule_type*
	lookup_rule(const expr_index_type) const;

	void
	update_time(const time_type t) {
		current_time = t;
	}

	void
	advance_time(const time_type t) {
		current_time += t;
	}

	const time_type&
	time(void) const { return current_time; }

	ostream&
	dump_timing(ostream&) const;

	bool
	set_timing(const string&, const string_list&);

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
	bool
	reset_channel(const string&);

	void
	reset_all_channels(void);

	bool
	resume_channel(const string&);

	void
	resume_all_channels(void);

	void
	reset_tcounts(void);

	ostream&
	dump_mode(ostream&) const;

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

#if PRSIM_INVARIANT_RULES
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
#endif

	DEFINE_POLICY_CONTROL_SET(unstable)
	DEFINE_POLICY_CONTROL_SET(weak_unstable)
	DEFINE_POLICY_CONTROL_SET(interference)
	DEFINE_POLICY_CONTROL_SET(weak_interference)
	DEFINE_POLICY_CONTROL_GET(unstable)
	DEFINE_POLICY_CONTROL_GET(weak_unstable)
	DEFINE_POLICY_CONTROL_GET(interference)
	DEFINE_POLICY_CONTROL_GET(weak_interference)

	DEFINE_POLICY_CONTROL_SET(assert_fail)
	DEFINE_POLICY_CONTROL_SET(channel_expect_fail)
	DEFINE_POLICY_CONTROL_SET(excl_check_fail)
	DEFINE_POLICY_CONTROL_GET(assert_fail)
	DEFINE_POLICY_CONTROL_GET(channel_expect_fail)
	DEFINE_POLICY_CONTROL_GET(excl_check_fail)

#undef	DEFINE_POLICY_CONTROL_SET
#undef	DEFINE_POLICY_CONTROL_GET

	bool
	pending_events(void) const { return !event_queue.empty(); }

	static
	time_type
	uniform_random_delay(void);

	static
	time_type
	exponential_random_delay(void);

	time_type
	next_event_time(void) const;

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

	void
	unset_node(const node_index_type);

	void
	unset_all_nodes(void);

	void
	set_node_breakpoint(const node_index_type);

	void
	clear_node_breakpoint(const node_index_type);

	void
	clear_all_breakpoints(void);

	ostream&
	dump_breakpoints(ostream&) const;

	step_return_type
	step(void) THROWS_STEP_EXCEPTION;

	step_return_type
	cycle(void) THROWS_STEP_EXCEPTION;

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

	ostream&
	status_nodes(ostream&, const value_enum, const bool) const;

	bool
	dequeue_unstable_events(void) const {
		return flags & FLAG_UNSTABLE_DEQUEUE;
	}

	void
	dequeue_unstable_events(const bool dq) {
		if (dq)	flags |= FLAG_UNSTABLE_DEQUEUE;
		else	flags &= ~FLAG_UNSTABLE_DEQUEUE;
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
	dump_mk_excl_ring(ostream&, const ring_set_type&) const;

	ostream&
	dump_mk_exclhi_rings(ostream&) const;

	ostream&
	dump_mk_excllo_rings(ostream&) const;

	ostream&
	dump_node_mk_excl_rings(ostream&, const node_index_type) const;

	ostream&
	dump_check_excl_ring(ostream&, const lock_index_list_type&) const;

	ostream&
	dump_check_exclhi_rings(ostream&) const;

	ostream&
	dump_check_excllo_rings(ostream&) const;

	ostream&
	dump_node_check_excl_rings(ostream&, const node_index_type) const;

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
	__allocate_event(node_type&, const node_index_type n,
		cause_arg_type,	// this is the causing node/event
		const rule_index_type, const value_enum
#if PRSIM_WEAK_RULES
		, const bool weak
#endif
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

public:
	bool
	reschedule_event_now(const node_index_type);

	bool
	reschedule_event(const node_index_type, const time_type);

	bool
	reschedule_event_future(const node_index_type, const time_type);

	bool
	reschedule_event_relative(const node_index_type, const time_type);

private:
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

	void
	enqueue_pending(const event_index_type);

	break_type
	flush_pending_queue(void);

	void
	__flush_pending_event_with_interference(
		node_type&, const event_index_type, event_type&);

	void
	__flush_pending_event_no_interference(
		node_type&, const event_index_type, event_type&);

	void
	__flush_pending_event_replacement(
		node_type&, const event_index_type, event_type&);

	struct auto_flush_queues;

	event_placeholder_type
	dequeue_event(void);

	time_type
	get_delay_up(const event_type&) const;

	time_type
	get_delay_dn(const event_type&) const;

	pull_enum
	get_pull(const expr_index_type ei) const
#if PRSIM_INDIRECT_EXPRESSION_MAP
	;	// define in .cc file
#else
	{
		return ei ? expr_pool[ei].pull_state() : PULL_OFF;
	}
#endif

	evaluate_return_type
	evaluate(const node_index_type, expr_index_type, 
		pull_enum prev, pull_enum next);

	break_type
	propagate_evaluation(cause_arg_type, const expr_index_type, 
		pull_enum prev);

#if 0
	void
	kill_evaluation(const node_index_type, expr_index_type, 
		value_enum prev, value_enum next);
#endif

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
		const node_index_type, const event_type&) const;

	break_type
	__report_interference(ostream&, const bool wk, 
		const node_index_type, const event_type&) const;

	ostream&
	__report_cause(ostream&, const event_type&) const;

public:
#if PRSIM_TRACE_GENERATION
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
#endif

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
	dump_node_fanin(ostream&, const node_index_type, const bool) const;

#if PRSIM_INDIRECT_EXPRESSION_MAP
	ostream&
	dump_rules(ostream&, const process_index_type, const bool) const;

	ostream&
	dump_all_rules(ostream&, const bool) const;
#endif

	ostream&
	dump_node_why_X(ostream&, const node_index_type, 
		const size_t, const bool) const;

	ostream&
	dump_node_why_not(ostream&, const node_index_type, 
		const size_t, const bool, const bool, const bool) const;

	void
	find_nodes(vector<node_index_type>&,
		bool (*)(const node_type&)) const;

	void
	find_nodes(vector<node_index_type>&,
		bool (node_type::*)(void) const) const;

	void
	find_nodes(vector<node_index_type>&,
		bool (this_type::*)(const node_index_type) const) const;

	void
	find_nodes(vector<node_index_type>&,
		bool (*)(const State&, const node_index_type)) const;

	ostream&
	print_nodes(ostream&, const vector<node_index_type>&,
		const char*) const;

	ostream&
	dump_dangling_unknown_nodes(ostream&, const bool) const;

	ostream&
	dump_output_nodes(ostream&) const;

	ostream&
	dump_output_unknown_nodes(ostream&) const;

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

	bool
	dump_channel(ostream& o, const string& s) const {
		return _channel_manager.dump_channel(o, *this, s);
	}

	ostream&
	dump_channels(ostream& o) const {
		return _channel_manager.dump(o, *this);
	}

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

#if PRSIM_INDIRECT_EXPRESSION_MAP
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
#endif

	template <typename Iter>
	ostream&
	__print_nodes(ostream&, Iter, Iter, const char*) const;

	template <typename Iter>
	ostream&
	__print_nodes_infix(ostream&, Iter, Iter, const char*) const;

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

#endif	// __HAC_SIM_PRSIM_STATE_H__

