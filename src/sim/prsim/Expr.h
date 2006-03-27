/**
	\file "sim/prsim/Expr.h"
	Structure for PRS expressions.  
	$Id: Expr.h,v 1.2.26.4 2006/03/27 05:40:49 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXPR_H__
#define	__HAC_SIM_PRSIM_EXPR_H__

#include <iosfwd>
// #include <valarray>
#include <vector>
#include <utility>
#include "sim/common.h"
#include "util/macros.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;
// using std::valarray;
using std::vector;
using std::pair;
//=============================================================================
/**
	Based on ye old struct prs_expr (PrsExpr).
	Except that this is never used to represent a literal Node.  
	Because of const non-static members, 
	Expr is not Assignable, only copy-constructible.  
	TODO: split up into structural and stateful information.  
 */
struct Expr {
	typedef	unsigned char		count_type;
	/**
		There is no NODE expr, just use AND/OR of size 1.  
		There is no NOT expr, just use and NAND/NOR of size 1.  
		NOTE: one nibble is sufficient to encode this.  
	 */
	typedef	enum {
		EXPR_NODE = 0x0,	///< default to using OR
		EXPR_OR = 0x0,
		EXPR_AND = 0x1,
		EXPR_NOT = 0x2,	///< could be used to mask for negation
		EXPR_NAND = 0x2,
		EXPR_NOR = 0x3,
		EXPR_MASK = 0x3, ///< two LSB encode the logic function
		EXPR_ROOT = 0x4, ///< if the parent expression is a node
		EXPR_DIR = 0x8	 ///< if parent is node, what direction to pull
	} type_enum;

	/**
		Uplink to parent expression (or node).
		Interpreted as expression index if !(type & EXPR_ROOT), 
		else interpreted as node index.  
	 */
//	const
	expr_index_type		parent;

	/**
		Type enumeration.  
		OR, AND, NOR, NAND, NOT, UP, DN.
		If is UP or DN, then parent is interpreted as 
		a node_index_type!
	 */
//	const
	unsigned char		type;
	/**
		The number of children.
		const b/c it should be set once for the duration 
		of the simulation.  
		Is this redundant with the ExprGraphNode::children::size()?
	 */
//	const
	count_type		size;

public:
	Expr();

	Expr(const unsigned char, const count_type);

	Expr(const expr_index_type, const unsigned char, const count_type);

	~Expr();

	void
	set_root(void) { type |= EXPR_ROOT; }

	void
	unset_root(void) { type &= ~EXPR_ROOT; }

	bool
	is_root(void) const { return type & EXPR_ROOT; }

	bool
	is_not(void) const { return type & EXPR_NOT; }


	/**
		\pre direction is only meaningful if this expression is 
		a root pull-up or pull-dn.  
	 */
	bool
	direction(void) const {
		INVARIANT(is_root());
		return type & EXPR_DIR;
	}

	void
	set_parent_expr(const expr_index_type e) {
		unset_root();
		parent = e;
	}

	void
	set_parent_node(const node_index_type n) {
		set_root();
		parent = n;
	}

	void
	pull_up(const node_index_type ni) {
		type |= EXPR_DIR;
		set_root();
		parent = ni;
	}

	void
	pull_dn(const node_index_type ni) {
		type &= ~EXPR_DIR;
		set_root();
		parent = ni;
	}

	void
	pull(const node_index_type ni, const bool d) {
		if (d)	type |= EXPR_DIR;
		else	type &= ~EXPR_DIR;
		set_root();
		parent = ni;
	}

	bool
	is_or(void) const { return !(type & EXPR_AND) && !(type & EXPR_NOT); }

	/// see "Object/lang/PRS_enum.h"
	bool
	parenthesize(const char) const;

	/// see "Object/lang/PRS_enum.h"
	char
	to_prs_enum(void) const;

	ostream&
	dump_struct(ostream&) const;

	ostream&
	dump_type_dot_shape(ostream&) const;

