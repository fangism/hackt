/**
	\file "art_parser_prs.cc"
	PRS-related syntax class method definitions.
	$Id: art_parser_prs.cc,v 1.10.8.1 2005/04/09 23:09:53 fang Exp $
 */

#ifndef	__ART_PARSER_PRS_CC__
#define	__ART_PARSER_PRS_CC__

#include <iostream>

#include "art_parser_node_position.h"
#include "art_parser.tcc"
#include "art_parser_prs.h"
#include "art_parser_expr.h"		// for id_expr
#include "art_parser_range.h"
#include "art_parser_token.h"
#include "art_parser_token_char.h"
#include "art_parser_token_string.h"

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
#include "using_ostream.h"

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

PARSER_WHAT_DEFAULT_IMPLEMENTATION(rule)

line_position
rule::leftmost(void) const {
	return guard->leftmost();
}

line_position
rule::rightmost(void) const {
	return dir->rightmost();
}

never_ptr<const object>
rule::check_build(context& c) const {
	cerr << "Fang, finish PRS::rule::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class loop method definitions

loop::loop(const char_punctuation_type* l, const char_punctuation_type* c1,
		const token_identifier* id, const char_punctuation_type* c2, 
		const range* b, const char_punctuation_type* c3, 
		const rule_list* rl, const char_punctuation_type* r) :
		body_item(), 
		lp(l), col1(c1), index(id), col2(c2), bounds(b), 
		col3(c3), rules(rl), rp(r) {
	NEVER_NULL(index); NEVER_NULL(bounds); NEVER_NULL(rules);
}

loop::~loop() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(loop)

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

never_ptr<const object>
loop::check_build(context& c) const {
	cerr << "Fang, finish PRS::loop::check_build()!" << endl;
	return never_ptr<const object>(NULL);
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

PARSER_WHAT_DEFAULT_IMPLEMENTATION(body)

line_position
body::leftmost(void) const {
	return language_body::leftmost();
}

line_position
body::rightmost(void) const {
	return rules->rightmost();
}

never_ptr<const object>
body::check_build(context& c) const {
	cerr << "Fang, finish PRS::body::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class op_loop method definitions

op_loop::op_loop(const char_punctuation_type* l, const token_char* o,
		const char_punctuation_type* c1,
		const token_identifier* id, const char_punctuation_type* c2, 
		const range* b, const char_punctuation_type* c3, 
		const expr* e, const char_punctuation_type* r) :
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
	cerr << "Fang, finish op_loop::check_build()!" << endl;
	return never_ptr<const object>(NULL);
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

