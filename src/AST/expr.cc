/**
	\file "AST/expr.cc"
	Class method definitions for HAC::parser, related to expressions.  
	$Id: expr.cc,v 1.43 2011/02/25 23:19:28 fang Exp $
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
#include "util/STL/functional.hh"		// for _Select{1st,2nd}

#include "AST/token.hh"
#include "AST/token_char.hh"
#include "AST/expr.hh"
#include "AST/reference.hh"
#include "AST/range_list.hh"
#include "AST/node_list.tcc"
#include "AST/PRS.hh"			// for precharge
#include "util/sublist.tcc"
#include "AST/parse_context.hh"

// will need these come time for type-checking
#include "Object/inst/instance_placeholder_base.hh"
#include "Object/inst/physical_instance_placeholder.hh"
#include "Object/inst/param_value_placeholder.hh"
#include "Object/def/definition_base.hh"
#include "Object/def/process_definition.hh"
#include "Object/ref/aggregate_meta_value_reference.hh"
#include "Object/ref/aggregate_meta_instance_reference.hh"
#include "Object/ref/simple_meta_instance_reference.hh"
#include "Object/ref/simple_nonmeta_instance_reference_base.hh"
#include "Object/ref/meta_instance_reference_subtypes.hh"
#include "Object/ref/nonmeta_instance_reference_subtypes.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/pbool_const.hh"
#include "Object/expr/preal_const.hh"
#include "Object/expr/meta_range_expr.hh"
#include "Object/expr/dynamic_param_expr_list.hh"
#include "Object/expr/meta_index_list.hh"
#include "Object/expr/nonmeta_index_list.hh"
#include "Object/expr/convert_expr.hh"
#include "Object/expr/pbool_unary_expr.hh"
#include "Object/expr/pint_unary_expr.hh"
#include "Object/expr/pint_arith_expr.hh"
#include "Object/expr/pint_relational_expr.hh"
#include "Object/expr/preal_unary_expr.hh"
#include "Object/expr/preal_arith_expr.hh"
#include "Object/expr/preal_relational_expr.hh"
#include "Object/expr/pbool_logical_expr.hh"
#include "Object/expr/bool_negation_expr.hh"
#include "Object/expr/int_negation_expr.hh"
#include "Object/expr/int_arith_expr.hh"
#include "Object/expr/int_relational_expr.hh"
#include "Object/expr/bool_logical_expr.hh"
#include "Object/expr/pstring_expr.hh"
#include "Object/expr/pstring_relational_expr.hh"
#include "Object/expr/loop_meta_expr.hh"
#include "Object/expr/loop_nonmeta_expr.hh"
#include "Object/expr/nonmeta_func_call.hh"
#include "Object/expr/nonmeta_cast_expr.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/common/namespace.hh"
#include "Object/lang/PRS.hh"
#include "Object/lang/RTE.hh"
#include "Object/type/template_actuals.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/traits/int_traits.hh"
#include "Object/traits/proc_traits.hh"
#include "Object/inst/param_value_collection.hh"
#include "Object/ref/meta_reference_union.hh"

#include "common/ICE.hh"
#include "common/TODO.hh"

#include "util/what.hh"
#include "util/operators.hh"
#include "util/stacktrace.hh"
#include "util/iterator_more.hh"
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
SPECIALIZE_UTIL_WHAT(HAC::parser::extended_connection_actuals,
	"(ext-connection-actuals)")
// SPECIALIZE_UTIL_WHAT(HAC::parser::expr_list, "(expr-list)")
SPECIALIZE_UTIL_WHAT(HAC::parser::qualified_id, "(qualified-id)")
SPECIALIZE_UTIL_WHAT(HAC::parser::id_expr, "(id-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::prefix_expr, "(prefix-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::member_expr, "(member-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::index_expr, "(index-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::arith_expr, "(arith-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::relational_expr, "(relational-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::logical_expr, "(logical-expr)")
SPECIALIZE_UTIL_WHAT(HAC::parser::loop_operation, "(loop-or)")
SPECIALIZE_UTIL_WHAT(HAC::parser::array_concatenation, "(array-concatenation)")
SPECIALIZE_UTIL_WHAT(HAC::parser::loop_concatenation, "(loop-concatenation)")
SPECIALIZE_UTIL_WHAT(HAC::parser::array_construction, "(array-construction)")
SPECIALIZE_UTIL_WHAT(HAC::parser::template_argument_list_pair,
		"(expr-list-pair)")
namespace memory {
// explicit template instantiations
using HAC::parser::expr;
using HAC::parser::inst_ref_expr;
template class count_ptr<const expr>;
template class count_ptr<const inst_ref_expr>;
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace HAC {
namespace parser {
#include "util/using_ostream.hh"
using std::copy;
using std::back_inserter;
using std::transform;
using std::distance;
using std::_Select1st;		// TODO: configure check wrap this
using std::_Select2nd;		// TODO: configure check wrap this
using util::back_insert_assigner;
using entity::expr_dump_context;
using entity::aggregate_meta_value_reference_base;
using entity::aggregate_meta_instance_reference_base;
using entity::meta_instance_reference_base;
using entity::simple_meta_indexed_reference_base;
using entity::simple_nonmeta_instance_reference_base;
using entity::simple_process_meta_instance_reference;
using entity::int_expr;
using entity::bool_expr;
using entity::real_expr;
using entity::int_arith_expr;
using entity::int_negation_expr;
using entity::int_relational_expr;
using entity::bool_logical_expr;
using entity::bool_negation_expr;
using entity::bool_return_cast_expr;
using entity::int_return_cast_expr;
using entity::pint_expr;
using entity::pint_const;
using entity::pbool_expr;
using entity::pbool_const;
using entity::preal_expr;
using entity::preal_const;
using entity::pint_arith_expr;
using entity::pint_unary_expr;
using entity::pint_relational_expr;
using entity::pbool_logical_expr;
using entity::pbool_unary_expr;
using entity::preal_arith_expr;
using entity::preal_unary_expr;
using entity::preal_relational_expr;
using entity::pstring_expr;
using entity::pstring_relational_expr;
using entity::meta_loop_base;
using entity::meta_range_expr;
using entity::nonmeta_func_call;
using entity::physical_instance_placeholder;
using entity::param_value_placeholder;
using entity::convert_pint_to_preal_expr;

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary placeholder, never really supposed to be called.  
 */
