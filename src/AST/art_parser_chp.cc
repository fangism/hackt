/**
	\file "art_parser_chp.cc"
	Class method definitions for CHP parser classes.
	$Id: art_parser_chp.cc,v 1.9.8.2 2005/04/09 23:09:50 fang Exp $
 */

#ifndef	__ART_PARSER_CHP_CC__
#define	__ART_PARSER_CHP_CC__

#include <iostream>

#include "art_parser_node_position.h"
#include "art_parser.tcc"
#include "art_parser_chp.h"
#include "art_parser_expr_list.h"
#include "art_parser_token.h"
#include "art_parser_token_char.h"

#include "what.h"

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
body::body(const token_keyword* t, const stmt_list* s) :
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
guarded_command::guarded_command(const chp_expr* g, const terminal* a,
		const stmt_list* c) : 
		node(), guard(g),
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

never_ptr<const object>
guarded_command::check_build(context& c) const {
	cerr << "Fang, finish CHP::guarded_command::check_build()!" << endl;
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
skip::skip(const token_keyword* s) : statement(),
		token_keyword(IS_A(const token_keyword*, s)->c_str()) {
	// transfers string contents
	excl_ptr<const token_keyword> delete_me(s);
}

DESTRUCTOR_INLINE
skip::~skip() { }

// check that nothing appears after skip statement

PARSER_WHAT_DEFAULT_IMPLEMENTATION(skip)

line_position
skip::leftmost(void) const {
	return token_keyword::leftmost();
}

line_position
skip::rightmost(void) const {
	return token_keyword::rightmost();
}

never_ptr<const object>
skip::check_build(context& c) const {
	cerr << "Fang, finish CHP::skip::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}


//=============================================================================
// class wait method definitions

CONSTRUCTOR_INLINE
wait::wait(const char_punctuation_type* l, const expr* c,
		const char_punctuation_type* r) :
		statement(), lb(l), cond(c), rb(l) {
	NEVER_NULL(cond); NEVER_NULL(lb); NEVER_NULL(rb);
}

DESTRUCTOR_INLINE
wait::~wait() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(wait)

line_position
wait::leftmost(void) const {
	return lb->leftmost();
}

line_position
wait::rightmost(void) const {
	return rb->rightmost();
}

never_ptr<const object>
wait::check_build(context& c) const {
	cerr << "Fang, finish CHP::wait::check_build()!" << endl;
	return never_ptr<const object>(NULL);
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

never_ptr<const object>
assignment::check_build(context& c) const {
	cerr << "Fang, finish CHP::assignment::check_build()!" << endl;
	return never_ptr<const object>(NULL);
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

never_ptr<const object>
incdec_stmt::check_build(context& c) const {
	cerr << "Fang, finish CHP::incdec_stmt::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}


//=============================================================================
// class communication method definitions

CONSTRUCTOR_INLINE
communication::communication(const expr* c, const token_char* d) :
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

never_ptr<const object>
comm_list::check_build(context& c) const {
	cerr << "Fang, finish CHP::comm_list::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}


//=============================================================================
// class send method definitions

CONSTRUCTOR_INLINE
send::send(const expr* c, const token_char* d, const expr_list* r) :
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

never_ptr<const object>
send::check_build(context& c) const {
	cerr << "Fang, finish CHP::send::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class receive method definitions

CONSTRUCTOR_INLINE
receive::receive(const expr* c, const token_char* d, const expr_list* l) :
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

never_ptr<const object>
receive::check_build(context& c) const {
	cerr << "Fang, finish CHP::received::check_build()!" << endl;
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
	cerr << "Fang, finish CHP::det_selection::check_build()!" << endl;
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
	cerr << "Fang, finish CHP::nondet_selection::check_build()!" << endl;
	return never_ptr<const object>(NULL);
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

never_ptr<const object>
prob_selection::check_build(context& c) const {
	cerr << "Fang, finish CHP::prob_selection::check_build()!" << endl;
	return never_ptr<const object>(NULL);
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

never_ptr<const object>
loop::check_build(context& c) const {
	cerr << "Fang, finish CHP::loop::check_build()!" << endl;
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
	cerr << "Fang, finish CHP::do_until::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class log method definitions

CONSTRUCTOR_INLINE
log::log(const token_keyword* l, const expr_list* n) : statement(),
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

never_ptr<const object>
log::check_build(context& c) const {
	cerr << "Fang, finish CHP::log::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

#if USE_NEW_NODE_LIST
template class node_list<const statement>;	// CHP::stmt_list
template class node_list<const guarded_command>;	// CHP::det_sel_base
							// CHP::prob_sel_base
							// CHP::nondet_sel_base
	// actually distinguish these types for the sake of printing?
template class node_list<const communication>;	// CHP::comm_list_base
#else
template class node_list<const statement,semicolon>;	// CHP::stmt_list
template class node_list<const guarded_command,thickbar>;	// CHP::det_sel_base
							// CHP::prob_sel_base
template class node_list<const guarded_command,colon>;	// CHP::nondet_sel_base
template class node_list<const communication,comma>;	// CHP::comm_list_base
#endif

//=============================================================================
}	// end namespace CHP
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __ART_PARSER_CHP_CC__

