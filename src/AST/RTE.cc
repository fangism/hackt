/**
	\file "AST/RTE.cc"
	RTE-related syntax class method definitions.
	$Id: RTE.cc,v 1.44 2011/02/08 02:06:44 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <iterator>
#include <algorithm>
#include <functional>

#include "AST/RTE.hh"
#include "AST/reference.hh"	// for id_expr
#include "AST/range.hh"
#include "AST/range_list.hh"
#include "AST/token.hh"
#include "AST/token_char.hh"
#include "AST/token_string.hh"
#include "AST/node_list.tcc"
#include "AST/parse_context.hh"

#include "Object/def/process_definition.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/pbool_expr.hh"
#include "Object/expr/param_expr.hh"
#include "Object/expr/dynamic_param_expr_list.hh"
#include "Object/expr/data_expr.hh"
#include "Object/expr/pstring_expr.hh"
#include "Object/expr/meta_range_expr.hh"
#include "Object/expr/meta_index_list.hh"
#include "Object/lang/RTE.hh"
#include "Object/inst/pint_value_collection.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/ref/meta_instance_reference_base.hh"
#include "Object/ref/meta_instance_reference_subtypes.hh" // for conversion
#include "Object/ref/nonmeta_instance_reference_subtypes.hh"
#include "Object/ref/simple_meta_instance_reference.hh"	// for conversion
#include "Object/ref/meta_reference_union.hh"
#include "Object/module.hh"
#include "Object/ref/simple_meta_dummy_reference.hh"
#include "Object/ref/references_fwd.hh"
#include "Object/traits/node_traits.hh"

#include "common/ICE.hh"
#include "common/TODO.hh"

#include "util/what.hh"
#include "util/stacktrace.hh"
#include "util/memory/count_ptr.tcc"

#define	CONSTRUCTOR_INLINE
#define	DESTRUCTOR_INLINE

// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::RTE::assignment, "(rte-assignment)")
SPECIALIZE_UTIL_WHAT(HAC::parser::RTE::body, "(rte-body)")
#if 0
SPECIALIZE_UTIL_WHAT(HAC::parser::RTE::loop, "(rte-loop)")
SPECIALIZE_UTIL_WHAT(HAC::parser::RTE::conditional, "(rte-conditional)")
SPECIALIZE_UTIL_WHAT(HAC::parser::RTE::guarded_body, "(rte-guarded-body)")
SPECIALIZE_UTIL_WHAT(HAC::parser::RTE::op_loop, "(rte-op-loop)")
#endif

namespace memory {
// explicit template instantiations
using namespace HAC::parser::RTE;
template class count_ptr<const body_item>;
// template class count_ptr<const guarded_body>;
}	// end namespace memory
}	// end namespace util

namespace HAC {
namespace parser {
namespace RTE {
using std::back_inserter;
#include "util/using_ostream.hh"
using entity::definition_base;
using entity::process_definition;
using entity::pint_scalar;
using entity::pbool_expr;
using entity::meta_range_expr;
using entity::meta_loop_base;
using std::find;
using std::find_if;
using std::mem_fun_ref;

//=============================================================================
// class body_item method definitions

#define	CHECK_ASSIGN_THROW	THROW_EXIT
#define	GUARDED_RTE_THROW	THROW_EXIT

body_item::body_item() { }

body_item::~body_item() { }

//=============================================================================
// class assignment method definitions

/**
	Constructor.
 */
CONSTRUCTOR_INLINE
assignment::assignment(const inst_ref_expr* l, const expr* r) :
		lvalue(l), rvalue(r) {
	NEVER_NULL(l); NEVER_NULL(r);
}

DESTRUCTOR_INLINE
assignment::~assignment() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(assignment)

line_position
assignment::leftmost(void) const {
	return lvalue->leftmost();
}

