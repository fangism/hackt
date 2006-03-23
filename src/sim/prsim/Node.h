/**
	\file "sim/prsim/Node.h"
	Structure of basic PRS node.  
	$Id: Node.h,v 1.2.26.1 2006/03/23 07:05:18 fang Exp $
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
	Sort of imported definition from prsim's struct prs_node (PrsNode)
	Note: no alias information here, that comes from the module's
		object hierarchy and unique allocation.  
	Tools that only perform static analysis on netlists can use
		just this structure.  
	Consider: padding and alignment?
	TODO: attribute packed for density, or align for speed?
	TODO: define enum for value?
	TODO: instead of declaring bitfield, use integer fields and define
		mask enums.  
 */
struct Node {
	typedef	std::vector<expr_index_type>	fanout_array_type;
	typedef	enum {
		LOGIC_LOW = 0x0,		// 0
		LOGIC_HIGH = 0x1,		// 1
		LOGIC_VALUE = 0x1,		// value mask
		LOGIC_OTHER = 0x2		// 2
	} value_enum;
#if 0
	/**
		Optional: the globally allocated index.  
		Might not be needed.
	 */
	node_index_type			index;
#endif
	/**
		Technically, this is stateful, not structural information.
		enum:
		0 = 0, 1 = 1, 2 = X, 3 = X
	 */
	unsigned int			value : 2;
	/**
		Mask instability events for this node?
	 */
	unsigned int			unstab : 1;
	/**
		Whether or not this node is a breakpoint.  
		Technically, this is stateful, not structural information.
	 */
	unsigned int			breakpoint : 1;
	/**
		A visit-once auxiliary flag.  
		Could consider doing this in a separate array.  
		Probably not needed *during* simulation.
		This should not be here.  
	 */
	mutable unsigned int		flag : 1;
	/**
		Whether or not this is part of 
		an exclusive high ring.  
		Not yet supported.  
	 */
	unsigned int			exclhi : 1;
	/**
		Whether or not this is part of 
		an exclusive low ring.  
		Not yet supported.  
	 */
	unsigned int			excllo : 1;
	/**
		True if is not in normal event queue.
		No idea WTF this is.  
		This sounds like stateful information and need not be here.  
	 */
	unsigned int			ex_queue : 1;

	/**
		Transition counts.  
	 */
	size_t				tcount;
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
	 */
	fanout_array_type		fanout;
public:
	static const char		value_to_char[3];
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

	void
	initialize(void);

	ostream&
	dump_fanout_dot(ostream&, const std::string&) const;

	ostream&
	dump_value(ostream&) const;

	ostream&
	dump_struct(ostream&) const;

	ostream&
	dump_state(ostream&) const;

	char
	current_value(void) const { return value; }

	/// \return < 0 on error, else returns 0, 1, 2
	static
	char
	string_to_value(const std::string&);
};	// end struct Node

//=============================================================================
/**
	Structural information extended with stateful information.  
 */
struct NodeState : public Node {
	typedef	Node				parent_type;
	/**
		Current enqueued event index associated with this node.
		INVALID_EVENT_INDEX (0) means no pending event.  
	 */
	event_index_type			event_index;
public:
	/// count on compiler to optimize zero comparison
	bool
	pending_event(void) const {
		return event_index != INVALID_NODE_INDEX;
	}

	void
	set_event(const event_index_type i) {
		INVARIANT(event_index == INVALID_EVENT_INDEX);
		INVARIANT(i != INVALID_EVENT_INDEX);
		event_index = i;
	}

	void
	initialize(void);
};	// end struct NodeState

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_NODE_H__

