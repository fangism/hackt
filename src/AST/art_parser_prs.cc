/**
	\file "art_parser_prs.cc"
	PRS-related syntax class method definitions.
	$Id: art_parser_prs.cc,v 1.8 2005/01/28 19:58:39 fang Exp $
 */

#ifndef	__ART_PARSER_PRS_CC__
#define	__ART_PARSER_PRS_CC__

#include "art_parser.tcc"
#include "art_parser_prs.h"
#include "art_parser_expr.h"
#include "art_parser_token.h"

#include "what.h"

#define	CONSTRUCTOR_INLINE
#define	DESTRUCTOR_INLINE

// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(ART::parser::PRS::rule, "(prs-rule)")
SPECIALIZE_UTIL_WHAT(ART::parser::PRS::loop, "(prs-loop)")
SPECIALIZE_UTIL_WHAT(ART::parser::PRS::body, "(prs-body)")
SPECIALIZE_UTIL_WHAT(ART::parser::PRS::op_loop, "op-loop")
}

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
		body_item(), guard(g), arrow(a),
		r(rhs), dir(d) {
	NEVER_NULL(guard); NEVER_NULL(arrow); NEVER_NULL(r); NEVER_NULL(dir);
	INVARIANT(r.is_a<const id_expr>() || r.is_a<const postfix_expr>());
//	INVARIANT(IS_A(id_expr*, r) || IS_A(postfix_expr*, r));
}

DESTRUCTOR_INLINE
rule::~rule() {
}

#if 0
ostream&
rule::what(ostream& o) const {
	return o << "(prs-rule)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(rule)
#endif

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
	NEVER_NULL(index); NEVER_NULL(bounds); NEVER_NULL(rules);
}

loop::~loop() {
}

#if 0
ostream&
loop::what(ostream& o) const {
	return o << "(prs-loop)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(loop)
#endif

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
	if (r) NEVER_NULL(rules);
}

DESTRUCTOR_INLINE
body::~body() {
}

#if 0
ostream&
body::what(ostream& o) const { return o << "(prs-body)"; }
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(body)
#endif

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
	NEVER_NULL(op); NEVER_NULL(index); NEVER_NULL(bounds); NEVER_NULL(ex);
}

op_loop::~op_loop() {
}

ostream&
op_loop::what(ostream& o) const {
#if 0
	o << "(op-loop ";
#else
	o << '(' << util::what<op_loop>::name() << ' ';
#endif
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
never_ptr<const object>
op_loop::check_build(context& c) const {
	return node::check_build(c);
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

template class node_list<const body_item>;		// PRS::rule_list

//=============================================================================
}	// end namespace PRS
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __ART_PARSER_PRS_CC__

