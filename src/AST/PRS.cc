/**
	\file "AST/PRS.cc"
	PRS-related syntax class method definitions.
	$Id: PRS.cc,v 1.33 2009/06/05 16:28:04 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_prs.cc,v 1.21.10.1 2005/12/11 00:45:09 fang Exp
 */

#ifndef	__HAC_AST_PRS_CC__
#define	__HAC_AST_PRS_CC__

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <iterator>
#include <algorithm>
#include <functional>

#include "AST/PRS.h"
#include "AST/reference.h"	// for id_expr
#include "AST/expr_list.h"	// for attributes
#include "AST/attribute.h"
#include "AST/range.h"
#include "AST/range_list.h"
#include "AST/token.h"
#include "AST/token_char.h"
#include "AST/token_string.h"
#include "AST/node_list.tcc"
#include "AST/parse_context.h"

#include "Object/def/process_definition.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_expr.h"
#include "Object/expr/param_expr.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/data_expr.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/expr/meta_index_list.h"
#include "Object/lang/PRS.h"
#include "Object/lang/PRS_attribute_registry.h"
#include "Object/lang/PRS_macro_registry.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/traits/bool_traits.h"
#include "Object/ref/meta_instance_reference_base.h"
#include "Object/ref/meta_instance_reference_subtypes.h" // for conversion
#include "Object/ref/nonmeta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"	// for conversion
#include "Object/ref/meta_reference_union.h"
#include "Object/module.h"
#include "Object/ref/simple_meta_dummy_reference.h"
#include "Object/ref/references_fwd.h"
#include "Object/traits/node_traits.h"

#include "common/TODO.h"

#include "util/what.h"
#include "util/stacktrace.h"
#include "util/qmap.tcc"
#include "util/memory/count_ptr.tcc"

#define	CONSTRUCTOR_INLINE
#define	DESTRUCTOR_INLINE

// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::PRS::rule, "(prs-rule)")
SPECIALIZE_UTIL_WHAT(HAC::parser::PRS::literal, "(prs-literal)")
SPECIALIZE_UTIL_WHAT(HAC::parser::PRS::precharge, "(prs-precharge)")
SPECIALIZE_UTIL_WHAT(HAC::parser::PRS::loop, "(prs-loop)")
SPECIALIZE_UTIL_WHAT(HAC::parser::PRS::conditional, "(prs-conditional)")
SPECIALIZE_UTIL_WHAT(HAC::parser::PRS::body, "(prs-body)")
SPECIALIZE_UTIL_WHAT(HAC::parser::PRS::guarded_body, "(prs-guarded-body)")
SPECIALIZE_UTIL_WHAT(HAC::parser::PRS::op_loop, "(prs-op-loop)")

namespace memory {
// explicit template instantiations
using namespace HAC::parser::PRS;
template class count_ptr<const body_item>;
template class count_ptr<const guarded_body>;
}	// end namespace memory
}	// end namespace util

