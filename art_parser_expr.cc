// "art_parser_expr.cc"
// class method definitions for ART::parser, related to expr

// template instantiations are at the end of the file

#include <iostream>

#include "art_macros.h"
#include "art_parser_template_methods.h"

// will need these come time for type-checking
// #include "art_symbol_table.h"
// #include "art_object.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

namespace ART {
namespace parser {

//=============================================================================
// class expr method definitions

/// Empty constructor
CONSTRUCTOR_INLINE
expr::expr() : node() { }

/// Empty virtual destructor
DESTRUCTOR_INLINE
expr::~expr() { }

//=============================================================================
// class paren_expr method definitions

CONSTRUCTOR_INLINE
paren_expr::paren_expr(node* l, node* n, node* r) : expr(),
		lp(dynamic_cast<token_char*>(l)),
		e(dynamic_cast<expr*>(n)),
		rp(dynamic_cast<token_char*>(r)) {
	assert(lp); assert(e); assert(rp);
}

DESTRUCTOR_INLINE
paren_expr::~paren_expr() {
	SAFEDELETE(lp); SAFEDELETE(e); SAFEDELETE(rp);
}

ostream&
paren_expr::what(ostream& o) const {
	return o << "(paren-expr)";
}

line_position
paren_expr::leftmost(void) const {
	return lp->leftmost();
}

line_position
paren_expr::rightmost(void) const {
	return rp->rightmost();
}


//=============================================================================
// class id_expr method definitions

CONSTRUCTOR_INLINE
id_expr::id_expr(node* n) : expr(), id_expr_base(n) { }

/// copy constructor, no transfer of ownership
CONSTRUCTOR_INLINE
id_expr::id_expr(const id_expr& i) : id_expr_base(i) { }

DESTRUCTOR_INLINE
id_expr::~id_expr() { }

ostream&
id_expr::what(ostream& o) const {
	return o << "(id-expr)";
}

line_position
id_expr::leftmost(void) const {
	return id_expr_base::leftmost();
}

line_position
id_expr::rightmost(void) const {
	return id_expr_base::rightmost();
}

// friend operator (not a method)
ostream& operator << (ostream& o, const id_expr& id) {
	id_expr::const_iterator i = id.begin();
	token_identifier* tid = dynamic_cast<token_identifier*>(*i);
	assert(tid);
	o << *tid;
	for (i++ ; i!=id.end(); i++) {
		i++;		// skip scope operator token
		tid = dynamic_cast<token_identifier*>(*i);
		assert(tid);
		o << scope << *tid;
	}
	return o;
}

//=============================================================================
// class range method definitions

CONSTRUCTOR_INLINE
range::range(node* l) : expr(), 
		lower(dynamic_cast<expr*>(l)), op(NULL), upper(NULL) {
	assert(lower); 
}

CONSTRUCTOR_INLINE
range::range(node* l, node* o, node* u) : expr(),
		lower(dynamic_cast<expr*>(l)),
		op(dynamic_cast<terminal*>(o)),
		upper(dynamic_cast<expr*>(u)) {
	assert(lower); assert(op); assert(u);
}

DESTRUCTOR_INLINE
range::~range() {
	SAFEDELETE(lower); SAFEDELETE(op); SAFEDELETE(upper);
}

ostream&
range::what(ostream& o) const {
	return o << "(range)";
}

line_position
range::leftmost(void) const {
	return lower->leftmost();
}

line_position
range::rightmost(void) const {
        if (upper)      return upper->rightmost();
        else if (op)    return op->rightmost();
        else            return lower->rightmost();
}

//=============================================================================
// class unary_expr method definitions

/**
	Failure to dynamic_cast will result in assignment to a NULL pointer,
	which will be detected, and properly memory managed, assuming
	that the arguments exclusively "owned" their memory locations.
 */
CONSTRUCTOR_INLINE
unary_expr::unary_expr(node* n, node* o) : expr(), nonterminal(),
	e(dynamic_cast<expr*>(n)),
	op(dynamic_cast<terminal*>(o)) {
		if (n && !e) delete n;  // or use assert?
		if (o && !op) delete o;
	}

DESTRUCTOR_INLINE
unary_expr::~unary_expr() {
	SAFEDELETE(e); SAFEDELETE(op);
}

//=============================================================================
// class prefix_expr method definitions

CONSTRUCTOR_INLINE
prefix_expr::prefix_expr(node* op, node* n) : unary_expr(n,op) { }

DESTRUCTOR_INLINE
prefix_expr::~prefix_expr() { }

ostream&
prefix_expr::what(ostream& o) const {
	return o << "(prefix-expr)";
}

line_position
prefix_expr::leftmost(void) const {
	return op->leftmost();
}

line_position
prefix_expr::rightmost(void) const {
	return e->rightmost();
}

//=============================================================================
// class postfix_expr method definitions

CONSTRUCTOR_INLINE
postfix_expr::postfix_expr(node* n, node* op) : unary_expr(n,op) { }

DESTRUCTOR_INLINE
postfix_expr::~postfix_expr() { }

line_position
postfix_expr::leftmost(void) const {
	return e->leftmost();
}

line_position
postfix_expr::rightmost(void) const {
	return op->rightmost();
}

//=============================================================================
// class member_expr method definitions

CONSTRUCTOR_INLINE
member_expr::member_expr(node* l, node* op, node* m) :
		postfix_expr(l,op), member(dynamic_cast<expr*>(m)) {
	assert(member);
}

DESTRUCTOR_INLINE
member_expr::~member_expr() { SAFEDELETE(member); }

ostream&
member_expr::what(ostream& o) const {
	return o << "(member-expr)";
}

line_position
member_expr::rightmost(void) const {
	return member->rightmost();
}

//=============================================================================
// class index_expr method definitions

CONSTRUCTOR_INLINE
index_expr::index_expr(node* l, node* i) : postfix_expr(l, NULL),
	ranges(dynamic_cast<range_list*>(i)) { }

DESTRUCTOR_INLINE
index_expr::~index_expr() { SAFEDELETE(ranges); }

ostream&
index_expr::what(ostream& o) const {
	return o << "(index-expr)";
}

line_position
index_expr::rightmost(void) const {
	return ranges->rightmost();
}

//=============================================================================
// class binary_expr method definitions

CONSTRUCTOR_INLINE
binary_expr::binary_expr(node* left, node* o, node* right) : expr(),
		l(dynamic_cast<expr*>(left)),
		op(dynamic_cast<terminal*>(o)),
		r(dynamic_cast<expr*>(right)) {
	assert(l); assert(op); assert(r);
}

DESTRUCTOR_INLINE
binary_expr::~binary_expr() {
	SAFEDELETE(l); SAFEDELETE(op); SAFEDELETE(r);
}

line_position
binary_expr::leftmost(void) const {
	return l->leftmost();
}

line_position
binary_expr::rightmost(void) const {
	return r->rightmost();
}

//=============================================================================
// class arith_expr method definitions

CONSTRUCTOR_INLINE
arith_expr::arith_expr(node* left, node* o, node* right) :
	binary_expr(left, o, right) {
}

DESTRUCTOR_INLINE
arith_expr::~arith_expr() { }

ostream&
arith_expr::what(ostream& o) const {
	return o << "(arith-expr)";
}

//=============================================================================
// class relational_expr method definitions

CONSTRUCTOR_INLINE
relational_expr::relational_expr(node* left, node* o, node* right) :
	binary_expr(left, o, right) {
}

DESTRUCTOR_INLINE
relational_expr::~relational_expr() { }

ostream&
relational_expr::what(ostream& o) const {
	return o << "(relational-expr)";
}

//=============================================================================
// class logical_expr method definitions

CONSTRUCTOR_INLINE
logical_expr::logical_expr(node* left, node* o, node* right) :
	binary_expr(left, o, right) {
}

DESTRUCTOR_INLINE
logical_expr::~logical_expr() { }

ostream&
logical_expr::what(ostream& o) const {
	return o << "(logical-expr)";
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes
							// also known as...
template class node_list<expr,comma>;			// expr_list
template class node_list<token_identifier,scope>;	// id_expr_base
template class node_list<range,comma>;			// range_list


//=============================================================================
};
};


#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE


