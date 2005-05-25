/**
	\file "AST/art_parser_chp.cc"
	Class method definitions for CHP parser classes.
	$Id: art_parser_chp.cc,v 1.14.2.1 2005/05/25 00:41:45 fang Exp $
 */

#ifndef	__AST_ART_PARSER_CHP_CC__
#define	__AST_ART_PARSER_CHP_CC__

#include <iostream>

#include "AST/art_parser_chp.h"
#include "AST/art_parser_expr_list.h"
#include "AST/art_parser_token.h"
#include "AST/art_parser_node_list.tcc"
#include "Object/art_object_CHP.h"

#include "util/what.h"
#include "util/memory/count_ptr.tcc"

#define	CONSTRUCTOR_INLINE
#define	DESTRUCTOR_INLINE

// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::body, "(chp-body)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::guarded_command, "(chp-guarded-cmd)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::else_clause, "(chp-else-clause)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::skip, "(chp-skip)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::wait, "(chp-wait)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::assignment, "(chp-assignment)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::incdec_stmt, "(chp-assignment)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::comm_list, "(chp-comm-list)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::send, "(chp-send)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::receive, "(chp-receive)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::det_selection, "(chp-det-sel)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::nondet_selection, "(chp-nondet-sel)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::prob_selection, "(chp-prob-sel)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::loop, "(chp-loop)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::do_until, "(chp-do-until)")
SPECIALIZE_UTIL_WHAT(ART::parser::CHP::log, "(chp-log)")
}

