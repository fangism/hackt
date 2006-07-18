/**
	\file "sim/prsim/State.h"
	The state of the prsim simulator.  
	$Id: State.h,v 1.10 2006/07/18 04:09:16 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_STATE_H__
#define	__HAC_SIM_PRSIM_STATE_H__

#include <iosfwd>
#include <map>
#include <set>
#include "util/STL/hash_map.h"
#include "sim/time.h"
#include "sim/prsim/Event.h"
#include "sim/prsim/Node.h"
#include "sim/prsim/Expr.h"
#include "sim/prsim/Rule.h"
#include "Object/lang/PRS_enum.h"	// for expression parenthesization
#include "util/string_fwd.h"
#include "util/list_vector.h"
#include "util/named_ifstream_manager.h"
#include "util/tokenize_fwd.h"

namespace HAC {
namespace entity {
	class module;
}

namespace SIM {
namespace PRSIM {
class ExprAlloc;
struct ExprAllocFlags;
using std::string;
using entity::module;
using util::list_vector;
using std::ostream;
using std::istream;
using util::ifstream_manager;
using util::string_list;
using HASH_MAP_NAMESPACE::hash_map;

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
class State {
	// too lazy to write public mutator methods for the moment.  
	friend class ExprAlloc;
public:
	// these typedefs will make it convenient to template this
	// class in the future...
	/// can switch between integer and real-valued time
	// typedef	discrete_time			time_type;
	typedef	real_time			time_type;
	typedef	delay_policy<time_type>		time_traits;
	typedef	NodeState			node_type;
	typedef	ExprState			expr_type;
	typedef	ExprGraphNode			graph_node_type;
	typedef	Event				event_type;
	typedef	EventPool			event_pool_type;
	typedef	EventPlaceholder<time_type>	event_placeholder_type;
	typedef	EventQueue<event_placeholder_type>	event_queue_type;
	typedef	vector<node_type>		node_pool_type;
	typedef	vector<expr_type>		expr_pool_type;
	typedef	RuleState<time_type>		rule_type;
	typedef	hash_map<expr_index_type, rule_type>	rule_map_type;

	typedef	std::map<node_index_type, watch_entry>	watch_list_type;
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
	struct excl_exception {
		/// true for exclhi, false for excllo
		bool				type;
		/// index of the mutex lock triggered
		lock_index_type			lock_id;
		/// node that failed to set lock
		node_index_type			node_id;

		excl_exception(const bool b, const lock_index_type li, 
			const node_index_type ni) : type(b), 
			lock_id(li), node_id(ni) { }
	};	// end struct excl_exception
#define	THROWS_EXCL_EXCEPTION	throw (excl_exception)
private:
	struct evaluate_return_type {
		node_index_type			node_index;
		expr_type*			root_ex;
		char				root_pull;

		evaluate_return_type() : node_index(INVALID_NODE_INDEX) { }

		evaluate_return_type(const node_index_type ni,
			expr_type* const e, const char p) :
			node_index(ni), root_ex(e), root_pull(p) { }
	};	// end struct evaluate_return_type
private:
	/**
		A fast, realloc-free vector-like structure
		to built-up expressions.  
		Will have log(N) time access due to internal tree structure.
	 */
	typedef	list_vector<expr_type>		temp_expr_pool_type;
	/**
		The structure for top-down expression topology.  
		Will have log(N) time access due to internal tree structure.
	 */
	typedef	list_vector<graph_node_type>	expr_graph_node_pool_type;

	enum {
		/// index of the first valid node
		FIRST_VALID_NODE = SIM::INVALID_NODE_INDEX +1,
		/// index of the first valid expr/expr_graph_node
		FIRST_VALID_EXPR = SIM::INVALID_EXPR_INDEX +1,
		/// index of the first valid event
		FIRST_VALID_EVENT = SIM::INVALID_EVENT_INDEX +1
	};
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
			If true, then no weak interference is reported.  
		 */
		FLAG_NO_WEAK_INTERFERENCE = 0x01,
		/**
			Whether or not the simulation was stopped
			by interrupt or event error/warning.  
			TODO: This could be redundant with the 
			interrupted flag. 
		 */
		FLAG_STOP_SIMULATION = 0x02,
		FLAG_ESTIMATE_ENERGY = 0x04,
		/**
			TODO: use different field to track timing mode.  
		 */
		FLAG_RANDOM_TIMING = 0x08,
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
#if PRSIM_ALLOW_UNSTABLE_DEQUEUE
		/**
			Allow unstable events to be dropped off queue
			instead of propagating unknowns.  
			Also dequeues weakly unstable events (involving X).  
		 */
		FLAG_UNSTABLE_DEQUEUE = 0x80,
