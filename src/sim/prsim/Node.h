/**
	\file "sim/prsim/Node.h"
	Structure of basic PRS node.  
	$Id: Node.h,v 1.1.2.3 2005/12/16 02:43:20 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_NODE_H__
#define	__HAC_SIM_PRSIM_NODE_H__

#include <valarray>
#include "sim/common.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::valarray;
//=============================================================================
/**
	Node state information structure.  
	Sort of imported definition from prsim's struct prs_node (PrsNode)
	Note: no alias information here, that comes from the module's
		object hierarchy and unique allocation.  
	Consider: padding and alignment?
	TODO: define enum for value?
 */
struct Node {
	typedef	enum {
		LOGIC_LOW = 0,		// 0
		LOGIC_HIGH = 1,		// 1
		LOGIC_OTHER = 2		// 2
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
	valarray<expr_index_type>	fanout;

public:
	/// these aren't created frequently, inline doesn't matter
	Node();

	/// these aren't destroyed frequently, inline doesn't matter
	~Node();

	void
	initialize(void);

};	// end struct node

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_NODE_H__

