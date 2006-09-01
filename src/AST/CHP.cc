/**
	\file "AST/CHP.cc"
	Class method definitions for CHP parser classes.
	$Id: CHP.cc,v 1.9.4.1 2006/09/01 05:17:15 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_chp.cc,v 1.21.20.1 2005/12/11 00:45:03 fang Exp
 */

#ifndef	__HAC_AST_CHP_CC__
#define	__HAC_AST_CHP_CC__

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <vector>
#include <functional>

#include "AST/CHP.h"
#include "AST/expr_list.h"
#include "AST/token.h"
#include "AST/node_list.tcc"
#include "AST/parse_context.h"
#include "AST/range.h"

#include "Object/lang/CHP.tcc"
#include "Object/type/data_type_reference.h"
#include "Object/type/channel_type_reference_base.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/bool_expr.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/ref/data_nonmeta_instance_reference.h"
#include "Object/ref/nonmeta_instance_reference_subtypes.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/inst/channel_instance_collection.h"
#if USE_INSTANCE_PLACEHOLDERS
#include "Object/inst/instance_placeholder.h"
#endif
#include "Object/inst/pint_value_collection.h"
#include "Object/def/user_def_datatype.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/process_definition.h"
#include "Object/inst/general_collection_type_manager.h"

#include "util/wtf.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"

#define	CONSTRUCTOR_INLINE
#define	DESTRUCTOR_INLINE

// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::body, "(chp-body)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::statement, "(chp-statement)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::guarded_command, "(chp-guarded-cmd)")
// SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::else_clause, "(chp-else-clause)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::skip, "(chp-skip)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::wait, "(chp-wait)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::binary_assignment, "(chp-assignment)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::bool_assignment, "(chp-assignment)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::send, "(chp-send)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::receive, "(chp-receive)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::det_selection, "(chp-det-sel)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::nondet_selection, "(chp-nondet-sel)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::prob_selection, "(chp-prob-sel)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::metaloop_selection, "(chp-metaloop-sel)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::loop, "(chp-loop)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::do_until, "(chp-do-until)")
SPECIALIZE_UTIL_WHAT(HAC::parser::CHP::log, "(chp-log)")
}	// end namespace util

