// "art_parser_prs.cc"

#include "art_parser_template_methods.h"
#include "art_parser_prs.h"

#define	CONSTRUCTOR_INLINE
#define	DESTRUCTOR_INLINE

namespace ART {
namespace parser {
namespace PRS {
//=============================================================================
// class body_item method definitions

body_item::body_item() : node() {
}

body_item::~body_item() {
}

//=============================================================================
// class rule method definitions

CONSTRUCTOR_INLINE
rule::rule(const expr* g, const terminal* a,
		const expr* rhs, const terminal* d) :
		body_item(),
		guard(g),
		arrow(a),
		r(rhs),
		dir(d) {
	assert(guard); assert(arrow); assert(r); assert(dir);
	assert(r.is_a<id_expr>() || r.is_a<postfix_expr>());
//	assert(IS_A(id_expr*, r) || IS_A(postfix_expr*, r));
}

DESTRUCTOR_INLINE
rule::~rule() {
}

ostream&
rule::what(ostream& o) const {
	return o << "(prs-rule)";
}

line_position
rule::leftmost(void) const {
	return guard->leftmost();
}

line_position
rule::rightmost(void) const {
	return dir->rightmost();
}

//=============================================================================
// class loop method definitions

loop::loop(const token_char* l, const token_char* c1,
		const token_identifier* id, const token_char* c2, 
		const range* b, const token_char* c3, 
		const rule_list* rl, const token_char* r) :
		body_item(), 
		lp(l), col1(c1), index(id), col2(c2), bounds(b), 
		col3(c3), rules(rl), rp(r) {
	assert(index); assert(bounds); assert(rules);
}

loop::~loop() {
}

ostream&
loop::what(ostream& o) const {
	return o << "(prs-loop)";
}

line_position
loop::leftmost(void) const {
	if (lp)		return lp->leftmost();
	else if (col1)	return col1->leftmost();
	else		return index->leftmost();
}

line_position
loop::rightmost(void) const {
	if (rp)		return rp->rightmost();
	else		return rules->rightmost();
}

//=============================================================================
// class body method definitions

CONSTRUCTOR_INLINE
body::body(const token_keyword* t, const rule_list* r) :
		language_body(t), rules(r) {
	if (r) assert(rules);
}

DESTRUCTOR_INLINE
body::~body() {
}

ostream&
body::what(ostream& o) const { return o << "(prs-body)"; }

line_position
body::leftmost(void) const {
	return language_body::leftmost();
}

line_position
body::rightmost(void) const {
	return rules->rightmost();
}

//=============================================================================
// class op_loop method definitions

op_loop::op_loop(const token_char* l, const token_char* o, const token_char* c1,
		const token_identifier* id, const token_char* c2, 
		const range* b, const token_char* c3, 
		const expr* e, const token_char* r) :
		expr(), 
		lp(l), op(o), col1(c1), index(id), col2(c2), bounds(b), 
		col3(c3), ex(e), rp(r) {
	assert(op); assert(index); assert(bounds); assert(ex);
}

op_loop::~op_loop() {
}

ostream&
op_loop::what(ostream& o) const {
	o << "(op-loop ";
	return op->what(o) << ")";
}

line_position
op_loop::leftmost(void) const {
	if (lp)		return lp->leftmost();
	else		return op->leftmost();
}

line_position
op_loop::rightmost(void) const {
	if (rp)		return rp->rightmost();
	else		return ex->rightmost();
}

/** temporary: FINISH ME */
never_const_ptr<object>
op_loop::check_build(never_ptr<context> c) const {
	return node::check_build(c);
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

template class node_list<body_item>;			// PRS::rule_list

//=============================================================================
};	// end namespace PRS
};	// end namespace parser
};	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

