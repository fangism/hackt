/**
	\file "art_parser_chp.cc"
	Class method definitions for CHP parser classes.
	$Id: art_parser_chp.cc,v 1.7.40.1 2005/02/22 08:15:18 fang Exp $
 */

#ifndef	__ART_PARSER_CHP_CC__
#define	__ART_PARSER_CHP_CC__

#include "art_parser.tcc"
#include "art_parser_chp.h"
#include "art_parser_expr_list.h"
#include "art_parser_token.h"

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

#if 0
ostream&
body::what(ostream& o) const {
	return o << "(chp-body)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(body)
#endif

line_position 
body::rightmost(void) const {
	return stmts->rightmost();
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

#if 0
ostream&
guarded_command::what(ostream& o) const {
	return o << "(chp-guarded-cmd)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(guarded_command)
#endif

line_position
guarded_command::leftmost(void) const {
	return guard->leftmost();
}

line_position
guarded_command::rightmost(void) const {
	NEVER_NULL(command);
	return command->rightmost();
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

#if 0
ostream&
else_clause::what(ostream& o) const {
	return o << "(chp-else-clause)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(else_clause)
#endif

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

#if 0
ostream&
skip::what(ostream& o) const {
	return o << "(chp-skip)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(skip)
#endif

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
wait::wait(const terminal* l, const expr* c, const terminal* r) :
		statement(),
		lb(IS_A(const terminal*, l)),
		cond(IS_A(const expr*, c)),
		rb(IS_A(const terminal*, r)) {
	NEVER_NULL(cond); NEVER_NULL(lb); NEVER_NULL(rb);
}

DESTRUCTOR_INLINE
wait::~wait() { }

#if 0
ostream&
wait::what(ostream& o) const {
	return o << "(chp-wait)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(wait)
#endif

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

#if 0
ostream&
assignment::what(ostream& o) const {
	return o << "(chp-assignment)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(assignment)
#endif

line_position
assignment::leftmost(void) const {
	return assign_stmt::leftmost();
}

line_position
assignment::rightmost(void) const {
	return assign_stmt::rightmost();
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

#if 0
ostream&
incdec_stmt::what(ostream& o) const {
	return o << "(chp-assignment)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(incdec_stmt)
#endif

line_position
incdec_stmt::leftmost(void) const {
	return incdec_stmt::leftmost();
}

line_position
incdec_stmt::rightmost(void) const {
	return incdec_stmt::rightmost();
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
		statement(), comm_list_base(c) {
}

DESTRUCTOR_INLINE
comm_list::~comm_list() {
}

#if 0
ostream&
comm_list::what(ostream& o) const {
	return o << "(chp-comm-list)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(comm_list)
#endif

line_position
comm_list::leftmost(void) const {
	return comm_list_base::leftmost();
}

line_position
comm_list::rightmost(void) const {
	return comm_list_base::rightmost();
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

#if 0
ostream&
send::what(ostream& o) const {
	return o << "(chp-send)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(send)
#endif

line_position
send::rightmost(void) const {
	return rvalues->rightmost();
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

#if 0
ostream&
receive::what(ostream& o) const {
	return o << "(chp-receive)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(receive)
#endif

line_position
receive::rightmost(void) const {
	return lvalues->rightmost();
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
		selection(), det_sel_base(n) {
}

DESTRUCTOR_INLINE
det_selection::~det_selection() { }

#if 0
ostream&
det_selection::what(ostream& o) const {
	return o << "(chp-det-sel)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(det_selection)
#endif

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
nondet_selection::nondet_selection(const guarded_command* n) :
		selection(), nondet_sel_base(n) {
}

DESTRUCTOR_INLINE
nondet_selection::~nondet_selection() { }

#if 0
ostream&
nondet_selection::what(ostream& o) const {
	return o << "(chp-nondet-sel)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(nondet_selection)
#endif

line_position
nondet_selection::leftmost(void) const {
	return nondet_sel_base::leftmost();
}

line_position
nondet_selection::rightmost(void) const {
	return nondet_sel_base::rightmost();
}

//=============================================================================
// class prob_selection method definitions

CONSTRUCTOR_INLINE
prob_selection::prob_selection(const guarded_command* n) : selection(),
		prob_sel_base(n) {
}

DESTRUCTOR_INLINE
prob_selection::~prob_selection() { }

#if 0
ostream&
prob_selection::what(ostream& o) const {
	return o << "(chp-prob-sel)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(prob_selection)
#endif

line_position
prob_selection::leftmost(void) const {
	return prob_sel_base::leftmost();
}

line_position
prob_selection::rightmost(void) const {
	return prob_sel_base::rightmost();
}


//=============================================================================
// class loop method definitions

CONSTRUCTOR_INLINE
loop::loop(const stmt_list* n) : statement(), commands(n) {
}

DESTRUCTOR_INLINE
loop::~loop() { }

#if 0
ostream&
loop::what(ostream& o) const {
	return o << "(chp-loop)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(loop)
#endif

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
do_until::do_until(const det_selection* n) : statement(),
		sel(n) { }

DESTRUCTOR_INLINE
do_until::~do_until() { }

#if 0
ostream&
do_until::what(ostream& o) const {
	return o << "(chp-do-until)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(do_until)
#endif

line_position
do_until::leftmost(void) const {
	return sel->leftmost();
}

line_position
do_until::rightmost(void) const {
	return sel->rightmost();
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

#if 0
ostream&
log::what(ostream& o) const {
	return o << "(chp-log)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(log)
#endif

line_position
log::leftmost(void) const {
	return lc->leftmost();
}

line_position
log::rightmost(void) const {
	return args->rightmost();
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

template class node_list<const statement,semicolon>;	// CHP::stmt_list
template class node_list<const guarded_command,thickbar>;	// CHP::det_sel_base
							// CHP::prob_sel_base
template class node_list<const guarded_command,colon>;	// CHP::nondet_sel_base
template class node_list<const communication,comma>;	// CHP::comm_list_base

//=============================================================================
}	// end namespace CHP
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __ART_PARSER_CHP_CC__

