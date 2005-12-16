/**
	\file "sim/prsim/Expr.h"
	Structure for PRS expressions.  
	$Id: Expr.h,v 1.1.2.4 2005/12/16 02:43:20 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXPR_H__
#define	__HAC_SIM_PRSIM_EXPR_H__

#include <valarray>
#include <utility>
#include "sim/common.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::valarray;
using std::pair;
//=============================================================================
/**
	Based on ye old struct prs_expr (PrsExpr).
	Except that this is never used to represent a literal Node.  
	Because of const non-static members, 
	Expr is not Assignable, only copy-constructible.  
 */
struct Expr {
	typedef	unsigned char		count_type;
	/**
		There is no NODE expr, just use AND/OR of size 1.  
		There is no NOT expr, just use and NAND/NOR of size 1.  
		NOTE: one nibble is sufficient to encode this.  
	 */
	typedef	enum {
		EXPR_OR = 0x0,
		EXPR_AND = 0x1,
		EXPR_NOT = 0x2,	///< could be used to mask for negation
		EXPR_NAND = 0x2,
		EXPR_NOR = 0x3,
		EXPR_MASK = 0x3, ///< two LSB encode the logic function
		EXPR_ROOT = 0x4, ///< if the parent expression is a node
		EXPR_DIR = 0x8	 ///< if is node, then what direction to pull
	} type_enum;
#if 0
	/**
		The globally assigned ID of this expression node.
		May not be needed.  
	 */
	expr_index_type			index;
#endif
	/**
		Uplink to parent expression (or node).
	 */
	const expr_index_type		parent;


	/**
		Type enumeration.  
		OR, AND, NOR, NAND, NOT, UP, DN.
		If is UP or DN, then parent is interpreted as 
		a node_index_type!
	 */
	const unsigned char		type;
	/**
		The number of children.
		const b/c it should be set once for the duration 
		of the simulation.  
	 */
	const count_type		size;
	
	/**
		'val'
	 */
	count_type			countdown;
	/**
		'valx'
	 */
	count_type			unknowns;
public:
	Expr();

	Expr(const expr_index_type, const unsigned char, const count_type);

	~Expr();

	void
	initialize(void);

private:
#if 0
	Expr&
	operator = (const Expr&) {
		parent = 
	}
#endif
};	// end struct Expr

//=============================================================================
/**
	There should be one of these per Expr.  
	Access to these is not performance critical, 
	which is why we keep it separate.  
	This maintains the graph node type information, 
	which is not fully needed in simulation, only needed during
	feedback or anything that requires downward traversal.  
	This graph should check parent-child invariant relationships.  
	NOTE: could also use a vector<bool> for efficiency.  
 */
struct ExprGraphNode {
	typedef	size_t			index_type;
	typedef	unsigned char		count_type;
	/**
		The second value can be interpreted as an index
		to an expression or bool node.  
	 */
	typedef	pair<bool, expr_index_type>	child_entry_type;
public:
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
		The size info is redundant with Expr::size, hmmm.  
	 */
	valarray<child_entry_type>	children;
};

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EXPR_H__

