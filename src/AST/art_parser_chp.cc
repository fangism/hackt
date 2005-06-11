/**
	\file "AST/art_parser_chp.cc"
	Class method definitions for CHP parser classes.
	$Id: art_parser_chp.cc,v 1.14.2.5 2005/06/11 21:48:05 fang Exp $
 */

#ifndef	__AST_ART_PARSER_CHP_CC__
#define	__AST_ART_PARSER_CHP_CC__

#include <iostream>
#include <vector>

#include "AST/art_parser_chp.h"
#include "AST/art_parser_expr_list.h"
#include "AST/art_parser_token.h"
#include "AST/art_parser_node_list.tcc"
#include "Object/art_object_CHP.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_expr_base.h"
#include "Object/art_object_nonmeta_inst_ref.h"
#include "Object/art_object_nonmeta_inst_ref_subtypes.h"
#include "Object/art_object_classification_details.h"
#include "Object/art_object_instance.h"
#include "Object/art_object_instance_collection.h"

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
}	// end namespace util

namespace ART {
namespace parser {
namespace CHP {
using std::vector;
using std::list;
using std::find;
using std::copy;
using std::back_inserter;
#include "util/using_ostream.h"
using entity::bool_expr;
using entity::CHP::action_sequence;
using entity::CHP::guarded_action;
using entity::CHP::condition_wait;
using entity::channel_type_reference_base;

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: Need to figure out the boundaries between loop statements with 
	initializations, in the case of more than one loop body.  
	First check individual statements and loops, 
	worry about proper construction second.  
 */
never_ptr<const object>
body::check_build(context& c) const {
	cerr << "Fang, finish CHP::body::check_build()!" << endl;
if (stmts) {
	typedef	list<statement::return_type>	checked_stmts_type;
#if 0
	never_ptr<definition_base> d(c.get_current_open_definition());
	// can be datatype or process definition...
#endif
	checked_stmts_type checked_stmts;
	stmts->check_list(checked_stmts, &statement::check_action, c);
	// for now, (this is wrong) construct as sequence.  
	const checked_stmts_type::const_iterator i(checked_stmts.begin());
	const checked_stmts_type::const_iterator e(checked_stmts.end());
	const checked_stmts_type::const_iterator
		ni(find(i, e, statement::return_type()));
	if (ni == e) {
		const count_ptr<action_sequence> ret(new action_sequence);
		copy(i, e, back_inserter(*ret));
		// for now, dropping checked action sequence
		// until we figure out how to cleanly add it to a definition.  
	} else {
		cerr << "ERROR: at least one error in CHP body." << endl;
		THROW_EXIT;
	}
}
	// else empty, no CHP to add
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks and constructs a guarded CHP action.  
	Temporary implementation.
	TODO: account for else semantics, and skip (null) statement.  
		(2005-05-30)
 */
guarded_command::return_type
guarded_command::check_guarded_action(context& c) const {
#if 1
	cerr << "Fang, finish CHP::guarded_command::check_guarded_action()!"
		<< endl;
	return return_type(NULL);
#else
	typedef list<statement::return_type>	checked_stmts_type;
	// will need to be more general non-meta (bool) expression
	const expr::nonmeta_return_type
		checked_guard(guard->check_nonmeta_expr(c));
	if (!checked_guard) {
		cerr << "ERROR in guard expression at " << where(*guard)
			<< endl;
		return return_type(NULL);
	}
	const guarded_action::guard_ptr_type
		checked_bool_guard(checked_guard.is_a<bool_expr>());
	if (!checked_bool_guard) {
		cerr << "ERROR expression at " << where(*guard) <<
			" is not boolean." << endl;
		return return_type(NULL);
	}
	checked_stmts_type checked_stmts;
	NEVER_NULL(command);
	command->check_list(checked_stmts, &statement::check_action, c);
	const checked_stmts_type::const_iterator i(checked_stmts.begin());
	const checked_stmts_type::const_iterator e(checked_stmts.end());
	const checked_stmts_type::const_iterator
		ni(find(i, e, statement::return_type(NULL)));
	if (ni == e) {
		// no NULLs found
		const count_ptr<action_sequence>
			act_seq(new action_sequence);
		copy(i, e, back_inserter(*act_seq));
		return return_type(new guarded_action(
			checked_bool_guard, act_seq));
	} else {
		cerr << "ERROR in CHP statement(s) at " <<
			where(*command) << endl;
		return return_type(NULL);
	}
#endif
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
//	cerr << "Fang, finish CHP::skip::check_action()!" << endl;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
statement::return_type
wait::check_action(context& c) const {
	typedef	statement::return_type		return_type;
#if 1
	cerr << "Fang, finish CHP::wait::check_action()!" << endl;
	return return_type(NULL);
#else
	const expr::nonmeta_return_type ret(cond->check_nonmeta_expr(c));
	if (!ret) {
		cerr << "ERROR in wait condition expression at " <<
			where(*cond) << endl;
		return return_type(NULL);
	}
	const count_ptr<bool_expr> bret(ret.is_a<bool_expr>());
	if (!bret) {
		cerr << "ERROR: wait condition at " << where(*cond) <<
			" is not boolean." << endl;
		return return_type(NULL);
	} else {
		return return_type(new condition_wait(ret));
	}
#endif
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
	const excl_ptr<base_assign> delete_me(a);
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
communication::communication(const inst_ref_expr* c, 
		const char_punctuation_type* d) :
		statement(), chan(c), dir(d) {
	NEVER_NULL(chan); NEVER_NULL(dir);
}

DESTRUCTOR_INLINE
communication::~communication() { }

line_position
communication::leftmost(void) const {
	return chan->leftmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks the referenced channel expression (nonmeta).  
	Does not check direction here, send and recv will check.  
 */
communication::checked_channel_type
communication::check_channel(context& c) const {
	typedef	checked_channel_type::element_type	ret_chan_type;
	const inst_ref_expr::nonmeta_return_type
		ch(chan->check_nonmeta_reference(c));
	if (!ch) {
		cerr << "ERROR resolving instance reference at " <<
			where(*chan) << endl;
		return checked_channel_type(NULL);
	}
	const checked_channel_type ret(ch.is_a<ret_chan_type>());
	if (!ret) {
		cerr << "ERROR instance referenced at " <<
			where(*chan) << " is not a channel!" << endl;
		return checked_channel_type(NULL);
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param c reference to resolved channel instance reference.  
	\return channel type direction (char).  
 */
char
communication::get_channel_direction(
		const checked_channel_type::element_type& c) {
	typedef	checked_channel_type::element_type	ret_chan_type;
	const ret_chan_type::instance_collection_ptr_type
		inst_base(c.get_inst_base_subtype());
	NEVER_NULL(inst_base);
	// get type reference
	const count_ptr<const channel_type_reference_base>
		type_ref(inst_base->get_type_ref()
			.is_a<const channel_type_reference_base>());
	NEVER_NULL(type_ref);
	return type_ref->get_direction();
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
statement::return_type
comm_list::check_action(context& c) const {
#if 0
	cerr << "Fang, finish CHP::comm_list::check_action()!" << endl;
	return statement::return_type(NULL);
#else
	// typedef	list<statement::return_type>	checked_actions_type;
	checked_actions_type actions;
	// actions.reserve(size());
	// static_cast<const parent_type&>(*this).
#if 0
	// WTF, this should compile!!!
	check_list(actions, &communication::check_action, c);
	// parent_type::template check_list<>(actions, &communication::check_action, c);
#else
	const_iterator ci(begin());
	const const_iterator ce(end());
	for ( ; ci!=ce; ci++) {
		actions.push_back((*ci)->check_action(c));
	}
#endif
	const checked_actions_type::const_iterator i(actions.begin());
	const checked_actions_type::const_iterator e(actions.end());
	const checked_actions_type::const_iterator
		ni(find(i, e, statement::return_type()));
	if (ni != e) {
		cerr << "ERROR in one of the communcation actions in "
			<< where(*this) << endl;
		return statement::return_type(NULL);
	} else {
		const count_ptr<entity::CHP::concurrent_actions>
			ret(new entity::CHP::concurrent_actions);
		copy(i, e, back_inserter(*ret));
		return ret;
	}
#endif
}

//=============================================================================
// class send method definitions

CONSTRUCTOR_INLINE
send::send(const inst_ref_expr* c, const char_punctuation_type* d, 
		const expr_list* r) :
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: check expression list, type-for-type
 */
statement::return_type
send::check_action(context& c) const {
#if 0
	cerr << "Fang, finish CHP::send::check_action()!" << endl;
	return statement::return_type(NULL);
#else
	const communication::checked_channel_type
		sender(check_channel(c));
	if (!sender) {
		return statement::return_type(NULL);
	}
	if (get_channel_direction(*sender) == '?') {
		cerr << "ERROR: cannot send on a receive-only channel, at " <<
			where(*chan) << endl;
		return statement::return_type(NULL);
	}
	// check expression list...
//	cerr << "Fang, check expression list in send::check_action()!" << endl;
	typedef	expr_list::checked_nonmeta_exprs_type::const_iterator
							const_iterator;
	expr_list::checked_nonmeta_exprs_type checked_exprs;
	rvalues->postorder_check_nonmeta_exprs(checked_exprs, c);
	const_iterator i(checked_exprs.begin());
	const const_iterator e(checked_exprs.end());
	const const_iterator ni(find(i, e, expr::nonmeta_return_type(NULL)));
	if (ni != e) {
		cerr << "At least one error in expr-list at " <<
			where(*rvalues) << endl;
		return statement::return_type(NULL);
	}

	typedef	count_ptr<entity::CHP::channel_send>	return_type;
	const return_type ret(new entity::CHP::channel_send(sender));
	NEVER_NULL(ret);
	good_bool g(true);
	for ( ; i!=e && g.good; i++) {
		if (!ret->push_back(*i).good) {
			cerr << "Type-check failed for expression, "
				"somewhere in " << where(*rvalues) << endl;
			g.good = false;
		}
	}
	if (!g.good) {
		cerr << "At least one type error in expr-list in " <<
			where(*rvalues) << endl;
		return statement::return_type(NULL);
	} else {
		return ret;
	}
#endif
}

//=============================================================================
// class receive method definitions

CONSTRUCTOR_INLINE
receive::receive(const inst_ref_expr* c, const char_punctuation_type* d, 
		const inst_ref_expr_list* l) :
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