rte_expr_return_type
expr::check_rte_expr(context& c) const {
	cerr << "Fang, unimplemented expr::check_rte_expr!" << endl;
	return rte_expr_return_type();
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
	Current limitation, nonmeta expressions must be scalar
	util array expressions and semantics are supported.  
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
		if (data_ref->dimensions()) {
			cerr << "Error: expression at " << where(*this) <<
				" is not scalar.\n"
		"Sorry, array nonmeta expressions are not yet supported."
				<< endl;
			return return_type(NULL);
		}
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
	After checking an meta_instance_reference, this checks to make sure
	that a bool is referenced, appropriate for RTE.  
	RTE allows both atomic and non-atomic bools to be referenced.
 */
rte_lvalue_ptr_type
inst_ref_expr::check_rte_lvalue(const context& c) const {
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
			return rte_lvalue_ptr_type(NULL);
		} else {
			// shared to exclusive ownership
			entity::RTE::literal_base_ptr_type
				lit(bool_ref.exclusive_release());
			return rte_lvalue_ptr_type(
				new entity::RTE::literal(lit));
		}
	} else {
		cerr << "ERROR: expression at " << where(*this) <<
			" does not reference a bool." << endl;
		return rte_lvalue_ptr_type(NULL);
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
bool
inst_ref_expr::check_grouped_literals(checked_proc_group_type& g, 
		const context& c) const {
	const meta_return_type ref(check_meta_reference(c));
	const count_ptr<const simple_process_meta_instance_reference>
		proc_ref(ref.inst_ref().is_a<const simple_process_meta_instance_reference>());
	if (proc_ref) {
		// skip dimensions check
		g.push_back(proc_ref);
		return false;
	} else {
		cerr << "ERROR: expression at " << where(*this) <<
			" does not reference a process." << endl;
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
rte_expr_return_type
inst_ref_expr::check_rte_expr(context& c) const {
	// now virtual
	return check_rte_lvalue(c);
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
	A: No.  Cannot send null value (undefined).  
	Just collects the result of type-checking of items in list.
	\param temp the type-checked result list.
	\param c the context.
	Caller should catch error by checking for NULL.  
 */
void
expr_list::postorder_check_nonmeta_exprs(checked_nonmeta_exprs_type& temp,
		const context& c) const {
	STACKTRACE("expr_list::postorder_check_nonmeta_exprs()");
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	if ((size() == 1) && !*i) {
	// exception: lone NULL argument is allowed for function calls (void)
		return;
	}
	for ( ; i!=e; i++) {
		if (*i) {
			temp.push_back((*i)->check_nonmeta_expr(c));
		} else {
			typedef	checked_nonmeta_exprs_type::value_type
							checked_value_type;
			cerr << "Error: missing rvalue expression in "
				"argument list at position " << 
				distance(begin(), i) +1 << " at " <<
				where(*this) << endl;
			temp.push_back(checked_value_type(NULL));
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
expr_list::select_checked_meta_exprs(const checked_meta_generic_type& src, 
		checked_meta_exprs_type& dst) {
	STACKTRACE_VERBOSE;
	INVARIANT(dst.empty());
	transform(src.begin(), src.end(), back_inserter(dst),
		_Select1st<checked_meta_generic_type::value_type>()
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
expr_list::select_checked_meta_refs(const checked_meta_generic_type& src, 
		checked_meta_refs_type& dst) {
	STACKTRACE_VERBOSE;
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

inst_ref_expr_list::inst_ref_expr_list() : parent_type() { }

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
	STACKTRACE_VERBOSE;
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		temp.push_back((*i)->check_prs_literal(c));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variant allows for optional arguments.  
	\return true on error.
 */
bool
inst_ref_expr_list::postorder_check_bool_refs_optional(
		checked_bool_refs_type& temp, const context& c) const {
	STACKTRACE_VERBOSE;
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		if (*i) {
			temp.push_back((*i)->check_prs_literal(c));
			if (!temp.back()) {
				// TODO: error message
				return true;
			}
		} else {
			temp.push_back(prs_literal_ptr_type(NULL));
		}
	}
	return false;
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
	STACKTRACE_VERBOSE;
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		typedef	checked_bool_groups_type::value_type	group_type;
		temp.push_back(group_type());	// create empty
		// then append in-place (beats creating and copying)
		NEVER_NULL(*i);
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
	Cloned from above.
	This variant collects all constituent references into a single 
	group, not a list of groups.  
	\return true on first error.
 */
bool
inst_ref_expr_list::postorder_check_grouped_bool_refs(
		checked_bool_group_type& temp, const context& c) const {
	STACKTRACE_VERBOSE;
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
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
/**
	Cloned from above.
	Even more specialized: checks amorphous groups of references, 
	relaxing all dimension and packedness requirements.  
	Each element of this list is treated as a group.  
	\return true on first error.  
 */
bool
inst_ref_expr_list::postorder_check_grouped_proc_refs(
		checked_proc_groups_type& temp, const context& c) const {
	STACKTRACE_VERBOSE;
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		typedef	checked_proc_groups_type::value_type	group_type;
		temp.push_back(group_type());	// create empty
		// then append in-place (beats creating and copying)
		NEVER_NULL(*i);
		if ((*i)->check_grouped_literals(temp.back(), c)) {
			// TODO: more specific error message, use std::distance
			cerr << "Error in proc group reference list in "
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
inst_ref_expr_list::postorder_check_grouped_proc_refs(
		checked_proc_group_type& temp, const context& c) const {
	STACKTRACE_VERBOSE;
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		if ((*i)->check_grouped_literals(temp, c)) {
			// TODO: more specific error message, use std::distance
			cerr << "Error in proc group reference list in "
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
	STACKTRACE_VERBOSE;
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		temp.push_back((*i)->check_meta_reference(c));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allow null lvalues.  
	\throw an exception upon error, since values are allowed to be NULL.  
 */
void
inst_ref_expr_list::postorder_check_nonmeta_data_refs(
		checked_nonmeta_data_refs_type& temp, const context& c) const {
	STACKTRACE_VERBOSE;
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		typedef	checked_nonmeta_data_refs_type::value_type
						checked_value_type;
		if (*i) {
			const checked_value_type
				r((*i)->check_nonmeta_data_reference(c));
			if (!r) {
				// already have error message
				THROW_EXIT;
			}
			temp.push_back(r);
		} else {
			// TODO: don't support null lvalues until middle-end
			// propagates expected type information into 
			// run-time type casts for checking.
			// Needed for nonmeta_assign to work correctly.
#if 0
			FINISH_ME_EXIT(Fang);
#else
			cerr << "Fang add support for NULL lvalues!" << endl;
			THROW_EXIT;
#endif
			temp.push_back(checked_value_type(NULL));
		}
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
	TODO: upgrade expressions to generalized template arguments.  
	TODO: make sure strict paramters do not depend on relaxed, 
		relaxed parameters may depend on relaxed parameters.  
	TODO: sugar: check for const_param expressions to make const list?
 */
template_argument_list_pair::return_type
template_argument_list_pair::check_template_args(const context& c) const {
	STACKTRACE_VERBOSE;
	const count_ptr<dynamic_param_expr_list>
		strict(strict_args ?
			new dynamic_param_expr_list(strict_args->size()) :
			NULL);
	if (strict_args) {
		expr_list::checked_meta_exprs_type temp;
		strict_args->postorder_check_meta_exprs(temp, c);
		// NULL are allowed, where should we check?
		copy(temp.begin(), temp.end(), back_inserter(*strict));
		if (c.get_current_prototype() &&
			// is now OK outside of formal context
				strict->is_relaxed_formal_dependent()) {
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
// class expr_attr_list methd definitions

expr_attr_list::expr_attr_list() : expr_list(), attrs() { }

expr_attr_list::expr_attr_list(const expr* e) : expr_list(e), attrs() { }

expr_attr_list::~expr_attr_list() { }

void
expr_attr_list::attach_attributes(const generic_attribute_list* a) {
	attrs = excl_ptr<const generic_attribute_list>(a);
}

//=============================================================================
// class extended_connection_actuals method definitions

extended_connection_actuals::extended_connection_actuals(
		const inst_ref_expr_list* a, 
		const expr_list* b) :
		implicit_ports(a),
		actual_ports(b) {
	// either set of ports are optional (may be NULL)
	INVARIANT(implicit_ports || actual_ports);
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(extended_connection_actuals)

line_position
extended_connection_actuals::leftmost(void) const {
	if (implicit_ports)
		return implicit_ports->leftmost();
	return actual_ports->leftmost();
}

line_position
extended_connection_actuals::rightmost(void) const {
	if (actual_ports)
		return actual_ports->rightmost();
	return implicit_ports->rightmost();
}

//=============================================================================
// class qualified_id method definitions

CONSTRUCTOR_INLINE
qualified_id::qualified_id(const token_identifier* n) : 
		parent_type(n), absolute(NULL) {
}

qualified_id::qualified_id(const count_ptr<const token_identifier>& n) : 
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

ostream&
qualified_id::dump(ostream& o) const {
	const_iterator i(begin()), e(end());
	INVARIANT(i!=e);
	(*i)->dump(o);
	for (++i ; i!=e; ++i) {
		(*i)->dump(o << scope);
	}
	return o;
}

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
never_ptr<const instance_placeholder_base>
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
	NEVER_NULL(qid);
}

id_expr::id_expr(const token_identifier& i) : parent_type(), 
		qid(new qualified_id(new token_identifier(i))) {
	NEVER_NULL(qid);
}

id_expr::id_expr(const count_ptr<const token_identifier>& i) : parent_type(), 
		qid(new qualified_id(i)) {
	NEVER_NULL(qid);
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

ostream&
id_expr::dump(ostream& o) const {
	return qid->dump(o);
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
	const never_ptr<const instance_placeholder_base>
		o(qid->lookup_instance(c));
	if (o) {
		const never_ptr<const instance_placeholder_base>
			inst(o.is_a<const instance_placeholder_base>());
		if (inst) {
			STACKTRACE("valid instance collection found");
			// we found an instance which may be single
			// or collective... info is in inst.
			const never_ptr<const physical_instance_placeholder>
				pinst(inst.is_a<const physical_instance_placeholder>());
			if (pinst) {
				// physical instance collection
				// verify that definition doesn't reference
				// global!
				if (!c.at_top_level() &&
#if PROCESS_DEFINITION_IS_NAMESPACE
					pinst->get_owner() != c.get_current_named_scope()
#else
					pinst->get_owner()
					.is_a<const entity::name_space>()
#endif
					) {
					cerr <<
	"Error: cannot reference top-level instance from within a definition!  "
						<< where(*qid) << endl;
					THROW_EXIT;
				}
				return pinst->make_meta_instance_reference();
			} else {
				// then must be a value collection
				const never_ptr<const param_value_placeholder>
					vinst(inst.is_a<const param_value_placeholder>());
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
	const never_ptr<const instance_placeholder_base>
		o(qid->lookup_instance(c));	// not ->check_build(c);
if (o) {
	const never_ptr<const instance_placeholder_base>
		inst(o.is_a<const instance_placeholder_base>());
	if (inst) {
		STACKTRACE("valid instance collection found");
		// we found an instance which may be single
		// or collective... info is in inst.
		const never_ptr<const physical_instance_placeholder>
			pinst(inst.is_a<const physical_instance_placeholder>());
		if (pinst) {
			if (!c.at_top_level() &&
#if PROCESS_DEFINITION_IS_NAMESPACE
				pinst->get_owner() != c.get_current_named_scope()
#else
				pinst->get_owner()
				.is_a<const entity::name_space>()
#endif
				) {
				cerr <<
	"Error: cannot reference top-level instance from within a definition!  "
					<< where(*qid) << endl;
				return return_type(NULL);
			}
			return pinst->make_nonmeta_instance_reference();
		} else {
			STACKTRACE_INDENT_PRINT("is parameter value" << endl);
			const never_ptr<const param_value_placeholder>
				vinst(inst.is_a<const param_value_placeholder>());
			NEVER_NULL(vinst);
			return inst->make_nonmeta_instance_reference();
		}
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
/**
	This seeds the return array with a base identifier.
 */
int
id_expr::expand_const_reference(
		const count_ptr<const inst_ref_expr>& _this, 
		reference_array_type& a) const {
	STACKTRACE_VERBOSE;
	INVARIANT(_this == this);
	INVARIANT(a.empty());
	a.push_back(_this);
	return 0;
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

ostream&
prefix_expr::dump(ostream& o) const {
	op->dump(o);
	e->dump(o);
	return o;
}

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
	count_ptr<int_expr> ie(pe.is_a<int_expr>());
	count_ptr<bool_expr> be(pe.is_a<bool_expr>());
	const count_ptr<nonmeta_func_call> fe(pe.is_a<nonmeta_func_call>());
	if (fe) {
		// punt to run-time type check
		ie = count_ptr<int_expr>(new int_return_cast_expr(fe));
		be = count_ptr<bool_expr>(new bool_return_cast_expr(fe));
	}

	const int ch = op->text[0];
	switch(ch) {
		case '-':
			// integer negation
			if (ie) {
				return return_type(new int_negation_expr(ie, ch));
			} else {
				cerr << "Unary \'-\' operator requires an "
					"int argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				return return_type(NULL);
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
	Hack: if argument is a literal, bind the negation to the literal
	and return it instead.  
 */
prs_expr_return_type
prefix_expr::check_prs_expr(context& c) const {
	prs_expr_return_type pe(e->check_prs_expr(c));
	if (!pe) {
		cerr << "ERROR resolving PRS-expr at " << where(*e) <<
			"." << endl;
		THROW_EXIT;		// for now
	}
	if ((c.is_rte_syntax_mode() && (op->text[0] != '!'))
		|| (!c.is_rte_syntax_mode() && (op->text[0] != '~'))) {
		ICE(cerr, 
			cerr << "FATAL: Invalid unary operator: \'" <<
			op->text[0] << "\' at " << where(*op) <<
			".  Aborting... have a nice day." << endl;
		);
	}
	// intercept internal node
	typedef	entity::PRS::literal		literal_type;
	const count_ptr<literal_type>
		lit(pe.is_a<literal_type>());
	if (lit) {
		if (lit->is_internal()) {
			lit->negate_node();
			return lit;
		}
	}
	return prs_expr_return_type(new entity::PRS::not_expr(pe));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks for logical-NOT for RTE.  
 */
rte_expr_return_type
prefix_expr::check_rte_expr(context& c) const {
	rte_expr_return_type pe(e->check_rte_expr(c));
	if (!pe) {
		cerr << "ERROR resolving atomic expr at " << where(*e) <<
			"." << endl;
		THROW_EXIT;		// for now
	}
	if (op->text[0] != '!') {
		ICE(cerr, 
			cerr << "FATAL: Invalid unary operator: \'" <<
			op->text[0] << "\' at " << where(*op) <<
			".  Aborting... have a nice day." << endl;
		);
	}
	// intercept internal node
	typedef	entity::RTE::literal		literal_type;
	const count_ptr<literal_type>
		lit(pe.is_a<literal_type>());
	if (lit) {
		if (lit->is_internal()) {
			lit->negate_node();
			return lit;
		}
	}
	return rte_expr_return_type(new entity::RTE::not_expr(pe));
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

member_expr::member_expr(const count_ptr<const inst_ref_expr>& l,
		const count_ptr<const token_identifier>& m) :
		parent_type(), owner(l), member(m) {
	NEVER_NULL(owner);
	NEVER_NULL(member);
}

DESTRUCTOR_INLINE
member_expr::~member_expr() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(member_expr)

ostream&
member_expr::dump(ostream& o) const {
	owner->dump(o) << '.';
	member->dump(o);
	return o;
}

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

	const never_ptr<const definition_base>
		base_def(inst_ref->get_base_def());
	NEVER_NULL(base_def);

	// use that meta_instance_reference, get its referenced definition_base, 
	// and make sure it has a member m, lookup ports only in the 
	// current_definition_reference, don't lookup anywhere else!

	// don't use context's general lookup
	never_ptr<const instance_placeholder_base> member_inst;
	// NOTE: what about typedefs?  they should lookup using
	// canonical definitions' scopespaces... is this happening?
if (c.is_publicly_viewable()) {
	// FINISH_ME(Fang);
	const never_ptr<const object>
		probe(base_def->lookup_nonparameter_member(*member));
	member_inst = probe.is_a<const instance_placeholder_base>();
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
#if !NONMETA_MEMBER_REFERENCES
	cerr << "Not enabled yet: member_nonmeta_instance_references, "
		"bug Fang about it." << endl;
	cerr << "Suggested workaround: create local aliases to member references."
		<< endl;
	return return_type(NULL);
#else
	const return_type o(owner->check_nonmeta_reference(c));
	// expect simple_nonmeta_instance_reference_base returned
	if (!o) {
		cerr << "ERROR in base nonmeta instance reference of "
			"member expr at " << where(*owner) << endl;
		THROW_EXIT;
	}
	const return_type inst_ref(o);
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

// copied from check_meta_reference(), above
	// don't use context's general lookup
	never_ptr<const instance_placeholder_base> member_inst;
	// NOTE: what about typedefs?  they should lookup using
	// canonical definitions' scopespaces... is this happening?
if (c.is_publicly_viewable()) {
	// FINISH_ME(Fang);
	const never_ptr<const object>
		probe(base_def->lookup_nonparameter_member(*member));
	member_inst = probe.is_a<const instance_placeholder_base>();
	if (!member_inst) {
		base_def->what(cerr << "ERROR: ") << " " <<
			base_def->get_qualified_name() << 
			" has no subinstance member named \"" << *member <<
			"\" at " << where(*member) << endl;
		if (probe) {
			cerr << '(' << *member << " is a ";
			probe->what(cerr) << ')' << endl;
		}
		THROW_EXIT;
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
	const nonmeta_return_type
	ret_inst_ref(member_inst->make_member_nonmeta_instance_reference(
		inst_ref));
	return ret_inst_ref;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This can explode into a large number of references.  
 */
int
member_expr::expand_const_reference(
		const count_ptr<const inst_ref_expr>& _this, 
		reference_array_type& a) const {
	STACKTRACE_VERBOSE;
	INVARIANT(_this == this);
	if (owner->expand_const_reference(owner, a)) {
		return 1;	// error
	}
	if (a.size() == 1) {
		// singleton, quick return
		a.front() = _this;
		return 0;
	}
	// else have more than 1
	reference_array_type::iterator i(a.begin()), e(a.end());
	for ( ; i!=e; ++i) {
		*i = count_ptr<const inst_ref_expr>(
			new this_type(*i, member));
#if 0
			(*i)->dump(cerr << "expanded-member: ") << endl;
#endif
	}
	return 0;
}

//=============================================================================
// class index_expr method definitions

CONSTRUCTOR_INLINE
index_expr::index_expr(const inst_ref_expr* l, const range_list* i) :
		parent_type(), base(l), ranges(i) {
	NEVER_NULL(base);
	NEVER_NULL(ranges);
}

index_expr::index_expr(const count_ptr<const inst_ref_expr>& l,
		const count_ptr<const range_list>& i) :
		parent_type(), base(l), ranges(i) {
	NEVER_NULL(base);
	NEVER_NULL(ranges);
}

DESTRUCTOR_INLINE
index_expr::~index_expr() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(index_expr)

ostream&
index_expr::dump(ostream& o) const {
	base->dump(o);
	ranges->dump(o);
	return o;
}

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
	\return the number of dimensions of the referenced instance's
		collection, based on the number of indices, 
		which assumes that this is a scalar reference.  
 */
size_t
index_expr::implicit_dimensions(void) const {
	return ranges ? ranges->size() : 0;
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
	const range_list::checked_meta_indices_type
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

	const bad_bool ai(base_inst->attach_indices(checked_indices));
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This can explode into a large number of references.  
 */
int
index_expr::expand_const_reference(
		const count_ptr<const inst_ref_expr>& _this, 
		reference_array_type& a) const {
	STACKTRACE_VERBOSE;
	INVARIANT(_this == this);
	if (base->expand_const_reference(base, a)) {
		return 1;	// error
	}
	// expand const indices
	range_list::range_list_list_type rtmp;
	if (!ranges->expand_const_indices(rtmp).good) {
		return 1;	// error
	}
	// cross-product
	reference_array_type ret;
	ret.reserve(a.size() * rtmp.size());
	reference_array_type::const_iterator i(a.begin()), e(a.end());
	for ( ; i!=e; ++i) {
		range_list::range_list_list_type::const_iterator
			ri(rtmp.begin()), re(rtmp.end());
		for (; ri!=re; ++ri) {
			const count_ptr<const index_expr>
				n(new index_expr(*i, *ri));
			ret.push_back(n);
#if 0
			n->dump(cerr << "expanded-index: ") << endl;
#endif
		}
	}
	a.swap(ret);
	return 0;
}

//=============================================================================
// class binary_expr method definitions

CONSTRUCTOR_INLINE
binary_expr::binary_expr(const expr* left, const char_punctuation_type* o, 
		const expr* right) :
		expr(), l(left), op(o), r(right) {
	NEVER_NULL(l); NEVER_NULL(op); NEVER_NULL(r);
}

binary_expr::binary_expr(const count_ptr<const expr>& left, 
		const char_punctuation_type* o, 
		const count_ptr<const expr>& right) :
		expr(), l(left), op(o), r(right) {
	NEVER_NULL(l); NEVER_NULL(op); NEVER_NULL(r);
}

DESTRUCTOR_INLINE
binary_expr::~binary_expr() {
}

/**
	Caveat: Is not smart about parenthesization.
 */
ostream&
binary_expr::dump(ostream& o) const {
	l->dump(o);
	op->dump(o);
	l->dump(o);
	return o;
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

arith_expr::arith_expr(const count_ptr<const expr>& left, 
		const char_punctuation_type* o, 
		const count_ptr<const expr>& right) :
		binary_expr(left, o, right) {
	NEVER_NULL(l); NEVER_NULL(op); NEVER_NULL(r);
}

DESTRUCTOR_INLINE
arith_expr::~arith_expr() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(arith_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: factor out operator checks into common code somewhere, 
		for consistency and maintainability.  
 */
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
	const char ch2 = op->text[1];
if (li && ri) {
#if 1
	// catch misuse of operators
	switch (ch2) {
	case '&':
		cerr << "Error: Use \'&&\' for logical-AND on pbools.  "
			<< where(*op) << endl;
		return return_type(NULL);
	case '|':
		cerr << "Error: Use \'||\' for logical-OR on pbools.  "
			<< where(*op) << endl;
		return return_type(NULL);
	default: {}
	}
#endif
	// else is safe to make pint_arith_expr object
	const count_ptr<pint_arith_expr>
		ret(new entity::pint_arith_expr(li, ch, ri));
	// try to fold parse-time constant
	if (ret->is_static_constant()) {
		try {
			return return_type(
				new pint_const(ret->static_constant_value()));
		} catch (const std::exception& e) {
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
		} catch (const std::exception& e) {
			// possibly divide by zero
			cerr << e.what() << endl;
			cerr << "Caught numerical exception." << endl;
			return return_type(NULL);
		}
	} else {
		return ret;
	}
#if 1
// accept implicit conversion from pint to preal
} else if (li && rr) {
	const count_ptr<convert_pint_to_preal_expr>
		clr(new convert_pint_to_preal_expr(li));
	const count_ptr<preal_arith_expr>
		ret(new entity::preal_arith_expr(clr, ch, rr));
	return ret;
} else if (lr && ri) {
	const count_ptr<convert_pint_to_preal_expr>
		crr(new convert_pint_to_preal_expr(ri));
	const count_ptr<preal_arith_expr>
		ret(new entity::preal_arith_expr(lr, ch, crr));
	return ret;
#endif
} else if (lb && rb) {
	// accidentally using arithmetic operations on booleans
	// will forgive with a warning for now...
	string op_str;
	switch(ch) {
	case '&':
		if (!ch2) {
		cerr << "WARNING: Use \'&&\' for logical-AND on pbools.  "
			<< where(*op) << endl;
		op_str = "&&";
		++c.warning_count;
		}
		break;
	case '|':
		if (!ch2) {
		cerr << "WARNING: Use \'||\' for logical-OR on pbools.  "
			<< where(*op) << endl;
		op_str = "||";
		++c.warning_count;
		}
		break;
	case '^':
		// I'm tempted to allow this...
		cerr << "WARNING: Use \'!=\' for logical-XOR on pbools.  "
			<< where(*op) << endl;
		op_str = "!=";
		++c.warning_count;
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

	count_ptr<int_expr>
		li(lo.is_a<int_expr>()),
		ri(ro.is_a<int_expr>());
	count_ptr<bool_expr>
		lb(lo.is_a<bool_expr>()),
		rb(ro.is_a<bool_expr>());
{
	// hack: intercept operands that are (untyped) function calls, 
	// and wrap them in dynamic-cast expressions
	const count_ptr<nonmeta_func_call>
		lf(lo.is_a<nonmeta_func_call>()),
		rf(ro.is_a<nonmeta_func_call>());
	if (lf) {
		li = count_ptr<int_expr>(new int_return_cast_expr(lf));
		lb = count_ptr<bool_expr>(new bool_return_cast_expr(lf));
	}
	if (rf) {
		ri = count_ptr<int_expr>(new int_return_cast_expr(rf));
		rb = count_ptr<bool_expr>(new bool_return_cast_expr(rf));
	}
}
	const char ch = op->text[0];
	const char ch2 = op->text[1];
	if (li && ri) {
#if 1
		// catch misuse of operators
		switch (ch2) {
		case '&':
			cerr << "Error: Use \'&&\' for logical-AND on pbools.  "
				<< where(*op) << endl;
			return return_type(NULL);
		case '|':
			cerr << "Error: Use \'||\' for logical-OR on pbools.  "
				<< where(*op) << endl;
			return return_type(NULL);
		default: {}
		}
#endif
		return return_type(new entity::int_arith_expr(li, ch, ri));
	} else if (lb && rb) {
		// we'll forgive using the wrong operator for now...
		switch (ch) {
		case '&':
			cerr << "WARNING: Use \'&&\' for logical-AND on bools.  "
				<< where(*op) << endl;
			++c.warning_count;
			return return_type(new entity::bool_logical_expr(lb, "&&", rb));
		case '|':
			cerr << "WARNING: Use \'||\' for logical-OR on bools.  "
				<< where(*op) << endl;
			++c.warning_count;
			return return_type(new entity::bool_logical_expr(lb, "||", rb));
		case '^':
			cerr << "WARNING: Use \'!=\' for logical-XOR on bools.  "
				<< where(*op) << endl;
			++c.warning_count;
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
	const count_ptr<pstring_expr> ls(lo.is_a<pstring_expr>());
	const count_ptr<pstring_expr> rs(ro.is_a<pstring_expr>());
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
	typedef	entity::pbool_logical_expr::op_map_type	op_map_type;
	const op_map_type& op_map(entity::pbool_logical_expr::op_map);
	const op_map_type::const_iterator o(op_map.find(op_str));
	if (o == op_map.end()) {
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
} else if (ls && rs) {
	const count_ptr<pstring_relational_expr>
		ret(new entity::pstring_relational_expr(ls, op_str, rs));
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
	TODO: add support for string expressions.
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
	count_ptr<int_expr>
		li(lo.is_a<int_expr>()),
		ri(ro.is_a<int_expr>());
	const count_ptr<bool_expr>
		lb(lo.is_a<bool_expr>()),
		rb(ro.is_a<bool_expr>());
{
	// hack: intercept operands that are (untyped) function calls, 
	// and wrap them in dynamic-cast expressions
	const count_ptr<nonmeta_func_call>
		lf(lo.is_a<nonmeta_func_call>()),
		rf(ro.is_a<nonmeta_func_call>());
	if (lf) {
		li = count_ptr<int_expr>(new int_return_cast_expr(lf));
	}
	if (rf) {
		ri = count_ptr<int_expr>(new int_return_cast_expr(rf));
	}
}
	if (li && ri) {
		const string op_str(op->text);
		typedef entity::int_relational_expr::op_map_type	op_map_type;
		const op_map_type& op_map(entity::int_relational_expr::op_map);
		const op_map_type::const_iterator o(op_map.find(op_str));
		INVARIANT(o != op_map.end());
		return return_type(new entity::int_relational_expr(li, o->second, ri));
	} else if (lb && rb) {
		const string op_str(op->text);
		typedef	entity::bool_logical_expr::op_map_type	op_map_type;
		const op_map_type& op_map(entity::bool_logical_expr::op_map);
		const op_map_type::const_iterator o(op_map.find(op_str));
		if (o == op_map.end()) {
			cerr << "ERROR: operator \'" << op_str <<
				"\' not supported.  " << where(*op) << endl;
			return return_type(NULL);
		}
		return return_type(new entity::bool_logical_expr(lb, o->second, rb));
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
		binary_expr(left, o, right), 
		pchg(NULL) {
}

logical_expr::logical_expr(const expr* _l,
		const char_punctuation_type* c,
		const PRS::precharge* p,
		const expr* _r) :
		binary_expr(_l, c, _r), 
		pchg(p) {
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
	typedef	entity::pbool_logical_expr::op_map_type	op_map_type;
	const op_map_type& op_map(entity::pbool_logical_expr::op_map);
	const op_map_type::const_iterator o(op_map.find(op_str));
	if (o == op_map.end()) {
		cerr << "ERROR: \"" << op_str << "\" is not a valid "
			"boolean logical operator, at " << where(*op) << endl;
		return return_type(NULL);
	}
	if (lb->is_static_constant() && rb->is_static_constant()) {
		const bool lc = lb->static_constant_value();
		const bool rc = rb->static_constant_value();
		return return_type(new pbool_const((*o->second)(lc,rc)));
	} else {
		return return_type(new entity::pbool_logical_expr(lb, o->second, rb));
	}
} else if (li && ri) {
	const char ch = op->text[0];
	switch (ch) {
	case '&':
		cerr << "WARNING: use \'&\' for bitwise-AND on pints.  "
			<< where(*op) << endl;
		++c.warning_count;
		return return_type(new entity::pint_arith_expr(li, ch, ri));
	case '|':
		cerr << "WARNING: use \'|\' for bitwise-OR on pints.  "
			<< where(*op) << endl;
		++c.warning_count;
		return return_type(new entity::pint_arith_expr(li, ch, ri));
	case '!':
		cerr << "WARNING: use \'^\' for bitwise-XOR on pints.  "
			<< where(*op) << endl;
		++c.warning_count;
		return return_type(new entity::pint_arith_expr(li, ch, ri));
	default:
		cerr << "ERROR: \"" << op_str << "\" is not a valid "
			"boolean logical operator, at " << where(*op) << endl;
	}
	return return_type(NULL);
} else {
	static const char err_str[] =
		"ERROR logical_expr expected a pbool, but got a ";
	if (!lb) {
		lo->what(cerr << err_str) << " at " << where(*l) << endl;
	}
	if (!rb) {
		ro->what(cerr << err_str) << " at " << where(*r) << endl;
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
	count_ptr<bool_expr>
		lb(lo.is_a<bool_expr>()),
		rb(ro.is_a<bool_expr>());
	const count_ptr<int_expr>
		li(lo.is_a<int_expr>()),
		ri(ro.is_a<int_expr>());
{
	// hack: intercept operands that are (untyped) function calls, 
	// and wrap them in dynamic-cast expressions
	const count_ptr<nonmeta_func_call>
		lf(lo.is_a<nonmeta_func_call>()),
		rf(ro.is_a<nonmeta_func_call>());
	if (lf) {
		lb = count_ptr<bool_expr>(new bool_return_cast_expr(lf));
	}
	if (rf) {
		rb = count_ptr<bool_expr>(new bool_return_cast_expr(rf));
	}
}
	// else is safe to make entity::bool_logical_expr object
	const string op_str(op->text);
if (lb && rb) {
	typedef	entity::bool_logical_expr::op_map_type	op_map_type;
	const op_map_type& op_map(entity::bool_logical_expr::op_map);
	const op_map_type::const_iterator o(op_map.find(op_str));
	if (o == op_map.end()) {
		cerr << "ERROR: \"" << op_str << "\" is not a valid "
			"boolean logical operator, at " << where(*op) << endl;
		return return_type(NULL);
	}
	return return_type(new entity::bool_logical_expr(lb, o->second, rb));
} else if (li && ri) {
	const char ch = op->text[0];
	switch (ch) {
	case '&':
		cerr << "WARNING: use \'&\' for bitwise-AND on ints.  "
			<< where(*op) << endl;
		++c.warning_count;
		return return_type(new entity::int_arith_expr(li, ch, ri));
	case '|':
		cerr << "WARNING: use \'|\' for bitwise-OR on ints.  "
			<< where(*op) << endl;
		++c.warning_count;
		return return_type(new entity::int_arith_expr(li, ch, ri));
	case '!':
		cerr << "WARNING: use \'^\' for bitwise-XOR on ints.  "
			<< where(*op) << endl;
		++c.warning_count;
		return return_type(new entity::int_arith_expr(li, ch, ri));
	default:
		cerr << "ERROR: \"" << op_str << "\" is not a valid "
			"boolean logical operator, at " << where(*op) << endl;
	}
	return return_type(NULL);
} else {
	static const char err_str[] =
		"ERROR logical_expr expected a bool, but got a ";
	if (!lb) {
		lo->what(cerr << err_str) <<
			" at " << where(*l) << endl;
	}
	if (!rb) {
		ro->what(cerr << err_str) <<
			" at " << where(*r) << endl;
	}
	return return_type(NULL);
}
}	// end method logical_expr::check_nonmeta_expr

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
	entity::PRS::precharge_expr precharge;	// default
	if (pchg) {
		precharge = pchg->check_prs_expr(c);
		if (!precharge) {
			cerr << "ERROR in precharge expression at " <<
				where(*pchg) << endl;
			THROW_EXIT;		// for now
			return prs_expr_return_type(NULL);
		}
	}
#if 0
	lo->check();
	ro->check();
#endif
	const char op_char = op->text[0];
	// also works for RTE syntax
	if (op_char == '&') {
		typedef	entity::PRS::and_expr::iterator		iterator;
		typedef	entity::PRS::and_expr::const_iterator	const_iterator;
		const count_ptr<entity::PRS::and_expr>
			l_and(lo.is_a<entity::PRS::and_expr>());
		const count_ptr<entity::PRS::and_expr>
			r_and(ro.is_a<entity::PRS::and_expr>());
		if (l_and && !l_and.is_a<entity::PRS::and_expr_loop>()) {
			// TODO: handle precharges in loops
			if (r_and) {
				copy(r_and->begin(), r_and->end(), 
					back_inserter(*l_and));
			} else {
				l_and->push_back(ro, precharge);
			}
			return l_and;
		} else if (r_and && !r_and.is_a<entity::PRS::and_expr_loop>()) {
			r_and->push_front(lo, precharge);
			return r_and;
		} else {
			const count_ptr<entity::PRS::and_expr>
				ret(new entity::PRS::and_expr(lo));
			// ret->push_back(lo);
			ret->push_back(ro, precharge);
//			ret->check();	// paranoia
			return ret;
		}
	// also works for RTE syntax
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
			cerr << "FATAL: Invalid PRS operator: \'" << op_char <<
				"\' at " << where(*op) <<
				".  Aborting... have a nice day." << endl;
		);
		return prs_expr_return_type(NULL);
	}
}	// end method logical_expr::check_prs_expr

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't forget to check for cases of PRS loop expressions. 
 */
rte_expr_return_type
logical_expr::check_rte_expr(context& c) const {
	STACKTRACE("parser::RTE::logical_expr::check_rte_expr()");
	const rte_expr_return_type lo(l->check_rte_expr(c));
	const rte_expr_return_type ro(r->check_rte_expr(c));
	if (!ro || !lo) {
		static const char err_str[] = "ERROR building RTE-expr at ";
		if (!lo)
			cerr << err_str << where(*l) << endl;
		if (!ro)
			cerr << err_str << where(*r) << endl;
		THROW_EXIT;		// for now
		return rte_expr_return_type(NULL);
	}
#if 0
	lo->check();
	ro->check();
#endif
	const char op_char = op->text[0];
	const char op_char2 = op->text[1];
	if ((op_char == '&' && op_char2 == '&') ||
		(op_char == '|' && op_char2 == '|')) {
		typedef	entity::RTE::binop_expr::iterator	iterator;
		typedef	entity::RTE::binop_expr::const_iterator	const_iterator;
		const count_ptr<entity::RTE::binop_expr>
			l_and(lo.is_a<entity::RTE::binop_expr>());
		const count_ptr<entity::RTE::binop_expr>
			r_and(ro.is_a<entity::RTE::binop_expr>());
		// assumes operator associativity
		if (l_and && l_and->get_op() == op_char
#if 0
			&& !l_and.is_a<entity::RTE::binop_expr_loop>()
#endif
				) {
			if (r_and) {
				copy(r_and->begin(), r_and->end(), 
					back_inserter(*l_and));
			} else {
				l_and->push_back(ro);
			}
			return l_and;
		} else if (r_and && r_and->get_op() == op_char
#if 0
				&& !r_and.is_a<entity::RTE::binop_expr_loop>()
#endif
				) {
			r_and->push_front(lo);
			return r_and;
		} else {
			const count_ptr<entity::RTE::binop_expr>
				ret(new entity::RTE::binop_expr(lo, op_char));
			ret->push_back(ro);
//			ret->check();	// paranoia
			return ret;
		}
	} else {
		ICE(cerr, 
			cerr << "FATAL: Invalid RTE operator: \'" << op_char <<
				"\' at " << where(*op) <<
				".  Aborting... have a nice day." << endl;
		);
		return rte_expr_return_type(NULL);
	}
}	// end method logical_expr::check_rte_expr

//=============================================================================
// class loop_operation method definitions

loop_operation::loop_operation(const char_punctuation_type* l,
		const char_punctuation_type* o, 
		const token_identifier* id, const range* b,
		const expr* e, const char_punctuation_type* r) :
		lp(l), op(o), index(id), bounds(b), body(e), rp(r) {
	NEVER_NULL(op);
	NEVER_NULL(index);
	NEVER_NULL(bounds);
	NEVER_NULL(body);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
loop_operation::~loop_operation() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARSER_WHAT_DEFAULT_IMPLEMENTATION(loop_operation)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
loop_operation::dump(ostream& o) const {
	FINISH_ME(Fang);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
loop_operation::leftmost(void) const {
	if (lp)
		return lp->leftmost();
	else	return op->leftmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
loop_operation::rightmost(void) const {
	if (rp)
		return rp->leftmost();
	else	return body->leftmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_return_type
loop_operation::check_prs_expr(context& c) const {
	FINISH_ME(Fang);
	return prs_expr_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: context shouldn't be const, needs temporary loop frame.  
 */
expr::meta_return_type
loop_operation::check_meta_expr(const context& c) const {
	typedef	expr::meta_return_type	return_type;
	const range::meta_return_type rng(bounds->check_meta_index(c));
	if (!rng) {
		cerr << "Error in loop range at " << where(*bounds) << endl;
		return return_type(NULL);
	}
	const entity::meta_loop_base::range_ptr_type
		loop_range(meta_range_expr::make_explicit_range(rng));
	NEVER_NULL(loop_range);
	// create new loop scope for checking:
	const context::loop_var_frame _lvf(const_cast<context&>(c), *index);
		// we promise context will be restored before this returns
	const meta_loop_base::ind_var_ptr_type& loop_ind(_lvf.var);
	if (!loop_ind) {
		cerr << "Error registering loop variable: " << *index <<
			" at " << where(*index) << endl;
		return return_type(NULL);
	}

	const return_type lo(body->check_meta_expr(c));
	if (!lo) {
		static const char err_str[] = "ERROR building expression at ";
		cerr << err_str << where(*body) << endl;
		return return_type(NULL);
	}
	const count_ptr<pbool_expr> lb(lo.is_a<pbool_expr>());
	const count_ptr<pint_expr> li(lo.is_a<pint_expr>());
	const count_ptr<preal_expr> lr(lo.is_a<preal_expr>());
	string op_str(op->text);
if (lb) {
#if 1
	switch (op->text[0]) {
		// I make an exception for loop operators
	case '^':
		op_str = "!=";
		break;
	default: {}
	}
#endif
	typedef	entity::pbool_logical_expr::op_map_type	op_map_type;
	const op_map_type& op_map(entity::pbool_logical_expr::op_map);
	const op_map_type::const_iterator o(op_map.find(op_str));
	if (o == op_map.end()) {
		cerr << "ERROR: \"" << op_str << "\" is not a valid "
			"boolean logical operator, at " << where(*op) << endl;
		return return_type(NULL);
	}
	return return_type(new entity::pbool_logical_loop_expr(
		loop_ind, loop_range, lb, o->second));
} else if (li) {
	const char ch = op->text[0];
#if 1
	const char ch2 = op->text[1];
	switch (ch2) {
	case '&':
		cerr << "Error: Use \'&&\' for logical-AND on pbools.  "
			<< where(*op) << endl;
		return return_type(NULL);
	case '|':
		cerr << "Error: Use \'||\' for logical-OR on pbools.  "
			<< where(*op) << endl;
		return return_type(NULL);
	default: {}
	}
#endif
	typedef	entity::pint_arith_expr::op_map_type	op_map_type;
	const op_map_type& op_map(entity::pint_arith_expr::op_map);
	const op_map_type::const_iterator o(op_map.find(ch));
	if (o == op_map.end()) {
		cerr << "ERROR: \"" << op_str << "\" is not a valid "
			"integer arithmetic operator, at " <<
			where(*op) << endl;
		return return_type(NULL);
	}
	return return_type(new entity::pint_arith_loop_expr(
		loop_ind, loop_range, li, o->second));
	return return_type(NULL);
} else if (lr) {
	FINISH_ME(Fang);
	cerr << "Haven\'t expanded this function for preals yet..." << endl;
	return return_type(NULL);
} else {
	FINISH_ME(Fang);
	cerr << "WTF?" << endl;
	return return_type(NULL);
}
}	// end method loop_operation::check_meta_expr

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: context shouldn't be const, needs temporary loop frame.  
	TODO: make better re-use of code...
 */
nonmeta_expr_return_type
loop_operation::check_nonmeta_expr(const context& c) const {
	typedef	nonmeta_expr_return_type	return_type;
	const range::meta_return_type rng(bounds->check_meta_index(c));
	if (!rng) {
		cerr << "Error in loop range at " << where(*bounds) << endl;
		return return_type(NULL);
	}
	const entity::meta_loop_base::range_ptr_type
		loop_range(meta_range_expr::make_explicit_range(rng));
	NEVER_NULL(loop_range);
	// create new loop scope for checking:
	const context::loop_var_frame _lvf(const_cast<context&>(c), *index);
	// we promise context will be restored before this returns
	const meta_loop_base::ind_var_ptr_type& loop_ind(_lvf.var);
	if (!loop_ind) {
		cerr << "Error registering loop variable: " << *index <<
			" at " << where(*index) << endl;
		return return_type(NULL);
	}

	const return_type lo(body->check_nonmeta_expr(c));
	if (!lo) {
		static const char err_str[] = "ERROR building expression at ";
		cerr << err_str << where(*body) << endl;
		return return_type(NULL);
	}
	const count_ptr<bool_expr> lb(lo.is_a<bool_expr>());
	const count_ptr<int_expr> li(lo.is_a<int_expr>());
	const count_ptr<real_expr> lr(lo.is_a<real_expr>());
	string op_str(op->text);
if (lb) {
#if 1
	switch (op->text[0]) {
		// I make an exception for loop operators
	case '^':
		op_str = "!=";
		break;
	default: {}
	}
#endif
	typedef	entity::bool_logical_expr::op_map_type	op_map_type;
	const op_map_type& op_map(entity::bool_logical_expr::op_map);
	const op_map_type::const_iterator o(op_map.find(op_str));
	if (o == op_map.end()) {
		cerr << "ERROR: \"" << op_str << "\" is not a valid "
			"boolean logical operator, at " << where(*op) << endl;
		return return_type(NULL);
	}
	return return_type(new entity::bool_logical_loop_expr(
		loop_ind, loop_range, lb, o->second));
} else if (li) {
	const char ch = op->text[0];
#if 1
	const char ch2 = op->text[1];
	switch (ch2) {
	case '&':
		cerr << "Error: Use \'&&\' for logical-AND on pbools.  "
			<< where(*op) << endl;
		return return_type(NULL);
	case '|':
		cerr << "Error: Use \'||\' for logical-OR on pbools.  "
			<< where(*op) << endl;
		return return_type(NULL);
	default: {}
	}
#endif
	typedef	entity::int_arith_expr::op_map_type	op_map_type;
	const op_map_type& op_map(entity::int_arith_expr::op_map);
	const op_map_type::const_iterator o(op_map.find(ch));
	if (o == op_map.end()) {
		cerr << "ERROR: \"" << op_str << "\" is not a valid "
			"integer arithmetic operator, at " <<
			where(*op) << endl;
		return return_type(NULL);
	}
	return return_type(new entity::int_arith_loop_expr(
		loop_ind, loop_range, li, o->second));
	return return_type(NULL);
} else if (lr) {
	FINISH_ME(Fang);
	cerr << "Haven\'t expanded this function for reals yet..." << endl;
	return return_type(NULL);
} else {
	FINISH_ME(Fang);
	cerr << "WTF?" << endl;
	return return_type(NULL);
}
}	// end method loop_operation::check_nonmeta_expr

//=============================================================================
// class array_concatenation method definitions

array_concatenation::array_concatenation(const expr* e) :
		expr(), parent_type(e) {
	NEVER_NULL(e);
}

array_concatenation::~array_concatenation() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(array_concatenation)

ostream&
array_concatenation::dump(ostream& o) const {
	FINISH_ME(Fang);
	return o;
}

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

ostream&
loop_concatenation::dump(ostream& o) const {
	FINISH_ME(Fang);
	return o;
}

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

ostream&
array_construction::dump(ostream& o) const {
	FINISH_ME(Fang);
	return o;
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

ostream&
reference_group_construction::dump(ostream& o) const {
	FINISH_ME(Fang);
	return o;
}

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

bool
reference_group_construction::check_grouped_literals(
		checked_proc_group_type& g, const context& c) const {
	return ex->postorder_check_grouped_proc_refs(g, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not yet implemented.
 */
int
reference_group_construction::expand_const_reference(
		const count_ptr<const inst_ref_expr>& _this, 
		reference_array_type& a) const {
	STACKTRACE_VERBOSE;
	INVARIANT(_this == this);
	FINISH_ME(Fang);
	a.clear();
	return 1;
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS

template
ostream&
node_list<const token_identifier>::what(ostream&) const;

template
line_position
node_list<const token_identifier>::leftmost(void) const;

template
line_position
node_list<const token_identifier>::rightmost(void) const;

template
ostream&
node_list<const inst_ref_expr>::what(ostream&) const;

template
line_position
node_list<const inst_ref_expr>::leftmost(void) const;

template
line_position
node_list<const inst_ref_expr>::rightmost(void) const;

template
ostream&
node_list<const expr>::what(ostream&) const;

template
line_position
node_list<const expr>::leftmost(void) const;

template
line_position
node_list<const expr>::rightmost(void) const;

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __HAC_AST_EXPR_CC__

