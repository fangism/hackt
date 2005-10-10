/**
	\file "AST/art_parser_prs.cc"
	PRS-related syntax class method definitions.
	$Id: art_parser_prs.cc,v 1.19.20.2 2005/10/10 22:13:43 fang Exp $
 */

#ifndef	__AST_ART_PARSER_PRS_CC__
#define	__AST_ART_PARSER_PRS_CC__

#define	ENABLE_STACKTRACE		0

#include <iostream>

#include "AST/art_parser_prs.h"
#include "AST/art_parser_expr.h"		// for id_expr
#include "AST/art_parser_range.h"
#include "AST/art_parser_token.h"
#include "AST/art_parser_token_char.h"
#include "AST/art_parser_token_string.h"
#include "AST/art_parser_node_list.tcc"
#include "AST/parse_context.h"

#include "Object/def/process_definition.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/param_expr.h"
#include "Object/expr/data_expr.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/lang/PRS.h"
#include "Object/inst/pint_value_collection.h"

#include "common/TODO.h"

#include "util/what.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"

#define	CONSTRUCTOR_INLINE
#define	DESTRUCTOR_INLINE

// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(ART::parser::PRS::rule, "(prs-rule)")
SPECIALIZE_UTIL_WHAT(ART::parser::PRS::loop, "(prs-loop)")
SPECIALIZE_UTIL_WHAT(ART::parser::PRS::body, "(prs-body)")
SPECIALIZE_UTIL_WHAT(ART::parser::PRS::op_loop, "(prs-op-loop)")
}

namespace ART {
namespace parser {
namespace PRS {
#include "util/using_ostream.h"
using entity::definition_base;
using entity::process_definition;
using entity::pint_scalar;
using entity::meta_range_expr;

//=============================================================================
// class body_item method definitions

body_item::body_item() { }

body_item::~body_item() { }

//=============================================================================
// class rule method definitions

CONSTRUCTOR_INLINE
rule::rule(const expr* g, const char_punctuation_type* a,
		const inst_ref_expr* rhs, const char_punctuation_type* d) :
		body_item(), guard(g), arrow(a),
		r(rhs), dir(d) {
	NEVER_NULL(guard); NEVER_NULL(arrow); NEVER_NULL(r); NEVER_NULL(dir);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Type-checks and constructs a production rule.  
	\return a newly constructed, type-checked production rule, 
		to be added to a definition.  
 */
body_item::return_type
rule::check_rule(context& c) const {
	STACKTRACE("parser::PRS::rule::check_rule()");
	prs_expr_return_type g(guard->check_prs_expr(c));
	if (!g) {
		cerr << "ERROR in production rule guard at " <<
			where(*guard) << "." << endl;
		THROW_EXIT;
	}
//	g->check();	// paranoia
	prs_literal_ptr_type o(r->check_prs_literal(c));
	if (!o) {
		cerr << "ERROR in the output node reference at " <<
			where(*r) << "." << endl;
		THROW_EXIT;
	}
	// temporary support for normal arrow only!
	const bool arrow_type = (arrow->text[0] == '=');
	return body_item::return_type((dir->text[0] == '+') ?
		static_cast<entity::PRS::rule*>(
			new entity::PRS::pull_up(g, *o, arrow_type)) :
		static_cast<entity::PRS::rule*>(
			new entity::PRS::pull_dn(g, *o, arrow_type)));
}

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
body_item::return_type
loop::check_rule(context& c) const {
	cerr << "Fang, write PRS::loop::check_rule()!" << endl;
	return body_item::return_type();
}

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: remember to update return type with ROOT_CHECK_PROTO.
	Currently, exits upon error.  
 */
never_ptr<const object>
body::check_build(context& c) const {
	STACKTRACE("PRS::body::check_build()");
if (rules) {
	// check context's current open definition
	const never_ptr<definition_base> d(c.get_current_open_definition());
	const never_ptr<process_definition> pd(d.is_a<process_definition>());
	NEVER_NULL(pd);
	checked_rules_type checked_rules;
	rules->check_list(checked_rules, &body_item::check_rule, c);
	checked_rules_type::const_iterator
		null_iter = find(checked_rules.begin(), 
			checked_rules.end(), 
			body_item::return_type());
	if (null_iter == checked_rules.end()) {
		// no errors found, add them too the process definition
		checked_rules_type::iterator i = checked_rules.begin();
		for ( ; i!=checked_rules.end(); i++) {
			excl_ptr<entity::PRS::rule>
				xfer(i->exclusive_release());
//			xfer->check();		// paranoia
			pd->add_production_rule(xfer);
		}
	} else {
		cerr << "ERROR: at least one error in PRS body."
			<< endl;
		THROW_EXIT;
	}
}
	// else empty, no PRS to add
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class op_loop method definitions

op_loop::op_loop(const char_punctuation_type* l, 
		const char_punctuation_type* o,
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: FINISH ME
	Is this even needed outside of the PRS context?
 */
expr::meta_return_type
op_loop::check_meta_expr(context& c) const {
	FINISH_ME(Fang);
	return expr::meta_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: FINISH ME
	Is this even needed outside of the PRS context?
 */
nonmeta_expr_return_type
op_loop::check_nonmeta_expr(context& c) const {
	FINISH_ME(Fang);
	return nonmeta_expr_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_return_type
op_loop::check_prs_expr(context& c) const {
	// declare induction variable using token_identifier
	// check for shadowing by looking up
	// extend/modify the parse context with token_identifier on stack
	// type-check the range expression
	// type-check the inside expression with modified context
	const range::meta_return_type rng(bounds->check_meta_index(c));
	if (!rng) {
		cerr << "Error in loop range at " << where(*bounds) << endl;
		// bounds->dump(cerr) <<
		return prs_expr_return_type(NULL);
	}
	// convert implicit range to explicit range, if necessary
	entity::PRS::expr_loop_base::range_ptr_type
		loop_range(meta_range_expr::make_explicit_range(rng));
	NEVER_NULL(loop_range);
	// create loop index variable and push onto context stack
	const count_ptr<pint_scalar>
		loop_ind(c.push_loop_var(*index));
	if (!loop_ind) {
		// then push didn't succeed, no need to pop
		cerr << "Error registering loop variable: " << *index <<
			" at " << where(*index) << endl;
		return prs_expr_return_type(NULL);
	}
	const prs_expr_return_type body_expr(ex->check_prs_expr(c));
	if (!body_expr) {
		cerr << "Error in expr-loop body: at " << where(*ex) << endl;
		// ex->dump(cerr) <<
		return prs_expr_return_type(NULL);
	}
	// else everything passes
	c.pop_loop_var();
	return (op->text[0] == '&')
		? prs_expr_return_type(new entity::PRS::and_expr_loop(
			loop_ind, loop_range, body_expr))
		: prs_expr_return_type(new entity::PRS::or_expr_loop(
			loop_ind, loop_range, body_expr));
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

