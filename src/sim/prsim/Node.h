/**
	\file "sim/prsim/Node.h"
	Structure of basic PRS node.  
	$Id: Node.h,v 1.28 2011/05/07 21:34:26 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_NODE_H__
#define	__HAC_SIM_PRSIM_NODE_H__

#include <iosfwd>
#include <vector>
#include "util/string_fwd.h"
#include "util/macros.h"
#include "util/attributes.h"
#include "util/utypes.h"
#include "sim/common.h"
#include "sim/prsim/enums.h"
#include "sim/prsim/Cause.h"
#include <valarray>

/**
	Define to 1 to enable implementation of node event upsets, 
	for force/freeze nodes to certain values.  
	Goal: 1
	Rationale: simulate single event upsets.
	Status: begun
 */
#define	PRSIM_UPSET_NODES		1
/**
	Define to 1 to give NodeState its own dedicated watchpoint
	flag, instead of overloading it with the breakpoint flag.
	Goal: 1
	Rationale: fixes bug where breakpoints are masked by watchpoints.
	Status: done, tested, should be permanent.
 */
#define	USE_WATCHPOINT_FLAG		1

namespace HAC {
namespace entity {
struct bool_connect_policy;	// from "Object/inst/connection_policy.h"
}
namespace SIM {
namespace PRSIM {
using entity::bool_connect_policy;
using std::ostream;
using std::istream;
using std::vector;

struct faninout_struct_type;		// from "process_graph.h"

/**
	Define to 1 to use vector for node fanin, else valarray.
	Valarray's are more compact, but less convenient for resizing.
 */
#define	VECTOR_NODE_FANIN		1

/**
	List or array of fanin expressions.  
	OR-combination expression.  
	Valarray is lighter, but lacks container/iterator interface :(
 */
#if VECTOR_NODE_FANIN
typedef	std::vector<process_index_type>	process_fanin_type;
#else
typedef	std::valarray<process_index_type>	process_fanin_type;
#endif

/**
	Is like ExprState, but uses size_t instead of count_type.  
	NodeState needs one of these per direction, per strength.  
	Should we impose a limit on the number of rules? uint16, uint32?
 */
struct fanin_state_type {
#if 0
	// smaller?, should check for limit
	typedef	expr_count_type		count_type;
#else
	typedef	expr_index_type		count_type;
#endif
	/**
		This is the total number of fanin rules that are
			OR-combined to pull this node.
		Value is computed by taking sum over process fanins.  
		This isn't really state information, it's invariant, 
			but convenient to add here.  
	 */
	count_type			size;
	/**
		For OR-expressions, this represents the number of 1's, 
		i.e. number of rules that are currently on.
	 */
	count_type			countdown;
	/**
		The number of fanin rules that are X.  
	 */
	count_type			unknowns;

	fanin_state_type() : size(0), countdown(0), unknowns(0) { }

	/**
		Same as ExprState::initialize()
	 */
	void
	initialize(void) {
		unknowns = size;
		countdown = 0;
	}

	bool
	any(void) const { return size; }

	/**
		Simplified from ExprState's or_pull_state.
	 */
	pull_enum
	pull(void) const {
		return countdown ? PULL_ON :
			(unknowns ? PULL_WEAK : PULL_OFF);
	}

	ostream&
	dump_state(ostream&) const;
};	// end struct fanin_state_type

//=============================================================================
/**
	Node state information structure.  
	Structure remains invariant throughout the execution of most tools.  
	Sort of imported definition from prsim's struct prs_node (PrsNode)
	Note: no alias information here, that comes from the module's
		object hierarchy and unique allocation.  
	Tools that only perform static analysis on netlists can use
		just this structure.  
	TODO: attribute packed for density, or align for speed?
 */
struct Node {
	// compact: reference process instance indices with fanout
	typedef	std::vector<process_index_type>	fanout_array_type;

