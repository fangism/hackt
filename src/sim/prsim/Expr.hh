/**
	\file "sim/prsim/Expr.hh"
	Structure for PRS expressions.  
	$Id: Expr.hh,v 1.17 2009/02/07 04:08:41 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXPR_H__
#define	__HAC_SIM_PRSIM_EXPR_H__

#include <iosfwd>
// #include <valarray>
#include <vector>
#include <utility>
#include "sim/common.hh"
#include "util/macros.h"
#include "util/attributes.h"
#include "Object/lang/PRS_fwd.hh"	// for expr_count_type
#include "Object/common/frame_map.hh"	// for footprint_frame_map_type
#include "sim/prsim/devel_switches.hh"
#include "sim/prsim/enums.hh"

/**
	Define to 1 to use PULL_WEAK == 1.
 */
#define	USE_BETTER_PULL_ENCODING		0

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;
using std::istream;
// using std::valarray;
using std::vector;
using std::pair;
using entity::PRS::expr_count_type;
using entity::footprint_frame_map_type;

//=============================================================================
/**
	Based on ye old struct prs_expr (PrsExpr).
	Except that this is never used to represent a literal Node.  
	Because of const non-static members, 
	Expr is not Assignable, only copy-constructible.  
	TODO: split up into structural and stateful information.  
 */
