/**
	\file "sim/prsim/Node.h"
	Structure of basic PRS node.  
	$Id: Node.h,v 1.13.74.4 2008/02/22 06:07:25 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_NODE_H__
#define	__HAC_SIM_PRSIM_NODE_H__

#include <iosfwd>
// #include <valarray>
#include <vector>
#include "util/string_fwd.h"
#include "util/macros.h"
#include "util/attributes.h"
#include "util/utypes.h"
#include "sim/common.h"
#include "sim/prsim/devel_switches.h"
#include "sim/prsim/Cause.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;
using std::istream;
using std::vector;

#if PRSIM_WEAK_RULES
enum rule_strength {
	NORMAL_RULE = 0,
	WEAK_RULE = 1
};
#define	STR_INDEX(w)	[w]
#else
#define	STR_INDEX(w)
#endif


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
	typedef	std::vector<expr_index_type>	fanout_array_type;

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

	/**
		List of expression indices to which this node fans out.  
		Size of vector: 3 x sizeof(pointer), 12B on 32b arch.
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

	void
	push_back_fanout(const expr_index_type);

	bool
	contains_fanout(const expr_index_type) const;

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

	bool
	has_fanin(void) const {
#if PRSIM_WEAK_RULES
		return pull_up_index[NORMAL_RULE] ||
			pull_dn_index[NORMAL_RULE] ||
			pull_up_index[WEAK_RULE] || pull_dn_index[WEAK_RULE];
#else
		return pull_up_index || pull_dn_index;
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

	typedef	enum {
		LOGIC_LOW = 0x00,		// 0
		LOGIC_HIGH = 0x01,		// 1
		LOGIC_VALUE = 0x01,		// value mask
		LOGIC_OTHER = 0x02,		// 2
		LOGIC_MASK = 0x03
	} value_enum;

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

#if PRSIM_CHANNEL_SUPPORT
		/// true if this node participates in any registered channel
		NODE_IN_CHANNEL = 0x08,
#if 0
		// THESE OVERFLOW uchar!!!
		NODE_CHANNEL_VALID = 0x10,
		NODE_CHANNEL_DATA = 0x20,
#endif
#endif

		/// OR-mask for initialization
		NODE_INITIALIZE_SET_MASK = 0x00,
		/// AND-mask (negated) for initialization
		NODE_INITIALIZE_CLEAR_MASK =
			NODE_FLAG | NODE_EX_QUEUE
	} state_flags_enum;

public:
	static const uchar		value_to_char[3];
	static const uchar		invert_value[3];
protected:
	/**
		Uses enum value_enum:
		0 = 0, 1 = 1, 2 = X, 3 = X
	 */
	uchar					value;

	/**
		8-bit field for flagging stateful information.  
	 */
	char					state_flags;
	/**
		Current enqueued event index associated with this node.
		INVALID_EVENT_INDEX (0) means no pending event.  
	 */
	event_index_type			event_index;
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
	/**
		Structure for tracking last cause, by node and value.  
	 */
	LastCause				causes;
#else
	/**
		The firing of this node was caused by...
		like last arriving input, for critical path analysis.  
	 */
	node_index_type				caused_by_node;
#endif
public:
	/**
		Transition counts.  
		Not critical to simulation, unless we want statistics.  
	 */
	size_t					tcount;
public:
	NodeState() : parent_type(), value(LOGIC_OTHER), 
		state_flags(NODE_INITIAL_STATE_FLAGS),
		event_index(INVALID_EVENT_INDEX), 
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		causes(), 
#else
		caused_by_node(INVALID_NODE_INDEX), 
#endif
		tcount(0) { }

	/// count on compiler to optimize zero comparison
	bool
	pending_event(void) const {
		return event_index != INVALID_EVENT_INDEX;
	}

#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
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
	get_cause(const uchar v) const {
		return causes.get_cause(v);
	}
#else
	node_index_type
	get_cause_node(void) const { return caused_by_node; }

	void
	set_cause_node(const node_index_type ci) { caused_by_node = ci; }
#endif

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

#if PRSIM_CHANNEL_SUPPORT
	void
	set_in_channel(void) { state_flags |= NODE_IN_CHANNEL; }

	bool
	in_channel(void) const { return state_flags & NODE_IN_CHANNEL; }
#endif

	uchar
	current_value(void) const { return value; }

#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
	void
	set_value_and_cause(const uchar c, const event_cause_type& e) {
		value = c;
		causes.set_cause(c, e);
	}
#else
	void
	set_value(const uchar c) { value = c; }

#endif
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
		const uchar		match_val;

		status_dumper(ostream& o, const uchar v) :
			os(o), match_val(v) { }

		// no copy-ctor
		// default destructor

		void
		operator () (const NodeState&);

	};	// end struct status_dumper

	static
	bool
	is_valid_value(const uchar c) {
		// return c >= LOGIC_LOW && c <= LOGIC_OTHER;
		return c <= LOGIC_OTHER;
	}

	static
	uchar
	char_to_value(const char);

	/// \return < 0 on error, else returns 0, 1, 2
	static
	uchar
	string_to_value(const std::string&);

	void
	initialize(void);

	void
	reset(void);

	void
	reset_tcount(void) { tcount = 0; }

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

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_NODE_H__

