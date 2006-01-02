/**
	\file "sim/prsim/Expr.cc"
	Expression node implementation.  
	$Id: Expr.cc,v 1.1.2.4 2006/01/02 23:13:35 fang Exp $
 */

#include <iostream>
#include "sim/prsim/Expr.h"
#include "util/macros.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
//=============================================================================
// class Expr method definitions

/**
	0 is an invalid size, ID number for nodes/exprs.  
 */
Expr::Expr() : parent(0), type(0), size(0) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes type and child-count without initializing parent index.  
 */
Expr::Expr(const unsigned char t, const count_type s) :
	parent(0), type(t), size(s) {
	INVARIANT(size);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Expr::Expr(const expr_index_type p, const unsigned char t, 
		const count_type s) : parent(p), type(t), size(s) {
	INVARIANT(parent);
	INVARIANT(size);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Expr::~Expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
Expr::initialize(void) {
	unknowns = size;
	countdown = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints structural information of the Expr.  
 */
ostream&
Expr::dump_struct(ostream& o) const {
	o << "type: ";
	unsigned char t = type & EXPR_MASK;
	switch (t) {
	case EXPR_OR: o << "or("; break;
	case EXPR_AND: o << "and("; break;
	case EXPR_NAND: o << "nand("; break;
	case EXPR_NOR: o << "nor("; break;
	default: THROW_EXIT;
	}
	o << size << ')';
	if (type & EXPR_ROOT) {
		o << (type & EXPR_DIR) ? " (pull-up: " : " (pull-dn: ";
		o << parent << ')';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
Expr::dump_state(ostream& o) const {
	o << "ctdn: " << countdown << " X: " << unknowns << "(/" << size << ')';
}

//=============================================================================
// class ExprGraphNode method definitions

void
ExprGraphNode::push_back_expr(const expr_index_type i) {
	children.resize(children.size()+1);
	const size_t last = children.size()-1;
	children[last].first = false;
	children[last].second = i;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprGraphNode::push_back_node(const node_index_type i) {
	children.resize(children.size()+1);
	const size_t last = children.size()-1;
	children[last].first = true;
	children[last].second = i;
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
	return o;
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