#endif
		/// initial flags
		FLAGS_DEFAULT = FLAG_CHECK_EXCL,
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
	typedef	unsigned char			flags_type;

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
	static const char			pull_to_value[3][3];

public:
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
protected:
	typedef	vector<ring_set_type>
						mk_excl_ring_map_type;
	typedef	vector<event_placeholder_type>	mk_excl_queue_type;
	typedef	vector<event_index_type>	pending_queue_type;
	typedef	vector<event_queue_type::value_type>
						temp_queue_type;
	typedef	vector<expr_index_type>		expr_trace_type;
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
	/**
		Sparse map of nodes to their check-exclusive rings.  
	 */
	typedef	map<node_index_type, lock_index_list_type>
						check_excl_ring_map_type;
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
private:
//	count_ptr<const module>			mod;
	const module&				mod;
	node_pool_type				node_pool;
	expr_pool_type				expr_pool;
	expr_graph_node_pool_type		expr_graph_node_pool;
	event_pool_type				event_pool;
	event_queue_type			event_queue;
	// rule state and structural information (sparse map)
	rule_map_type				rule_map;
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
	// channels
	// mode of operation
	// operation flags
	flags_type				flags;
	/// timing mode
	char					timing_mode;
	// loadable random seed?
	/// set by the SIGINT signal handler
	/// (is this redundant with the STOP flag?)
	volatile bool				interrupted;
	// interpreter state
	ifstream_manager			ifstreams;
	/**
		For efficient tracing and lookup of root rule expressions.  
		Should not be maintained for state checkpointing.  
	 */
	expr_trace_type				__scratch_expr_trace;
public:
	/**
		Signal handler class that binds the State reference
		for the duration of the scope in which it is declared.  
	 */
	class signal_handler {
	private:
		static State*		_state;
		static void main(int);
	private:
		State*			_prev;
		void (*_main)(int);
	public:
		explicit
		signal_handler(State*);

		~signal_handler();
	} __ATTRIBUTE_UNUSED__ ;
public:
	explicit
	State(const module&, const ExprAllocFlags&);
private:
	// inaccessible undefined copy-constructor ... for now
	State(const State&);