	/**
		Bit fields for node structure flags.  
	 */
	typedef	enum {
		NODE_DEFAULT_STRUCT_FLAGS = 0x0000,
		// NODE_UNSTAB = 0x0001,	// removed
		/**
			Whether or not this node belongs to at least one
			forced exclusive high ring.  
		 */
		NODE_MK_EXCLHI = 0x0002,
		/**
			Whether or not this node belongs to at least one
			forced exclusive low ring.  
		 */
		NODE_MK_EXCLLO = 0x0004,
		/**
			Whether or not this node belongs to at least one
			checked exclusive high ring.  
		 */
		NODE_CHECK_EXCLHI = 0x0008,
		/**
			Whether or not this node belongs to at least one
			checked exclusive high ring.  
		 */
		NODE_CHECK_EXCLLO = 0x0010
		/**
			Whether or not to report interference on this node.
		 */
		,
		NODE_MAY_INTERFERE = 0x0020,
		NODE_MAY_WEAK_INTERFERE = 0x0040
	} struct_flags_enum;

	/**
		This refers to a list of global process instances (indices)
		that can drive this node.  
		TODO: replace this with a purely hierarchical lookup
		when MEMORY_MAPPED_GLOBAL_ALLOCATION is complete.
		This would be relocated to unique_process_subgraph
		for the respective owners of the nodes.
	 */
	process_fanin_type		fanin;

	/**
		List of expression indices to which this node fans out.  
		Size of vector: 3 x sizeof(pointer), 12B on 32b arch.
		TODO: far future, only fanout to *processes* and 
			do additional rule lookups from there.
			Performance-memory tradeoff.
		TODO: replace this with a purely hierarchical lookup
		when MEMORY_MAPPED_GLOBAL_ALLOCATION is complete.
		This would be relocated to unique_process_subgraph
		for the respective owners of the nodes.
	 */
	fanout_array_type		fanout;

protected:
	/**
		Bit-field collection of flags.  
		Only 3 of 16 available flag slots used.  
		The packed version is aligned to 2B (short, word) boundaries.
		Extending classes can use these!
	 */
	ushort				struct_flags;

public:
	typedef	fanout_array_type::const_iterator
					const_fanout_iterator;

public:
	/// these aren't created frequently, inline doesn't matter
	Node();

	/// these aren't destroyed frequently, inline doesn't matter
	~Node();


	bool
	has_fanin(void) const {
		return fanin.size();
	}

	bool
	has_fanout(void) const {
		return fanout.size();
	}

private:
	// only to be called by attribute assignment of allocation phase
	void
	allow_interference(void) { struct_flags |= NODE_MAY_INTERFERE; }

	void
	allow_weak_interference(void) {
		struct_flags |= NODE_MAY_WEAK_INTERFERE;
	}

public:
	bool
	may_interfere(void) const { return struct_flags & NODE_MAY_INTERFERE; }

	bool
	may_weak_interfere(void) const {
		return struct_flags & NODE_MAY_WEAK_INTERFERE;
	}

	void
	import_attributes(const bool_connect_policy&);

	bool
	has_mk_exclhi(void) const { return struct_flags & NODE_MK_EXCLHI; }

	bool
	has_mk_excllo(void) const { return struct_flags & NODE_MK_EXCLLO; }

	void
	make_exclhi(void) { struct_flags |= NODE_MK_EXCLHI; }

	void
	make_excllo(void) { struct_flags |= NODE_MK_EXCLLO; }

	bool
	has_check_exclhi(void) const {
		return struct_flags & NODE_CHECK_EXCLHI;
	}

	bool
	has_check_excllo(void) const {
		return struct_flags & NODE_CHECK_EXCLLO;
	}

	void
	check_exclhi(void) { struct_flags |= NODE_CHECK_EXCLHI; }

	void
	check_excllo(void) { struct_flags |= NODE_CHECK_EXCLLO; }

	static
	size_t
	add_fanout_size(size_t sum, const Node& n) {
		return sum + n.fanout.size();
	}

	ostream&
	dump_fanout_dot(ostream&, const std::string&) const;

	ostream&
	dump_attributes(ostream&) const;

	ostream&
	dump_struct(ostream&) const;

};	// end struct Node

//=============================================================================
/**
	Structural information extended with stateful information.  
	Size of this should be a total of 8 double-words, or 32 B.  
	Nice and quad-word aligned.  
 */
struct NodeState : public Node {
private:
	typedef	NodeState			this_type;
public:
	typedef	Node				parent_type;
	typedef	parent_type::fanout_array_type	fanout_array_type;
	typedef	LastCause::event_cause_type	event_cause_type;


