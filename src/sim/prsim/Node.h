/**
	\file "sim/prsim/Node.h"
	Structure of basic PRS node.  
	$Id: Node.h,v 1.11 2006/07/18 04:09:16 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_NODE_H__
#define	__HAC_SIM_PRSIM_NODE_H__

#include <iosfwd>
// #include <valarray>
#include <vector>
#include "util/string_fwd.h"
#include "util/macros.h"
#include "util/attributes.h"
#include "sim/common.h"
#include "sim/prsim/devel_switches.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;
using std::istream;
using std::vector;

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


	/**
		Index to the pull-up expression.
	 */
	expr_index_type			pull_up_index;
	/**
		Index to the pull-dn expression.
	 */
	expr_index_type			pull_dn_index;

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
	unsigned short			struct_flags;

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
	get_pull_expr(const bool b) {
		return b ? pull_up_index : pull_dn_index;
	}

	const expr_index_type&
	get_pull_expr(const bool b) const {
		return b ? pull_up_index : pull_dn_index;
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

	ostream&
	dump_fanout_dot(ostream&, const std::string&) const;

	ostream&
	dump_struct(ostream&) const;

};	// end struct Node

//=============================================================================
/**
	Structure for tracking event causality in greater detail.  
	Members kept in separate arrays for better alignment.  
 */
struct LastCause {
	/**
		The causing node.  
		Indexed by node's current value.  
	 */
	node_index_type				caused_by_node[3];
	/**
		The value of the causing node.  
		3 of 4 slots used for better padding.  
	 */
	unsigned char				caused_by_value[4];

	/**
		How the f-- do you initialize aggregate members?
	 */
	LastCause()
		// caused_by_node({0}),
		// caused_by_value({0})
	{
		// caused_by_node = {0};
		// caused_by_value = {0};
		// *this = {{0,0,0}, {0,0,0}};
		caused_by_node[0] = INVALID_NODE_INDEX;
		caused_by_node[1] = INVALID_NODE_INDEX;
		caused_by_node[2] = INVALID_NODE_INDEX;
		caused_by_value[0] = 0;	// don't care
		caused_by_value[1] = 0;	// don't care
		caused_by_value[2] = 0;	// don't care
	}
};	// end struct LastCause

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

		/// OR-mask for initialization
		NODE_INITIALIZE_SET_MASK = 0x00,
		/// AND-mask (negated) for initialization
		NODE_INITIALIZE_CLEAR_MASK =
			NODE_FLAG | NODE_EX_QUEUE
	} state_flags_enum;

public:
	static const char		value_to_char[3];
	static const char		invert_value[3];
protected:
	/**
		Uses enum value_enum:
		0 = 0, 1 = 1, 2 = X, 3 = X
	 */
	unsigned char				value;

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

	char
	current_value(void) const { return value; }

	void
	set_value(const char c) { value = c; }

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
		const char		match_val;

		status_dumper(ostream& o, const char v) :
			os(o), match_val(v) { }

		// no copy-ctor
		// default destructor

		void
		operator () (const NodeState&);

	};	// end struct status_dumper

	static
	bool
	is_valid_value(const char c) {
		return c >= LOGIC_LOW && c <= LOGIC_OTHER;
	}

	static
	char
	char_to_value(const char);

	/// \return < 0 on error, else returns 0, 1, 2
	static
	char
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

