/**
	\file "art_parser_hse.cc"
	Class method definitions for HSE-related syntax tree.  
	$Id: art_parser_hse.cc,v 1.4 2004/11/30 01:25:02 fang Exp $
 */

#include "art_parser.tcc"
#include "art_parser_hse.h"
#include "art_parser_token.h"
#include "art_parser_expr.h"

#define	CONSTRUCTOR_INLINE
#define	DESTRUCTOR_INLINE

namespace ART {
namespace parser {
namespace HSE {

//=============================================================================
// class statement method definitions

CONSTRUCTOR_INLINE
statement::statement() : node() { }

DESTRUCTOR_INLINE
statement::~statement() { }

ostream&
statement::what(ostream& o) const {
	return o << "(hse-statement)";
}

//=============================================================================
// class body method definitions

CONSTRUCTOR_INLINE
body::body(token_keyword* t, stmt_list* s) : language_body(t),
		stmts(s) {
	if(s) assert(stmts);
}

DESTRUCTOR_INLINE
body::~body() {
	SAFEDELETE(stmts);
}

ostream&
body::what(ostream& o) const {
	return o << "(hse-body)";
}

line_position 
body::rightmost(void) const {
	return stmts->rightmost();
}

//=============================================================================
// class guarded_command method definitions

CONSTRUCTOR_INLINE
guarded_command::guarded_command(hse_expr* g, terminal* a, stmt_list* c) : 
		node(),
		guard(g),
		// remember, may be keyword: else   
		arrow(a), command(c) {
	assert(guard);
	assert(arrow);
	if (c) assert(command);
}

DESTRUCTOR_INLINE
guarded_command::~guarded_command() {
	SAFEDELETE(guard); SAFEDELETE(arrow); SAFEDELETE(command);
}

ostream&
guarded_command::what(ostream& o) const {
	return o << "(hse-guarded-cmd)";
}

line_position
guarded_command::leftmost(void) const {
	return guard->leftmost();
}

line_position
guarded_command::rightmost(void) const {
	return command->rightmost();
}

//=============================================================================
// class else_clause method definitions

CONSTRUCTOR_INLINE
else_clause::else_clause(token_else* g, terminal* a, stmt_list* c) :
		guarded_command(g,a,c) {
	// check for keyword else, right-arrow terminal
}

DESTRUCTOR_INLINE
else_clause::~else_clause() { }

ostream&
else_clause::what(ostream& o) const {
	return o << "(hse-else-clause)";
}

//=============================================================================
// class skip method definitions

CONSTRUCTOR_INLINE
skip::skip(token_keyword* s) : statement(),
		token_keyword(IS_A(token_keyword*, s)->c_str()) {
	SAFEDELETE(s);
}

DESTRUCTOR_INLINE
skip::~skip() { }

// check that nothing appears after skip statement

ostream&
skip::what(ostream& o) const {
	return o << "(hse-skip)";
}

line_position
skip::leftmost(void) const {
	return token_keyword::leftmost();
}

line_position
skip::rightmost(void) const {
	return token_keyword::rightmost();
}

//=============================================================================
// class wait method definitions

CONSTRUCTOR_INLINE
wait::wait(terminal* l, expr* c, terminal* r) :
		statement(), lb(l), cond(c), rb(r) {
	assert(cond); assert(lb); assert(rb);
}

DESTRUCTOR_INLINE
wait::~wait() {
	SAFEDELETE(lb); SAFEDELETE(cond); SAFEDELETE(rb);
}

ostream&
wait::what(ostream& o) const {
	return o << "(hse-wait)";
}

line_position
wait::leftmost(void) const {
	return lb->leftmost();
}

line_position
wait::rightmost(void) const {
	return rb->rightmost();
}

//=============================================================================
// class assignment method definitions

CONSTRUCTOR_INLINE
assignment::assignment(base_assign* a) : ART::parser::HSE::statement(),
		// destructive transfer of ownership
		parser::incdec_stmt(a->release_expr(), a->release_op()) {
	SAFEDELETE(a);
}

DESTRUCTOR_INLINE
assignment::~assignment() { }

ostream&
assignment::what(ostream& o) const {
	return o << "(hse-assignment)";
}

line_position
assignment::leftmost(void) const {
	return incdec_stmt::leftmost();
}

line_position
assignment::rightmost(void) const {
	return incdec_stmt::rightmost();
}

//=============================================================================
// abstract class selection method definitions

CONSTRUCTOR_INLINE
selection::selection() : statement() { }

DESTRUCTOR_INLINE
selection::~selection() { }

ostream&
selection::what(ostream& o) const {
	return o << "(hse-selection)";
}

//=============================================================================
// class det_selection method definitions

CONSTRUCTOR_INLINE
det_selection::det_selection(guarded_command* n) :
		selection(), det_sel_base(n) {
}

DESTRUCTOR_INLINE
det_selection::~det_selection() { }

ostream&
det_selection::what(ostream& o) const {
	return o << "(hse-det-sel)";
}

line_position
det_selection::leftmost(void) const {
	return det_sel_base::leftmost();
}

line_position
det_selection::rightmost(void) const {
	return det_sel_base::rightmost();
}

//=============================================================================
// class nondet_selection method definitions

CONSTRUCTOR_INLINE
nondet_selection::nondet_selection(guarded_command* n) :
		selection(), nondet_sel_base(n) {
}

DESTRUCTOR_INLINE
nondet_selection::~nondet_selection() { }

ostream&
nondet_selection::what(ostream& o) const {
	return o << "(hse-nondet-sel)";
}

line_position
nondet_selection::leftmost(void) const {
	return nondet_sel_base::leftmost();
}

line_position
nondet_selection::rightmost(void) const {
	return nondet_sel_base::rightmost();
}

//=============================================================================
/*** not available... yet ***
// class prob_selection method definitions

CONSTRUCTOR_INLINE
prob_selection::prob_selection(guarded_command* n) : selection(),
		node_list<guarded_command,thickbar>(n) {
}

DESTRUCTOR_INLINE
prob_selection::~prob_selection() { }

ostream&
prob_selection::what(ostream& o) const {
	return o << "(hse-prob-sel)";
}

line_position
prob_selection::leftmost(void) const {
	return prob_sel_base::leftmost();
}

line_position
prob_selection::rightmost(void) const {
	return prob_sel_base::rightmost();
}
*** not available ***/

//=============================================================================
// class loop method definitions

CONSTRUCTOR_INLINE
loop::loop(stmt_list* n) : statement(), commands(n) {
}

DESTRUCTOR_INLINE
loop::~loop() {
	SAFEDELETE(commands);
}

ostream&
loop::what(ostream& o) const {
	return o << "(hse-loop)";
}

line_position
loop::leftmost(void) const {
	return commands->leftmost();
}

line_position
loop::rightmost(void) const {
	return commands->rightmost();
}

//=============================================================================
// class do_until method definitions

CONSTRUCTOR_INLINE
do_until::do_until(det_selection* n) : statement(),
		sel(n) { }

DESTRUCTOR_INLINE
do_until::~do_until() {
	SAFEDELETE(sel);
}

ostream&
do_until::what(ostream& o) const {
	return o << "(hse-do-until)";
}

line_position
do_until::leftmost(void) const {
	return sel->leftmost();
}

line_position
do_until::rightmost(void) const {
	return sel->rightmost();
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

template class node_list<const statement,semicolon>;	// HSE::stmt_list
template class node_list<const guarded_command,thickbar>;	// HSE::det_sel_base
							// HSE::prob_sel_base
template class node_list<const guarded_command,colon>;	// HSE::nondet_sel_base

//=============================================================================
};	// end namespace HSE
};	// end namespace parser
};	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

