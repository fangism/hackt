// "art_parser_chp.cc"

#include "art_parser_template_methods.h"
#include "art_parser_chp.h"

#define	CONSTRUCTOR_INLINE
#define	DESTRUCTOR_INLINE

namespace ART {
namespace parser {
namespace CHP {
//=============================================================================
// class statement method definitions

CONSTRUCTOR_INLINE
statement::statement() : node() { }

DESTRUCTOR_INLINE
statement::~statement() { }

ostream&
statement::what(ostream& o) const {
	return o << "(chp-statement)";
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
	return o << "(chp-body)";
}

line_position 
body::rightmost(void) const {
	return stmts->rightmost();
}

//=============================================================================
// class guarded_command method definitions

CONSTRUCTOR_INLINE
guarded_command::guarded_command(chp_expr* g, terminal* a, stmt_list* c) : 
		node(),
		guard(g),
		// remember, may be keyword: else   
		arrow(a),
		command(c) {
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
	return o << "(chp-guarded-cmd)";
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
	return o << "(chp-else-clause)";
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
	return o << "(chp-skip)";
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
		statement(),
		lb(IS_A(terminal*, l)),
		cond(IS_A(expr*, c)),
		rb(IS_A(terminal*, r)) {
	assert(cond); assert(lb); assert(rb);
}

DESTRUCTOR_INLINE
wait::~wait() {
	SAFEDELETE(lb); SAFEDELETE(cond); SAFEDELETE(rb);
}

ostream&
wait::what(ostream& o) const {
	return o << "(chp-wait)";
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

/**
	This constructor upgrades a regular parser::assign_stmt into
	a CHP-class assignment statement.  
	The constructor releases the members of the assign_stmt
	and re-wraps them.  
	\param a the constructed assign_stmt.
 */
CONSTRUCTOR_INLINE
assignment::assignment(base_assign* a) : ART::parser::CHP::statement(),
	// destructive transfer of ownership
	assign_stmt(a->release_lhs(), a->release_op(), a->release_rhs()) {
	SAFEDELETE(a);
}

DESTRUCTOR_INLINE
assignment::~assignment() { }

ostream&
assignment::what(ostream& o) const {
	return o << "(chp-assignment)";
}

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
incdec_stmt::incdec_stmt(base_assign* a) :
		ART::parser::CHP::statement(),
		// destructive transfer of ownership
		parser::incdec_stmt(a->release_expr(), a->release_op()) {
	SAFEDELETE(a);
}

DESTRUCTOR_INLINE
incdec_stmt::~incdec_stmt() { }

ostream&
incdec_stmt::what(ostream& o) const {
	return o << "(chp-assignment)";
}

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
communication::communication(expr* c, token_char* d) : statement(),
		chan(c), dir(d) {
	assert(chan); assert(dir);
}

DESTRUCTOR_INLINE
communication::~communication() {
	SAFEDELETE(chan); SAFEDELETE(dir);
}

line_position
communication::leftmost(void) const {
	return chan->leftmost();
}

//=============================================================================
// class comm_list method definitions

CONSTRUCTOR_INLINE
comm_list::comm_list(communication* c) : statement(), comm_list_base(c) {
}

DESTRUCTOR_INLINE
comm_list::~comm_list() {
}

ostream&
comm_list::what(ostream& o) const {
	return o << "(comm-list)";
}

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
send::send(expr* c, token_char* d, expr_list* r) : communication(c, d),
		rvalues(r) {
	assert(rvalues);
}

DESTRUCTOR_INLINE
send::~send() {
	SAFEDELETE(rvalues);
}

line_position
send::rightmost(void) const {
	return rvalues->rightmost();
}

//=============================================================================
// class receive method definitions

CONSTRUCTOR_INLINE
receive::receive(expr* c, token_char* d, expr_list* l) : communication(c, d),
		lvalues(l) {
	assert(lvalues);
}

DESTRUCTOR_INLINE
receive::~receive() {
	SAFEDELETE(lvalues);
}

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

ostream&
selection::what(ostream& o) const {
	return o << "(chp-selection)";
}

//=============================================================================
// class det_selection method definitions

CONSTRUCTOR_INLINE
det_selection::det_selection(guarded_command* n) : selection(),
		node_list<guarded_command,thickbar>(n) {
}

DESTRUCTOR_INLINE
det_selection::~det_selection() { }

ostream&
det_selection::what(ostream& o) const {
	return o << "(chp-det-sel)";
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
nondet_selection::nondet_selection(guarded_command* n) : selection(),
		node_list<guarded_command,colon>(n) {
}

DESTRUCTOR_INLINE
nondet_selection::~nondet_selection() { }

ostream&
nondet_selection::what(ostream& o) const {
	return o << "(chp-nondet-sel)";
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
// class prob_selection method definitions

CONSTRUCTOR_INLINE
prob_selection::prob_selection(guarded_command* n) : selection(),
		node_list<guarded_command,thickbar>(n) {
}

DESTRUCTOR_INLINE
prob_selection::~prob_selection() { }

ostream&
prob_selection::what(ostream& o) const {
	return o << "(chp-prob-sel)";
}

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
loop::loop(stmt_list* n) : statement(), commands(n) {
}

DESTRUCTOR_INLINE
loop::~loop() {
	SAFEDELETE(commands);
}

ostream&
loop::what(ostream& o) const {
	return o << "(chp-loop)";
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
	return o << "(chp-do-until)";
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
// class log method definitions

CONSTRUCTOR_INLINE
log::log(token_keyword* l, expr_list* n) : statement(),
		lc(l), args(n) {
	assert(lc); assert(args);
}

DESTRUCTOR_INLINE
log::~log() {
	SAFEDELETE(lc); SAFEDELETE(args);
}

ostream&
log::what(ostream& o) const {
	return o << "(chp-log)";
}

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

template class node_list<statement,semicolon>;		// CHP::stmt_list
template class node_list<guarded_command,thickbar>;	// CHP::det_sel_base
							// CHP::prob_sel_base
template class node_list<guarded_command,colon>;	// CHP::nondet_sel_base
template class node_list<communication,comma>;		// CHP::comm_list_base

//=============================================================================
};	// end namespace CHP
};	// end namespace parser
};	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