line_position
assignment::rightmost(void) const {
	return rvalue->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	See PRS::rule::check_rule() for reference.
 */
body_item::return_type
assignment::check_assignment(context& c) const {
	STACKTRACE_VERBOSE;
	INVARIANT(c.inside_atomic_rte());
	// TODO: change prs to rte types
	rte_expr_return_type r(rvalue->check_rte_expr(c));
		// make this work for RTE as well
	if (!r) {
		cerr << "ERROR in atomic run-time expression at " <<
			where(*rvalue) << "." << endl;
		THROW_EXIT;
	}
	rte_lvalue_ptr_type l(lvalue->check_rte_lvalue(c));
	if (!l) {
		cerr << "ERROR in run-time expression lvalue at " <<
			where(*lvalue) << "." << endl;
		THROW_EXIT;
	}
	FINISH_ME(Fang);
}

//=============================================================================
#if 0
// class loop method definitions

loop::loop(const char_punctuation_type* l,
		const token_identifier* id, const range* b,
		const assignment_list* rl, const char_punctuation_type* r) :
		body_item(), 
		lp(l), index(id), bounds(b), 
		assignments(rl), rp(r) {
	NEVER_NULL(index); NEVER_NULL(bounds); NEVER_NULL(assignments);
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
	else		return assignments->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: have RTE::rule return a entity::RTE::rule.
 */
body_item::return_type
loop::check_assignment(context& c) const {
	STACKTRACE_VERBOSE;
	// declare induction variable using token_identifier
	// check for shadowing by looking up
	// extend/modify the parse context with token_identifier on stack
	// type-check the range expression
	// type-check the inside expression with modified context
	const range::meta_return_type rng(bounds->check_meta_index(c));
	if (!rng) {
		cerr << "Error in loop range at " << where(*bounds) << endl;
		// bounds->dump(cerr) <<
		return return_type(NULL);
	}
	// convert implicit range to explicit range, if necessary
	entity::RTE::expr_loop_base::range_ptr_type
		loop_range(meta_range_expr::make_explicit_range(rng));
	NEVER_NULL(loop_range);
	// create loop index variable and push onto context stack
	const context::loop_var_frame _lvf(c, *index);
	const meta_loop_base::ind_var_ptr_type& loop_ind(_lvf.var);
	if (!loop_ind) {
		// then push didn't succeed, no need to pop
		cerr << "Error registering loop variable: " << *index <<
			" at " << where(*index) << endl;
		return return_type(NULL);
	}
	excl_ptr<entity::RTE::rule_loop>
		ret(new entity::RTE::rule_loop(loop_ind, loop_range));
	NEVER_NULL(ret);

	// copied from body::check_assignment
	// const never_ptr<definition_base> d(c.get_current_open_definition());
	never_ptr<entity::RTE::rule_loop> retc(ret);
	c.get_current_rte_body().append_rule(ret);
try {
	const context::rte_body_frame prlf(c, retc);
	assignments->check_list_void(&body_item::check_assignment, c);
} catch (...) {
	cerr << "ERROR: at least one error in RTE rule-loop.  "
		<< where(*assignments) << endl;
	throw;		// re-throw
}
	if (retc->empty()) {
		c.get_current_rte_body().pop_back();
	}
}	// end method loop::check_assignment

//=============================================================================
// class guarded_body method definitions

guarded_body::guarded_body(const expr* g, const char_punctuation_type* a, 
		const assignment_list* r) :
		guard(g), arrow(a), assignments(r) {
	// guard may be NULL (else-clause)
	NEVER_NULL(arrow);
	NEVER_NULL(assignments);
}

guarded_body::~guarded_body() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(guarded_body)

line_position
guarded_body::leftmost(void) const {
	if (guard)
		return guard->leftmost();
	else	return arrow->leftmost();
}

line_position
guarded_body::rightmost(void) const {
	return assignments->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
guarded_body::return_type
guarded_body::check_clause(context& c) const {
	STACKTRACE_VERBOSE;
	expr::meta_return_type _g(NULL);
	entity::meta_conditional_base::guard_ptr_type bg(NULL);
	if (guard && !guard.is_a<const token_else>()) {
		_g = guard->check_meta_expr(c);
		if (!_g) {
			cerr << "Error checking guard expression of "
				"conditional RTE.  " << where(*guard) << endl;
			GUARDED_RTE_THROW;
		} else {
		bg = _g.is_a<const pbool_expr>();
		if (!bg) {
			cerr << "Error: guard expression is not boolean.  " <<
				where(*guard) << endl;
			GUARDED_RTE_THROW;
		}
		}
	}
	// see also guarded_instance_management::check_build()
	const never_ptr<entity::RTE::rule_conditional>
		rs(IS_A(entity::RTE::rule_conditional*,
			&*c.get_current_rte_body().back()));
	NEVER_NULL(rs);
	rs->append_guarded_clause(bg);
	const context::rte_body_frame _pbf(c, 
		never_ptr<entity::RTE::assignment_set_base>(&rs->get_last_clause()));
	STACKTRACE_INDENT_PRINT("current rule set: " <<
		&rs->get_last_clause() << endl);
	// code below mostly ripped from loop::check_assignment()
try {
	assignments->check_list_void(&body_item::check_assignment, c);
} catch (...) {
	cerr << "ERROR: at least one error in conditional RTE assignments.  "
		<< where(*assignments) << endl;
}
}	// end guarded_body::check_clause

//=============================================================================
// class conditional method definitions

conditional::conditional(const guarded_rte_list* p) : gp(p) {
	NEVER_NULL(gp);
}

conditional::~conditional() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(conditional)

line_position
conditional::leftmost(void) const {
	return gp->leftmost();
}

line_position
conditional::rightmost(void) const {
	return gp->leftmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: generalize to multiple else-if clauses.
 */
body_item::return_type
conditional::check_assignment(context& c) const {
	STACKTRACE_VERBOSE;
	// see also conditional_instantiation::check_build()
	excl_ptr<entity::RTE::rule_conditional>
		rs(new entity::RTE::rule_conditional());
	const never_ptr<const entity::RTE::rule_conditional> crs(rs);
	c.get_current_rte_body().append_rule(rs);	// xfer ownership
	MUST_BE_NULL(rs);
	gp->check_list_void(&guarded_body::check_clause, c);
        if (crs->empty()) {
                c.get_current_rte_body().pop_back();
        } 
}	// end conditional::check_assignment
#endif

//=============================================================================
// class body method definitions

CONSTRUCTOR_INLINE
body::body(const generic_keyword_type* t, 
		const assignment_list* r) :
		language_body(t), 
		assignments(r) {
	if (r) NEVER_NULL(assignments);
}

DESTRUCTOR_INLINE
body::~body() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(body)

line_position
body::leftmost(void) const {
	return language_body::leftmost();
}

line_position
body::rightmost(void) const {
	return assignments->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return false on error.
 */
bool
body::__check_assignments(context& c) const {
if (assignments) {
	try {
		assignments->check_list_void(&body_item::check_assignment, c);
	} catch (...) {
		return false;
	}
}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: remember to update return type with ROOT_CHECK_PROTO.
	Currently, exits upon error.  
	Added support for rte inside conditionals and loops from the
	instance management scope.  
	\pre context has already set the current_rte_body.
 */
never_ptr<const object>
body::check_build(context& c) const {
	STACKTRACE_VERBOSE;
if (assignments) {
	if (c.reject_namespace_lang_body()) {
		cerr << "Error: top-level RTE is only supported "
			"in the global namespace." << endl;
		cerr << "\tgot: rte { ... } " << where(*this)
			<< endl;
		THROW_EXIT;
	}
	// check context's current open definition
	const never_ptr<definition_base> d(c.get_current_open_definition());
	const never_ptr<process_definition> pd(d.is_a<process_definition>());
	// if !pd, then rte is in a top-level scope (outside definition)

	using entity::RTE::assignment_set;
	excl_ptr<assignment_set> ret(new assignment_set());
	NEVER_NULL(ret);
	const never_ptr<assignment_set> retc(ret);
	entity::RTE::assignment_set_base& rb(c.get_current_rte_body());
	rb.append_assignment(ret);

	const context::rte_body_frame rf(c, retc);
	if (!__check_assignments(c)) {
		cerr << "ERROR: at least one error in RTE body."
			<< endl;
		THROW_EXIT;
	}
}
	// else empty, no RTE to add
	return never_ptr<const object>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Check assignment statements, semantically.
 */
body_item::return_type
body::check_assignment(context& c) const {
	if (!__check_assignments(c)) {
		cerr << "ERROR: at least one error in RTE rule-nest.  "
			<< where(*assignments) << endl;
		CHECK_ASSIGN_THROW;
	}
}

//=============================================================================
#if 0
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

ostream&
op_loop::dump(ostream& o) const {
	FINISH_ME(Fang);
	return o;
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
	Is this even needed outside of the RTE context?
 */
expr::meta_return_type
op_loop::check_meta_expr(const context& c) const {
	FINISH_ME(Fang);
	return expr::meta_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: FINISH ME
	Is this even needed outside of the RTE context?
 */
nonmeta_expr_return_type
op_loop::check_nonmeta_expr(const context& c) const {
	FINISH_ME(Fang);
	return nonmeta_expr_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rte_expr_return_type
op_loop::check_rte_expr(context& c) const {
	// declare induction variable using token_identifier
	// check for shadowing by looking up
	// extend/modify the parse context with token_identifier on stack
	// type-check the range expression
	// type-check the inside expression with modified context
	const range::meta_return_type rng(bounds->check_meta_index(c));
	if (!rng) {
		cerr << "Error in loop range at " << where(*bounds) << endl;
		// bounds->dump(cerr) <<
		return rte_expr_return_type(NULL);
	}
	// convert implicit range to explicit range, if necessary
	const entity::RTE::expr_loop_base::range_ptr_type
		loop_range(meta_range_expr::make_explicit_range(rng));
	NEVER_NULL(loop_range);
	// create loop index variable and push onto context stack
	const context::loop_var_frame _lvf(c, *index);
	const meta_loop_base::ind_var_ptr_type& loop_ind(_lvf.var);
	if (!loop_ind) {
		// then push didn't succeed, no need to pop
		cerr << "Error registering loop variable: " << *index <<
			" at " << where(*index) << endl;
		return rte_expr_return_type(NULL);
	}
	const rte_expr_return_type body_expr(ex->check_rte_expr(c));
	if (!body_expr) {
		cerr << "Error in expr-loop body: at " << where(*ex) << endl;
		// ex->dump(cerr) <<
		return rte_expr_return_type(NULL);
	}
	// else everything passes
	return (op->text[0] == '&')
		? rte_expr_return_type(new entity::RTE::and_expr_loop(
			loop_ind, loop_range, body_expr))
		: rte_expr_return_type(new entity::RTE::or_expr_loop(
			loop_ind, loop_range, body_expr));
}
#endif

//=============================================================================
#if 0
// class attribute method definitions

rule::attribute_type
rule::check_rte_attribute(const generic_attribute& a, context& c) {
	typedef	attribute_type			return_type;
	typedef	expr_list::checked_meta_exprs_type vals_type;
	typedef	vals_type::const_iterator	const_iterator;
	typedef	vals_type::value_type		val_type;
	// all macros must be registered with the master registry list (cflat)
	const entity::RTE::cflat_rule_attribute_registry_type::const_iterator
		f(entity::RTE::cflat_rule_attribute_registry.find(*a.key));
	if (f == entity::RTE::cflat_rule_attribute_registry.end()) {
		cerr << "Error: unrecognized RTE rule attribute \"" << *a.key <<
			"\" at " << where(*a.key) << endl;
		return return_type();
	}
	vals_type vals;
	if (a.values) {
		a.values->postorder_check_meta_exprs(vals, c);
	}
	const const_iterator i(vals.begin()), e(vals.end());
	if (find(i, e, val_type(NULL)) != e) {
		// one of the param expressions failed checking
		// blank will signal error
		cerr << "Error in checking attribute value expressions in "
			<< (a.values ? where(*a.values) : where(a)) << endl;
		return return_type();
	}
	return_type ret(*a.key);
	copy(i, e, back_inserter(ret));
	return ret;
}
#endif

//=============================================================================
}	// end namespace RTE

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

// template class node_list<const body_item>;		// RTE::assignment_list

#if 1
// This is temporary, until node_list::check_build is overhauled.  
template
node_list<const RTE::body_item>::node_list();

template
node_list<const RTE::body_item>::node_list(const RTE::body_item*);

template
node_list<const RTE::body_item>::~node_list();

template
ostream&
node_list<const RTE::body_item>::what(ostream&) const;

template
line_position
node_list<const RTE::body_item>::leftmost(void) const;

template
line_position
node_list<const RTE::body_item>::rightmost(void) const;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

