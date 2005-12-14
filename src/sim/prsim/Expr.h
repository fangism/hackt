/**
	\file "sim/prsim/Expr.h"
	Structure for PRS expressions.  
	$Id: Expr.h,v 1.1.2.1 2005/12/14 05:16:52 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXPR_H__
#define	__HAC_SIM_PRSIM_EXPR_H__

#include <valarray>
#include <utility>

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::valarray;
using std::pair;
//=============================================================================
/**
	Based on ye old struct prs_expr (PrsExpr).
	Except that this is never used to represent a literal Node.  
 */
struct Expr {
	/// we reserve the right to tweak this!
	typedef	size_t			index_type;
	typedef	unsigned char		count_type;

	/**
		The globally assigned ID of this expression node.
		May not be needed.  
	 */
	index_type			index;

	/**
		Uplink to parent expression.
	 */
	index_type			parent;


	/**
		Type enumeration.  
		OR, AND, NOR, NAND, NOT, UP, DN.
		Never Node.  
	 */
	unsigned char			type;

	/**
		'val'
	 */
	count_type			countdown;
	/**
		'valx'
	 */
	count_type			unknowns;
	/**
		The number of children.
		const b/c it should be set once for the duration 
		of the simulation.  
	 */
	const count_type		size;
	
public:
	Expr();
	~Expr();

};	// end struct Expr

//=============================================================================
/**
	This maintains the graph node type information, 
	which is not fully needed in simulation, only needed during
	feedback or anything that requires downward traversal.  
	This graph should check parent-child invariant relationships.  
	NOTE: could also use a vector<bool> for efficiency.  
 */
struct ExprGraphNode {
	typedef	size_t			index_type;
	typedef	unsigned char		count_type;
	typedef	pair<bool, index_type>	child_entry_type;

	/**
		The offset position in the parent's subexpression list.
		This may not be needed during simulation in Expr, 
		such information is only needed for downward traversals.  
	 */
	count_type			offset;

	/**
		Technically, children information is not needed during 
		simulation in Expr, only during feedback, or user interface,
		or anything that requires a downward traversal.  
	 */
	valarray<child_entry_type>	children;
};

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EXPR_H__