	/**
		Enumerations for stateful information flags for a node.  
	 */
	enum {
		NODE_INITIAL_STATE_FLAGS = 0x00,
		/**
			Whether or not a breakpoint is enabled on this node.
			This includes watching state.  
		 */
		NODE_BREAKPOINT = 0x01,
#if USE_WATCHPOINT_FLAG
		/**
			Flag is set true if this node is being watched.
		 */
		NODE_WATCHPOINT = 0x02,
#endif
#if !PRSIM_MK_EXCL_BLOCKING_SET
		/**
			Whether or not this node is currently in
			one of the exclusive ring queues.  
		 */
		NODE_EX_QUEUE = 0x04,
#endif

		/// true if this node participates in any registered channel
		NODE_IN_CHANNEL = 0x08,
#if PRSIM_UPSET_NODES
		/**
			Activity on this node ceases.
			Used to simulate event upsets.
		 */
		NODE_FROZEN = 0x10,
#endif
		/**
			Auxiliary flag for general purpose visit tracking.
			Is this used anywhere?
		 */
		NODE_FLAG = 0x80,
#if 0
		// THESE OVERFLOW uchar!!!
		NODE_CHANNEL_VALID = 0x10,
		NODE_CHANNEL_DATA = 0x20,
#endif

		/// OR-mask for initialization
		NODE_INITIALIZE_SET_MASK = 0x00,
		/// AND-mask (negated) for initialization
		NODE_INITIALIZE_CLEAR_MASK =
			NODE_FLAG
#if !PRSIM_MK_EXCL_BLOCKING_SET
			| NODE_EX_QUEUE
#endif
#if PRSIM_UPSET_NODES
			| NODE_FROZEN,
#endif
		NODE_RESET_SET_MASK = 0x00,
		// as if simulator just started up fresh
		NODE_RESET_CLEAR_MASK =
			NODE_INITIALIZE_CLEAR_MASK |
			NODE_BREAKPOINT |
			NODE_WATCHPOINT |
			NODE_IN_CHANNEL
	} state_flags_enum;

public:
	// also use this as pull_to_char
	static const uchar		value_to_char[3];
	static const value_enum		invert_value[3];
protected:
	/**
		Uses enum value_enum:
		0 = 0, 1 = 1, 2 = X, 3 = X
	 */
	value_enum				value;

	/**
		8-bit field for flagging stateful information.  
	 */
	uchar					state_flags;
	/**
		Current enqueued event index associated with this node.
		INVALID_EVENT_INDEX (0) means no pending event.  
		If we ever need to squeeze for memory, this field 
		could conceivably be eliminated because the information
		should be redundant with events in the event queue, 
		but require a slower search operation.
	 */
	event_index_type			event_index;
	/**
		Structure for tracking last cause, by node and value.  
		This substructure is a huge memory hog, and could
		be eliminated in favor of saving memory, 
		at a loss of some cause-tracking capabilities.  
	 */
	LastCause				causes;

public:
	/**
		Transition counts.  
		Not critical to simulation, unless we want statistics.  
		This could also be elimiated in a memory-lite version.
	 */
	size_t					tcount;
	/**
		The current state of each pull.  
	 */
#if PRSIM_WEAK_RULES
	fanin_state_type			pull_up_state[2];
	fanin_state_type			pull_dn_state[2];
#else
	fanin_state_type			pull_up_state;
	fanin_state_type			pull_dn_state;
#endif
public:
	NodeState() : parent_type(), value(LOGIC_OTHER), 
		state_flags(NODE_INITIAL_STATE_FLAGS),
		event_index(INVALID_EVENT_INDEX), 
		causes(), 
		tcount(0) { }

	/// count on compiler to optimize zero comparison
	bool
	pending_event(void) const {
		return event_index != INVALID_EVENT_INDEX;
	}

	/**
		By default, uses the current value.  
	 */
	node_index_type
	get_cause_node(void) const {
		return causes.get_cause_node(value);
	}

	event_cause_type
	get_cause(void) const {
		return causes.get_cause(value);
	}

	event_cause_type
	get_cause(const value_enum v) const {
		return causes.get_cause(v);
	}

	event_index_type
	get_event(void) const { return event_index; }

	void
	set_event(const event_index_type i) {
		INVARIANT(event_index == INVALID_EVENT_INDEX);
		INVARIANT(i != INVALID_EVENT_INDEX);
		event_index = i;
	}

