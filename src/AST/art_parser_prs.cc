/**
	\file "AST/art_parser_prs.cc"
	PRS-related syntax class method definitions.
	$Id: art_parser_prs.cc,v 1.14.2.1 2005/05/14 22:38:36 fang Exp $
 */

#ifndef	__AST_ART_PARSER_PRS_CC__
#define	__AST_ART_PARSER_PRS_CC__

#include <iostream>

#include "AST/art_parser_prs.h"
#include "AST/art_parser_expr.h"		// for id_expr
#include "AST/art_parser_range.h"
#include "AST/art_parser_token.h"
#include "AST/art_parser_token_char.h"
#include "AST/art_parser_token_string.h"
#include "AST/art_parser_node_list.tcc"

#include "Object/art_object_expr_base.h"

#include "util/what.h"

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
#include "util/using_ostream.h"

//=============================================================================
// class body_item method definitions

body_item::body_item() { }

body_item::~body_item() { }

//=============================================================================
// class rule method definitions

CONSTRUCTOR_INLINE
rule::rule(const expr* g, const terminal* a,
		const expr* rhs, const terminal* d) :
		body_item(), guard(g), arrow(a),
		r(rhs), dir(d) {
	NEVER_NULL(guard); NEVER_NULL(arrow); NEVER_NULL(r); NEVER_NULL(dir);
	INVARIANT(r.is_a<const id_expr>() || r.is_a<const index_expr>());
//	INVARIANT(r.is_a<const id_expr>() || r.is_a<const postfix_expr>());
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

#if 0
never_ptr<const object>
rule::check_build(context& c) const {
	cerr << "Fang, finish PRS::rule::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}
#endif

//=============================================================================
// class loop method definitions

loop::loop(const char_punctuation_type* l,
		const token_identifier* id, const range* b,
		const rule_list* rl, const char_punctuation_type* r) :
		body_item(), 
		lp(l), index(id), bounds(b), 
		rules(rl), rp(r) {
	NEVER_NULL(index); NEVER_NULL(bounds); NEVER_NULL(rules);
}

loop::~loop() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(loop)

line_position
loop::leftmost(void) const {
	if (lp)		return lp->leftmost();
	else		return index->leftmost();
}

line_position
loop::rightmost(void) const {
	if (rp)		return rp->rightmost();
	else		return rules->rightmost();
}

#if 0
never_ptr<const object>
loop::check_build(context& c) const {
	cerr << "Fang, finish PRS::loop::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}
#endif

//=============================================================================
// class body method definitions

CONSTRUCTOR_INLINE
body::body(const generic_keyword_type* t, const rule_list* r) :
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
		const token_identifier* id, 
		const range* b, 
		const expr* e, const char_punctuation_type* r) :
		expr(), 
		lp(l), op(o), index(id), bounds(b), ex(e), rp(r) {
	NEVER_NULL(op); NEVER_NULL(index); NEVER_NULL(bounds); NEVER_NULL(ex);
}

op_loop::~op_loop() {
}

ostream&
op_loop::what(ostream& o) const {
	o << '(' << util::what<op_loop>::name() << ' ';
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

#if 0
/** temporary: FINISH ME */
never_ptr<const object>
op_loop::check_build(context& c) const {
	cerr << "Fang, finish op_loop::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}
#endif

/** temporary: FINISH ME */
expr::return_type
op_loop::check_expr(context& c) const {
	cerr << "Fang, finish op_loop::check_expr()!" << endl;
	return expr::return_type(NULL);
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

// template class node_list<const body_item>;		// PRS::rule_list

#if 1
// This is temporary, until node_list::check_build is overhauled.  
template
node_list<const body_item>::node_list(const PRS::body_item*);

template
ostream&
node_list<const body_item>::what(ostream&) const;

template
line_position
node_list<const body_item>::leftmost(void) const;
#endif

//=============================================================================
}	// end namespace PRS
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __AST_ART_PARSER_PRS_CC__