namespace HAC {
namespace parser {
namespace CHP {
using std::vector;
using std::list;
using std::find;
using std::find_if;
using std::copy;
using std::back_inserter;
using std::mem_fun_ref;
#include "util/using_ostream.h"
using entity::bool_expr;
using entity::meta_range_expr;
using entity::CHP::action_sequence;
using entity::CHP::concurrent_actions;
using entity::CHP::guarded_action;
using entity::CHP::condition_wait;
using entity::channel_type_reference_base;
using entity::user_def_chan;
using entity::user_def_datatype;
using entity::process_definition;
using entity::data_nonmeta_instance_reference;
using entity::data_type_reference;
using entity::pint_scalar;

//=============================================================================
// class statement method definitions

CONSTRUCTOR_INLINE
statement::statement() { }

DESTRUCTOR_INLINE
statement::~statement() { }

//=============================================================================
// class stmt_list method definitions

stmt_list::stmt_list() : statement(), stmt_list_base(), is_concurrent(false) { }

stmt_list::stmt_list(const statement* s) :
		statement(), stmt_list_base(s), is_concurrent(false) {
	NEVER_NULL(s);
}

stmt_list::~stmt_list() { }

ostream&
stmt_list::what(ostream& o) const {
	return stmt_list_base::what(o);
}

line_position
stmt_list::leftmost(void) const {
	return stmt_list_base::leftmost();
}

line_position
stmt_list::rightmost(void) const {
	return stmt_list_base::rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks all statements and appends their results in a checked-list.  
	Provided as a public convenience function.
	\param sl the list in which to return results.  
	\param c the context of the occurrence of this statement list.  
 */
void
stmt_list::postorder_check_stmts(checked_stmts_type& sl, context& c) const {
	check_list(sl, &statement::check_action, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If there is only one statement in the list, then 
	this will return the result of the one statement.  
	If there is more than one statement in the list, then depending on 
	the is_concurrent flag, this will either return a 
	entity::CHP::concurrent_actions, or an entity::CHP::action_sequence.  
	\param c parser context.
	\return singular or collective checked CHP action.  
 */
statement::return_type
stmt_list::check_action(context& c) const {
if (size() == 1) {
	// single actions can be returned more efficiently, no need to group
	return front()->check_action(c);
} else {
	typedef	checked_stmts_type::const_iterator	const_checked_iterator;
	checked_stmts_type checked_stmts;
	postorder_check_stmts(checked_stmts, c);
	const const_checked_iterator i(checked_stmts.begin());
	const const_checked_iterator e(checked_stmts.end());
	const const_checked_iterator
		ni(find(i, e, statement::return_type(NULL)));
	if (ni == e) {
		if (is_concurrent) {
			const count_ptr<concurrent_actions>
				act_pll(new concurrent_actions);
			copy(i, e, back_inserter(*act_pll));
			return act_pll;
		} else {	// is not concurrent
			const count_ptr<action_sequence>
				act_seq(new action_sequence);
			copy(i, e, back_inserter(*act_seq));
			return act_seq;
		}
	} else {
		cerr << "ERROR in CHP statement(s) at " <<
			where(*this) << endl;
		return return_type(NULL);
	}
}	// endif size() == 1
}	// end stmt_list::check_action

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
	Basic common routine to check CHP statements, 
	with no special assembling.  
	\return good if all succeeded, else bad.  
 */
good_bool
body::check_CHP(checked_stmts_type& checked_stmts, context& c) const {
	if (!stmts) {
		// no CHP to add
		return good_bool(true);
	}
	stmts->postorder_check_stmts(checked_stmts, c);
	// for now, (this is wrong) construct as sequence.  
	const const_checked_iterator i(checked_stmts.begin());
	const const_checked_iterator e(checked_stmts.end());
	const const_checked_iterator
		ni(find(i, e, statement::return_type()));
	if (ni != e) {
		cerr << "ERROR: at least one error in CHP body." << endl;
		return good_bool(false);
	} else {
		return good_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This routine is special.  
	It checks all CHP statements in the list, but assembles them
	in the following manner:
	A 'cut' is made after each do-forever-loop statement.  
	All sequences of statements between the cuts are assembled
		as action_sequences.  
		(Presumably, anything after an infinite loop would be
		unreachable, so we use it as a natural boundary.)
	All sequences are then composed concurrently.  
 */
never_ptr<const object>
body::check_build(context& c) const {
	if (!stmts)
		return never_ptr<const object>(NULL);
	// else proceed to check body
	checked_stmts_type checked_stmts;
	if (check_CHP(checked_stmts, c).good) {
		const never_ptr<definition_base>
			def(c.get_current_open_definition());
		NEVER_NULL(def);
		const never_ptr<process_definition>
			proc_def(def.is_a<process_definition>());
		if (!proc_def) {
			cerr << "Currently only support CHP in "
				"process definition, bug Fang about it."
				<< endl;
			cerr << "\tgot: chp { ... } " << where(*this) << endl;
			return never_ptr<const object>(NULL);
		}
		// also reject if current namespace is not the global one
		const_checked_iterator loop_iter(checked_stmts.begin());
		const const_checked_iterator e(checked_stmts.end());
		while (loop_iter != e) {
			const const_checked_iterator start(loop_iter);
			loop_iter = find_if(loop_iter, e, 
				entity::CHP::do_forever_loop::detector<
					const_checked_iterator::value_type>());
			if (loop_iter != e) loop_iter++;
			// need if-guard, else will loop infintely!
			if (distance(start, loop_iter) == 1) {
				proc_def->add_concurrent_chp_body(*start);
			} else {
				const count_ptr<action_sequence>
					seq(new action_sequence);
				NEVER_NULL(seq);
				copy(start, loop_iter, back_inserter(*seq));
				proc_def->add_concurrent_chp_body(seq);
			}
		}
	} else {
		// until better error handling comes along
		THROW_EXIT;
	}
	return never_ptr<const object>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks CHP in the context of a channel definition, 
	either the send or recv body.  
	\param c parse context.
	\param is_send whether or not this CHP belongs to the 
		send body or the recv body of the channel_definition.  
 */
good_bool
body::check_channel_CHP(context& c, const bool is_send) const {
	if (!stmts)
		return good_bool(true);
	checked_stmts_type checked_stmts;
	if (check_CHP(checked_stmts, c).good) {
		const never_ptr<definition_base>
			def(c.get_current_open_definition());
		NEVER_NULL(def);
		const never_ptr<user_def_chan>
			chan_def(def.is_a<user_def_chan>());
		NEVER_NULL(chan_def);
		entity::CHP::action_sequence&
			seq = is_send ? chan_def->get_send_body() :
				chan_def->get_recv_body();
		copy(checked_stmts.begin(), checked_stmts.end(), 
			back_inserter(seq));
		return good_bool(true);
	} else {
		return good_bool(false);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks CHP in the context of a data type definition, 
	either the send or recv body.  
	\param c parse context.
	\param is_send whether or not this CHP belongs to the 
		send body or the recv body of the channel_definition.  
 */
good_bool
body::check_datatype_CHP(context& c, const bool is_send) const {
	if (!stmts)
		return good_bool(true);
	checked_stmts_type checked_stmts;
	if (check_CHP(checked_stmts, c).good) {
		const never_ptr<definition_base>
			def(c.get_current_open_definition());
		NEVER_NULL(def);
		const never_ptr<user_def_datatype>
			chan_def(def.is_a<user_def_datatype>());
		NEVER_NULL(chan_def);
		entity::CHP::action_sequence&
			seq = is_send ? chan_def->get_set_body() :
				chan_def->get_get_body();
		copy(checked_stmts.begin(), checked_stmts.end(), 
			back_inserter(seq));
		return good_bool(true);
	} else {
		return good_bool(false);
	}
}

//=============================================================================
// class guarded_command method definitions

CONSTRUCTOR_INLINE
guarded_command::guarded_command(const chp_expr* g, 
		const string_punctuation_type* a, const statement* c) : 
		guard(g),
		// remember, may be keyword: else   
		arrow(a), command(c) {
	NEVER_NULL(guard);
	NEVER_NULL(arrow);
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
	typedef stmt_list::checked_stmts_type	checked_stmts_type;
	typedef	entity::bool_traits	bool_traits;
	// will need to be more general non-meta (bool) expression
	const never_ptr<const token_else>
		have_else(guard.is_a<const token_else>());
	expr::nonmeta_return_type checked_guard;
	if (!have_else) {
		checked_guard = guard->check_nonmeta_expr(c);
		if (!checked_guard) {
			cerr << "ERROR in guard expression at " << where(*guard)
				<< endl;
			return return_type(NULL);
		}
	}
	// else with else clause, guard is allowed to be NULL
#if 1
	// NOTE this won't work once we implement template types
	const guarded_action::guard_ptr_type
		checked_bool_guard(checked_guard.is_a<bool_expr>());
	if (!have_else && !checked_bool_guard) {
		cerr << "ERROR expression at " << where(*guard) <<
			" is not boolean." << endl;
		return return_type(NULL);
	}
#else
	static const bool_traits::type_ref_ptr_type&
		bool_type_ptr(bool_traits::built_in_type_ptr);
	const count_ptr<const data_type_reference>
		gtype(checked_guard->get_data_type_ref());
	if (!gtype) {
		cerr << "Error resolving guard type at " << where(*guard)
			<< endl;
		return return_type(NULL);
	}
	if (!gtype->may_be_connectibly_type_equivalent(*bool_type_ptr)) {
		cerr << "Error: guard expression at " << where(*guard) <<
			" expected bool, but got: ";
		gtype->dump(cerr) << endl;
		return return_type(NULL);
	}
#endif
	checked_stmts_type checked_stmts;
	NEVER_NULL(command);
	const statement::return_type
		seq(command->check_action(c));
	return return_type(new guarded_action(checked_bool_guard, seq));
}

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Skip statements are allowed to return NULL, 
	and thus, must be handled exceptionally by calls to check_action.  
 */
statement::return_type
skip::check_action(context& c) const {
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
/**
	TODO: far future: handle template-dependent types
 */
statement::return_type
wait::check_action(context& c) const {
//	typedef	statement::return_type		return_type;
	const expr::nonmeta_return_type ret(cond->check_nonmeta_expr(c));
	if (!ret) {
		cerr << "ERROR in wait condition expression at " <<
			where(*cond) << endl;
		return return_type(NULL);
	}
	// this will work until we introduce template-dependent types
	const count_ptr<bool_expr> bret(ret.is_a<bool_expr>());
	if (!bret) {
		cerr << "ERROR: wait condition at " << where(*cond) <<
			" is not boolean." << endl;
		return return_type(NULL);
	} else {
		return return_type(new condition_wait(bret));
	}
}

//=============================================================================
// class binary_assignment method definitions

/**
	This constructor upgrades a regular parser::assign_stmt into
	a CHP-class binary_assignment statement.  
	The constructor releases the members of the assign_stmt
	and re-wraps them.  
	\param a the constructed assign_stmt.
 */
CONSTRUCTOR_INLINE
binary_assignment::binary_assignment(const inst_ref_expr* l, 
		const expr* r) : 
		parent_type(), lval(l), rval(r) {
	NEVER_NULL(lval);
	NEVER_NULL(rval);
}

DESTRUCTOR_INLINE
binary_assignment::~binary_assignment() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(binary_assignment)

line_position
binary_assignment::leftmost(void) const {
	return lval->leftmost();
}

line_position
binary_assignment::rightmost(void) const {
	return rval->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Type checks a nonmeta value assignment.  
 */
statement::return_type
binary_assignment::check_action(context& c) const {
	typedef	data_nonmeta_instance_reference			lref_type;
	const inst_ref_expr::nonmeta_data_return_type
		lr(lval->check_nonmeta_data_reference(c));
	if (!lr) {
		cerr << "Error resolving lvalue of assignment at " <<
			where(*lval) << endl;
		return statement::return_type(NULL);
	}
	const count_ptr<lref_type> lref(lr.is_a<lref_type>());
	if (!lref) {
		cerr << "Unsupported reference at " << where(*lval) << endl;
		cerr << "Sorry, currently only support simple datatype "
			"instance references, bug Fang about it." << endl;
		return statement::return_type(NULL);
	}
	// check that LHS is referenceable as an lvalue
	// e.g. parameters cannot be lvalues.  
	if (!lref->is_lvalue()) {
		cerr << "Error: left-side of expression is not an lvalue."
			<< endl;
		return statement::return_type(NULL);
	}
	const expr::nonmeta_return_type
		rv(rval->check_nonmeta_expr(c));
	if (!rv) {
		cerr << "Error in rvalue of assignment at " <<
			where(*rval) << endl;
		return statement::return_type(NULL);
	}
	// type-check
	if (lref->dimensions()) {
		cerr << "Sorry, non-scalar instance reference at " <<
			where(*lval) << " not supported in CHP yet." << endl;
		return statement::return_type(NULL);
	}
	const count_ptr<const data_type_reference>
		ltype(lref->get_data_type_ref());
	if (!ltype) {
		// may be template-dependent, but shouldn't be null
		cerr << "Error resolving type of lval at " <<
			where(*lval) << endl;
		return statement::return_type(NULL);
	}
	const count_ptr<const data_type_reference>
		rtype(rv->get_data_type_ref());
	if (!rtype) {
		// may be template-dependent, but shouldn't be null
		cerr << "Error resolving type of rval at " <<
			where(*rval) << endl;
		return statement::return_type(NULL);
	}
	// dimension-check
	if (lref->dimensions() != rv->dimensions()) {
		cerr << "Error: mismatch in dimensions of assignment." << endl
			<< "\tgot: " << lref->dimensions() << " at " <<
			where(*lval) << endl <<
			"\tand: " << rv->dimensions() << " at " <<
			where(*rval) << endl;
		return statement::return_type(NULL);
	}
	// used to be ...connectibly_type_equivalent()
	if (!ltype->may_be_assignably_type_equivalent(*rtype)) {
		cerr << "Type mismatch in assignment at " <<
			where(*this) << ':' << endl;
		ltype->dump(cerr << "\tleft: ") << endl;
		rtype->dump(cerr << "\tright: ") << endl;
		return statement::return_type(NULL);
	}
	// at this point, all is good
	return statement::return_type(new entity::CHP::assignment(lref, rv));
}	// end method binary_assignment::check_action

//=============================================================================
// class bool_assignment method definitions

CONSTRUCTOR_INLINE
bool_assignment::bool_assignment(const inst_ref_expr* l, 
		const char_punctuation_type* d) :
		parent_type(), bool_var(l), dir(d) {
	NEVER_NULL(bool_var);
	NEVER_NULL(dir);
}

DESTRUCTOR_INLINE
bool_assignment::~bool_assignment() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(bool_assignment)

line_position
bool_assignment::leftmost(void) const {
	return bool_var->leftmost();
}

line_position
bool_assignment::rightmost(void) const {
	return dir->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
statement::return_type
bool_assignment::check_action(context& c) const {
	typedef	entity::bool_traits	bool_traits;
	typedef	data_nonmeta_instance_reference			lref_type;
	static const bool_traits::type_ref_ptr_type&
		bool_type_ptr(bool_traits::built_in_type_ptr);
	const inst_ref_expr::nonmeta_data_return_type
		lr(bool_var->check_nonmeta_data_reference(c));
	if (!lr) {
		cerr << "Error resolving lvalue of assignment at " <<
			where(*bool_var) << endl;
		return statement::return_type(NULL);
	}
	const count_ptr<lref_type> lref(lr.is_a<lref_type>());
	if (!lref) {
		cerr << "Unsupported reference at " << where(*bool_var) << endl;
		cerr << "Sorry, currently only support simple datatype "
			"instance references, bug Fang about it." << endl;
		return statement::return_type(NULL);
	}
	if (lref->dimensions()) {
		cerr << "Sorry, non-scalar boolean reference at " <<
			where(*bool_var) << " not supported in CHP yet."
			<< endl;
		return statement::return_type(NULL);
	}
	// wait, there is no rvalue in this assignment, it is implicit
	// in the direction (dir: +/-) field.  
	const count_ptr<const data_type_reference>
		ltype(lref->get_data_type_ref());
	if (!ltype) {
		// may be template-dependent, but shouldn't be null
		cerr << "Error resolving type of lval at " <<
			where(*bool_var) << endl;
		return statement::return_type(NULL);
	}
	if (!ltype->may_be_connectibly_type_equivalent(*bool_type_ptr)) {
		cerr << "Type mismatch in boolean assignment at " <<
			where(*this) << ':' << endl;
		ltype->dump(cerr << "\tgot: ") << endl;
		return statement::return_type(NULL);
	}
	// at this point, all is good
	return statement::return_type(
		new entity::CHP::assignment(lref, 
			count_ptr<entity::pbool_const>(
				new entity::pbool_const(dir->text[0] == '+'))));
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
	Checks list of expressions send across channel.  
 */
statement::return_type
send::check_action(context& c) const {
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

	typedef	count_ptr<entity::CHP::channel_send>	local_return_type;
	const local_return_type ret(new entity::CHP::channel_send(sender));
	// need to check that number of arguments match...
	NEVER_NULL(ret);
	const good_bool g(ret->add_expressions(checked_exprs));
	if (!g.good) {
		cerr << "At least one type error in expr-list in " <<
			where(*rvalues) << endl;
		return statement::return_type(NULL);
	} else {
		return ret;
	}
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
statement::return_type
receive::check_action(context& c) const {
	const communication::checked_channel_type
		receiver(check_channel(c));
	typedef	data_nonmeta_instance_reference			lref_type;
	if (!receiver) {
		return statement::return_type(NULL);
	}
	if (get_channel_direction(*receiver) == '!') {
		cerr << "ERROR: cannot receive on a send-only channel, at " <<
			where(*chan) << endl;
		return statement::return_type(NULL);
	}
	// check expression list...
	typedef	inst_ref_expr_list::checked_nonmeta_data_refs_type::
		value_type::element_type		checked_element_type;

	typedef	inst_ref_expr_list::checked_nonmeta_data_refs_type::const_iterator
							const_iterator;
	// NOTE: these are generic instance references, may not even be data
	inst_ref_expr_list::checked_nonmeta_data_refs_type checked_refs;
	lvalues->postorder_check_nonmeta_data_refs(checked_refs, c);
	const_iterator i(checked_refs.begin());
	const const_iterator e(checked_refs.end());
	const const_iterator
		ni(find(i, e, inst_ref_expr::nonmeta_data_return_type(NULL)));
	if (ni != e) {
		cerr << "At least one error in inst-ref-list at " <<
			where(*lvalues) << endl;
		return statement::return_type(NULL);
	}
	// need to dynamic cast the list into simple_nonmeta_datatype_value_refs
	typedef vector<count_ptr<lref_type> >
						val_refs_type;
	val_refs_type val_refs;
	transform(i, e, back_inserter(val_refs), 
		mem_fun_ref(&count_ptr<checked_element_type>::is_a<lref_type>)
	);
	if (find(val_refs.begin(), val_refs.end(), val_refs_type::value_type())
			!= val_refs.end()) {
		cerr << "Houston, we have a problem." << endl;
		return statement::return_type(NULL);
	}

	typedef	count_ptr<entity::CHP::channel_receive>	local_return_type;
	const local_return_type ret(new entity::CHP::channel_receive(receiver));
	// need to check that number of arguments match...
	NEVER_NULL(ret);
	const good_bool g(ret->add_references(val_refs));
	if (!g.good) {
		cerr << "At least one type error in expr-list in " <<
			where(*lvalues) << endl;
		return statement::return_type(NULL);
	} else {
		return ret;
	}
}

//=============================================================================
// abstract class selection method definitions

CONSTRUCTOR_INLINE
selection::selection() : statement(), list_type() { }

CONSTRUCTOR_INLINE
selection::selection(const guarded_command* n) : statement(), list_type(n) {
	NEVER_NULL(n);
}

DESTRUCTOR_INLINE
selection::~selection() { }

line_position
selection::leftmost(void) const {
	return list_type::leftmost();
}

line_position
selection::rightmost(void) const {
	return list_type::rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
selection::postorder_check_gcs(checked_gcs_type& gl, context& c) const {
	INVARIANT(size() > 1);		// otherwise, not a selection!
	check_list(gl, &guarded_command::check_guarded_action, c);
	typedef	checked_gcs_type::const_iterator	const_checked_iterator;
	const const_checked_iterator ci(gl.begin());
	const const_checked_iterator ce(gl.end());
	const const_checked_iterator
		ni(find(ci, ce, guarded_command::return_type(NULL)));
	if (ni != ce) {
		cerr << "At least one error in guarded statement list in " <<
			where(*this) << endl;
		return good_bool(false);
	} else {
		return good_bool(true);
	}
}

//=============================================================================
// class det_selection method definitions

CONSTRUCTOR_INLINE
det_selection::det_selection(const guarded_command* n) :
		parent_type(n) {
}

DESTRUCTOR_INLINE
det_selection::~det_selection() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(det_selection)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
statement::return_type
det_selection::check_action(context& c) const {
	checked_gcs_type checked_gcs;	// checked guarded commands
	if (!postorder_check_gcs(checked_gcs, c).good) {
		// already have error message
		return statement::return_type(NULL);
	}
	const count_ptr<entity::CHP::deterministic_selection>
		ret(new entity::CHP::deterministic_selection);
	NEVER_NULL(ret);
	copy(checked_gcs.begin(), checked_gcs.end(), back_inserter(*ret));
	return ret;
}

//=============================================================================
// class nondet_selection method definitions

CONSTRUCTOR_INLINE
nondet_selection::nondet_selection(const guarded_command* n) :
		parent_type(n) {
}

DESTRUCTOR_INLINE
nondet_selection::~nondet_selection() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(nondet_selection)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
statement::return_type
nondet_selection::check_action(context& c) const {
	checked_gcs_type checked_gcs;	// checked guarded commands
	if (!postorder_check_gcs(checked_gcs, c).good) {
		// already have error message
		return statement::return_type(NULL);
	}
	const count_ptr<entity::CHP::nondeterministic_selection>
		ret(new entity::CHP::nondeterministic_selection);
	NEVER_NULL(ret);
	copy(checked_gcs.begin(), checked_gcs.end(), back_inserter(*ret));
	return ret;
}

//=============================================================================
// class prob_selection method definitions

CONSTRUCTOR_INLINE
prob_selection::prob_selection(const guarded_command* n) :
		parent_type(n) {
}

DESTRUCTOR_INLINE
prob_selection::~prob_selection() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(prob_selection)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
statement::return_type
prob_selection::check_action(context& c) const {
	cerr << "Fang, finish CHP::prob_selection::check_action()!" << endl;
	return statement::return_type(NULL);
}

//=============================================================================
// class metaloop_selection method definitions

metaloop_selection::metaloop_selection(const char_punctuation_type* l, 
		const char_punctuation_type* st, 
		const token_identifier* i, 
		const range* b, 
		const guarded_command* gc, 
		const char_punctuation_type* r) :
		parent_type(), 
		lb(l), selection_type(st), index(i), bounds(b), 
		body(gc), rb(r) {
	NEVER_NULL(selection_type);
	NEVER_NULL(index);
	NEVER_NULL(bounds);
	NEVER_NULL(body);
}

metaloop_selection::~metaloop_selection() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(metaloop_selection)

line_position
metaloop_selection::leftmost(void) const {
	if (lb)	return lb->leftmost();
	else	return index->leftmost();
}

line_position
metaloop_selection::rightmost(void) const {
	if (rb)	return rb->rightmost();
	else	return body->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
statement::return_type
metaloop_selection::check_action(context& c) const {
	const range::meta_return_type rng(bounds->check_meta_index(c));
	if (!rng) {
		cerr << "Error in loop range at " << where(*bounds) << endl;
		return return_type(NULL);
	}
	const entity::CHP::metaloop_selection::range_ptr_type
		loop_range(meta_range_expr::make_explicit_range(rng));
	NEVER_NULL(loop_range);
	// induction variable scope in effect until return
	const context::loop_var_frame _lvf(c, *index);
	const count_ptr<pint_scalar>& loop_ind(_lvf.var);
	if (!loop_ind) {
		cerr << "Error registering loop variable: " << *index <<
			" at " << where(*index) << endl;
		return return_type(NULL);
	}
	const guarded_command::return_type
		gc(body->check_guarded_action(c));
	if (!gc) {
		cerr << "Error in guarded-command body: at " << where(*body)
			<< endl;
		return return_type(NULL);
	}
	// selection type is either ":" (nondet) or "[]" (deterministic)
	return count_ptr<entity::CHP::metaloop_selection>(
		new entity::CHP::metaloop_selection(loop_ind, loop_range, gc, 
			selection_type->text[0] != ':'));
}	// end method metaloop_selection::check_action

//=============================================================================
// class loop method definitions

CONSTRUCTOR_INLINE
loop::loop(const stmt_list* n) : statement(), commands(n) {
	NEVER_NULL(commands);
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
	STACKTRACE_VERBOSE;
	const statement::return_type
		body(commands->check_action(c));
	if (!body) {
		// already printed error message
		cerr << "ERROR in one of the actions in "
			<< where(*this) << endl;
		return statement::return_type(NULL);
	} else {
		return statement::return_type(
			new entity::CHP::do_forever_loop(body));
	}
}

//=============================================================================
// class do_until method definitions

CONSTRUCTOR_INLINE
do_until::do_until(const det_selection* n) : statement(), sel(n) { }

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
	selection::checked_gcs_type checked_gcs;
	if (!sel->postorder_check_gcs(checked_gcs, c).good) {
		return statement::return_type(NULL);
	}
	const count_ptr<entity::CHP::do_while_loop>
		ret(new entity::CHP::do_while_loop);
	NEVER_NULL(ret);
	copy(checked_gcs.begin(), checked_gcs.end(), back_inserter(*ret));
	return ret;
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
}	// end namespace CHP

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __HAC_AST_CHP_CC__