	ostream&
	dump_parent_dot_edge(ostream&) const;

#if 0
private:
	Expr&
	operator = (const Expr&) {
		parent = 
	}
#endif
};	// end struct Expr

//=============================================================================
/**
	Stateful expression class, derived from expression structure.  
 */
struct ExprState : public Expr {
public:
	typedef	Expr			parent_type;
	/**
		These values are special, they correspond to 
		LOGIC_LOW, LOGIC_HIGH, LOGIC_OTHER.  
	 */
	typedef	enum {
		PULL_OFF = 0,
		PULL_ON = 1,
		PULL_WEAK = 2
	} pull_enum;

public:
	/**
		'val' of the old PrsExpr.
	 */
	count_type			countdown;
	/**
		'valx' of the old PrsExpr.
	 */
	count_type			unknowns;
protected:
	// consider a redundant pull-state enum (cached) to avoid re-evaluation?
public:
	ExprState() : parent_type() { }

	/**
		Leaves countdown and unknowns uninitialized, 
		because they shouldn't be set until the structure is finalized
		by the responsible State object.  
	 */
	ExprState(const unsigned char t, const count_type s) :
		parent_type(t, s) { }


	void
	initialize(void);

	/**
		\pre this->is_or();
	 */
	pull_enum
	or_pull_state(void) const {
		return (countdown ? PULL_ON :
			(unknowns ? PULL_WEAK : PULL_OFF));
	}

	/**
		\pre !this->is_or();
	 */
	pull_enum
	and_pull_state(void) const {
		return (countdown ? PULL_OFF :
			(unknowns ? PULL_WEAK : PULL_ON));
	}

	/**
		See pull_enum enumerations.  
		\return 0 if off, 1 if on, 2 if weak (X)
	 */
	pull_enum
	pull_state(void) const {
		return is_or() ? or_pull_state() : and_pull_state();
	}

	ostream&
	dump_state(ostream&) const;

};	// end struct ExprState

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
	TODO: possibly add back-link to the uniquely allocated process-id
		from which a particular subexpresion originated
		for profiling purposes!
 */
struct ExprGraphNode {
	typedef	size_t			index_type;
	typedef	unsigned char		count_type;
	/**
		The second value can be interpreted as an index
		to an expression or bool node.  
		The first field indicates whether or not this is a 
		(leaf) node.  
	 */
	typedef	pair<bool, expr_index_type>	child_entry_type;
#if 0
	typedef	valarray<child_entry_type>	children_array_type;
	typedef	child_entry_type*		iterator;
	typedef	const child_entry_type*		const_iterator;
#else
	typedef	vector<child_entry_type>	children_array_type;
	typedef	children_array_type::iterator	iterator;
	typedef	children_array_type::const_iterator	const_iterator;
#endif
private:
	struct node_membership_checker;
	enum {
		INVALID_OFFSET = 0xFF
	};
public:
	/**
		The offset position in the parent's subexpression list.
		This may not be needed during simulation in Expr, 
		such information is only needed for downward traversals.  
		This offset is 0-indexed, into the parent 
		ExprGraphNode::children array.  
		This field is ignored for the root expressions
		(whose parents are nodes).  
	 */
	count_type			offset;

	/**
		Technically, children information is not needed during 
		simulation in Expr, only during feedback, or user interface,
		or anything that requires a downward traversal.  
		The size info is redundant with Expr::size, hmmm.  
		Consider using vector? (bigger by 1 pointer)
	 */
	children_array_type		children;

public:
	ExprGraphNode() : offset(INVALID_OFFSET), children() { }

	void
	push_back_expr(const size_t);

	void
	push_back_node(const size_t);

	ostream&
	dump_struct(ostream&) const;

	bool
	contains_node_fanin(const node_index_type) const;

#if 0
	const_iterator
	begin(void) const { return &children[0]; }

	const_iterator
	end(void) const { return &children[children.size()]; }
#else
	const_iterator
	begin(void) const { return children.begin(); }

	const_iterator
	end(void) const { return children.end(); }
#endif

};	// end struct ExprGraphNode

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EXPR_H__