	/**
		Use this version to set-if-unset, else assert
		that is already set consistently.
	 */
	void
	set_event_consistent(const event_index_type i) {
		if (!pending_event()) {
			set_event(i);
		} else {
			INVARIANT(event_index == i);
		}
	}

	/**
		This variation should only be used during checkpoint
		loading.  
	 */
	void
	load_event(const event_index_type i) {
		INVARIANT(i != INVALID_EVENT_INDEX);
		event_index = i;
	}

	void
	clear_event(void) { event_index = INVALID_EVENT_INDEX; }

#if USE_WATCHPOINT_FLAG
	bool
	is_watchpoint(void) const { return state_flags & NODE_WATCHPOINT; }

	void
	set_watchpoint(void) { state_flags |= NODE_WATCHPOINT; }

	void
	clear_watchpoint(void) { state_flags &= ~NODE_WATCHPOINT; }
#endif

	bool
	is_breakpoint(void) const { return state_flags & NODE_BREAKPOINT; }

	void
	set_breakpoint(void) { state_flags |= NODE_BREAKPOINT; }

	void
	clear_breakpoint(void) { state_flags &= ~NODE_BREAKPOINT; }

	bool
	is_flagged(void) const { return state_flags & NODE_FLAG; }

#if !PRSIM_MK_EXCL_BLOCKING_SET
	bool
	in_excl_queue(void) const { return state_flags & NODE_EX_QUEUE; }

	void
	set_excl_queue(void) { state_flags |= NODE_EX_QUEUE; }

	void
	clear_excl_queue(void) { state_flags &= ~NODE_EX_QUEUE; }
#endif

	void
	set_in_channel(void) { state_flags |= NODE_IN_CHANNEL; }

	bool
	in_channel(void) const { return state_flags & NODE_IN_CHANNEL; }

#if PRSIM_UPSET_NODES
	// forces a node to stick to a value
	void
	freeze(void) { state_flags |= NODE_FROZEN; }

	// releases a node from stuck state
	void
	restore(void) { state_flags &= ~NODE_FROZEN; }
#endif

	bool
	is_frozen(void) const {
#if PRSIM_UPSET_NODES
		return state_flags & NODE_FROZEN;
#else
		return false;
#endif
	}

	value_enum
	current_value(void) const { return value; }

	bool
	match_value(const value_enum v) const {
		return current_value() == v;
	}

	bool
	is_0(void) const {
		return current_value() == LOGIC_LOW;
	}

	bool
	is_1(void) const {
		return current_value() == LOGIC_HIGH;
	}

	bool
	is_X(void) const {
		return current_value() == LOGIC_OTHER;
	}

	void
	set_value_and_cause(const value_enum c, const event_cause_type& e) {
		value = c;
		causes.set_cause(c, e);
	}

	void
	x_value_and_cause(void);

	ostream&
	dump_value(ostream&) const;

	ostream&
	dump_state(ostream&) const;

	ostream&
	dump_debug(ostream&, const bool h) const;

	/**
		Predicated functor helper for printing nodes with
		a given status.  
	 */
	struct status_dumper {
		ostream&		os;
		const value_enum		match_val;

		status_dumper(ostream& o, const value_enum v) :
			os(o), match_val(v) { }

		// no copy-ctor
		// default destructor

		void
		operator () (const NodeState&);

	};	// end struct status_dumper

	static
	bool
	is_valid_value(const value_enum c) {
		// return c >= LOGIC_LOW && c <= LOGIC_OTHER;
		return c <= LOGIC_OTHER;
	}

	static
	bool
	is_valid_pull(const pull_enum c) {
		// return c >= LOGIC_LOW && c <= LOGIC_OTHER;
		return c <= PULL_WEAK;
	}

	static
	value_enum
	char_to_value(const char);

	static
	pull_enum
	char_to_pull(const char);

	static
	char
	translate_value_to_char(const char e) {
		return value_to_char[size_t(e)];
	}

	/// \return < 0 on error, else returns 0, 1, 2
	static
	value_enum
	string_to_value(const std::string&);

	static
	pull_enum
	string_to_pull(const std::string&);

	void
	initialize(void);

	void
	reset(void);

	void
	reset_tcount(void) { tcount = 0; }

	void
	count_fanins(const faninout_struct_type&);

	fanin_state_type&
	get_pull_struct(const bool dir
#if PRSIM_WEAK_RULES
		, const bool w
#endif
		) {
		return dir ? pull_up_state STR_INDEX(w) :
			pull_dn_state STR_INDEX(w);
	}

