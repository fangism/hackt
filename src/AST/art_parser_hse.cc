/**
	\file "art_parser_hse.cc"
	Class method definitions for HSE-related syntax tree.  
	$Id: art_parser_hse.cc,v 1.10.4.1 2005/05/03 03:35:15 fang Exp $
 */

#ifndef	__ART_PARSER_HSE_CC__
#define	__ART_PARSER_HSE_CC__

#include <iostream>

#include "art_parser_hse.h"
#include "art_parser_token.h"
#include "art_parser_token_char.h"
#include "art_parser_node_list.tcc"

#include "what.h"

#define	CONSTRUCTOR_INLINE
#define	DESTRUCTOR_INLINE

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(ART::parser::HSE::statement, "(hse-statement)")
SPECIALIZE_UTIL_WHAT(ART::parser::HSE::body, "(hse-body)")
SPECIALIZE_UTIL_WHAT(ART::parser::HSE::guarded_command, "(hse-guarded-cmd)")
SPECIALIZE_UTIL_WHAT(ART::parser::HSE::else_clause, "(hse-else-clause)")
SPECIALIZE_UTIL_WHAT(ART::parser::HSE::skip, "(hse-skip)")
SPECIALIZE_UTIL_WHAT(ART::parser::HSE::wait, "(hse-wait)")
SPECIALIZE_UTIL_WHAT(ART::parser::HSE::assignment, "(hse-assignment)")
SPECIALIZE_UTIL_WHAT(ART::parser::HSE::det_selection, "(hse-det-sel)")
SPECIALIZE_UTIL_WHAT(ART::parser::HSE::nondet_selection, "(hse-nondet-sel)")
// SPECIALIZE_UTIL_WHAT(ART::parser::HSE::prob_selection, "(hse-prob-sel)")
SPECIALIZE_UTIL_WHAT(ART::parser::HSE::loop, "(hse-loop)")
SPECIALIZE_UTIL_WHAT(ART::parser::HSE::do_until, "(hse-do-until)")
}