public:
	~State();

	const module&
	get_module(void) const { return mod; }

	ifstream_manager&
	get_stream_manager(void) {
		return ifstreams;
	}

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
	check_node(const node_index_type) const;

	const node_type&
	get_node(const node_index_type) const;

	node_type&
	get_node(const node_index_type);

	string
	get_node_canonical_name(const node_index_type) const;

	rule_map_type&
	get_rule_map(void) { return rule_map; }

	const rule_map_type&
	get_rule_map(void) const { return rule_map; }

	bool
	is_rule_expr(const expr_index_type) const;

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

	void
	reset_tcounts(void);

	bool
	pending_events(void) const { return !event_queue.empty(); }

	static
	time_type
	random_delay(void);

	time_type
	next_event_time(void) const;

	int
	set_node_time(const node_index_type, const char val, 
		const time_type t, const bool f);

	int
	set_node_after(const node_index_type n, const char val, 
		const time_type t, const bool f) {
		return set_node_time(n, val, this->current_time +t, f);
	}

	int
	set_node(const node_index_type n, const char val,
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
	step(void) THROWS_EXCL_EXCEPTION;

	step_return_type
	cycle(void) THROWS_EXCL_EXCEPTION;

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

	/// for any user-defined structures from the .hac
	void
	watch_structure(void);

	/// for any user-defined structures from the .hac
	void
	unwatch_structure(void);

	ostream&
	dump_watched_nodes(ostream&) const;

	ostream&
	status_nodes(ostream&, const char) const;

	template <class L>
	void
	import_source_paths(const L& l) {
		typedef	typename L::const_iterator	const_iterator;
		const_iterator i(l.begin()), e(l.end());
		for ( ; i!=e; ++i) {
			ifstreams.add_path(*i);
		}
	}

	void
	add_source_path(const string& s) {
		ifstreams.add_path(s);
	}

	ostream&
	dump_source_paths(ostream&) const;

#if PRSIM_ALLOW_UNSTABLE_DEQUEUE
	bool
	dequeue_unstable_events(void) const {
		return flags & FLAG_UNSTABLE_DEQUEUE;
	}

	void
	dequeue_unstable_events(const bool dq) {
		if (dq)	flags |= FLAG_UNSTABLE_DEQUEUE;
		else	flags &= ~FLAG_UNSTABLE_DEQUEUE;
	}
#endif

	void
	append_mk_exclhi_ring(ring_set_type&);

	void
	append_mk_excllo_ring(ring_set_type&);

protected:
	excl_exception
	check_excl_rings(const node_index_type, const node_type&, 
		const char prev, const char next);

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

	void
	inspect_excl_exception(const excl_exception&, ostream&) const;

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
	__allocate_event(node_type&, const node_index_type n,
		const node_index_type c, // this is the causing node
		const rule_index_type, const char);

#if PRSIM_FIX_BOGUS_INTERFERENCE
	event_index_type
	__allocate_pending_interference_event(
		node_type&, const node_index_type n,
		const node_index_type c,	// this is the causing node
		const char);

	void
	__deallocate_pending_interference_event(const event_index_type);
#endif

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
	enqueue_event(const time_type, const event_index_type);

	void
	enqueue_exclhi(const time_type, const event_index_type);

	void
	enforce_exclhi(const node_index_type);

	void
	flush_exclhi_queue(void);

	void
	enqueue_excllo(const time_type, const event_index_type);

	void
	enforce_excllo(const node_index_type);

	void
	flush_excllo_queue(void);

	void
	enqueue_pending(const event_index_type);

	void
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

	event_placeholder_type
	dequeue_event(void);

	time_type
	get_delay_up(const event_type&) const;

	time_type
	get_delay_dn(const event_type&) const;

	evaluate_return_type
	evaluate(const node_index_type, expr_index_type, 
		char prev, char next);

	void
	propagate_evaluation(const node_index_type, expr_index_type, 
		char prev, char next);

#if PRSIM_FIX_BOGUS_INTERFERENCE
	void
	kill_evaluation(const node_index_type, expr_index_type, 
		char prev, char next);
#endif

	void
	__diagnose_violation(ostream&, const char next, 
		const event_index_type, event_type&, 
		const node_index_type ui, node_type& n, 
		const node_index_type ni, const bool dir);

public:
	void
	check_expr(const expr_index_type) const;

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
	dump_event_queue(ostream&) const;

	ostream&
	dump_node_value(ostream&, const node_index_type) const;

	ostream&
	dump_node_fanout(ostream&, const node_index_type) const;

	ostream&
	dump_node_fanin(ostream&, const node_index_type) const;

	ostream&
	dump_subexpr(ostream&, const expr_index_type, 
		const char p, const bool cp = false) const;

	ostream&
	dump_subexpr(ostream& o, const expr_index_type ei) const {
		// really don't care what kind of expr, is ignored
		return dump_subexpr(o, ei, expr_type::EXPR_ROOT, true);
	}

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

	static
	ostream&
	dump_checkpoint(ostream&, istream&);

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