	const fanin_state_type&
	get_pull_struct(const bool dir
#if PRSIM_WEAK_RULES
		, const bool w
#endif
		) const {
		return dir ? pull_up_state STR_INDEX(w) :
			pull_dn_state STR_INDEX(w);
	}

	bool
	interfering(void) const;

	bool
	weak_interfering(void) const;

	pull_enum
	drive_state(void) const;

	bool
	driven(void) const {
		return drive_state() == PULL_ON;
	}

	bool
	x_driven(void) const {
		return drive_state() == PULL_WEAK;
	}

	bool
	undriven(void) const {
		return drive_state() == PULL_OFF;
	}

	void
	save_state(ostream&) const;

	void
	load_state(istream&);

	static
	ostream&
	dump_checkpoint_state_header(ostream&);

	static
	ostream&
	dump_checkpoint_state(ostream&, istream&);

} __ATTRIBUTE_ALIGNED__ ;	// end struct NodeState

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Set of pull states, in all directions.  
	Use member functions of this class for readability.
 */
struct pull_set {
	pull_enum			up;
	pull_enum			dn;
#if PRSIM_WEAK_RULES
	pull_enum			wup;
	pull_enum			wdn;
#endif

	pull_set() : up(PULL_OFF), dn(PULL_OFF)
#if PRSIM_WEAK_RULES
		, wup(PULL_OFF), wdn(PULL_OFF)
#endif
		{ }

	/**
		\param w is true if weak rules are enabled.
		When weak rules are disabled, weak pulls are always OFF.
	 */
	explicit
	pull_set(const NodeState& n, const bool w) :
		up(n.pull_up_state STR_INDEX(NORMAL_RULE).pull()),
		dn(n.pull_dn_state STR_INDEX(NORMAL_RULE).pull())
#if PRSIM_WEAK_RULES
		, wup(w ? n.pull_up_state STR_INDEX(WEAK_RULE).pull()
			: PULL_OFF)
		, wdn(w ? n.pull_dn_state STR_INDEX(WEAK_RULE).pull()
			: PULL_OFF)
#endif
		{
	}

	bool
	normal_rules_off(void) const {
		return (up == PULL_OFF) && (dn == PULL_OFF);
	}

	bool
	weak_rules_off(void) const {
#if PRSIM_WEAK_RULES
		return (wup == PULL_OFF) && (wdn == PULL_OFF);
#else
		return true;
#endif
	}

	bool
	state_holding(void) const {
		return normal_rules_off() && weak_rules_off();
	}

	bool
	pulling_up(void) const {
		return (up == PULL_ON)
#if PRSIM_WEAK_RULES
			|| (wup == PULL_ON)
#endif
			;
	}

	bool
	pulling_dn(void) const {
		return (dn == PULL_ON)
#if PRSIM_WEAK_RULES
			|| (wdn == PULL_ON)
#endif
			;
	}

	// disregards stronger pull-on
	bool
	__pulling_up_x(void) const {
		return (up == PULL_WEAK)
#if PRSIM_WEAK_RULES
			|| (wup == PULL_WEAK)
#endif
			;
	}

	// disregards stronger pull-on
	bool
	__pulling_dn_x(void) const {
		return (dn == PULL_WEAK)
#if PRSIM_WEAK_RULES
			|| (wdn == PULL_WEAK)
#endif
			;
	}

	bool
	cutoff_up(void) const {
		return up == PULL_OFF && wup == PULL_OFF;
	}

	bool
	cutoff_dn(void) const {
		return dn == PULL_OFF && wdn == PULL_OFF;
	}

	// forced overrides
	void
	cut_off_pull_up(void) {
		up = PULL_OFF;
		wup = PULL_OFF;
	}

	// forced overrides
	void
	cut_off_pull_dn(void) {
		dn = PULL_OFF;
		wdn = PULL_OFF;
	}

	// \return true if one of the strong pulls is X
	bool
	normal_pulling_x(void) const {
		return dn == PULL_WEAK || up == PULL_WEAK;
	}

	bool
	weak_pulling_x(void) const {
		return wdn == PULL_WEAK || wup == PULL_WEAK;
	}

