/**
	\file "sim/prsim/Node.h"
	Structure of basic PRS node.  
	$Id: Node.h,v 1.18.6.1 2009/01/31 04:46:08 fang Exp $
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
#if PRSIM_INDIRECT_EXPRESSION_MAP
#include <valarray>
#endif

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;
using std::istream;
using std::vector;

#if PRSIM_INDIRECT_EXPRESSION_MAP
struct faninout_struct_type;		// from "State-prsim.h"

/**
	Define to 1 to use vector for node fanin, else valarray.
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

#endif	// PRSIM_INDIRECT_EXPRESSION_MAP


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
#if PRSIM_INDIRECT_EXPRESSION_MAP
	// compact: reference process instance indices with fanout
	typedef	std::vector<process_index_type>	fanout_array_type;
#else
	typedef	std::vector<expr_index_type>	fanout_array_type;
#endif

	/**
		Bit fields for node structure flags.  
	 */
	typedef	enum {
		NODE_DEFAULT_STRUCT_FLAGS = 0x0000,
		/**
			Whether or not events on this node are allowed
			to be unstable.  
		 */
		NODE_UNSTAB = 0x0001,
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
	} struct_flags_enum;

#if PRSIM_INDIRECT_EXPRESSION_MAP
	/**
		This refers to a list of global process instances (indices)
		that can drive this node.  
	 */
	process_fanin_type		fanin;
#else
#if PRSIM_WEAK_RULES
	/**
		Index to the pull-up expression (normal, weak).
	 */
	expr_index_type			pull_up_index[2];
	/**
		Index to the pull-dn expression (normal, weak).
	 */
	expr_index_type			pull_dn_index[2];
#else
	/**
		Index to the pull-up expression.
	 */
	expr_index_type			pull_up_index;
	/**
		Index to the pull-dn expression.
	 */
	expr_index_type			pull_dn_index;
#endif
#endif	// PRSIM_INDIRECT_EXPRESSION_MAP

	/**
		List of expression indices to which this node fans out.  
		Size of vector: 3 x sizeof(pointer), 12B on 32b arch.
		TODO: far future, only fanout to *processes* and 
			do additional rule lookups from there.
			Performance-memory tradeoff.
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

#if !PRSIM_INDIRECT_EXPRESSION_MAP
	void
	push_back_fanout(const expr_index_type);

	bool
	contains_fanout(const expr_index_type) const;
#endif

#if !PRSIM_INDIRECT_EXPRESSION_MAP
	expr_index_type&
	get_pull_expr(const bool b
#if PRSIM_WEAK_RULES
		, const rule_strength w
#endif
		) {
		return b ? pull_up_index STR_INDEX(w)
			: pull_dn_index STR_INDEX(w);
	}

	const expr_index_type&
	get_pull_expr(const bool b
#if PRSIM_WEAK_RULES
		, const rule_strength w
#endif
		) const {
		return b ? pull_up_index STR_INDEX(w)
			: pull_dn_index STR_INDEX(w);
	}

	void
	replace_pull_index(const bool dir, const expr_index_type
#if PRSIM_WEAK_RULES
		, const rule_strength w
#endif
		);
#endif	// PRSIM_INDIRECT_EXPRESSION_MAP

	bool
	has_fanin(void) const {
#if PRSIM_INDIRECT_EXPRESSION_MAP
		return fanin.size();
#else
#if PRSIM_WEAK_RULES
		return pull_up_index[NORMAL_RULE] ||
			pull_dn_index[NORMAL_RULE] ||
			pull_up_index[WEAK_RULE] || pull_dn_index[WEAK_RULE];
#else
		return pull_up_index || pull_dn_index;
#endif
#endif
	}

	bool
	has_fanout(void) const {
		return fanout.size();
	}

	bool
	is_unstab(void) const { return struct_flags & NODE_UNSTAB; }

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
		/**
			Auxiliary flag for general purpose visit tracking.
		 */
		NODE_FLAG = 0x02,
		/**
			Whether or not this node is currently in
			one of the exclusive ring queues.  
		 */
		NODE_EX_QUEUE = 0x04,

		/// true if this node participates in any registered channel
		NODE_IN_CHANNEL = 0x08,
