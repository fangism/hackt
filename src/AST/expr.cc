/**
	\file "AST/expr.cc"
	Class method definitions for HAC::parser, related to expressions.  
	$Id: expr.cc,v 1.16.4.2 2006/08/30 04:27:57 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_expr.cc,v 1.27.12.1 2005/12/11 00:45:05 fang Exp
 */

#ifndef	__HAC_AST_EXPR_CC__
#define	__HAC_AST_EXPR_CC__

#define	ENABLE_STACKTRACE		0

#include <exception>
#include <iostream>
#include <algorithm>
#include <limits>				// for numeric_limits
#include <iterator>
#include "util/STL/functional.h"		// for _Select{1st,2nd}

#include "AST/token.h"
#include "AST/token_char.h"
#include "AST/expr.h"
#include "AST/reference.h"
#include "AST/range_list.h"
#include "AST/node_list.tcc"
#include "util/sublist.tcc"
#include "AST/parse_context.h"

// will need these come time for type-checking
#include "Object/devel_switches.h"
#if USE_INSTANCE_PLACEHOLDERS
#include "Object/inst/instance_placeholder_base.h"
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/inst/param_value_placeholder.h"
#else
#include "Object/inst/instance_collection_base.h"
#include "Object/inst/physical_instance_collection.h"
#endif
#include "Object/def/definition_base.h"
#include "Object/ref/aggregate_meta_value_reference.h"
#include "Object/ref/aggregate_meta_instance_reference.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/simple_nonmeta_instance_reference_base.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/nonmeta_instance_reference_subtypes.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/meta_index_list.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/expr/pbool_unary_expr.h"
#include "Object/expr/pint_unary_expr.h"
#include "Object/expr/pint_arith_expr.h"
#include "Object/expr/pint_relational_expr.h"
#include "Object/expr/preal_unary_expr.h"
#include "Object/expr/preal_arith_expr.h"
#include "Object/expr/preal_relational_expr.h"
#include "Object/expr/pbool_logical_expr.h"
#include "Object/expr/bool_negation_expr.h"
#include "Object/expr/int_negation_expr.h"
#include "Object/expr/int_arith_expr.h"
#include "Object/expr/int_relational_expr.h"
#include "Object/expr/bool_logical_expr.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/lang/PRS.h"
#include "Object/type/template_actuals.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/inst/param_value_collection.h"
#include "Object/ref/meta_reference_union.h"

#include "common/ICE.h"
#include "common/TODO.h"