namespace HAC {
namespace parser {
namespace PRS {
using std::back_inserter;
#include "util/using_ostream.h"
using entity::definition_base;
using entity::process_definition;
using entity::pint_scalar;
using entity::pbool_expr;
using entity::meta_range_expr;
using entity::meta_loop_base;
using entity::PRS::pull_base;
using std::find;
using std::find_if;
using std::mem_fun_ref;

//=============================================================================
// class body_item method definitions

#define	CHECK_RULE_THROW	THROW_EXIT
#define	GUARDED_PRS_THROW	THROW_EXIT

body_item::body_item() { }

body_item::~body_item() { }

//=============================================================================
// class literal method definitions

literal::literal(inst_ref_expr* r, const expr_list* p) :
		ref(r), params(p), internal(false) {
	NEVER_NULL(ref);
	// params are optional
}

literal::literal(inst_ref_expr* r) :
		ref(r), params(NULL), internal(false) {
	NEVER_NULL(ref);
	// params are optional
}

literal::~literal() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(literal)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Destructively releases and exposes underlying identifier
	if it is a simple id_expr, else returns NULL (no error message).
	\return token_identifier if instance_reference is a 
		single unqualified ID.  
 */
excl_ptr<const token_identifier>
literal::extract_identifier(void) {
	typedef	excl_ptr<const token_identifier>	return_type;
	excl_ptr<id_expr> idex = ref.is_a_xfer<id_expr>();
	if (!idex)
		return return_type(NULL);
	excl_ptr<qualified_id> qid = idex->release_qualified_id();
	NEVER_NULL(qid);
	if (qid->is_absolute())
		return return_type(NULL);
	if (qid->size() > 1)
		return return_type(NULL);
	qualified_id_base::reference id(qid->front());
	return_type ret(id.exclusive_release());	// transfer ownership
	NEVER_NULL(ret);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Releases the parameters list to the caller.  
 */
excl_ptr<const expr_list>
literal::extract_parameters(void) {
	return params;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param e is exclusivel owned (or freshly allocated).
 */
void
literal::attach_parameters(const expr_list* e) {
	params = excl_ptr<const expr_list>(e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
literal::leftmost(void) const {
	return ref->leftmost();
}

line_position
literal::rightmost(void) const {
	if (params)
		return params->rightmost();
	else	return ref->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: use different lookup for internal nodes
 */
prs_literal_ptr_type
literal::check_prs_literal(const context& c) const {
	STACKTRACE_VERBOSE;
	prs_literal_ptr_type ret;
if (internal) {
	STACKTRACE_INDENT_PRINT("internal node setup" << endl);
	const never_ptr<const index_expr>
		ir(ref.is_a<const index_expr>());
	never_ptr<const token_identifier> dr;
	never_ptr<const range_list> ind;
	if (ir) {
		if (!c.parse_opts.array_internal_nodes) {
			cerr << "Error: internal node arrays are unsupported "
				"in ACT mode.  " << where(*this) << endl;
			return prs_literal_ptr_type(NULL);
		}
		const never_ptr<const inst_ref_expr> b(ir->get_base());
		dr = b.is_a<const token_identifier>();
		NEVER_NULL(b && dr);
		ind = ir->get_indices();
	} else {
		const never_ptr<const id_expr>
			b(ref.is_a<const id_expr>());
		NEVER_NULL(b);
		dr = never_ptr<const token_identifier>(&*b->get_id()->front());
		if (!dr) {
			cerr << "Unexpected prs-literal type: "
				<< where(*ref) << endl;
			return prs_literal_ptr_type(NULL);
		}
	}
	const token_identifier& id(*dr);
	const never_ptr<const node_instance_placeholder>
		np(c.lookup_internal_node(id));
	if (!np) {
		cerr << "Internal node `" << id << "\' not found." << endl;
		return prs_literal_ptr_type(NULL);
	}
	const count_ptr<entity::simple_node_meta_instance_reference>
		nref(new entity::simple_node_meta_instance_reference(np));
	if (ind) {
		const range_list::checked_meta_indices_type
			checked_indices(ind->check_meta_indices(c));
		if (!checked_indices) {
			cerr << "Error in internal node reference." << endl;
			return prs_literal_ptr_type(NULL);
		}
		nref->attach_indices(checked_indices);
	}
	ret = prs_literal_ptr_type(new entity::PRS::literal(
		nref.as_a<const entity::simple_node_meta_instance_reference>()));
	NEVER_NULL(ret);
} else {
	ret = ref->check_prs_literal(c);
}
if (ret && params) {
	// NOTE: parameters are not applicable to RHS or rules
	if (params->size() > 3) {
		// third optional parameter is transistor type [ACT]
		cerr << "Error: rule literals can take a maximum of 3 "
			"(width, length, type) parameters.  " << where(*params)
			<< endl;
		return prs_literal_ptr_type(NULL);
	}
	typedef expr_list::checked_meta_exprs_type	checked_exprs_type;
	typedef checked_exprs_type::const_iterator	const_iterator;
	typedef checked_exprs_type::value_type		value_type;
	checked_exprs_type temp;
	params->postorder_check_meta_exprs(temp, c);
	const const_iterator i(temp.begin()), e(temp.end());
	if (find(i, e, value_type(NULL)) != e) {
		cerr << "Error checking literal parameters in "
			<< where(*params) << endl;
		return prs_literal_ptr_type(NULL);
	}
	INVARIANT(temp.size());
	NEVER_NULL(ret);
	copy(i, e, back_inserter(ret->get_params()));
}
	return ret;
}	// end literal::check_prs_literal

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called to check a literal on the RHS of a rule.
	Special case needed when literal is marked as internal node, 
	in which case this is a *declaration* of an internal node.  
	Signal to caller that this is internal (flag the created IR literal).
 */
prs_literal_ptr_type
literal::check_prs_rhs(context& c) const {
	STACKTRACE_VERBOSE;
if (internal) {
	STACKTRACE_INDENT_PRINT("internal node setup" << endl);
	// inject an implicit internal node declaration
	const never_ptr<const index_expr>
		ir(ref.is_a<const index_expr>());
	const never_ptr<const id_expr>
		dr(ref.is_a<const id_expr>());
	never_ptr<const node_instance_placeholder> nd;
	if (ir) {
		if (!c.parse_opts.array_internal_nodes) {
			cerr << "Error: internal node arrays are unsupported "
				"in ACT mode.  " << where(*this) << endl;
			return prs_literal_ptr_type(NULL);
		}
		// extract base and index dimensions
		const never_ptr<const inst_ref_expr> b(ir->get_base());
		const never_ptr<const token_identifier>
			bd(b.is_a<const token_identifier>());
		NEVER_NULL(b && bd);
		nd = c.add_internal_node(*bd, 
			ir->implicit_dimensions());
		// only care about dimensions, not indices
	} else if (dr) {
		// is a scalar
		nd = c.add_internal_node(*dr->get_id()->front(), 0);
	} else {
		cerr << "Unexpected prs-literal type: "
			<< where(*ref) << endl;
		return prs_literal_ptr_type(NULL);
	}
	if (!nd) {
		cerr << "Error implicitly declaring internal node.  "
			<< where(*ref) << endl;
		return prs_literal_ptr_type(NULL);
	}
	// now return to normal lookup
}
	return check_prs_literal(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This really should never be called.  
	Meta-parameter references should never be adorned with other parameters!
 */
inst_ref_expr::meta_return_type
literal::check_meta_reference(const context& c) const {
	// return ref->check_meta_reference(c);
	return meta_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This really should never be called.  
	Postpone parameter checks until later.  
 */
inst_ref_expr::nonmeta_return_type
literal::check_nonmeta_reference(const context& c) const {
	// return ref->check_nonmeta_reference(c);
	return nonmeta_return_type(NULL);
}

//=============================================================================
// class precharge method definitions
precharge::precharge(const node_position* d, const expr* e) :
		dir(d), pchg_expr(e) {
	NEVER_NULL(d);
	NEVER_NULL(pchg_expr);
}

precharge::~precharge() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(precharge)

line_position
precharge::leftmost(void) const {
	return dir->leftmost();
}

line_position
precharge::rightmost(void) const {
	return pchg_expr->rightmost();
}

entity::PRS::precharge_expr
precharge::check_prs_expr(context& c) const {
if (pchg_expr) {
	const prs_expr_return_type e(pchg_expr->check_prs_expr(c));
	if (e) {
		return entity::PRS::precharge_expr(e, dir->text[0] == '+');
	}
}
	return entity::PRS::precharge_expr();	// default
}


//=============================================================================
// class rule method definitions

/**
	\param atts the attribute list is optional.  
 */
CONSTRUCTOR_INLINE
rule::rule(const generic_attribute_list* atts, const expr* g, 
		const char_punctuation_type* a,
		literal* rhs, const char_punctuation_type* d) :
		body_item(), attribs(atts), guard(g), arrow(a),
		r(rhs), dir(d) {
	NEVER_NULL(guard); NEVER_NULL(arrow); NEVER_NULL(r); NEVER_NULL(dir);
}

DESTRUCTOR_INLINE
rule::~rule() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(rule)

line_position
rule::leftmost(void) const {
	if (attribs)
		return attribs->leftmost();
	else	return guard->leftmost();
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
	TODO: better error handling, instead of throwing exception...
 */
body_item::return_type
rule::check_rule(context& c) const {
	STACKTRACE_VERBOSE;
	prs_expr_return_type g(guard->check_prs_expr(c));
	if (!g) {
		cerr << "ERROR in production rule guard at " <<
			where(*guard) << "." << endl;
		THROW_EXIT;
	}
//	g->check();	// paranoia
	prs_literal_ptr_type o(r->check_prs_rhs(c));
	if (!o) {
		cerr << "ERROR in the output node reference at " <<
			where(*r) << "." << endl;
		THROW_EXIT;
	}
	char arrow_type = pull_base::ARROW_NORMAL;
	switch (arrow->text[0]) {
	case '=': arrow_type = pull_base::ARROW_COMPLEMENT; break;
	case '#': arrow_type = pull_base::ARROW_FLIP; break;
	default: {}
	}
	if (arrow_type && o->is_internal()) {
		cerr << "ERROR: internal nodes may only be defined with -> .  "
			<< where(*this) << endl;
		THROW_EXIT;
	}
	excl_ptr<pull_base>
		ret((dir->text[0] == '+') ?
			AS_A(pull_base*,
				new entity::PRS::pull_up(g, *o, arrow_type)) :
			AS_A(pull_base*,
				new entity::PRS::pull_dn(g, *o, arrow_type)));
	NEVER_NULL(ret);
	if (attribs) {
		entity::PRS::rule_attribute_list_type&
			atts(ret->get_attribute_list());
		attribs->check_list(atts, &check_prs_attribute, c);
		if (find(atts.begin(), atts.end(), false) != atts.end()) {
			cerr << "ERROR in attributes list before rule." << endl;
			THROW_EXIT;
		}
	}
	c.get_current_prs_body().append_rule(ret);
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
/**
	TODO: have PRS::rule return a entity::PRS::rule.
 */
body_item::return_type
loop::check_rule(context& c) const {
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
	entity::PRS::expr_loop_base::range_ptr_type
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
	excl_ptr<entity::PRS::rule_loop>
		ret(new entity::PRS::rule_loop(loop_ind, loop_range));
	NEVER_NULL(ret);

	// copied from body::check_rule
	// const never_ptr<definition_base> d(c.get_current_open_definition());
	never_ptr<entity::PRS::rule_loop> retc(ret);
	c.get_current_prs_body().append_rule(ret);
try {
	const context::prs_body_frame prlf(c, retc);
	rules->check_list_void(&body_item::check_rule, c);
} catch (...) {
	cerr << "ERROR: at least one error in PRS rule-loop.  "
		<< where(*rules) << endl;
	throw;		// re-throw
}
	if (retc->empty()) {
		c.get_current_prs_body().pop_back();
	}
}	// end method loop::check_rule

//=============================================================================
// class guarded_body method definitions

guarded_body::guarded_body(const expr* g, const char_punctuation_type* a, 
		const rule_list* r) :
		guard(g), arrow(a), rules(r) {
	// guard may be NULL (else-clause)
	NEVER_NULL(arrow);
	NEVER_NULL(rules);
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
	return rules->rightmost();
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
				"conditional PRS.  " << where(*guard) << endl;
			GUARDED_PRS_THROW;
		} else {
		bg = _g.is_a<const pbool_expr>();
		if (!bg) {
			cerr << "Error: guard expression is not boolean.  " <<
				where(*guard) << endl;
			GUARDED_PRS_THROW;
		}
		}
	}
	// see also guarded_instance_management::check_build()
	const never_ptr<entity::PRS::rule_conditional>
		rs(IS_A(entity::PRS::rule_conditional*,
			&*c.get_current_prs_body().back()));
	NEVER_NULL(rs);
	rs->append_guarded_clause(bg);
	const context::prs_body_frame _pbf(c, 
		never_ptr<entity::PRS::rule_set>(&rs->get_last_clause()));
	STACKTRACE_INDENT_PRINT("current rule set: " <<
		&rs->get_last_clause() << endl);
	// code below mostly ripped from loop::check_rule()
try {
	rules->check_list_void(&body_item::check_rule, c);
} catch (...) {
	cerr << "ERROR: at least one error in conditional PRS rules.  "
		<< where(*rules) << endl;
}
}	// end guarded_body::check_clause

//=============================================================================
// class conditional method definitions

conditional::conditional(const guarded_prs_list* p) : gp(p) {
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
conditional::check_rule(context& c) const {
	STACKTRACE_VERBOSE;
	// see also conditional_instantiation::check_build()
	excl_ptr<entity::PRS::rule_conditional>
		rs(new entity::PRS::rule_conditional());
	const never_ptr<const entity::PRS::rule_conditional> crs(rs);
	c.get_current_prs_body().append_rule(rs);	// xfer ownership
	MUST_BE_NULL(rs);
	gp->check_list_void(&guarded_body::check_clause, c);
        if (crs->empty()) {
                c.get_current_prs_body().pop_back();
        } 
}	// end conditional::check_rule

//=============================================================================
// class body method definitions

CONSTRUCTOR_INLINE
body::body(const generic_keyword_type* t, const rule_list* r) :
		language_body(t), rules(r) {
	if (r) NEVER_NULL(rules);
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
	return rules->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return false on error.
 */
bool
body::__check_rules(context& c) const {
if (rules) {
	try {
		rules->check_list_void(&body_item::check_rule, c);
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
	Added support for prs inside conditionals and loops from the
	instance management scope.  
	\pre context has already set the current_prs_body.
 */
never_ptr<const object>
body::check_build(context& c) const {
	STACKTRACE_VERBOSE;
if (rules) {
	if (c.reject_namespace_lang_body()) {
		cerr << "Error: top-level PRS is only supported "
			"in the global namespace." << endl;
		cerr << "\tgot: prs { ... } " << where(*this)
			<< endl;
		THROW_EXIT;
	}
	// check context's current open definition
	const never_ptr<definition_base> d(c.get_current_open_definition());
	const never_ptr<process_definition> pd(d.is_a<process_definition>());
	// if !pd, then prs is in a top-level scope (outside definition)
	if (!__check_rules(c)) {
		cerr << "ERROR: at least one error in PRS body."
			<< endl;
		THROW_EXIT;
	}
}
	// else empty, no PRS to add
	return never_ptr<const object>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Treat nested body of rules as a continuation of the list of rules.  
	TODO: factor out common checking loop with check_build, above.  
 */
body_item::return_type
body::check_rule(context& c) const {
	if (!__check_rules(c)) {
		cerr << "ERROR: at least one error in PRS rule-nest.  "
			<< where(*rules) << endl;
		CHECK_RULE_THROW;
	}
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
op_loop::check_meta_expr(const context& c) const {
	FINISH_ME(Fang);
	return expr::meta_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: FINISH ME
	Is this even needed outside of the PRS context?
 */
nonmeta_expr_return_type
op_loop::check_nonmeta_expr(const context& c) const {
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
	const entity::PRS::expr_loop_base::range_ptr_type
		loop_range(meta_range_expr::make_explicit_range(rng));
	NEVER_NULL(loop_range);
	// create loop index variable and push onto context stack
	const context::loop_var_frame _lvf(c, *index);
	const meta_loop_base::ind_var_ptr_type& loop_ind(_lvf.var);
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
	return (op->text[0] == '&')
		? prs_expr_return_type(new entity::PRS::and_expr_loop(
			loop_ind, loop_range, body_expr))
		: prs_expr_return_type(new entity::PRS::or_expr_loop(
			loop_ind, loop_range, body_expr));
}

//=============================================================================
// class macro method definitions

macro::macro(const generic_attribute_list* a, 
		literal* l, const inst_ref_expr_list* r) :
		attribs(a), name(), params(), args(r) {
	const excl_ptr<literal> lit(l);	// will self-destruct at end of ctor
	name = lit->extract_identifier();
	params = lit->extract_parameters();
	// punt checking until later
}

macro::~macro() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(macro)

line_position
macro::leftmost(void) const {
	// there could be an error extracting the name
	if (attribs)
		return attribs->leftmost();
	else if (name)
		return name->leftmost();
	else if (params)
		return params->leftmost();
	else	return args->leftmost();
}

line_position
macro::rightmost(void) const {
	return args->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: process params.
 */
body_item::return_type
macro::check_rule(context& c) const {
	if (!name) {
		cerr << "Error parsing macro name before " << where(*args)
			<< endl;
		cerr << "Expected: prs_macro : ID . [ \'<\' shift_exprs \'>\' ] \'(\' inst_ref_exprs \')\'" << endl;
		CHECK_RULE_THROW;
	}
	const entity::PRS::cflat_macro_definition_entry
		mde(entity::PRS::cflat_macro_registry[*name]);
	if (!mde) {
		cerr << "Error: unrecognized PRS macro \"" << *name << "\" at "
			<< where(*name) << endl;
		CHECK_RULE_THROW;
	}

	excl_ptr<entity::PRS::macro>
		ret(new entity::PRS::macro(*name));
if (params) {
	if (!mde.check_num_params(params->size()).good) {
		// already have error message
		cerr << "\tat " << where(*params) << endl;
		CHECK_RULE_THROW;
	}
	typedef expr_list::checked_meta_exprs_type	checked_exprs_type;
	typedef checked_exprs_type::const_iterator	const_iterator;
	typedef checked_exprs_type::value_type		value_type;
	checked_exprs_type temp;
	params->postorder_check_meta_exprs(temp, c);
	const const_iterator i(temp.begin()), e(temp.end());
	if (find(i, e, value_type(NULL)) != e) {
		cerr << "Error checking macro parameters in " << where(*args)
			<< endl;
		CHECK_RULE_THROW;
	}
	INVARIANT(temp.size());
	NEVER_NULL(ret);
	copy(i, e, back_inserter(ret->get_params()));
} else if (!mde.check_num_params(0).good) {
	// no params given where required and already have error message
	cerr << "\tat " << where(*this) << endl;
	CHECK_RULE_THROW;
}
{
	typedef	inst_ref_expr_list::checked_bool_groups_type
							checked_bools_type;
	NEVER_NULL(args);
	if (!mde.check_num_nodes(args->size()).good) {
		// already have error message
		cerr << "\tat " << where(*args) << endl;
		CHECK_RULE_THROW;
	}
	typedef checked_bools_type::const_iterator	const_iterator;
	typedef checked_bools_type::value_type		value_type;
	checked_bools_type temp;
	INVARIANT(args->size());
	args->postorder_check_grouped_bool_refs(temp, c);
	const const_iterator i(temp.begin()), e(temp.end());
	if (find_if(i, e, mem_fun_ref(&value_type::empty)) != e) {
		cerr << "Error checking macro arguments in " << where(*args)
			<< endl;
		CHECK_RULE_THROW;
	}
	INVARIANT(temp.size());
	NEVER_NULL(ret);
	copy(i, e, back_inserter(ret->get_nodes()));
}
if (attribs) {
	cerr << "WARNING: attributes on PRS macros are tentatively ignored.  "
		<< where(*attribs) << endl;
	// FINISH_ME(Fang);
}
	c.get_current_prs_body().append_rule(ret);
}	// end macro::check_rule

//=============================================================================
// class attribute method definitions

rule::attribute_type
rule::check_prs_attribute(const generic_attribute& a, context& c) {
	typedef	attribute_type			return_type;
	typedef	expr_list::checked_meta_exprs_type vals_type;
	typedef	vals_type::const_iterator	const_iterator;
	typedef	vals_type::value_type		val_type;
	// all macros must be registered with the master registry list (cflat)
	if (!entity::PRS::cflat_attribute_registry[*a.key]) {
		cerr << "Error: unrecognized PRS rule attribute \"" << *a.key <<
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

//=============================================================================
}	// end namespace PRS

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

// template class node_list<const body_item>;		// PRS::rule_list

#if 1
// This is temporary, until node_list::check_build is overhauled.  
template
node_list<const PRS::body_item>::node_list();

template
node_list<const PRS::body_item>::node_list(const PRS::body_item*);

template
node_list<const PRS::body_item>::~node_list();

template
ostream&
node_list<const PRS::body_item>::what(ostream&) const;

template
line_position
node_list<const PRS::body_item>::leftmost(void) const;

template
line_position
node_list<const PRS::body_item>::rightmost(void) const;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// template
// node_list<const PRS::guarded_body>::node_list();

template
node_list<const PRS::guarded_body>::node_list(const PRS::guarded_body*);

template
node_list<const PRS::guarded_body>::~node_list();

template
ostream&
node_list<const PRS::guarded_body>::what(ostream&) const;

template
line_position
node_list<const PRS::guarded_body>::leftmost(void) const;

template
line_position
node_list<const PRS::guarded_body>::rightmost(void) const;

#endif

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __HAC_AST_PRS_CC__