	// this is not true if weak-pull-1 overtakes strong-pull-X!
	bool
	normal_pulling_up_x(void) const {
		return (up == PULL_WEAK
#if PRSIM_WEAK_RULES
			&& wup != PULL_ON
#endif
			);
	}

	bool
	weak_pulling_up_x(void) const {
#if PRSIM_WEAK_RULES
		return (up == PULL_OFF && wup == PULL_WEAK);
#else
		return false;
#endif
	}

	bool
	pulling_up_x(void) const {
		return normal_pulling_up_x() || weak_pulling_up_x();
	}

	bool
	normal_pulling_dn_x(void) const {
		return (dn == PULL_WEAK
#if PRSIM_WEAK_RULES
			&& wdn != PULL_ON
#endif
			);
	}

	bool
	weak_pulling_dn_x(void) const {
#if PRSIM_WEAK_RULES
		return (dn == PULL_OFF && wdn == PULL_WEAK);
#else
		return false;
#endif
	}

	bool
	pulling_dn_x(void) const {
		return normal_pulling_dn_x() || weak_pulling_dn_x();
	}

	// the following 4 functions do not account for opposition
	bool
	weak_pull_up_fires(void) const {
#if PRSIM_WEAK_RULES
		// true if weak-pull-up to 1 overtakes strong rule
		return (up != PULL_ON && wup == PULL_ON);
#else
		return false;
#endif
	}

	bool
	weak_pull_up_fires_x(void) const {
#if PRSIM_WEAK_RULES
		// true if weak-pull-up to X overtakes strong rule
		return (up == PULL_OFF && wup == PULL_WEAK);
#else
		return false;
#endif
	}

	bool
	weak_pull_dn_fires(void) const {
#if PRSIM_WEAK_RULES
		// true if weak-pull-dn to 1 overtakes strong rule
		return (dn != PULL_ON && wdn == PULL_ON);
#else
		return false;
#endif
	}

	bool
	weak_pull_dn_fires_x(void) const {
#if PRSIM_WEAK_RULES
		// true if weak-pull-dn to X overtakes strong rule
		return (dn == PULL_OFF && wdn == PULL_WEAK);
#else
		return false;
#endif
	}


	// true if weak pull to 1 or X wins
	bool
	weak_pull_up_wins(void) const {
		return dn == PULL_OFF && // wdn == PULL_OFF &&
			(weak_pull_up_fires_x() || weak_pull_up_fires());
	}

	bool
	weak_pull_dn_wins(void) const {
#if PRSIM_WEAK_RULES
		return up == PULL_OFF && // wup == PULL_OFF &&
			(weak_pull_dn_fires_x() || weak_pull_dn_fires());
#else
		return false;
#endif
	}

	/// \return true if output should have LOGIC_HIGH next
	bool
	pull_up_wins_any(void) const {
		return dn == PULL_OFF &&
			(up == PULL_ON || (wup == PULL_ON && wdn == PULL_OFF));
	}

	/// \return true if output should have LOGIC_LOW next
	bool
	pull_dn_wins_any(void) const {
		return up == PULL_OFF &&
			(dn == PULL_ON || (wdn == PULL_ON && wup == PULL_OFF));
	}

	// non-interfering pull-up-X wins
	bool
	pull_up_x_wins_any(void) const {
		return cutoff_dn() && pulling_up_x();
	}

	// non-interfering pull-dn-X wins
	bool
	pull_dn_x_wins_any(void) const {
		return cutoff_up() && pulling_dn_x();
	}

	bool
	x_wins_any(void) const {
		return pull_up_x_wins_any() || pull_dn_x_wins_any();
	}

	bool
	weak_wins_any(void) const {
#if PRSIM_WEAK_RULES
		return weak_pull_up_wins() || weak_pull_dn_wins();
#else
		return false;
#endif
	}

	bool
	definite_interference_strong(void) const {
		return up == PULL_ON && dn == PULL_ON;
	}

	// definite is a subset of possible
	bool
	possible_interference_strong(void) const {
		return (dn != PULL_OFF) && (up != PULL_OFF);
	}

	// no such things as definite_interference_strong_vs_weak

	// a strong-X vs. weak-1-or-X
	bool
	possible_interference_strong_x_up_vs_weak_dn(void) const {
		return (up == PULL_WEAK && wup != PULL_ON) &&
			(dn == PULL_OFF && wdn != PULL_OFF);
	}

