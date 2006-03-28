/**
	\file "sim/prsim/Node.h"
	Structure of basic PRS node.  
	$Id: Node.h,v 1.2.26.5 2006/03/28 03:48:05 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_NODE_H__
#define	__HAC_SIM_PRSIM_NODE_H__

#include <iosfwd>
// #include <valarray>
#include <vector>
#include "util/string_fwd.h"
#include "util/macros.h"
#include "sim/common.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;
using std::vector;
// using std::valarray;
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
			exclusive high ring.  
		 */
		NODE_EXCLHI = 0x0002,
		/**
			Whether or not this node belongs to at least one
			exclusive low ring.  
		 */
		NODE_EXCLLO = 0x0004
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

#if 0
	void
	initialize(void);
#endif

	bool
	is_unstab(void) const { return struct_flags & NODE_UNSTAB; }

	bool
	has_exclhi(void) const { return struct_flags & NODE_EXCLHI; }

	bool
	has_excllo(void) const { return struct_flags & NODE_EXCLLO; }

	ostream&
	dump_fanout_dot(ostream&, const std::string&) const;

	ostream&
	dump_struct(ostream&) const;

	/// \return < 0 on error, else returns 0, 1, 2
	static
	char
	string_to_value(const std::string&);
};	// end struct Node

//=============================================================================
/**
	Structural information extended with stateful information.  
	Size of this should be a total of 8 double-words, or 32 B.  
	Nice and aligned.  
 */
struct NodeState : public Node {
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
		NODE_EX_QUEUE = 0x04
	} state_flags_enum;

public:
	static const char		value_to_char[3];
	static const char		invert_value[3];
protected:
	/**
		Technically, this is stateful, not structural information.
		enum:
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

	/**
		Transition counts.  
		Not critical to simulation, unless we want statistics.  
	 */
	size_t					tcount;
public:
	/// count on compiler to optimize zero comparison
	bool
	pending_event(void) const {
		return event_index != INVALID_EVENT_INDEX;
	}

	event_index_type
	get_event(void) const { return event_index; }

	void
	set_event(const event_index_type i) {
		INVARIANT(event_index == INVALID_EVENT_INDEX);
		INVARIANT(i != INVALID_EVENT_INDEX);
		event_index = i;
	}

	void
	clear_event(void) { event_index = INVALID_EVENT_INDEX; }

	bool
	is_breakpoint(void) const { return state_flags & NODE_BREAKPOINT; }

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

	void
	initialize(void);
};	// end struct NodeState

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_NODE_H__

