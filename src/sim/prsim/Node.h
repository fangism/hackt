/**
	\file "sim/prsim/Node.h"
	Structure of basic PRS node.  
	$Id: Node.h,v 1.1.2.8 2006/01/19 00:16:16 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_NODE_H__
#define	__HAC_SIM_PRSIM_NODE_H__

#include <iosfwd>
// #include <valarray>
#include <vector>
#include "util/string_fwd.h"
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
	Consider: padding and alignment?
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
	 */
	unsigned int			breakpoint : 1;
	/**
		A visit-once auxiliary flag.  
		Could consider doing this in a separate array.  
		Probably not needed *during* simulation.
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

};	// end struct Node

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_NODE_H__