#if 0
		// THESE OVERFLOW uchar!!!
		NODE_CHANNEL_VALID = 0x10,
		NODE_CHANNEL_DATA = 0x20,
#endif

		/// OR-mask for initialization
		NODE_INITIALIZE_SET_MASK = 0x00,
		/// AND-mask (negated) for initialization
		NODE_INITIALIZE_CLEAR_MASK =
			NODE_FLAG | NODE_EX_QUEUE
	} state_flags_enum;

public:
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
	 */
	event_index_type			event_index;
	/**
		Structure for tracking last cause, by node and value.  
	 */
	LastCause				causes;

public:
	/**
		Transition counts.  
		Not critical to simulation, unless we want statistics.  
	 */
	size_t					tcount;
#if PRSIM_INDIRECT_EXPRESSION_MAP
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
#endif	// PRSIM_INDIRECT_EXPRESSION_MAP
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

	bool
	is_breakpoint(void) const { return state_flags & NODE_BREAKPOINT; }

	void
	set_breakpoint(void) { state_flags |= NODE_BREAKPOINT; }

	void
	clear_breakpoint(void) { state_flags &= ~NODE_BREAKPOINT; }

	bool
	is_flagged(void) const { return state_flags & NODE_FLAG; }

	bool
	in_excl_queue(void) const { return state_flags & NODE_EX_QUEUE; }

	void
	set_excl_queue(void) { state_flags |= NODE_EX_QUEUE; }

	void
	clear_excl_queue(void) { state_flags &= ~NODE_EX_QUEUE; }

	void
	set_in_channel(void) { state_flags |= NODE_IN_CHANNEL; }

	bool
	in_channel(void) const { return state_flags & NODE_IN_CHANNEL; }

	value_enum
	current_value(void) const { return value; }

	void
	set_value_and_cause(const value_enum c, const event_cause_type& e) {
		value = c;
		causes.set_cause(c, e);
	}

	ostream&
	dump_value(ostream&) const;

	ostream&
	dump_state(ostream&) const;

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
	value_enum
	char_to_value(const char);

	static
	char
	translate_value_to_char(const char e) {
		return value_to_char[size_t(e)];
	}

	/// \return < 0 on error, else returns 0, 1, 2
	static
	value_enum
	string_to_value(const std::string&);

	void
	initialize(void);

	void
	reset(void);

	void
	reset_tcount(void) { tcount = 0; }

#if PRSIM_INDIRECT_EXPRESSION_MAP
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
#endif	// PRSIM_INDIRECT_EXPRESSION_MAP

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
#if PRSIM_INDIRECT_EXPRESSION_MAP
/**
	Set of pull states, in all directions.  
 */
struct pull_set {
#if PRSIM_WEAK_RULES
	pull_enum			up[2];
	pull_enum			dn[2];
#else
	pull_enum			up;
	pull_enum			dn;
#endif

	explicit
	pull_set(const NodeState& n) {
		up STR_INDEX(NORMAL_RULE) =
			n.pull_up_state STR_INDEX(NORMAL_RULE).pull();
		dn STR_INDEX(NORMAL_RULE) =
			n.pull_dn_state STR_INDEX(NORMAL_RULE).pull();
#if PRSIM_WEAK_RULES
		up STR_INDEX(WEAK_RULE) =
			n.pull_up_state STR_INDEX(WEAK_RULE).pull();
		dn STR_INDEX(WEAK_RULE) =
			n.pull_dn_state STR_INDEX(WEAK_RULE).pull();
#endif
	}
};	// end struct pull_set
#endif

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_NODE_H__

