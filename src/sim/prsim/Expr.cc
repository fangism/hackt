/**
	\file "sim/prsim/Expr.cc"
	Expression node implementation.  
	$Id: Expr.cc,v 1.5.82.2 2008/10/15 06:09:40 fang Exp $
 */

#include <iostream>
#include <algorithm>
#include "sim/prsim/Expr.h"
#include "Object/lang/PRS_enum.h"
#include "util/macros.h"
#include "util/IO_utils.tcc"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
using util::read_value;
using util::write_value;

//=============================================================================
// class Expr method definitions

/**
	0 is an invalid size, ID number for nodes/exprs.  
 */
Expr::Expr() : parent(INVALID_EXPR_INDEX), type(0), size(0) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes type and child-count without initializing parent index.  
 */
Expr::Expr(const uchar t, const count_type s) :
	parent(INVALID_EXPR_INDEX), type(t), size(s) {
	INVARIANT(size);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Expr::Expr(const expr_index_type p, const uchar t, 
		const count_type s) : parent(p), type(t), size(s) {
	INVARIANT(parent);
	INVARIANT(size);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Expr::~Expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uchar
Expr::to_prs_enum(void) const {
	if (is_not())
		return entity::PRS::PRS_NOT_EXPR_TYPE_ENUM;
	else if (is_or())
		return entity::PRS::PRS_OR_EXPR_TYPE_ENUM;
	else	return entity::PRS::PRS_AND_EXPR_TYPE_ENUM;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Determine whether or not to parenthesize this subexpression
	during pretty-printing.  
	\param ptype is the parent expression type enumeration.
	\param proot
 */
bool
Expr::parenthesize(const uchar ptype, const bool proot) const {
	const bool neg = type & EXPR_NOT;
	const bool mismatch = (type ^ ptype) & EXPR_MASK;
	return (size > 1 && (neg || (mismatch && !proot)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints structural information of the Expr.  
 */
ostream&
Expr::dump_struct(ostream& o) const {
	o << "type: ";
	const uchar t = type & EXPR_MASK;
	switch (t) {
	case EXPR_OR: o << "or<"; break;
	case EXPR_AND: o << "and<"; break;
	case EXPR_NAND: o << "nand<"; break;
	case EXPR_NOR: o << "nor<"; break;
	default: THROW_EXIT;
	}
	o << size_t(size) << '>';
	if (type & EXPR_ROOT) {
		o << ((type & EXPR_DIR) ? " (pull-up: " : " (pull-dn: ");
#if PRSIM_INDIRECT_EXPRESSION_MAP
		// parent local node index is 0-indexed
#else
		// parent node index must be non-zero
		INVARIANT(is_valid_node_index(parent));
#endif
		o << parent << ')';
	} else {
		o << " (parent: ";
#if PRSIM_INDIRECT_EXPRESSION_MAP
		o << parent;		// local expressions are 0-indexed
#else
		if (is_valid_expr_index(parent))
			o << parent;
		else	o << '-';
#endif
		o << ')';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
Expr::dump_parent_dot_edge(ostream& o) const {
	if (is_root()) {
		o << "NODE_" << parent << "\t[arrowhead=" <<
#if 1
			(direction() ? "odot" : "dot")
#else
			(direction() ? "normal" : "inv")
#endif
			<< ']';
	} else {
		o << "EXPR_" << parent;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
Expr::dump_type_dot_shape(ostream& o) const {
	const uchar t = type & EXPR_MASK;
	switch (t) {
	case EXPR_OR: o << "triangle"; break;
	case EXPR_AND: o << "trapezium"; break;
	case EXPR_NAND: o << "invtrapezium"; break;
	case EXPR_NOR: o << "invtriangle"; break;
	default: THROW_EXIT;
	}
	return o;
}

//=============================================================================
#if PRSIM_INDIRECT_EXPRESSION_MAP
#define	EXPR_PARAM		const Expr& e
#define	EXPR_REF		e
#define	EXPR_MEM(x)		(e.x)
#else
#define	EXPR_PARAM		void
#define	EXPR_REF
#define	EXPR_MEM(x)		x
#endif

/**
	Initializes expression state.  
	Assuming everything is X on startup, the initialization
	of the countdown depends on whether or not this expression
	is conjunctive or disjunctive.  
 */
void
ExprState::initialize(EXPR_PARAM) {
	unknowns = EXPR_MEM(size);
	countdown = 0;	// All X's, no 1's or 0's
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
ExprState::dump_state(ostream& o
#if PRSIM_INDIRECT_EXPRESSION_MAP
		, EXPR_PARAM
#endif
		) const {
	o << "ctdn: " << size_t(countdown) <<
		" X: " << size_t(unknowns) << "(/" <<
		size_t(EXPR_MEM(size)) << ')'
		<< " pull: " << size_t(pull_state(EXPR_REF));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Technically, this ExprState information can be reconstructed
	based on the state of all Nodes, and the event queue.  
	For now we just save and load values.  
	TODO: reconstruction algorithm.
 */
void
ExprState::save_state(ostream& o) const {
	write_value(o, countdown);
	write_value(o, unknowns);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprState::load_state(istream& i) {
	read_value(i, countdown);
	read_value(i, unknowns);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
ExprState::dump_checkpoint_state_header(ostream& o) {
	return o << "countdn\tunknowns";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
ExprState::dump_checkpoint_state(ostream& o, istream& i) {
	this_type temp;
	temp.load_state(i);
	return o << size_t(temp.countdown) << '\t' << size_t(temp.unknowns);
}

#undef	EXPR_PARAM
#undef	EXPR_REF
#undef	EXPR_MEM

//=============================================================================
// class ExprGraphNode method definitions

/**
	Predicate functor for finding node child.  
 */
struct ExprGraphNode::node_membership_checker {
	const node_index_type		node_index;

	explicit
	node_membership_checker(const node_index_type i) : node_index(i) { }

	/**
		\return true if this child entry is a node and its 
			index field (second) matches this node_index.
	 */
	bool
	operator () (const child_entry_type& c) const {
		return (c.first ? c.second == node_index : false);
	}
};	// end struct node_membership_checker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprGraphNode::push_back_expr(const expr_index_type i) {
#if 0
	children.resize(children.size()+1);
	const size_t last = children.size()-1;
	children[last].first = fase;
	children[last].second = i;
#else
	children.push_back(child_entry_type(false, i));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprGraphNode::push_back_node(const node_index_type i) {
#if 0
	children.resize(children.size()+1);
	const size_t last = children.size()-1;
	children[last].first = true;
	children[last].second = i;
#else
	children.push_back(child_entry_type(true, i));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
ExprGraphNode::contains_node_fanin(const node_index_type i) const {
	return std::find_if(begin(), end(), node_membership_checker(i))
		!= end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
ExprGraphNode::dump_struct(ostream& o) const {
	o << "children: ";
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		if (i->first)
			o << "#";	// denotes: is node
		o << i->second;
		o << ", ";
	}
	o << "offset: ";
	if (offset == INVALID_OFFSET)
		o << '-';
	else	o << size_t(offset);
	return o;
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