struct Expr {
	/**
		unsigned short or char, to count maximum number
		of expression-children per node.  
	 */
	typedef	expr_count_type		count_type;
	/**
		There is no NODE expr, just use AND/OR of size 1.  
		There is no NOT expr, just use and NAND/NOR of size 1.  
		NOTE: one nibble is sufficient to encode this.  
	 */
	typedef	enum {
		EXPR_NODE = 0x00,	///< default to using OR
		EXPR_OR = 0x00,
		EXPR_AND = 0x01,
		EXPR_NOT = 0x02,	///< could be used to mask for negation
		EXPR_NOR = 0x02,
		EXPR_NAND = 0x03,
		EXPR_MASK = 0x03, ///< two LSB encode the logic function
		EXPR_ROOT = 0x04 ///< if the parent expression is a node
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
	uchar			type;
//	uchar			__unused_padding;
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

	Expr(const uchar, const count_type);

	Expr(const expr_index_type, const uchar, const count_type);

#if 1
// explicit copy-constructor, if we don't trust (clang) compiler
// to synthesize one correctly
	Expr(const Expr& e) : parent(e.parent), type(e.type), size(e.size) { }

	// did it screw up default assignment too?  yes it did.  :(
	Expr&
	operator = (const Expr& e) {
		parent = e.parent;
		type = e.type;
		size = e.size;
		return *this;
	}
#endif

	~Expr();

	void
	set_root(void) { type |= EXPR_ROOT; }

	void
	unset_root(void) { type &= ~EXPR_ROOT; }

	bool
	is_root(void) const { return type & EXPR_ROOT; }

	bool
	is_not(void) const { return type & EXPR_NOT; }

	void
	toggle_not(void) { type ^= EXPR_NOT; }

	void
	toggle_demorgan(void) {
		// pushes the bubble: and <-> nor, nand <-> or
		type ^= (EXPR_NOT | EXPR_AND);
	}

	// should also be called
	void
	wipe(void) {
		size = 0;
	}

	bool
	wiped(void) const { return !size; }

	bool
	is_trivial(void) const {
		return (size == 1) && !is_not();
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
	pull(const node_index_type ni) {
		set_root();
		parent = ni;
	}

	void
	invariant_root(void) {
		set_root();
		// parent = 0;		// doesn't matter
	}

	bool
	is_or(void) const { return !(type & EXPR_AND) && !(type & EXPR_NOT); }

	bool
	is_conjunctive(void) const { return type & EXPR_AND; }

	bool
	is_disjunctive(void) const { return !(type & EXPR_AND); }

	bool
	is_nand(void) const { return (type & EXPR_AND) && (type & EXPR_NOT); }

	bool
	parenthesize(const uchar, const bool) const;

	/// see "Object/lang/PRS_enum.h"
	uchar
	to_prs_enum(void) const;

	ostream&
	dump_struct(ostream&, const bool) const;

	ostream&
	dump_type_dot_shape(ostream&) const;

	ostream&
	dump_parent_dot_edge(ostream&, const footprint_frame_map_type&,
		const expr_index_type, const bool) const;

#if 0
private:
	Expr&
	operator = (const Expr&) {
		parent = 
	}
#endif
} __ATTRIBUTE_ALIGNED__ ;	// end struct Expr

//=============================================================================
/**
	Stateful expression class, derived from expression structure.  
	TODO: figure out a clean way to use enums in code rather than chars.  
 */
struct ExprState {
private:
	typedef	ExprState		this_type;
public:
	typedef	expr_count_type		count_type;
public:
	/**
		'val' of the old PrsExpr.
		For OR-expressions, this represents the number of 1's.
		For AND-expressions, this represents the number of 0's.
	 */
	count_type			countdown;
	/**
		'valx' of the old PrsExpr.
	 */
	count_type			unknowns;
protected:
	// consider a redundant pull-state enum (cached) to avoid re-evaluation?
public:
	ExprState() { }

	/**
		Leaves countdown and unknowns uninitialized, 
		because they shouldn't be set until the structure is finalized
		by the responsible State object.  
	 */
	ExprState(const uchar t, const count_type s) { }

// these macros also used in .cc file
#define	EXPR_PARAM		const Expr& e
#define	EXPR_REF		e
#define	EXPR_MEM(x)		(e.x)

	void
	initialize(EXPR_PARAM);

	void
	reset(EXPR_PARAM) { initialize(EXPR_REF); }

	/**
		TODO: re-encode pull states so we can use 2-x.
	 */
	static
	pull_enum
	negate_pull(const pull_enum p) {
		return (p == PULL_WEAK) ? PULL_WEAK :
			((p == PULL_OFF) ? PULL_ON : PULL_OFF);
	}


	/**
		\pre this->is_or();
		countdown represents the number of 1's
		PULL_ON: countdown != 0
		PULL_OFF: countdown == 0 && unknowns == 0
		PULL_WEAK: countdown == 0 && unknowns != 0
		Negation only affects the ON/OFF states.
	 */
	pull_enum
	or_pull_state(EXPR_PARAM) const {
		const pull_enum ret = (countdown ? PULL_ON :
			(unknowns ? PULL_WEAK : PULL_OFF));
		return EXPR_MEM(is_not()) ? negate_pull(ret) : ret;
	}

	/**
		\pre !this->is_or();
		countdown represents the number of 0's
		PULL_OFF: countdown != 0
		PULL_ON: countdown == 0 && unknowns == 0
		PULL_WEAK: countdown == 0 && unknowns != 0
		Negation only affects the ON/OFF states.
	 */
	pull_enum
	and_pull_state(EXPR_PARAM) const {
		const pull_enum ret = (countdown ? PULL_OFF :
			(unknowns ? PULL_WEAK : PULL_ON));
		return EXPR_MEM(is_not()) ? negate_pull(ret) : ret;
	}

	/**
		See pull_enum enumerations.  
		NOTE: this does account for negation.  
		\return 0 if off, 1 if on, 2 if weak (X)
	 */
	pull_enum
	pull_state(EXPR_PARAM) const {
		return EXPR_MEM(is_disjunctive()) ?
			or_pull_state(EXPR_REF) : and_pull_state(EXPR_REF);
	}

	ostream&
	dump_state(ostream&, EXPR_PARAM) const;

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

} __ATTRIBUTE_PACKED__ ;	// end struct ExprState
// packed because state is space-critical

#undef	EXPR_PARAM
#undef	EXPR_REF
#undef	EXPR_MEM

//=============================================================================
/**
	There should be one of these per Expr.  
	Access to these is not performance critical, 
	which is why we keep it separate.  
	There is not stateful information here, just structural.  
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
	typedef	expr_count_type		count_type;
	/**
		The second value can be interpreted as an index
		to an expression or bool node.  
		NOTE: as a bool node, the value is 0-based.
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
		BEWARE of 2^k limit approached for high fanin!
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
	push_back_expr(const expr_index_type);

	void
	push_back_node(const node_index_type);

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

	void
	wipe(void) {
		children.clear();
	}

	bool
	wiped(void) const {
		return children.empty();
	}

	static
	size_t
	add_children_size(const size_t s, const ExprGraphNode& g) {
		return s +g.children.size();
	}
};	// end struct ExprGraphNode

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EXPR_H__