namespace ART {
namespace parser {
namespace HSE {
#include "using_ostream.h"

//=============================================================================
// class statement method definitions

CONSTRUCTOR_INLINE
statement::statement() : node() { }

DESTRUCTOR_INLINE
statement::~statement() { }

//=============================================================================
// class body method definitions

CONSTRUCTOR_INLINE
body::body(const generic_keyword_type* t, const stmt_list* s) :
		language_body(t), stmts(s) {
	if(s) NEVER_NULL(stmts);
}

DESTRUCTOR_INLINE
body::~body() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(body)

line_position 
body::rightmost(void) const {
	return stmts->rightmost();
}

never_ptr<const object>
body::check_build(context& c) const {
	cerr << "Fang, finish HSE::body::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class guarded_command method definitions

CONSTRUCTOR_INLINE
guarded_command::guarded_command(const hse_expr* g, const terminal* a, 
		const stmt_list* c) : 
		node(), guard(g),
		// remember, may be keyword: else   
		arrow(a), command(c) {
	NEVER_NULL(guard);
	NEVER_NULL(arrow);
	if (c) NEVER_NULL(command);
}

DESTRUCTOR_INLINE
guarded_command::~guarded_command() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(guarded_command)

line_position
guarded_command::leftmost(void) const {
	return guard->leftmost();
}

line_position
guarded_command::rightmost(void) const {
	return command->rightmost();
}

never_ptr<const object>
guarded_command::check_build(context& c) const {
	cerr << "Fang, finish HSE::guarded_command::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class else_clause method definitions

CONSTRUCTOR_INLINE
else_clause::else_clause(const token_else* g, const terminal* a, 
		const stmt_list* c) :
		guarded_command(g,a,c) {
	// check for keyword else, right-arrow terminal
}

DESTRUCTOR_INLINE
else_clause::~else_clause() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(else_clause)

//=============================================================================
// class skip method definitions

CONSTRUCTOR_INLINE
skip::skip(const generic_keyword_type* s) : statement(), kw(s) {
	NEVER_NULL(kw);
}

DESTRUCTOR_INLINE
skip::~skip() { }

// check that nothing appears after skip statement

PARSER_WHAT_DEFAULT_IMPLEMENTATION(skip)

line_position
skip::leftmost(void) const {
	return kw->leftmost();
}

line_position
skip::rightmost(void) const {
	return kw->rightmost();
}

never_ptr<const object>
skip::check_build(context& c) const {
	cerr << "Fang, finish HSE::skip::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class wait method definitions

CONSTRUCTOR_INLINE
wait::wait(const expr* c) :
		statement(), cond(c) {
	NEVER_NULL(cond);
}

DESTRUCTOR_INLINE
wait::~wait() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(wait)

line_position
wait::leftmost(void) const {
	return cond->leftmost();
}

line_position
wait::rightmost(void) const {
	return cond->rightmost();
}

never_ptr<const object>
wait::check_build(context& c) const {
	cerr << "Fang, finish HSE::wait::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class assignment method definitions

CONSTRUCTOR_INLINE
assignment::assignment(base_assign* a) : ART::parser::HSE::statement(),
		// destructive transfer of ownership
		parser::incdec_stmt(a->release_expr(), a->release_op()) {
	excl_ptr<base_assign> delete_me(a);
}

DESTRUCTOR_INLINE
assignment::~assignment() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(assignment)

line_position
assignment::leftmost(void) const {
	return incdec_stmt::leftmost();
}

line_position
assignment::rightmost(void) const {
	return incdec_stmt::rightmost();
}

never_ptr<const object>
assignment::check_build(context& c) const {
	cerr << "Fang, finish HSE::assignment::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// abstract class selection method definitions

CONSTRUCTOR_INLINE
selection::selection() : statement() { }

DESTRUCTOR_INLINE
selection::~selection() { }

//=============================================================================
// class det_selection method definitions

CONSTRUCTOR_INLINE
det_selection::det_selection(const guarded_command* n) :
		selection(), parent_type(n) {
}

DESTRUCTOR_INLINE
det_selection::~det_selection() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(det_selection)

line_position
det_selection::leftmost(void) const {
	return parent_type::leftmost();
}

line_position
det_selection::rightmost(void) const {
	return parent_type::rightmost();
}

never_ptr<const object>
det_selection::check_build(context& c) const {
	cerr << "Fang, finish HSE::det_selection::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class nondet_selection method definitions

CONSTRUCTOR_INLINE
nondet_selection::nondet_selection(const guarded_command* n) :
		selection(), parent_type(n) {
}

DESTRUCTOR_INLINE
nondet_selection::~nondet_selection() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(nondet_selection)

line_position
nondet_selection::leftmost(void) const {
	return parent_type::leftmost();
}

line_position
nondet_selection::rightmost(void) const {
	return parent_type::rightmost();
}

never_ptr<const object>
nondet_selection::check_build(context& c) const {
	cerr << "Fang, finish HSE::nondet_selection::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
/*** not available... yet ***
// class prob_selection method definitions

CONSTRUCTOR_INLINE
prob_selection::prob_selection(const guarded_command* n) : selection(),
		node_list<guarded_command,thickbar>(n) {
}

DESTRUCTOR_INLINE
prob_selection::~prob_selection() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(prob_selection)

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
loop::loop(const stmt_list* n) : statement(), commands(n) {
}

DESTRUCTOR_INLINE
loop::~loop() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(loop)

line_position
loop::leftmost(void) const {
	return commands->leftmost();
}

line_position
loop::rightmost(void) const {
	return commands->rightmost();
}

never_ptr<const object>
loop::check_build(context& c) const {
	cerr << "Fang, finish HSE::loop::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class do_until method definitions

CONSTRUCTOR_INLINE
do_until::do_until(const det_selection* n) : statement(),
		sel(n) { }

DESTRUCTOR_INLINE
do_until::~do_until() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(do_until)

line_position
do_until::leftmost(void) const {
	return sel->leftmost();
}

line_position
do_until::rightmost(void) const {
	return sel->rightmost();
}

never_ptr<const object>
do_until::check_build(context& c) const {
	cerr << "Fang, finish HSE::do_until::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

#if 1
template class node_list<const statement>;	// HSE::stmt_list
template class node_list<const guarded_command>;	// HSE::det_sel_base
							// HSE::prob_sel_base
							// HSE::nondet_sel_base
	// distinguish types, specialize delimiter (node_list_traits)
#else
template class node_list<const statement,semicolon>;	// HSE::stmt_list
template class node_list<const guarded_command,thickbar>;	// HSE::det_sel_base
							// HSE::prob_sel_base
template class node_list<const guarded_command,colon>;	// HSE::nondet_sel_base
#endif

//=============================================================================
}	// end namespace HSE
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __ART_PARSER_HSE_CC__