namespace ART {
namespace parser {
namespace CHP {
#include "util/using_ostream.h"

//=============================================================================
// class statement method definitions

CONSTRUCTOR_INLINE
statement::statement() { }

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
	cerr << "Fang, finish CHP::body::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class guarded_command method definitions

CONSTRUCTOR_INLINE
guarded_command::guarded_command(const chp_expr* g, 
		const string_punctuation_type* a, const stmt_list* c) : 
		guard(g),
		// remember, may be keyword: else   
		arrow(a), command(c) {
	NEVER_NULL(guard);
	NEVER_NULL(arrow);
	if (c) NEVER_NULL(command);
}

DESTRUCTOR_INLINE
guarded_command::~guarded_command() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(guarded_command)

line_position
guarded_command::leftmost(void) const {
	return guard->leftmost();
}

line_position
guarded_command::rightmost(void) const {
	NEVER_NULL(command);
	return command->rightmost();
}

guarded_command::return_type
guarded_command::check_guarded_action(context& c) const {
	cerr << "Fang, finish CHP::guarded_command::check_guarded_action()!"
		<< endl;
	return return_type(NULL);
}

//=============================================================================
// class else_clause method definitions

CONSTRUCTOR_INLINE
else_clause::else_clause(const token_else* g, const string_punctuation_type* a, 
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

statement::return_type
skip::check_action(context& c) const {
	cerr << "Fang, finish CHP::skip::check_action()!" << endl;
	return statement::return_type(NULL);
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

statement::return_type
wait::check_action(context& c) const {
	cerr << "Fang, finish CHP::wait::check_action()!" << endl;
	return statement::return_type(NULL);
}


//=============================================================================
// class assignment method definitions

/**
	This constructor upgrades a regular parser::assign_stmt into
	a CHP-class assignment statement.  
	The constructor releases the members of the assign_stmt
	and re-wraps them.  
	\param a the constructed assign_stmt.
 */
CONSTRUCTOR_INLINE
assignment::assignment(base_assign* a) : parent_type(),
	// destructive transfer of ownership
	assign_stmt(a->release_lhs(), a->release_op(), a->release_rhs()) {
	excl_ptr<base_assign> delete_me(a);
}

DESTRUCTOR_INLINE
assignment::~assignment() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(assignment)

line_position
assignment::leftmost(void) const {
	return assign_stmt::leftmost();
}

line_position
assignment::rightmost(void) const {
	return assign_stmt::rightmost();
}

statement::return_type
assignment::check_action(context& c) const {
	cerr << "Fang, finish CHP::assignment::check_action()!" << endl;
	return statement::return_type(NULL);
}


//=============================================================================
// class incdec_stmt method definitions

CONSTRUCTOR_INLINE
incdec_stmt::incdec_stmt(base_assign* a) : parent_type(), 
		// destructive transfer of ownership
		parser::incdec_stmt(a->release_expr(), a->release_op()) {
	excl_ptr<base_assign> delete_me(a);
}

DESTRUCTOR_INLINE
incdec_stmt::~incdec_stmt() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(incdec_stmt)

line_position
incdec_stmt::leftmost(void) const {
	return incdec_stmt::leftmost();
}

line_position
incdec_stmt::rightmost(void) const {
	return incdec_stmt::rightmost();
}

statement::return_type
incdec_stmt::check_action(context& c) const {
	cerr << "Fang, finish CHP::incdec_stmt::check_action()!" << endl;
	return statement::return_type(NULL);
}


//=============================================================================
// class communication method definitions

CONSTRUCTOR_INLINE
communication::communication(const expr* c, const char_punctuation_type* d) :
		statement(), chan(c), dir(d) {
	NEVER_NULL(chan); NEVER_NULL(dir);
}

DESTRUCTOR_INLINE
communication::~communication() { }

line_position
communication::leftmost(void) const {
	return chan->leftmost();
}

//=============================================================================
// class comm_list method definitions

CONSTRUCTOR_INLINE
comm_list::comm_list(const communication* c) :
		statement(), parent_type(c) {
}

DESTRUCTOR_INLINE
comm_list::~comm_list() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(comm_list)

line_position
comm_list::leftmost(void) const {
	return parent_type::leftmost();
}

line_position
comm_list::rightmost(void) const {
	return parent_type::rightmost();
}

statement::return_type
comm_list::check_action(context& c) const {
	cerr << "Fang, finish CHP::comm_list::check_action()!" << endl;
	return statement::return_type(NULL);
}


//=============================================================================
// class send method definitions

CONSTRUCTOR_INLINE
send::send(const expr* c, const char_punctuation_type* d, const expr_list* r) :
		communication(c, d), rvalues(r) {
	NEVER_NULL(rvalues);
}

DESTRUCTOR_INLINE
send::~send() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(send)

line_position
send::rightmost(void) const {
	return rvalues->rightmost();
}

statement::return_type
send::check_action(context& c) const {
	cerr << "Fang, finish CHP::send::check_action()!" << endl;
	return statement::return_type(NULL);
}

//=============================================================================
// class receive method definitions

CONSTRUCTOR_INLINE
receive::receive(const expr* c, const char_punctuation_type* d, 
		const expr_list* l) :
		communication(c, d), lvalues(l) {
	NEVER_NULL(lvalues);
}

DESTRUCTOR_INLINE
receive::~receive() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(receive)

line_position
receive::rightmost(void) const {
	return lvalues->rightmost();
}

statement::return_type
receive::check_action(context& c) const {
	cerr << "Fang, finish CHP::received::check_action()!" << endl;
	return statement::return_type(NULL);
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

statement::return_type
det_selection::check_action(context& c) const {
	cerr << "Fang, finish CHP::det_selection::check_action()!" << endl;
	return statement::return_type(NULL);
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

statement::return_type
nondet_selection::check_action(context& c) const {
	cerr << "Fang, finish CHP::nondet_selection::check_action()!" << endl;
	return statement::return_type(NULL);
}

//=============================================================================
// class prob_selection method definitions

CONSTRUCTOR_INLINE
prob_selection::prob_selection(const guarded_command* n) :
		selection(), parent_type(n) {
}

DESTRUCTOR_INLINE
prob_selection::~prob_selection() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(prob_selection)

line_position
prob_selection::leftmost(void) const {
	return parent_type::leftmost();
}

line_position
prob_selection::rightmost(void) const {
	return parent_type::rightmost();
}

statement::return_type
prob_selection::check_action(context& c) const {
	cerr << "Fang, finish CHP::prob_selection::check_action()!" << endl;
	return statement::return_type(NULL);
}


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

statement::return_type
loop::check_action(context& c) const {
	cerr << "Fang, finish CHP::loop::check_action()!" << endl;
	return statement::return_type(NULL);
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

statement::return_type
do_until::check_action(context& c) const {
	cerr << "Fang, finish CHP::do_until::check_action()!" << endl;
	return statement::return_type(NULL);
}

//=============================================================================
// class log method definitions

CONSTRUCTOR_INLINE
log::log(const generic_keyword_type* l, const expr_list* n) : statement(),
		lc(l), args(n) {
	NEVER_NULL(lc); NEVER_NULL(args);
}

DESTRUCTOR_INLINE
log::~log() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(log)

line_position
log::leftmost(void) const {
	return lc->leftmost();
}

line_position
log::rightmost(void) const {
	return args->rightmost();
}

statement::return_type
log::check_action(context& c) const {
	cerr << "Fang, finish CHP::log::check_action()!" << endl;
	return statement::return_type(NULL);
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

#if 0
template class node_list<const statement>;	// CHP::stmt_list
template class node_list<const guarded_command>;	// CHP::det_sel_base
							// CHP::prob_sel_base
							// CHP::nondet_sel_base
	// actually distinguish these types for the sake of printing?
template class node_list<const communication>;	// CHP::comm_list_base
#else
// This is temporary, until node_list::check_build is overhauled.  
template
node_list<const statement>::node_list();

template
node_list<const statement>::node_list(const CHP::statement*);

template
ostream&
node_list<const statement>::what(ostream&) const;

template
line_position
node_list<const statement>::leftmost(void) const;
#endif

//=============================================================================
}	// end namespace CHP
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __AST_ART_PARSER_CHP_CC__