	bool
	possible_interference_strong_x_dn_vs_weak_up(void) const {
		return (dn == PULL_WEAK && wdn != PULL_ON) &&
			(up == PULL_OFF && wup != PULL_OFF);
	}

	bool
	possible_interference_strong_vs_weak(void) const {
		return possible_interference_strong_x_up_vs_weak_dn() ||
			possible_interference_strong_x_dn_vs_weak_up();
	}

	bool
	definite_interference_weak(void) const {
		return normal_rules_off() &&
			wup == PULL_ON &&
			wdn == PULL_ON;
	}

	bool
	__possible_interference_weak(void) const {
		return wdn != PULL_OFF &&
			wup != PULL_OFF;
	}

	bool
	possible_interference_weak(void) const {
		return normal_rules_off() && __possible_interference_weak();
	}

	bool
	possible_interference_any(void) const {
		return possible_interference_strong() ||
			possible_interference_strong_vs_weak() ||
			possible_interference_weak();
	}

	ostream&
	dump(ostream&) const;


	struct interference_info {
		// value of interference root expressions
		enum interference_value_type {
			INTERFERENCE_VALUE_NONE,
			INTERFERENCE_1_vs_1,
			INTERFERENCE_1_vs_X,
			INTERFERENCE_X_vs_X
		};
		// interference between strong/weak rules
		enum interference_rule_type {
			INTERFERENCE_RULE_NONE,
			INTERFERENCE_STRONG_vs_STRONG,
			INTERFERENCE_STRONG_vs_WEAK,
			INTERFERENCE_WEAK_vs_WEAK
		};
		bool				have_interference;
		bool				possible_interference;
		bool				weak_rule_involved;
		interference_value_type		val_type;
		interference_rule_type		rule_type;

		explicit
		interference_info(const pull_set& p) {
		if (p.possible_interference_strong()) {
//		DEBUG_STEP_PRINT("strong vs. strong rule interference" << endl);
			have_interference = true;
			possible_interference = p.normal_pulling_x();
			weak_rule_involved = false;
			rule_type = INTERFERENCE_STRONG_vs_STRONG;
			val_type = (p.up == PULL_ON && p.dn == PULL_ON) ?
				INTERFERENCE_1_vs_1 : 
				(p.up == PULL_WEAK && p.dn == PULL_WEAK) ?
				INTERFERENCE_X_vs_X : INTERFERENCE_1_vs_X;
		}
#if PRSIM_WEAK_RULES
		else if (p.possible_interference_strong_x_up_vs_weak_dn()) {
//		DEBUG_STEP_PRINT("strong x up vs. weak dn rule interference" << endl);
			have_interference = true;
			weak_rule_involved = true;
			possible_interference = true;   // there is an X involved
			rule_type = INTERFERENCE_STRONG_vs_WEAK;
			val_type = (p.wdn == PULL_WEAK) ? INTERFERENCE_X_vs_X
				: INTERFERENCE_1_vs_X;
		} else if (p.possible_interference_strong_x_dn_vs_weak_up()) {
//		DEBUG_STEP_PRINT("strong x dn vs. weak up rule interference" << endl);
			have_interference = true;
			weak_rule_involved = true;
			possible_interference = true;   // there is an X involved
			rule_type = INTERFERENCE_STRONG_vs_WEAK;
			val_type = (p.wup == PULL_WEAK) ? INTERFERENCE_X_vs_X
				: INTERFERENCE_1_vs_X;
		} else if (p.possible_interference_weak()) {
//		DEBUG_STEP_PRINT("weak vs. weak rule interference" << endl);
			have_interference = true;
			weak_rule_involved = true;
			possible_interference = p.weak_pulling_x();
			rule_type = INTERFERENCE_WEAK_vs_WEAK;
			val_type = (p.wup == PULL_ON && p.wdn == PULL_ON) ?
				INTERFERENCE_1_vs_1 : 
				(p.wup == PULL_WEAK && p.wdn == PULL_WEAK) ?
				INTERFERENCE_X_vs_X : INTERFERENCE_1_vs_X;
#endif
		} else {
			// no interference
			have_interference = false;
			weak_rule_involved = false;
			possible_interference = false;
			rule_type = INTERFERENCE_RULE_NONE;
			val_type = INTERFERENCE_VALUE_NONE;
		}
		}
	};	// end struct interference_info

};	// end struct pull_set

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_NODE_H__