#include "util/what.h"
#include "util/operators.h"
#include "util/stacktrace.h"
#include "util/iterator_more.h"
#include "util/memory/count_ptr.tcc"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// debug flags
#define	DEBUG_ID_EXPR	0

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::expr, "(expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::inst_ref_expr, "(inst-ref-expr)")
// SPECIALIZE_UTIL_WHAT(HAC::parser::expr_list, "(expr-list)")
SPECIALIZE_UTIL_WHAT(HAC::parser::qualified_id, "(qualified-id)")
SPECIALIZE_UTIL_WHAT(HAC::parser::id_expr, "(id-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::prefix_expr, "(prefix-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::member_expr, "(member-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::index_expr, "(index-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::arith_expr, "(arith-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::relational_expr, "(relational-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::logical_expr, "(logical-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::array_concatenation, "(array-concatenation)")
SPECIALIZE_UTIL_WHAT(HAC::parser::loop_concatenation, "(loop-concatenation)")
SPECIALIZE_UTIL_WHAT(HAC::parser::array_construction, "(array-construction)")
SPECIALIZE_UTIL_WHAT(HAC::parser::template_argument_list_pair,
		"(expr-list-pair)")
}

//=============================================================================
namespace HAC {
using namespace entity;

namespace parser {
#include "util/using_ostream.h"
using std::copy;
using std::back_inserter;
using std::transform;
using std::distance;
using std::_Select1st;
using std::_Select2nd;
using util::back_insert_assigner;
using entity::expr_dump_context;

//=============================================================================
// local prototypes

static
expr::meta_return_type
aggregate_check_meta_expr(const expr_list& el, const context& c, 
	const bool cat);

static
expr::generic_meta_return_type
aggregate_check_meta_generic(const expr_list& el, const context& c, 
	const bool cat);

//=============================================================================
// class expr method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	All non-inst-ref expressions will dynamically cast
	the result of check_meta_expr to an instance reference.  
	This is overridden by inst_ref_expr::check_meta_generic.
	\return pair of typed pointers.  
 */
expr::generic_meta_return_type
expr::check_meta_generic(const context& c) const {
	STACKTRACE("expr::check_meta_generic()");
	const expr::meta_return_type ret(check_meta_expr(c));
	return generic_meta_return_type(ret, ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default behavior for non-overridden check_nonmeta_expr.
 */
nonmeta_expr_return_type
expr::check_nonmeta_expr(const context& c) const {
	const expr::meta_return_type ret(check_meta_expr(c));
	// check for ret && !cast?  diagnostic error message would be nice?
	return ret.is_a<nonmeta_expr_return_type::element_type>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary placeholder, never really supposed to be called.  
 */
prs_expr_return_type
expr::check_prs_expr(context& c) const {
	cerr << "Fang, unimplemented expr::check_prs_expr!" << endl;
	return prs_expr_return_type();
}

//=============================================================================
// class inst_ref_expr method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	All inst-ref expressions will dynamically cast
	the result of check_meta_reference to an parameter expression.  
	\return pair of typed pointers.  
 */
expr::generic_meta_return_type
inst_ref_expr::check_meta_generic(const context& c) const {
	STACKTRACE("inst_ref_expr::check_meta_generic()");
	const meta_return_type ret(check_meta_reference(c));
	return generic_meta_return_type(
		ret.value_ref().is_a<expr::meta_return_type::element_type>(),
		ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should really not be called...
 */
expr::meta_return_type
inst_ref_expr::check_meta_expr(const context& c) const {
	STACKTRACE("inst_ref_expr::check_meta_expr() (should not be called)");
	typedef	expr::meta_return_type::element_type	param_type;
	const inst_ref_expr::meta_return_type inst_ref(check_meta_reference(c));
	const expr::meta_return_type
		param_ref(inst_ref.value_ref().is_a<param_type>());
	if (param_ref) {
		// accepted
		return param_ref;
	} else {
		cerr << "ERROR: Expression at " << where(*this) <<
			" does not refer to a parameter." << endl;
		THROW_EXIT;
		return expr::meta_return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need to allow meta expressions as nonmeta expressions.  
 */
nonmeta_expr_return_type
inst_ref_expr::check_nonmeta_expr(const context& c) const {
	typedef	nonmeta_expr_return_type	return_type;
	typedef	expr::nonmeta_return_type::element_type	data_type;
	const nonmeta_return_type inst_ref(check_nonmeta_reference(c));
	if (!inst_ref) {
		// already printed error message
		return return_type(NULL);
	}
	const expr::nonmeta_return_type data_ref(inst_ref.is_a<data_type>());
	if (data_ref) {
		return data_ref;
	} else {
		cerr << "ERROR: Expression at " << where(*this) <<
			" does not refer to a data type." << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	After checking an meta_instance_reference, this checks to make sure
	that a bool is referenced, appropriate for PRS.  
 */
prs_literal_ptr_type
inst_ref_expr::check_prs_literal(const context& c) const {
	STACKTRACE_VERBOSE;
	meta_return_type ref(check_meta_reference(c));
	count_ptr<simple_bool_meta_instance_reference>
		bool_ref(ref.inst_ref().is_a<simple_bool_meta_instance_reference>());
	if (bool_ref) {
		ref.inst_ref().abandon();	// reduce ref-count to 1
		INVARIANT(bool_ref.refs() == 1);
		if (bool_ref->dimensions()) {
			cerr << "ERROR: bool reference at " << where(*this) <<
				" does not refer to a scalar instance." << endl;
			return prs_literal_ptr_type(NULL);
		} else {
			// shared to exclusive ownership
			entity::PRS::literal_base_ptr_type
				lit(bool_ref.exclusive_release());
			return prs_literal_ptr_type(
				new entity::PRS::literal(lit));
		}
	} else {
		cerr << "ERROR: expression at " << where(*this) <<
			" does not reference a bool." << endl;
		return prs_literal_ptr_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A more relaxed version of check_prs_literal where the result is
	allowed to reference a group of bools, which need not be a 
	scalar reference.  
	Most of the code is copied from ::check_prs_literal(), above.  
	\return true upon error, else false.
 */
bool
inst_ref_expr::check_grouped_literals(checked_bool_group_type& g, 
		const context& c) const {
	meta_return_type ref(check_meta_reference(c));
	count_ptr<simple_bool_meta_instance_reference>
		bool_ref(ref.inst_ref().is_a<simple_bool_meta_instance_reference>());
	if (bool_ref) {
		ref.inst_ref().abandon();	// reduce ref-count to 1
		INVARIANT(bool_ref.refs() == 1);
		// skip dimensions check
		// shared to exclusive ownership
		entity::PRS::literal_base_ptr_type
			lit(bool_ref.exclusive_release());
		g.push_back(prs_literal_ptr_type(
			new entity::PRS::literal(lit)));
		return false;
	} else {
		cerr << "ERROR: expression at " << where(*this) <<
			" does not reference a bool." << endl;
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Down-casting literal to general PRS guard expression.  
 */
prs_expr_return_type
inst_ref_expr::check_prs_expr(context& c) const {
	// now virtual
	return check_prs_literal(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inst_ref_expr::nonmeta_data_return_type
inst_ref_expr::check_nonmeta_data_reference(const context& c) const {
	typedef	nonmeta_data_return_type::element_type	ref_type;
	const nonmeta_return_type
		inst_ref(check_nonmeta_reference(c));
	if (!inst_ref) {
		// already have error message?
		return nonmeta_data_return_type(NULL);
	}
	const nonmeta_data_return_type ret(inst_ref.is_a<ref_type>());
	if (!ret) {
		cerr << "ERROR: expression at " << where(*this) <<
			" does not reference a data type." << endl;
	}
	return ret;
}

//=============================================================================
// class expr_list method definitions

expr_list::expr_list() : parent_type() { }

expr_list::expr_list(const expr* e) : parent_type(e) { }

expr_list::~expr_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just collects the result of type-checking of items in list.
	\param temp the type-checked result list.
	\param c the context.
 */
void
expr_list::postorder_check_meta_generic(checked_meta_generic_type& temp,
		const context& c) const {
	STACKTRACE("expr_list::postorder_check_meta_generic()");
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		temp.push_back((*i) ? (*i)->check_meta_generic(c) :
			checked_meta_generic_type::value_type());
		// else pushes a pair of NULL pointers
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just collects the result of type-checking of items in list.
	Expressions are allowed to be NULL or empty, 
	so don't catch NULLs for errors.  
	(To catch errors, will need to check against original list...)
	\param temp the type-checked result list.
	\param c the context.
 */
void
expr_list::postorder_check_meta_exprs(checked_meta_exprs_type& temp,
		const context& c) const {
	STACKTRACE("expr_list::postorder_check_meta_exprs()");
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		temp.push_back((*i) ? (*i)->check_meta_expr(c) :
			checked_meta_exprs_type::value_type(NULL));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: consider templating these traversals?
	Q: are these expressions allowed to be NULL?  (CHP context)
	Just collects the result of type-checking of items in list.
	\param temp the type-checked result list.
	\param c the context.
 */
void
expr_list::postorder_check_nonmeta_exprs(checked_nonmeta_exprs_type& temp,
		const context& c) const {
	STACKTRACE("expr_list::postorder_check_nonmeta_exprs()");
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		temp.push_back((*i)->check_nonmeta_expr(c));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
expr_list::select_checked_meta_exprs(const checked_meta_generic_type& src, 
		checked_meta_exprs_type& dst) {
	INVARIANT(dst.empty());
	transform(src.begin(), src.end(), back_inserter(dst),
		_Select1st<checked_meta_generic_type::value_type>()
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
expr_list::select_checked_meta_refs(const checked_meta_generic_type& src, 
		checked_meta_refs_type& dst) {
	INVARIANT(dst.empty());
	transform(src.begin(), src.end(), back_inserter(dst),
		_Select2nd<checked_meta_generic_type::value_type>()
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constucts an aggregate value reference from its consituents.  
 */
meta_expr_return_type
expr_list::make_aggregate_value_reference(const checked_meta_exprs_type& ex, 
		const bool cat) {
	typedef	meta_expr_return_type		return_type;
	typedef	checked_meta_exprs_type::const_iterator	const_iterator;
	INVARIANT(ex.size());
	const_iterator i(ex.begin()), e(ex.end());
	const meta_expr_return_type bi(*i);
	if (!bi) {
		cerr << "Error in first subreference, cannot construct "
			"aggregate value reference." << endl;
		return return_type(NULL);
	} else if (cat && !bi->dimensions()) {
		cerr << "Error: subreference of aggregate concatenations "
			"must be non-scalar." << endl;
		return return_type(NULL);
	}
	const count_ptr<aggregate_meta_value_reference_base>
		ret(param_expr::make_aggregate_meta_value_reference(bi));
	NEVER_NULL(ret);
	if (cat) ret->set_concatenation_mode();
	else	ret->set_construction_mode();
	size_t j = 2;
	for (++i; i!=e; ++i, ++j) {
		const meta_expr_return_type mi(*i);
		if (!mi) {
			// could be error in construction, or wrong type.
			cerr << "Error in subreference at position " << j <<
				", cannot construct aggregate value reference."
				<< endl;
			return return_type(NULL);
		} else if (cat && !mi->dimensions()) {
			cerr << "Error: subreference of aggregate concatenations "
				"must be non-scalar." << endl;
			return return_type(NULL);
		}
		if (!ret->append_meta_value_reference(mi).good) {
			cerr << "Error appending aggregate value reference "
				"at position " << j << "." << endl;
			return return_type(NULL);
		}
		// else keep going
	}
	// cross-cast from aggregate_meta_value_reference to param_expr
	return ret.is_a<return_type::element_type>();
}

//=============================================================================
// class expr_list method definitions

// inst_ref_expr_list::inst_ref_expr_list() : parent_type() { }

inst_ref_expr_list::inst_ref_expr_list(const inst_ref_expr* e) :
		parent_type(e) { }

inst_ref_expr_list::~inst_ref_expr_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A more specialized version of check_meta_refs that requires
	each resolved instance reference to refer to a scalar bool.
	Suitable for PRS literal list checking. 
	Consider using a transform algo with functor...
 */
void
inst_ref_expr_list::postorder_check_bool_refs(
		checked_bool_refs_type& temp, const context& c) const {
	STACKTRACE("inst_ref_expr_list::postorder_check_bool_refs()");
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		temp.push_back((*i)->check_prs_literal(c));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Even more specialized: checks amorphous groups of references, 
	relaxing all dimension and packedness requirements.  
	Each element of this list is treated as a group.  
	\return true on first error.  
 */
bool
inst_ref_expr_list::postorder_check_grouped_bool_refs(
		checked_bool_groups_type& temp, const context& c) const {
	STACKTRACE("inst_ref_expr_list::postorder_check_grouped_bool_refs()");
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		typedef	checked_bool_groups_type::value_type	group_type;
		temp.push_back(group_type());	// create empty
		// then append in-place (beats creating and copying)
		if ((*i)->check_grouped_literals(temp.back(), c)) {
			// TODO: more specific error message, use std::distance
			cerr << "Error in bool group reference list in "
				<< where(*this) << endl;
			return true;
		}
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variant collects all constituent references into a single 
	group, not a list of groups.  
	\return true on first error.
 */
bool
inst_ref_expr_list::postorder_check_grouped_bool_refs(
		checked_bool_group_type& temp, const context& c) const {
	STACKTRACE("inst_ref_expr_list::postorder_check_grouped_bool_refs()");
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		if ((*i)->check_grouped_literals(temp, c)) {
			// TODO: more specific error message, use std::distance
			cerr << "Error in bool group reference list in "
				<< where(*this) << endl;
			return true;
		}
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
inst_ref_expr_list::postorder_check_meta_refs(
		checked_meta_refs_type& temp, const context& c) const {
	STACKTRACE("inst_ref_expr_list::postorder_check_meta_refs()");
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		temp.push_back((*i)->check_meta_reference(c));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
inst_ref_expr_list::postorder_check_nonmeta_data_refs(
		checked_nonmeta_data_refs_type& temp, const context& c) const {
	STACKTRACE("inst_ref_expr_list::postorder_check_nonmeta_data_refs()");
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		temp.push_back((*i)->check_nonmeta_data_reference(c));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: Account for both instance references and value references.  
 */
inst_ref_meta_return_type
inst_ref_expr_list::make_aggregate_instance_reference(
		const checked_meta_refs_type& ref, const bool cat) {
	typedef	inst_ref_meta_return_type		return_type;
	typedef	checked_meta_refs_type::const_iterator	const_iterator;
	INVARIANT(ref.size());
	const_iterator i(ref.begin()), e(ref.end());
	const inst_ref_meta_return_type::inst_ref_ptr_type& bi(i->inst_ref());
	if (!bi) {
		cerr << "Error in first subreference, cannot construct "
			"aggregate instance reference." << endl;
		return return_type(NULL);
	} else if (cat && !bi->dimensions()) {
		cerr << "Error: subreference of aggregate concatenations "
			"must be non-scalar." << endl;
		return return_type(NULL);
	}
	const count_ptr<aggregate_meta_instance_reference_base>
		ret(meta_instance_reference_base::
			make_aggregate_meta_instance_reference(bi));
	NEVER_NULL(ret);
	if (cat) ret->set_concatenation_mode();
	else	ret->set_construction_mode();
	size_t j = 2;
	for (++i; i!=e; ++i, ++j) {
		const inst_ref_meta_return_type::inst_ref_ptr_type&
			mi(i->inst_ref());
		if (!mi) {
			cerr << "Error in subreference at position " << j <<
				", cannot construct aggregate "
				"instance reference." << endl;
			if (i->value_ref()) {
				cerr << "\tgot: ";
				i->value_ref()->dump(cerr, 
					expr_dump_context::error_mode)
					<< endl;
			}
			return return_type(NULL);
		}
		if (!ret->append_meta_instance_reference(mi).good) {
			cerr << "Error appending aggregate instance reference "
				"at position " << j << "." << endl;
			return return_type(NULL);
		}
		// else keep going
	}
	// cross-cast: from aggregate_meta_instance_reference 
	// to meta_instance_reference_base
	return ret.is_a<meta_instance_reference_base>();
}

//=============================================================================
// class template_argument_list_pair method definitions

template_argument_list_pair::template_argument_list_pair(
		const list_type* s, const list_type* r) :
		strict_args(s), relaxed_args(r) {
	if (relaxed_args)
		NEVER_NULL(strict_args);
		// though strict args is allowed to be empty
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template_argument_list_pair::~template_argument_list_pair() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARSER_WHAT_DEFAULT_IMPLEMENTATION(template_argument_list_pair)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
template_argument_list_pair::leftmost(void) const {
	if (strict_args)
		return strict_args->leftmost();
	else	return line_position();	// NONE
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
template_argument_list_pair::rightmost(void) const {
	if (relaxed_args)
		return relaxed_args->rightmost();
	else	return strict_args->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks template expressions.  
	TODO: how to signal error?
	TODO: upgrade expressions to generalized template arguments.  
	TODO: make sure strict paramters do not depend on relaxed, 
		relaxed parameters may depend on relaxed parameters.  
	TODO: sugar: check for const_param expressions to make const list?
 */
template_argument_list_pair::return_type
template_argument_list_pair::check_template_args(const context& c) const {
	const count_ptr<dynamic_param_expr_list>
		strict(strict_args ?
			new dynamic_param_expr_list(strict_args->size()) :
			NULL);
	if (strict_args) {
		expr_list::checked_meta_exprs_type temp;
		strict_args->postorder_check_meta_exprs(temp, c);
		// NULL are allowed, where should we check?
		copy(temp.begin(), temp.end(), back_inserter(*strict));
		if (strict->is_relaxed_formal_dependent()) {
			cerr << "ERROR at " << where(*this) <<
				": strict template arguments may never "
				"depend on relaxed formal parameters." << endl;
			THROW_EXIT;
		}
	}
	const count_ptr<dynamic_param_expr_list>
		relaxed(relaxed_args ?
			new dynamic_param_expr_list(relaxed_args->size()) :
			NULL);
	if (relaxed_args) {
		expr_list::checked_meta_exprs_type temp;
		relaxed_args->postorder_check_meta_exprs(temp, c);
		// relaxed arguments are allowed to depend on anything
		// because they are relaxed
		// NULL are allowed, where should we check?
		copy(temp.begin(), temp.end(), back_inserter(*relaxed));
	}
	return return_type(strict, relaxed);
}

//=============================================================================
// class qualified_id method definitions

CONSTRUCTOR_INLINE
qualified_id::qualified_id(const token_identifier* n) : 
		parent_type(n), absolute(NULL) {
}

/// copy constructor, no transfer of ownership
CONSTRUCTOR_INLINE
qualified_id::qualified_id(const qualified_id& i) :
		parent_type(i), absolute(NULL) {
#if DEBUG_ID_EXPR
	cerr << "qualified_id::qualified_id(const qualified_id&);" << endl;
#endif
	if (i.absolute) {
		absolute = excl_ptr<const string_punctuation_type>(
			new string_punctuation_type(*i.absolute));
		// actually *copy* the token
		NEVER_NULL(absolute);
	}
}

DESTRUCTOR_INLINE
qualified_id::~qualified_id() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Call this function in the parser to mark an un/qualified identifier
	as absolute, as oppposed to relative.  
	See class definition of qualified_id for an explanation.  
	\param s should be a scope (::) token.  
	\return pointer to this object
 */
qualified_id*
qualified_id::force_absolute(const string_punctuation_type* s) {
	absolute = excl_ptr<const string_punctuation_type>(s);
	INVARIANT(absolute);
	return this;
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(qualified_id)

line_position
qualified_id::leftmost(void) const {
	return qualified_id_base::leftmost();
}

line_position
qualified_id::rightmost(void) const {
	return qualified_id_base::rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
	Future: instead of copying, give an iterator range.
	These might be obsoleted by the sublist slice interface.  
***/
qualified_id
qualified_id::copy_namespace_portion(void) const {
	qualified_id ret(*this);		// copy, not-owned
	if (!ret.empty())
		ret.pop_back();		// remove last element
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
qualified_id
qualified_id::copy_beheaded(void) const {
	qualified_id ret(*this);		// copy, not-owned
	if (!ret.empty())
		ret.pop_front();		// remove last element
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Finds an object referenced by the name, be it type or instance.  
	Remember to check the return type in the caller, even virtual
	calls through the abstract expr class.  
	\param c the context from which the search starts.  
	\return a pointer to a definition_base or an instance_collection_base 
		with the matching [un]qualified identifier if found, else NULL.
		Other possibilities: namespace?
		Consumer should wrap in meta_instance_reference?
			might be collective, in the case of an array
 */
never_ptr<const object>
qualified_id::check_build(const context& c) const {
	return c.lookup_object(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Performs unqualified lookup or qualified lookup of identifier.  
	This is what id_expr::check_build() should call.  
 */
#if USE_INSTANCE_PLACEHOLDERS
never_ptr<const instance_placeholder_base>
#else
never_ptr<const instance_collection_base>
#endif
qualified_id::lookup_instance(const context& c) const {
	if (!absolute && size() == 1)
		return c.lookup_instance(*parent_type::back());
	else	return c.lookup_instance(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// non-member functions

// friend operator
ostream&
operator << (ostream& o, const qualified_id& id) {
//	o << "(size = " << id.size() << ", empty = " << id.empty() << ")";
	if (id.empty()) {
		return o << "<null qualified_id>";
	} else {
		qualified_id::const_iterator i(id.begin());
		if (id.is_absolute())
			o << scope;
		count_ptr<const token_identifier> tid(*i);
		NEVER_NULL(tid);
		o << *tid;
		for (i++ ; i!=id.end(); i++) {
			tid = *i;
			NEVER_NULL(tid);
			o << scope << *tid;
		}
		return o;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// friend operator
ostream&
operator << (ostream& o, const qualified_id_slice& id) {
	if (id.empty()) {
		return o << "<null qualified_id_slice>";
	} else {
		qualified_id_slice::const_iterator i(id.begin());
		if (id.is_absolute())
			o << scope;
//		count_ptr<const token_identifier> tid(*i);
		NEVER_NULL(*i);
		o << **i;
		for (i++ ; i!=id.end(); i++) {
//			tid = *i;
			NEVER_NULL(*i);
			o << scope << **i;
		}
		return o;
	}
}

//=============================================================================
// class qualified_id_slice method definitions

qualified_id_slice::qualified_id_slice(const qualified_id& qid) :
		parent(qid.raw_list()), absolute(qid.is_absolute()) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
qualified_id_slice::qualified_id_slice(const qualified_id_slice& qid) :
		parent(qid.parent), absolute(qid.absolute) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
qualified_id_slice::~qualified_id_slice() {
}

//=============================================================================
// class id_expr method definitions

id_expr::id_expr(qualified_id* i) : parent_type(), qid(i) {
	assert(qid);
}

id_expr::id_expr(const id_expr& i) :
		parent_type(), qid(new qualified_id(*i.qid)) {
	NEVER_NULL(qid);
}

id_expr::~id_expr() { }

ostream&
id_expr::what(ostream& o) const {
        return o << util::what<id_expr>::name() << ": " << *qid;
}

line_position     
id_expr::leftmost(void) const {
        return qid->leftmost();
}

line_position
id_expr::rightmost(void) const {
        return qid->rightmost();  
}

qualified_id*
id_expr::force_absolute(const string_punctuation_type* s) {
	return qid->force_absolute(s);
}

bool
id_expr::is_absolute(void) const {
	return qid->is_absolute();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: update this comment
	The qualified_id member's check build can return a definition 
	or instance pointer.  
	A different method will be used to lookup definition/type identifiers.
	\param c the context where to begin searching for named object.  
	\return pointer to the found instantiation base instance if found,
		else NULL.
 */
inst_ref_expr::meta_return_type
id_expr::check_meta_reference(const context& c) const {
	STACKTRACE_VERBOSE;
	// lookup_instance will check for unqualified references first
#if USE_INSTANCE_PLACEHOLDERS
	const never_ptr<const instance_placeholder_base>
#else
	const never_ptr<const instance_collection_base>
#endif
		o(qid->lookup_instance(c));
	if (o) {
#if USE_INSTANCE_PLACEHOLDERS
		const never_ptr<const instance_placeholder_base>
			inst(o.is_a<const instance_placeholder_base>());
#else
		const never_ptr<const instance_collection_base>
			inst(o.is_a<const instance_collection_base>());
#endif
		if (inst) {
			STACKTRACE("valid instance collection found");
			// we found an instance which may be single
			// or collective... info is in inst.
#if USE_INSTANCE_PLACEHOLDERS
			const never_ptr<const physical_instance_placeholder>
				pinst(inst.is_a<const physical_instance_placeholder>());
#else
			const never_ptr<const physical_instance_collection>
				pinst(inst.is_a<const physical_instance_collection>());
#endif
			if (pinst) {
				// physical instance collection
				return pinst->make_meta_instance_reference();
			} else {
				// then must be a value collection
				const never_ptr<const param_value_collection>
					vinst(inst.is_a<const param_value_collection>());
				return vinst->make_meta_value_reference();
			}	// no other possibility
		} else {
			cerr << "object \"" << *qid <<
				"\" does not refer to an instance, ERROR!  "
				<< where(*qid) << endl;
			THROW_EXIT;
		}
	} else {
		// push NULL or error object to continue?
		cerr << "object \"" << *qid << "\" not found, ERROR!  "
			<< where(*qid) << endl;
		THROW_EXIT;
	}
	return inst_ref_expr::meta_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: update this comment
	The qualified_id member's check build can return a definition 
	or instance pointer.  
	A different method will be used to lookup definition/type identifiers.
	\param c the context where to begin searching for named object.  
	\return pointer to the found instantiation base instance if found,
		else NULL.
 */
inst_ref_expr::nonmeta_return_type
id_expr::check_nonmeta_reference(const context& c) const {
	typedef inst_ref_expr::nonmeta_return_type	return_type;
	STACKTRACE_VERBOSE;
#if USE_INSTANCE_PLACEHOLDERS
	const never_ptr<const instance_placeholder_base>
#else
	const never_ptr<const instance_collection_base>
#endif
		o(qid->lookup_instance(c));	// not ->check_build(c);
	if (o) {
#if USE_INSTANCE_PLACEHOLDERS
		const never_ptr<const instance_placeholder_base>
			inst(o.is_a<const instance_placeholder_base>());
#else
		const never_ptr<const instance_collection_base>
			inst(o.is_a<const instance_collection_base>());
#endif
		if (inst) {
			STACKTRACE("valid instance collection found");
			// we found an instance which may be single
			// or collective... info is in inst.
			return inst->make_nonmeta_instance_reference();
		} else {
			cerr << "object \"" << *qid <<
				"\" does not refer to an instance, ERROR!  "
				<< where(*qid) << endl;
			return return_type(NULL);
		}
	} else {
		// push NULL or error object to continue?
		cerr << "object \"" << *qid << "\" not found, ERROR!  "
			<< where(*qid) << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// non-member functions
ostream& operator << (ostream& o, const id_expr& id) {
	return o << *id.qid;
}

//=============================================================================
// class unary_expr method definitions

/**
	Failure to dynamic_cast will result in assignment to a NULL pointer,
	which will be detected, and properly memory managed, assuming
	that the arguments exclusively "owned" their memory locations.
 */
CONSTRUCTOR_INLINE
unary_expr::unary_expr(const expr* n, const char_punctuation_type* o) :
		expr(), e(n), op(o) {
	NEVER_NULL(e);
	NEVER_NULL(op);
}

DESTRUCTOR_INLINE
unary_expr::~unary_expr() {
}

//=============================================================================
// class prefix_expr method definitions

CONSTRUCTOR_INLINE
prefix_expr::prefix_expr(const char_punctuation_type* o, const expr* n) :
		unary_expr(n,o) {
}

DESTRUCTOR_INLINE
prefix_expr::~prefix_expr() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(prefix_expr)

line_position
prefix_expr::leftmost(void) const {
	return op->leftmost();
}

line_position
prefix_expr::rightmost(void) const {
	return e->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param c parse context.
	\return pointer to type-checked expression if successfull, else null.  
 */
expr::meta_return_type
prefix_expr::check_meta_expr(const context& c) const {
	typedef	expr::meta_return_type		return_type;
	const return_type pe(e->check_meta_expr(c));
	if (!pe) {
		// error propagates up the stack
		cerr << "ERROR building expression at " << where(*e) << endl;
		return return_type(NULL);
	}
	// we have a valid param_expr
	const count_ptr<pint_expr> ie(pe.is_a<pint_expr>());
	const count_ptr<pbool_expr> be(pe.is_a<pbool_expr>());
	const count_ptr<preal_expr> re(pe.is_a<preal_expr>());

	const int ch = op->text[0];
	switch(ch) {
		case '-':
			// integer negation
			if (ie) {
			if (ie->is_static_constant()) {
				// constant simplification
				return return_type(new pint_const(
					pint_unary_expr::evaluate(ch,
						ie->static_constant_value())));
			} else {
				return return_type(new pint_unary_expr(ch,ie));
			}
			} else if (re) {
			if (re->is_static_constant()) {
				// constant simplification
				return return_type(new preal_const(
					preal_unary_expr::evaluate(ch, 
						re->static_constant_value())));
			} else {
				return return_type(new preal_unary_expr(ch,re));
			}
			} else {
				cerr << "Unary \'-\' operator requires a "
					"pint or preal argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				return return_type(NULL);
			}
		case '!':
			// TODO: this is NOT OFFICIALLY SUPPORTED YET
			// integer logical negation
			if (!ie) {
				cerr << "Unary \'!\' operator requires a "
					"pint argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				return return_type(NULL);
			} else if (ie->is_static_constant()) {
				// constant simplification
				return return_type(new pint_const(
					// use pint_unary_expr::evaluate
					!ie->static_constant_value()));
			} else {
				return return_type(new pint_unary_expr(ch, ie));
			}
		case '~':
			// is this valid in the meta-language?
			// context-dependent? in PRS or not?
			// is overloaded as bit-wise negation for ints, 
			// logical negation for bools?
			if (be) {
			if (be->is_static_constant()) {
				// constant fold boolean negation
				return return_type(new pbool_const(
					pbool_unary_expr::evaluate(ch, 
						be->static_constant_value())));
			} else {
				return return_type(
					new pbool_unary_expr(be, ch));
			}
			} else if (ie) {
			if (ie->is_static_constant()) {
				// constant fold 1's complement
				return return_type(new pint_const(
					pint_unary_expr::evaluate(ch, 
						ie->static_constant_value())));
			} else {
				return return_type(
					new pint_unary_expr(ie, ch));
			}
			} else {	// neither pbool/pint
				cerr << "Unary \'~\' operator requires a "
					"pbool or pint argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				return return_type(NULL);
			}
		default:
			cerr << "Bad operator char \'" << ch << "\' in "
				"prefix_expr::check_meta_expr()!" << endl;
			DIE;
	}
	return return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_expr_return_type
prefix_expr::check_nonmeta_expr(const context& c) const {
	typedef	nonmeta_expr_return_type		return_type;
	const return_type pe(e->check_nonmeta_expr(c));
	if (!pe) {
		// error propagates up the stack
		cerr << "ERROR building expression at " << where(*e) << endl;
		return return_type(NULL);
	}
	// we have a valid param_expr
	const count_ptr<int_expr> ie(pe.is_a<int_expr>());
	const count_ptr<bool_expr> be(pe.is_a<bool_expr>());

	const int ch = op->text[0];
	switch(ch) {
		case '-':
			// integer negation
			if (!ie) {
				cerr << "Unary \'-\' operator requires an "
					"int argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				return return_type(NULL);
			} else {
				return return_type(new int_negation_expr(ie, ch));
			}
		case '!':
			// integer logical negation
			cerr << "DOH.  !(int) -> bool operation "
				"not yet supported... bug Fang." << endl;
			return return_type(NULL);
		case '~':
			// C-style ones-complement?
			// is this valid in the meta-language?
			// context-dependent? in PRS or not?
			// is bit-wise negation for ints, 
			// logical negation for bools?
			// for now, restrict to bools only...
			if (be) {
				return return_type(new bool_negation_expr(be));
			} else if (ie) {
				return return_type(new int_negation_expr(ie, ch));
			} else {
				cerr << "Unary \'~\' operator requires a "
					"bool or int argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				return return_type(NULL);
			}
		default:
			ICE(cerr, 
				cerr << "Bad operator char \'" << ch << "\' in "
				"prefix_expr::check_nonmeta_expr()!" << endl;
			);
	}
	return return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks for logical-NOT for PRS.  
 */
prs_expr_return_type
prefix_expr::check_prs_expr(context& c) const {
	prs_expr_return_type pe(e->check_prs_expr(c));
	if (!pe) {
		cerr << "ERROR resolving PRS-expr at " << where(*e) <<
			"." << endl;
		THROW_EXIT;		// for now
	}
	if (op->text[0] != '~') {
		ICE(cerr, 
			cerr << "FATAL: Invalid unary operator: \'" <<
			op->text[0] << "\' at " << where(*op) <<
			".  Aborting... have a nice day." << endl;
		);
	}
	return prs_expr_return_type(new entity::PRS::not_expr(pe));
}

//=============================================================================
// class member_expr method definitions

CONSTRUCTOR_INLINE
member_expr::member_expr(const inst_ref_expr* l,
		const token_identifier* m) :
		parent_type(), owner(l), member(m) {
	NEVER_NULL(owner);
	NEVER_NULL(member);
}

DESTRUCTOR_INLINE
member_expr::~member_expr() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(member_expr)

line_position
member_expr::leftmost(void) const {
	return owner->leftmost();
}
line_position
member_expr::rightmost(void) const {
	return member->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Type-check of member reference.  
	Current restriction: left expression must be scalar 0-dimensional.
	\return type-checked meta_instance_reference or null.  
	Really, should never be able to refer to param_expr
	member of an instance.
 */
inst_ref_expr::meta_return_type
member_expr::check_meta_reference(const context& c) const {
	STACKTRACE_VERBOSE;
	typedef	inst_ref_expr::meta_return_type	return_type;
	const return_type o(owner->check_meta_reference(c));
	// useless return value
	// expect: simple_meta_indexed_reference_base on object stack
	if (!o) {
		cerr << "ERROR in base instance reference of member expr at "
			<< where(*owner) << endl;
		THROW_EXIT;
	}
	const return_type::inst_ref_ptr_type inst_ref(o.inst_ref());
	if (!inst_ref) {
		NEVER_NULL(o.value_ref());
		FINISH_ME_EXIT(Fang);
	}
	if (inst_ref->dimensions()) {
		cerr << "ERROR: cannot take the member of a " <<
			inst_ref->dimensions() << "-dimension array, "
			"must be scalar!  (for now...)  " <<
			where(*owner) << endl;
		THROW_EXIT;
	}

	const never_ptr<const definition_base>
		base_def(inst_ref->get_base_def());
	NEVER_NULL(base_def);

	// use that meta_instance_reference, get its referenced definition_base, 
	// and make sure it has a member m, lookup ports only in the 
	// current_definition_reference, don't lookup anywhere else!

	// don't use context's general lookup
#if USE_INSTANCE_PLACEHOLDERS
	never_ptr<const instance_placeholder_base> member_inst;
#else
	never_ptr<const instance_collection_base> member_inst;
#endif
	// NOTE: what about typedefs?  they should lookup using
	// canonical definitions' scopespaces... is this happening?
if (c.is_publicly_viewable()) {
	// FINISH_ME(Fang);
	const never_ptr<const object>
		probe(base_def->lookup_nonparameter_member(*member));
#if USE_INSTANCE_PLACEHOLDERS
	member_inst = probe.is_a<const instance_placeholder_base>();
#else
	member_inst = probe.is_a<const instance_collection_base>();
#endif
	if (!member_inst) {
		base_def->what(cerr << "ERROR: ") << " " <<
			base_def->get_qualified_name() << 
			" has no subinstance member named \"" << *member <<
			"\" at " << where(*member) << endl;
		if (probe) {
			cerr << '(' << *member << " is a ";
			probe->what(cerr) << ')' << endl;
		}
		return meta_return_type(NULL);
	}
} else {
	member_inst = base_def->lookup_port_formal(*member);
	// LATER: check and make sure definition is signed, 
	//	after we introduce forward template declarations
	if (!member_inst) {
		base_def->what(cerr << "ERROR: ") << " " <<
			base_def->get_qualified_name() << 
			" has no public member named \"" << *member <<
			"\" at " << where(*member) << endl;
		THROW_EXIT;
	}
}
	// build a member reference from parent and child
	const meta_return_type
	ret_inst_ref(member_inst->make_member_meta_instance_reference(inst_ref));
	return ret_inst_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Type-check of member reference.  
	Current restriction: left expression must be scalar 0-dimensional.
	\return type-checked meta_instance_reference or null.  
	Really, should never be able to refer to param_expr
	member of an instance.
 */
inst_ref_expr::nonmeta_return_type
member_expr::check_nonmeta_reference(const context& c) const {
	typedef	inst_ref_expr::nonmeta_return_type	return_type;
#if 0
	const return_type o(owner->check_nonmeta_reference(c));
	// expect simple_nonmeta_instance_reference_base returned
	if (!o) {
		cerr << "ERROR in base nonmeta instance reference of "
			"member expr at " << where(*owner) << endl;
		THROW_EXIT;
	}
	const count_ptr<const simple_nonmeta_instance_reference_base>
		inst_ref(o.is_a<const simple_nonmeta_instance_reference_base>());
	INVARIANT(inst_ref);
	if (inst_ref->dimensions()) {
		cerr << "ERROR: cannot take the member of a " <<
			inst_ref->dimensions() << "-dimension array, "
			"must be scalar!  (for now...)  " <<
			where(*owner) << endl;
		THROW_EXIT;
	}

	const never_ptr<const definition_base>
		base_def(inst_ref->get_base_def());
	NEVER_NULL(base_def);

	// use that meta_instance_reference, get its referenced definition_base, 
	// and make sure it has a member m, lookup ports only in the 
	// current_definition_reference, don't lookup anywhere else!

	// don't use context's general lookup
	const never_ptr<const instance_collection_base>
		member_inst(base_def->lookup_port_formal(*member));
	// LATER: check and make sure definition is signed, 
	//	after we introduce forward template declarations
	if (!member_inst) {
		base_def->what(cerr << "ERROR: ") << " " <<
			base_def->get_qualified_name() << 
			" has no public member named \"" << *member <<
			"\" at " << where(*member) << endl;
		THROW_EXIT;
	}

	const nonmeta_return_type
	ret_inst_ref(member_inst->make_member_nonmeta_instance_reference(
		inst_ref));
	return ret_inst_ref;
#else
	cerr << "Not enabled yet: member_nonmeta_instance_references, "
		"bug Fang about it." << endl;
	return return_type(NULL);
#endif
}

//=============================================================================
// class index_expr method definitions

CONSTRUCTOR_INLINE
index_expr::index_expr(const inst_ref_expr* l, const range_list* i) :
		parent_type(), base(l), ranges(i) {
	NEVER_NULL(base);
	NEVER_NULL(ranges);
}

DESTRUCTOR_INLINE
index_expr::~index_expr() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(index_expr)

line_position
index_expr::leftmost(void) const {
	return base->leftmost();
}

line_position
index_expr::rightmost(void) const {
	return ranges->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For convenience, intercept type-checked indices first, exit on error.
 */
range_list::checked_meta_indices_type
index_expr::intercept_meta_indices_error(const context& c) const {
	STACKTRACE_VERBOSE;
	const range_list::checked_meta_indices_type
		checked_indices(ranges->check_meta_indices(c));
	// should result in a HAC::entity::meta_index_list
	// what happened to object_list::make_index_list() ?
	if (!checked_indices) {
		cerr << "ERROR in index list!  " << where(*ranges) << endl;
		THROW_EXIT;
	}
	return checked_indices;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For convenience, intercept type-checked indices first, exit on error.
 */
range_list::checked_nonmeta_indices_type
index_expr::intercept_nonmeta_indices_error(const context& c) const {
	const range_list::checked_nonmeta_indices_type
		checked_indices(ranges->check_nonmeta_indices(c));
	// should result in a HAC::entity::meta_index_list
	// what happened to object_list::make_index_list() ?
	if (!checked_indices) {
		cerr << "ERROR in nonmeta index list!  " <<
			where(*ranges) << endl;
		return range_list::checked_nonmeta_indices_type(NULL);
	}
	return checked_indices;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For convenience, exit upon error.  
 */
inst_ref_expr::meta_return_type
index_expr::intercept_base_meta_ref_error(const context& c) const {
	STACKTRACE_VERBOSE;
	// should result in an meta_instance_reference
	const inst_ref_expr::meta_return_type
		base_expr(base->check_meta_reference(c));
	if (!base_expr) {
		cerr << "ERROR in base meta_instance_reference!  "
			<< where(*base) << endl;
		THROW_EXIT;
	}
	return base_expr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For convenience, exit upon error.  
 */
inst_ref_expr::nonmeta_return_type
index_expr::intercept_base_nonmeta_ref_error(const context& c) const {
	// should result in an meta_instance_reference
	const inst_ref_expr::nonmeta_return_type
		base_expr(base->check_nonmeta_reference(c));
	if (!base_expr) {
		cerr << "ERROR in base nonmeta_instance_reference!  "
			<< where(*base) << endl;
		THROW_EXIT;
	}
	return base_expr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Build's an indexed reference from base and index.  
	Check index expression first, must be an integer type.  
	\return pointer to meta_instance_reference_base.  
 */
inst_ref_expr::meta_return_type
index_expr::check_meta_reference(const context& c) const {
	STACKTRACE_VERBOSE;
	range_list::checked_meta_indices_type
		checked_indices(intercept_meta_indices_error(c));
	const inst_ref_expr::meta_return_type
		base_expr(intercept_base_meta_ref_error(c));

	// later this may be a member_meta_instance_reference...
	// should cast to meta_instance_reference_base instead, 
	// abstract attach_indices
	const count_ptr<simple_meta_indexed_reference_base>
		base_inst(base_expr.inst_ref() ?
			base_expr.inst_ref()
				.is_a<simple_meta_indexed_reference_base>() :
			base_expr.value_ref()
				.is_a<simple_meta_indexed_reference_base>()
			);
	NEVER_NULL(base_inst);

	excl_ptr<range_list::checked_meta_indices_type::element_type>
		passing_indices(checked_indices.exclusive_release());
	const bad_bool ai(base_inst->attach_indices(passing_indices));
	if (ai.bad) {
		cerr << where(*ranges) << endl;
		THROW_EXIT;
	}
	// return indexed instance reference
	return base_expr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Build's an indexed reference from base and index.  
	Check index expression first, must be an integer type.  
	\return pointer to meta_instance_reference_base.  
 */
inst_ref_expr::nonmeta_return_type
index_expr::check_nonmeta_reference(const context& c) const {
	typedef	inst_ref_expr::nonmeta_return_type	return_type;
	STACKTRACE_VERBOSE;
	range_list::checked_nonmeta_indices_type
		checked_indices(intercept_nonmeta_indices_error(c));
	if (!checked_indices) {
		// already printed error message
		return return_type(NULL);
	}
	const inst_ref_expr::nonmeta_return_type
		base_expr(intercept_base_nonmeta_ref_error(c));

	// later this may be a member_meta_instance_reference...
	// should cast to meta_instance_reference_base instead, 
	// abstract attach_indices
	const count_ptr<simple_nonmeta_instance_reference_base>
		base_inst(base_expr.is_a<simple_nonmeta_instance_reference_base>());
	NEVER_NULL(base_inst);

	excl_ptr<range_list::checked_nonmeta_indices_type::element_type>
		passing_indices(checked_indices.exclusive_release());
	const bad_bool ai(base_inst->attach_indices(passing_indices));
	if (ai.bad) {
		cerr << where(*ranges) << endl;
		return return_type(NULL);
	}
	// return indexed instance reference
	return base_inst.is_a<return_type::element_type>();
}

//=============================================================================
// class binary_expr method definitions

CONSTRUCTOR_INLINE
binary_expr::binary_expr(const expr* left, const char_punctuation_type* o, 
		const expr* right) :
		expr(), l(left), op(o), r(right) {
	NEVER_NULL(l); NEVER_NULL(op); NEVER_NULL(r);
}

DESTRUCTOR_INLINE
binary_expr::~binary_expr() {
}

line_position
binary_expr::leftmost(void) const {
	return l->leftmost();
}

line_position
binary_expr::rightmost(void) const {
	return r->rightmost();
}

//=============================================================================
// class arith_expr method definitions

CONSTRUCTOR_INLINE
arith_expr::arith_expr(const expr* left, const char_punctuation_type* o, 
		const expr* right) :
		binary_expr(left, o, right) {
}

DESTRUCTOR_INLINE
arith_expr::~arith_expr() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(arith_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::meta_return_type
arith_expr::check_meta_expr(const context& c) const {
	typedef	expr::meta_return_type	return_type;
	const return_type lo(l->check_meta_expr(c));
	const return_type ro(r->check_meta_expr(c));
	if (!ro || !lo) {
		static const char err_str[] = "ERROR building expression at ";
		if (!lo)
			cerr << err_str << where(*l) << endl;
		if (!ro)
			cerr << err_str << where(*r) << endl;
		return return_type(NULL);
	}
	const count_ptr<pint_expr> li(lo.is_a<pint_expr>());
	const count_ptr<pint_expr> ri(ro.is_a<pint_expr>());
	const count_ptr<preal_expr> lr(lo.is_a<preal_expr>());
	const count_ptr<preal_expr> rr(ro.is_a<preal_expr>());
	const count_ptr<pbool_expr> lb(lo.is_a<pbool_expr>());
	const count_ptr<pbool_expr> rb(ro.is_a<pbool_expr>());
	const char ch = op->text[0];
if (li && ri) {
	// else is safe to make pint_arith_expr object
	const count_ptr<pint_arith_expr>
		ret(new entity::pint_arith_expr(li, ch, ri));
	// try to fold parse-time constant
	if (ret->is_static_constant()) {
		try {
			return return_type(
				new pint_const(ret->static_constant_value()));
		} catch (std::exception& e) {
			// possibly divide by zero
			cerr << e.what() << endl;
			cerr << "Caught numerical exception." << endl;
			return return_type(NULL);
		}
	} else {
		return ret;
	}
} else if (lr && rr) {
	const count_ptr<preal_arith_expr>
		ret(new entity::preal_arith_expr(lr, ch, rr));
	// try to fold parse-time constant
	if (ret->is_static_constant()) {
		try {
			return return_type(
				new preal_const(ret->static_constant_value()));
		} catch (std::exception& e) {
			// possibly divide by zero
			cerr << e.what() << endl;
			cerr << "Caught numerical exception." << endl;
			return return_type(NULL);
		}
	} else {
		return ret;
	}
} else if (lb && rb) {
	// accidentally using arithmetic operations on booleans
	// will forgive with a warning for now...
	string op_str;
	switch(ch) {
	case '&':
		cerr << "WARNING: Use \'&&\' for logical-AND on pbools.  "
			<< where(*op) << endl;
		op_str = "&&";
		break;
	case '|':
		cerr << "WARNING: Use \'||\' for logical-OR on pbools.  "
			<< where(*op) << endl;
		op_str = "||";
		break;
	case '^':
		cerr << "WARNING: Use \'!=\' for logical-XOR on pbools.  "
			<< where(*op) << endl;
		op_str = "!=";
		break;
	default:
		cerr << "Bad operator char \'" << ch << "\' in "
			"arith_expr::check_build()!" << endl;
		return return_type(NULL);
	}
	const count_ptr<pbool_logical_expr>
		ret(new entity::pbool_logical_expr(lb, op_str, rb));
	// try to fold parse-time constant
	if (ret->is_static_constant()) {
		// no exceptions
		return return_type(
			new pbool_const(ret->static_constant_value()));
	} else {
		return ret;
	}
} else {
	static const char err_str[] =
		"ERROR: arith_expr expects two pints or two preals, but got:";
	cerr << err_str << endl;
	lo->what(cerr << '\t') << " at " << where(*l) << endl;
	ro->what(cerr << '\t') << " at " << where(*r) << endl;
	return return_type(NULL);
}
}	// end method arith_expr::check_meta_expr

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: support reals eventually.  
	Bother with folding consts?
 */
nonmeta_expr_return_type
arith_expr::check_nonmeta_expr(const context& c) const {
	typedef	nonmeta_expr_return_type	return_type;
	const return_type lo(l->check_nonmeta_expr(c));
	const return_type ro(r->check_nonmeta_expr(c));
	if (!ro || !lo) {
		static const char
			err_str[] = "ERROR building non-meta expression at ";
		if (!lo)
			cerr << err_str << where(*l) << endl;
		if (!ro)
			cerr << err_str << where(*r) << endl;
		return return_type(NULL);
	}
	// for now, only operate integer arithmetic on int_exprs
	// TODO: operator overloading on user-defined types, YEAH RIGHT!
	const count_ptr<int_expr> li(lo.is_a<int_expr>());
	const count_ptr<int_expr> ri(ro.is_a<int_expr>());
	const count_ptr<bool_expr> lb(lo.is_a<bool_expr>());
	const count_ptr<bool_expr> rb(ro.is_a<bool_expr>());
	const char ch = op->text[0];
	if (li && ri) {
		return return_type(new entity::int_arith_expr(li, ch, ri));
	} else if (lb && rb) {
		// we'll forgive using the wrong operator for now...
		switch (ch) {
		case '&':
			cerr << "WARNING: Use \'&&\' for logical-AND on bools.  "
				<< where(*op) << endl;
			return return_type(new entity::bool_logical_expr(lb, "&&", rb));
		case '|':
			cerr << "WARNING: Use \'||\' for logical-OR on bools.  "
				<< where(*op) << endl;
			return return_type(new entity::bool_logical_expr(lb, "||", rb));
		case '^':
			cerr << "WARNING: Use \'!=\' for logical-XOR on bools.  "
				<< where(*op) << endl;
			return return_type(new entity::bool_logical_expr(lb, "!=", rb));
		default:
			cerr << "ERROR: unrecognized operator \'" << op->text
				<< "\' at " << where(*op) << endl;
			return return_type(NULL);
		}
	} else {
		cerr << "ERROR: int_arith_expr expected ints, but got:\n\t";
		lo->what(cerr) << " at " << where(*l) << "\n\t";
		ro->what(cerr) << " at " << where(*r) << endl;
		return return_type(NULL);
	}
}	// end method arith_expr::check_nonmeta_expr

//=============================================================================
// class relational_expr method definitions

CONSTRUCTOR_INLINE
relational_expr::relational_expr(const expr* left, 
		const char_punctuation_type* o, const expr* right) :
		binary_expr(left, o, right) {
}

DESTRUCTOR_INLINE
relational_expr::~relational_expr() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(relational_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs a relation expression object, depending on whether
	operands are both integer or both real.  
	TODO: issue warning about floating-point equality comparisons.  
 */
expr::meta_return_type
relational_expr::check_meta_expr(const context& c) const {
	typedef	expr::meta_return_type	return_type;
	const return_type lo(l->check_meta_expr(c));
	const return_type ro(r->check_meta_expr(c));
	if (!ro || !lo) {
		static const char err_str[] = "ERROR building expression at ";
		if (!lo)
			cerr << err_str << where(*l) << endl;
		if (!ro)
			cerr << err_str << where(*r) << endl;
		return return_type(NULL);
	}
	const count_ptr<pint_expr> li(lo.is_a<pint_expr>());
	const count_ptr<pint_expr> ri(ro.is_a<pint_expr>());
	const count_ptr<preal_expr> lr(lo.is_a<preal_expr>());
	const count_ptr<preal_expr> rr(ro.is_a<preal_expr>());
	const count_ptr<pbool_expr> lb(lo.is_a<pbool_expr>());
	const count_ptr<pbool_expr> rb(ro.is_a<pbool_expr>());
	const string op_str(op->text);
	// maintainence: 
	// could let expr_type::static_constant_value() or resolve_value()
	// do the work, rather than copy-inline here.  
if (li && ri) {
	const count_ptr<pint_relational_expr>
		ret(new entity::pint_relational_expr(li, op_str, ri));
	if (ret->is_static_constant()) {
		return return_type(
			new pbool_const(ret->static_constant_value()));
	} else {
		return ret;
	}
} else if (lr && rr) {
	const count_ptr<preal_relational_expr>
		ret(new entity::preal_relational_expr(lr, op_str, rr));
	if (ret->is_static_constant()) {
		return return_type(
			new pbool_const(ret->static_constant_value()));
	} else {
		return ret;
	}
} else if (lb && rb) {
	const entity::pbool_logical_expr::op_type*
		o(entity::pbool_logical_expr::op_map[op_str]);
	if (!o) {
		cerr << "ERROR: operator \'" << op_str <<
			"\' not supported.  " << where(*op) << endl;
		return return_type(NULL);
	}
	const count_ptr<pbool_logical_expr>
		ret(new entity::pbool_logical_expr(lb, op_str, rb));
	if (ret->is_static_constant()) {
		return return_type(
			new pbool_const(ret->static_constant_value()));
	} else {
		return ret;
	}
} else {
	cerr << "ERROR: relational_expr expects two operands of the same type, "
		"but got:" << endl;
	lo->what(cerr << '\t') << " at " << where(*l) << endl;
	ro->what(cerr << '\t') << " at " << where(*r) << endl;
	return return_type(NULL);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: add support for reals if it ever comes about.
 */
nonmeta_expr_return_type
relational_expr::check_nonmeta_expr(const context& c) const {
	typedef	nonmeta_expr_return_type	return_type;
	const return_type lo(l->check_nonmeta_expr(c));
	const return_type ro(r->check_nonmeta_expr(c));
	if (!ro || !lo) {
		static const char err_str[] = "ERROR building expression at ";
		if (!lo)
			cerr << err_str << where(*l) << endl;
		if (!ro)
			cerr << err_str << where(*r) << endl;
		return return_type(NULL);
	}
	const count_ptr<int_expr> li(lo.is_a<int_expr>());
	const count_ptr<int_expr> ri(ro.is_a<int_expr>());
	const count_ptr<bool_expr> lb(lo.is_a<bool_expr>());
	const count_ptr<bool_expr> rb(ro.is_a<bool_expr>());
	if (li && ri) {
		const string op_str(op->text);
		const entity::int_relational_expr::op_type*
			o(entity::int_relational_expr::op_map[op_str]);
		INVARIANT(o);
		return return_type(new entity::int_relational_expr(li, o, ri));
	} else if (lb && rb) {
		const string op_str(op->text);
		const entity::bool_logical_expr::op_type*
			o(entity::bool_logical_expr::op_map[op_str]);
		if (!o) {
			cerr << "ERROR: operator \'" << op_str <<
				"\' not supported.  " << where(*op) << endl;
			return return_type(NULL);
		}
		return return_type(new entity::bool_logical_expr(lb, o, rb));
	} else {
		static const char err_str[] =
			"ERROR relational_expr expected two ints, "
				"or two bools, but got:\n\t";
		lo->what(cerr << err_str) << " at " << where(*l);
		ro->what(cerr << "\n\t") << " at " << where(*r) << endl;
		return return_type(NULL);
	}
}

//=============================================================================
// class logical_expr method definitions

CONSTRUCTOR_INLINE
logical_expr::logical_expr(const expr* left, const char_punctuation_type* o, 
		const expr* right) :
		binary_expr(left, o, right) {
}

DESTRUCTOR_INLINE
logical_expr::~logical_expr() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(logical_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::meta_return_type
logical_expr::check_meta_expr(const context& c) const {
	typedef	expr::meta_return_type	return_type;
	const return_type lo(l->check_meta_expr(c));
	const return_type ro(r->check_meta_expr(c));
	if (!ro || !lo) {
		static const char err_str[] = "ERROR building expression at ";
		if (!lo)
			cerr << err_str << where(*l) << endl;
		if (!ro)
			cerr << err_str << where(*r) << endl;
		return return_type(NULL);
	}
	const count_ptr<pbool_expr> lb(lo.is_a<pbool_expr>());
	const count_ptr<pbool_expr> rb(ro.is_a<pbool_expr>());
	const count_ptr<pint_expr> li(lo.is_a<pint_expr>());
	const count_ptr<pint_expr> ri(ro.is_a<pint_expr>());
	// else is safe to make entity::relational_expr object
	const string op_str(op->text);
if (lb && rb) {
	entity::pbool_logical_expr::op_type const* const
		o(entity::pbool_logical_expr::op_map[op_str]);
	if (!o) {
		cerr << "ERROR: \"" << op_str << "\" is not a valid "
			"boolean logical operator, at " << where(*op) << endl;
		return return_type(NULL);
	}
	if (lb->is_static_constant() && rb->is_static_constant()) {
		const bool lc = lb->static_constant_value();
		const bool rc = rb->static_constant_value();
		return return_type(new pbool_const((*o)(lc,rc)));
	} else {
		return return_type(new entity::pbool_logical_expr(lb, o, rb));
	}
} else if (li && ri) {
	const char ch = op->text[0];
	switch (ch) {
	case '&':
		cerr << "WARNING: use \'&\' for bitwise-AND on pints.  "
			<< where(*op) << endl;
		return return_type(new entity::pint_arith_expr(li, ch, ri));
	case '|':
		cerr << "WARNING: use \'|\' for bitwise-OR on pints.  "
			<< where(*op) << endl;
		return return_type(new entity::pint_arith_expr(li, ch, ri));
	case '!':
		cerr << "WARNING: use \'^\' for bitwise-XOR on pints.  "
			<< where(*op) << endl;
		return return_type(new entity::pint_arith_expr(li, ch, ri));
	default:
		cerr << "ERROR: \"" << op_str << "\" is not a valid "
			"boolean logical operator, at " << where(*op) << endl;
	}
	return return_type(NULL);
} else {
	static const char err_str[] =
		"ERROR relational_expr expected a pbool, but got a ";
	if (!lb) {
		cerr << err_str << lo->what(cerr) <<
			" at " << where(*l) << endl;
	}
	if (!rb) {
		cerr << err_str << ro->what(cerr) <<
			" at " << where(*r) << endl;
	}
	return return_type(NULL);
}
}	// end method logical_expr::check_meta_expr

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_expr_return_type
logical_expr::check_nonmeta_expr(const context& c) const {
	typedef	nonmeta_expr_return_type	return_type;
	const return_type lo(l->check_nonmeta_expr(c));
	const return_type ro(r->check_nonmeta_expr(c));
	if (!ro || !lo) {
		static const char err_str[] = "ERROR building expression at ";
		if (!lo)
			cerr << err_str << where(*l) << endl;
		if (!ro)
			cerr << err_str << where(*r) << endl;
		return return_type(NULL);
	}
	const count_ptr<bool_expr> lb(lo.is_a<bool_expr>());
	const count_ptr<bool_expr> rb(ro.is_a<bool_expr>());
	const count_ptr<int_expr> li(lo.is_a<int_expr>());
	const count_ptr<int_expr> ri(ro.is_a<int_expr>());
	// else is safe to make entity::bool_logical_expr object
	const string op_str(op->text);
if (lb && rb) {
	entity::bool_logical_expr::op_type const* const
		o(entity::bool_logical_expr::op_map[op_str]);
	if (!o) {
		cerr << "ERROR: \"" << op_str << "\" is not a valid "
			"boolean logical operator, at " << where(*op) << endl;
		return return_type(NULL);
	}
	return return_type(new entity::bool_logical_expr(lb, o, rb));
} else if (li && ri) {
	const char ch = op->text[0];
	switch (ch) {
	case '&':
		cerr << "WARNING: use \'&\' for bitwise-AND on ints.  "
			<< where(*op) << endl;
		return return_type(new entity::int_arith_expr(li, ch, ri));
	case '|':
		cerr << "WARNING: use \'|\' for bitwise-OR on ints.  "
			<< where(*op) << endl;
		return return_type(new entity::int_arith_expr(li, ch, ri));
	case '!':
		cerr << "WARNING: use \'^\' for bitwise-XOR on ints.  "
			<< where(*op) << endl;
		return return_type(new entity::int_arith_expr(li, ch, ri));
	default:
		cerr << "ERROR: \"" << op_str << "\" is not a valid "
			"boolean logical operator, at " << where(*op) << endl;
	}
	return return_type(NULL);
} else {
	static const char err_str[] =
		"ERROR relational_expr expected a bool, but got a ";
	if (!lb) {
		cerr << err_str << lo->what(cerr) <<
			" at " << where(*l) << endl;
	}
	if (!rb) {
		cerr << err_str << ro->what(cerr) <<
			" at " << where(*r) << endl;
	}
	return return_type(NULL);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't forget to check for cases of PRS loop expressions. 
 */
prs_expr_return_type
logical_expr::check_prs_expr(context& c) const {
	STACKTRACE("parser::PRS::logical_expr::check_prs_expr()");
	const prs_expr_return_type lo(l->check_prs_expr(c));
	const prs_expr_return_type ro(r->check_prs_expr(c));
	if (!ro || !lo) {
		static const char err_str[] = "ERROR building PRS-expr at ";
		if (!lo)
			cerr << err_str << where(*l) << endl;
		if (!ro)
			cerr << err_str << where(*r) << endl;
		THROW_EXIT;		// for now
		return prs_expr_return_type(NULL);
	}
#if 0
	lo->check();
	ro->check();
#endif
	const char op_char = op->text[0];
	if (op_char == '&') {
		typedef	entity::PRS::and_expr::iterator		iterator;
		typedef	entity::PRS::and_expr::const_iterator	const_iterator;
		const count_ptr<entity::PRS::and_expr>
			l_and(lo.is_a<entity::PRS::and_expr>());
		const count_ptr<entity::PRS::and_expr>
			r_and(ro.is_a<entity::PRS::and_expr>());
		if (l_and && !l_and.is_a<entity::PRS::and_expr_loop>()) {
			if (r_and) {
				copy(r_and->begin(), r_and->end(), 
					back_inserter(*l_and));
			} else {
				l_and->push_back(ro);
			}
			return l_and;
		} else if (r_and && !r_and.is_a<entity::PRS::and_expr_loop>()) {
			r_and->push_front(lo);
			return r_and;
		} else {
			const count_ptr<entity::PRS::and_expr>
				ret(new entity::PRS::and_expr);
			ret->push_back(lo);
			ret->push_back(ro);
//			ret->check();	// paranoia
			return ret;
		}
	} else if (op_char == '|') {
		typedef	entity::PRS::or_expr::iterator		iterator;
		typedef	entity::PRS::or_expr::const_iterator	const_iterator;
		const count_ptr<entity::PRS::or_expr>
			l_or(lo.is_a<entity::PRS::or_expr>());
		const count_ptr<entity::PRS::or_expr>
			r_or(ro.is_a<entity::PRS::or_expr>());
		if (l_or && !l_or.is_a<entity::PRS::or_expr_loop>()) {
			if (r_or) {
				copy(r_or->begin(), r_or->end(), 
					back_inserter(*l_or));
			} else {
				l_or->push_back(ro);
			}
			return l_or;
		} else if (r_or && !r_or.is_a<entity::PRS::or_expr_loop>()) {
			r_or->push_front(lo);
			return r_or;
		} else {
			const count_ptr<entity::PRS::or_expr>
				ret(new entity::PRS::or_expr);
			ret->push_back(lo);
			ret->push_back(ro);
//			ret->check();	// paranoia
			return ret;
		}
	} else {
		ICE(cerr, 
			cerr << "FATAL: Invalid PRS operor: \'" << op_char <<
				"\' at " << where(*op) <<
				".  Aborting... have a nice day." << endl;
		);
		return prs_expr_return_type(NULL);
	}
}

//=============================================================================
// class array_concatenation method definitions

array_concatenation::array_concatenation(const expr* e) :
		expr(), parent_type(e) {
	NEVER_NULL(e);
}

array_concatenation::~array_concatenation() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(array_concatenation)

line_position
array_concatenation::leftmost(void) const {
	return parent_type::leftmost();
}

line_position
array_concatenation::rightmost(void) const {
	return parent_type::rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If list contains only a single element, don't bother 
	constructing an aggregate object on the stack, 
	just do the check_build of the lone object.  
 */
expr::meta_return_type
array_concatenation::check_meta_expr(const context& c) const {
	if (size() == 1) {
		const const_iterator only(begin());
		return (*only)->check_meta_expr(c);
	} else {
		// pass true to indicate concatenation
		return aggregate_check_meta_expr(*this, c, true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_expr_return_type
array_concatenation::check_nonmeta_expr(const context& c) const {
	if (size() == 1) {
		const const_iterator only(begin());
		return (*only)->check_nonmeta_expr(c);
	} else {
		FINISH_ME(Fang);
		return nonmeta_expr_return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::generic_meta_return_type
array_concatenation::check_meta_generic(const context& c) const {
	STACKTRACE("array_concatenation::check_meta_generic()");
	if (size() == 1) {
		const const_iterator only(begin());
		return (*only)->check_meta_generic(c);
	} else {
		// pass true to indicate concatenation
		return aggregate_check_meta_generic(*this, c, true);
	}
}

//=============================================================================
// class loop_concatenation method definitions

loop_concatenation::loop_concatenation(
		const char_punctuation_type* l, 
		const token_identifier* i,   
		const range* rng,
		const expr* e,
		const char_punctuation_type* r) :
		lp(l), id(i), bounds(rng), ex(e), rp(r) {
	NEVER_NULL(id); NEVER_NULL(bounds); NEVER_NULL(ex);
}
		
loop_concatenation::~loop_concatenation() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(loop_concatenation)

line_position
loop_concatenation::leftmost(void) const {
	if (lp)		return lp->leftmost();
	else		return id->leftmost();
}

line_position
loop_concatenation::rightmost(void) const {
	if (rp)		return rp->rightmost();
	else 		return ex->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::meta_return_type
loop_concatenation::check_meta_expr(const context& c) const {
	FINISH_ME(Fang);
	return expr::meta_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_expr_return_type
loop_concatenation::check_nonmeta_expr(const context& c) const {
	FINISH_ME(Fang);
	return nonmeta_expr_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::generic_meta_return_type
loop_concatenation::check_meta_generic(const context& c) const {
	FINISH_ME(Fang);
	return expr::generic_meta_return_type();
}

//=============================================================================
// class array_construction method definitions

array_construction::array_construction(const char_punctuation_type* l,
		const expr_list* e, const char_punctuation_type* r) : 
		expr(), lb(l), ex(e), rb(r) {
	NEVER_NULL(ex);
}

array_construction::~array_construction() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(array_construction)

line_position
array_construction::leftmost(void) const {
	if (lb)		return lb->leftmost();
	else		return ex->leftmost();
}

line_position
array_construction::rightmost(void) const {
	if (rb)		return rb->rightmost();
	else		return ex->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Code partially ripped from check_meta_generic, below.  
 */
expr::meta_return_type
aggregate_check_meta_expr(const expr_list& el, const context& c, 
		const bool cat) {
	typedef	expr::meta_return_type			return_type;
	typedef	expr_list::checked_meta_exprs_type	checked_array_type;
	checked_array_type	temp;
	el.postorder_check_meta_exprs(temp, c);
	// pass 'false' to indicate construction, not concatenation
	const return_type
		ret(expr_list::make_aggregate_value_reference(temp, cat));
	if (!ret) {
		cerr << "Error constructing aggregate expression.  "
			<< where(el) << endl;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::meta_return_type
array_construction::check_meta_expr(const context& c) const {
	return aggregate_check_meta_expr(*ex, c, false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We may not allow aggregates in the non-meta language.  
 */
nonmeta_expr_return_type
array_construction::check_nonmeta_expr(const context& c) const {
	FINISH_ME(Fang);
	return nonmeta_expr_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We don't know yet whether we have a value assignment or
	alias connection.  So we check for both cases here.  
 */
expr::generic_meta_return_type
aggregate_check_meta_generic(const expr_list& el, const context& c, 
		const bool cat) {
	typedef	expr::generic_meta_return_type	return_type;
	typedef	expr_list::checked_meta_generic_type	checked_array_type;
	checked_array_type	temp;
	el.postorder_check_meta_generic(temp, c);
	const checked_array_type::const_iterator first_obj(temp.begin());
	// going to use the first object to determine whether to construct
	// aggregate value reference or aggregate instance reference
	if (!first_obj->first && !first_obj->second) {
		cerr << "Error checking first subreference of aggregate.  "
			<< where(el) << endl;
		return return_type();
	} else if (first_obj->first) {
		// then we have expressions and value references to combine
		expr_list::checked_meta_exprs_type checked_exprs;
		expr_list::select_checked_meta_exprs(temp, checked_exprs);
		// pass 'false' to indicate construction, not concatenation
		const meta_expr_return_type
		ret(expr_list::make_aggregate_value_reference(
				checked_exprs, cat));
		if (!ret) {
			cerr << "Error building aggregate value reference.  "
				<< where(el) << endl;
		}
		return return_type(ret, inst_ref_meta_return_type(NULL));
	} else {
		// then we have instance references to combine
		INVARIANT(first_obj->second);
		expr_list::checked_meta_refs_type checked_refs;
		expr_list::select_checked_meta_refs(temp, checked_refs);
		// pass 'false' to indicate construction, not concatenation
		const inst_ref_meta_return_type
		ret(inst_ref_expr_list::make_aggregate_instance_reference(
				checked_refs, cat));
		if (!ret) {
			cerr << "Error building aggregate instance reference.  "
				<< where(el) << endl;
		}
		return return_type(meta_expr_return_type(NULL), ret);
	}
	return return_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::generic_meta_return_type
array_construction::check_meta_generic(const context& c) const {
	return aggregate_check_meta_generic(*ex, c, false);
}

//=============================================================================
// class reference_group_construction method definitions

reference_group_construction::reference_group_construction(
		const char_punctuation_type* l,
		const inst_ref_expr_list* e,
		const char_punctuation_type* r) : 
		inst_ref_expr(), lb(l), ex(e), rb(r) {
	NEVER_NULL(ex);
}

reference_group_construction::~reference_group_construction() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(reference_group_construction)

line_position
reference_group_construction::leftmost(void) const {
	if (lb)		return lb->leftmost();
	else		return ex->leftmost();
}

line_position
reference_group_construction::rightmost(void) const {
	if (rb)		return rb->rightmost();
	else		return ex->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We may not allow aggregates in the non-meta language.  
 */
inst_ref_expr::nonmeta_return_type
reference_group_construction::check_nonmeta_reference(const context& c) const {
	FINISH_ME(Fang);
	return nonmeta_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We statically know to construct an instance_reference.  
	Same base code as 2nd part of array_construction::check_meta_expr().
 */
inst_ref_expr::meta_return_type
reference_group_construction::check_meta_reference(const context& c) const {
	typedef	inst_ref_expr::meta_return_type	return_type;
	typedef	inst_ref_expr_list::checked_meta_refs_type
						checked_array_type;
	checked_array_type	checked_refs;
	ex->postorder_check_meta_refs(checked_refs, c);
	// pass 'false' to indicate construction, not concatenation
	const inst_ref_meta_return_type
		ret(inst_ref_expr_list::make_aggregate_instance_reference(
			checked_refs, false));
	if (!ret) {
		cerr << "Error building aggregate instance reference.  "
			<< where(*ex) << endl;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Accumulates multiple references into a shapeless collection.  
	NOTE: this can be recursive, but results in a flat sequence of
	references in any case.  (Grammar doesn't allow nested groups anyhow.)
	\param g the collection of references to aggregate.  
	\return true on error.  
 */
bool
reference_group_construction::check_grouped_literals(
		checked_bool_group_type& g, const context& c) const {
	return ex->postorder_check_grouped_bool_refs(g, c);
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS

template
ostream&
node_list<const token_identifier>::what(ostream&) const;

template
ostream&
node_list<const inst_ref_expr>::what(ostream&) const;

template
line_position
node_list<const inst_ref_expr>::leftmost(void) const;

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __HAC_AST_EXPR_CC__

