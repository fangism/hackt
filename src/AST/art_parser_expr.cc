/**
	\file "AST/art_parser_expr.cc"
	Class method definitions for HAC::parser, related to expressions.  
	$Id: art_parser_expr.cc,v 1.27.12.1 2005/12/11 00:45:05 fang Exp $
 */

#ifndef	__AST_HAC_PARSER_EXPR_CC__
#define	__AST_HAC_PARSER_EXPR_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#define	ENABLE_STACKTRACE		0

#include <iterator>

#include "AST/art_parser_token.h"
#include "AST/art_parser_token_char.h"
#include "AST/art_parser_expr.h"
#include "AST/art_parser_expr_list.h"
#include "AST/art_parser_range_list.h"
#include "AST/art_parser_node_list.tcc"
#include "util/sublist.tcc"
#include "AST/parse_context.h"

// will need these come time for type-checking
#include "Object/inst/instance_collection_base.h"
#include "Object/def/definition_base.h"
#include "Object/ref/simple_datatype_meta_instance_reference_base.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/simple_nonmeta_instance_reference_base.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/meta_index_list.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/expr/pbool_unary_expr.h"
#include "Object/expr/pint_unary_expr.h"
#include "Object/expr/pint_arith_expr.h"
#include "Object/expr/pint_relational_expr.h"
#include "Object/expr/pbool_logical_expr.h"
#include "Object/expr/bool_negation_expr.h"
#include "Object/expr/int_negation_expr.h"
#include "Object/expr/int_arith_expr.h"
#include "Object/expr/int_relational_expr.h"
#include "Object/expr/bool_logical_expr.h"
#include "Object/lang/PRS.h"
#include "Object/type/template_actuals.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"

#include "common/ICE.h"

#include "util/what.h"
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

//=============================================================================
// class expr method definitions

#if 0
/// Empty constructor
CONSTRUCTOR_INLINE
expr::expr() { }

/// Empty virtual destructor
DESTRUCTOR_INLINE
expr::~expr() { }
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	All non-inst-ref expressions will dynamically cast
	the result of check_meta_expr to an instance reference.  
	This is overridden by inst_ref_expr::check_meta_generic.
	\return pair of typed pointers.  
 */
expr::generic_meta_return_type
expr::check_meta_generic(context& c) const {
	STACKTRACE("expr::check_meta_generic()");
	const expr::meta_return_type ret(check_meta_expr(c));
	return generic_meta_return_type(ret,
		ret.is_a<inst_ref_meta_return_type::element_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default behavior for non-overridden check_nonmeta_expr.
 */
nonmeta_expr_return_type
expr::check_nonmeta_expr(context& c) const {
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
inst_ref_expr::check_meta_generic(context& c) const {
	STACKTRACE("inst_ref_expr::check_meta_generic()");
	const meta_return_type ret(check_meta_reference(c));
	return generic_meta_return_type(
		ret.is_a<expr::meta_return_type::element_type>(), ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should really not be called...
 */
expr::meta_return_type
inst_ref_expr::check_meta_expr(context& c) const {
	STACKTRACE("inst_ref_expr::check_meta_expr() (should not be called)");
#if 0
	return check_meta_reference(c).is_a<expr::meta_return_type::element_type>();
#else
	typedef	expr::meta_return_type::element_type	param_type;
	const inst_ref_expr::meta_return_type inst_ref(check_meta_reference(c));
	const expr::meta_return_type param_ref(inst_ref.is_a<param_type>());
	if (param_ref) {
		// accepted
		return param_ref;
	} else {
		cerr << "ERROR: Expression at " << where(*this) <<
			" does not refer to a parameter." << endl;
		THROW_EXIT;
		return expr::meta_return_type(NULL);
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_expr_return_type
inst_ref_expr::check_nonmeta_expr(context& c) const {
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
inst_ref_expr::check_prs_literal(context& c) const {
	STACKTRACE_VERBOSE;
	meta_return_type ref(check_meta_reference(c));
	count_ptr<simple_bool_meta_instance_reference>
		bool_ref(ref.is_a<simple_bool_meta_instance_reference>());
	if (bool_ref) {
		ref.abandon();
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
	Down-casting literal to general PRS guard expression.  
 */
prs_expr_return_type
inst_ref_expr::check_prs_expr(context& c) const {
	return check_prs_literal(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inst_ref_expr::nonmeta_data_return_type
inst_ref_expr::check_nonmeta_data_reference(context& c) const {
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
		context& c) const {
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
		context& c) const {
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
		context& c) const {
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

//=============================================================================
// class expr_list method definitions

// inst_ref_expr_list::inst_ref_expr_list() : parent_type() { }

inst_ref_expr_list::inst_ref_expr_list(const inst_ref_expr* e) :
		parent_type(e) { }

inst_ref_expr_list::~inst_ref_expr_list() { }

void
inst_ref_expr_list::postorder_check_nonmeta_data_refs(
		checked_nonmeta_data_refs_type& temp, context& c) const {
	STACKTRACE("inst_ref_expr_list::postorder_check_nonmeta_data_refs()");
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		temp.push_back((*i)->check_nonmeta_data_reference(c));
	}
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
template_argument_list_pair::check_template_args(context& c) const {
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
qualified_id::check_build(context& c) const {
	return c.lookup_object(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Performs unqualified lookup or qualified lookup of identifier.  
	This is what id_expr::check_build() should call.  
 */
never_ptr<const instance_collection_base>
qualified_id::lookup_instance(context& c) const {
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
id_expr::check_meta_reference(context& c) const {
	STACKTRACE_VERBOSE;
	// lookup_instance will check for unqualified references first
	const never_ptr<const instance_collection_base>
		o(qid->lookup_instance(c));
	if (o) {
		const never_ptr<const instance_collection_base>
			inst(o.is_a<const instance_collection_base>());
		if (inst) {
			STACKTRACE("valid instance collection found");
			// we found an instance which may be single
			// or collective... info is in inst.
			return inst->make_meta_instance_reference();
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
id_expr::check_nonmeta_reference(context& c) const {
	typedef inst_ref_expr::nonmeta_return_type	return_type;
	STACKTRACE_VERBOSE;
	const never_ptr<const object>
		o(qid->check_build(c));		// will lookup_object
	if (o) {
		const never_ptr<const instance_collection_base>
			inst(o.is_a<const instance_collection_base>());
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
prefix_expr::check_meta_expr(context& c) const {
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

	const int ch = op->text[0];
	switch(ch) {
		case '-':
			// integer negation
			if (!ie) {
				cerr << "Unary \'-\' operator requires a "
					"pint argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				return return_type(NULL);
			} else if (ie->is_static_constant()) {
				// constant simplification
				return return_type(new pint_const(
					- ie->static_constant_value()));
			} else {
				return return_type(new pint_unary_expr(ch, ie));
			}
		case '!':
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
					!ie->static_constant_value()));
			} else {
				return return_type(new pint_unary_expr(ch, ie));
			}
		case '~':
			// is this valid in the meta-language?
			// context-dependent? in PRS or not?
			// is bit-wise negation for ints, 
			// logical negation for bools?
			// for now, restrict to bools only...
			if (!be) {
				cerr << "Unary \'~\' operator requires a "
					"pbool argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				return return_type(NULL);
			}
			if (be->is_static_constant()) {
				// constant simplification
				return return_type(new pbool_const(
					!be->static_constant_value()));
			} else {
				return return_type(
					new pbool_unary_expr(be, ch));
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
prefix_expr::check_nonmeta_expr(context& c) const {
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
				return return_type(new int_negation_expr(ie));
			}
		case '!':
			// integer logical negation
#if 0
			if (!ie) {
				cerr << "Unary \'!\' operator requires a "
					"pint argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				return return_type(NULL);
			} else {
				return return_type(new something(ie));
			}
#else
			cerr << "DOH.  !(int) -> bool operation "
				"not yet supported... bug Fang." << endl;
			return return_type(NULL);
#endif
		case '~':
			// C-style ones-complement?
			// is this valid in the meta-language?
			// context-dependent? in PRS or not?
			// is bit-wise negation for ints, 
			// logical negation for bools?
			// for now, restrict to bools only...
			if (!be) {
				cerr << "Unary \'~\' operator requires a "
					"bool argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				return return_type(NULL);
			} else {
				return return_type(new bool_negation_expr(be));
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
member_expr::check_meta_reference(context& c) const {
	STACKTRACE_VERBOSE;
	typedef	inst_ref_expr::meta_return_type	return_type;
	const return_type o(owner->check_meta_reference(c));
	// useless return value
	// expect: simple_meta_instance_reference_base on object stack
	if (!o) {
		cerr << "ERROR in base instance reference of member expr at "
			<< where(*owner) << endl;
		THROW_EXIT;
	}
	const count_ptr<const simple_meta_instance_reference_base>
		inst_ref(o.is_a<const simple_meta_instance_reference_base>());
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

	const meta_return_type
	ret_inst_ref(member_inst->make_member_meta_instance_reference(inst_ref));

	// old comments:
	// what should this return?  the same thing it expects:
	// a reference to an instance of some type.  
	// Problem: instances aren't concrete until they are unrolled.
	// What is available in type-check phase?
	// Maybe we don't care about the instances themselves, 
	// rather the *type* returned.  
	// after all this is type-checking, not range checking.  

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
member_expr::check_nonmeta_reference(context& c) const {
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
index_expr::intercept_meta_indices_error(context& c) const {
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
index_expr::intercept_nonmeta_indices_error(context& c) const {
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
index_expr::intercept_base_meta_ref_error(context& c) const {
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
index_expr::intercept_base_nonmeta_ref_error(context& c) const {
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
index_expr::check_meta_reference(context& c) const {
	STACKTRACE_VERBOSE;
	range_list::checked_meta_indices_type
		checked_indices(intercept_meta_indices_error(c));
	const inst_ref_expr::meta_return_type
		base_expr(intercept_base_meta_ref_error(c));

	// later this may be a member_meta_instance_reference...
	// should cast to meta_instance_reference_base instead, 
	// abstract attach_indices
	const count_ptr<simple_meta_instance_reference_base>
		base_inst(base_expr.is_a<simple_meta_instance_reference_base>());
	NEVER_NULL(base_inst);

	excl_ptr<range_list::checked_meta_indices_type::element_type>
		passing_indices(checked_indices.exclusive_release());
	const bad_bool ai(base_inst->attach_indices(passing_indices));
	if (ai.bad) {
		cerr << where(*ranges) << endl;
		THROW_EXIT;
	}
	// return indexed instance reference
	return base_inst;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Build's an indexed reference from base and index.  
	Check index expression first, must be an integer type.  
	\return pointer to meta_instance_reference_base.  
 */
inst_ref_expr::nonmeta_return_type
index_expr::check_nonmeta_reference(context& c) const {
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
	return base_inst;
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
arith_expr::check_meta_expr(context& c) const {
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
	if (!li || !ri) {
		static const char err_str[] =
			"ERROR arith_expr expected a pint, but got a ";
		if (!li) {
			cerr << err_str << lo->what(cerr) <<
				" at " << where(*l) << endl;
		}
		if (!ri) {
			cerr << err_str << ro->what(cerr) <<
				" at " << where(*r) << endl;
		}
		return return_type(NULL);
	}
	// else is safe to make arith_expr object
	const char ch = op->text[0];
	if (li->is_static_constant() && ri->is_static_constant()) {
		const int lc = li->static_constant_value();
		const int rc = ri->static_constant_value();
		switch(ch) {
			case '+': return return_type(new pint_const(lc +rc));
			case '-': return return_type(new pint_const(lc -rc));
			case '*': return return_type(new pint_const(lc *rc));
			case '/':
				if (!rc) {
					cerr << "Detected divide by 0 at " <<
						where(*this) << endl;
					THROW_EXIT;
				}
				return return_type(new pint_const(lc /rc));
			case '%':
				if (!rc) {
					cerr << "Detected divide by 0 at " <<
						where(*this) << endl;
					THROW_EXIT;
				}
				return return_type(new pint_const(lc %rc));
			default:
				cerr << "Bad operator char \'" << ch << "\' in "
					"arith_expr::check_build()!" << endl;
				DIE;
		}
		return return_type(NULL);	// never reached
	} else {
		return return_type(new entity::pint_arith_expr(li, ch, ri));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_expr_return_type
arith_expr::check_nonmeta_expr(context& c) const {
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
	if (!li || !ri) {
		static const char err_str[] =
			"ERROR: int_arith_expr expected an int, but got a ";
		if (!li) {
			lo->what(cerr << err_str) <<
				" at " << where(*l) << endl;
		}
		if (!ri) {
			ro->what(cerr << err_str) <<
				" at " << where(*r) << endl;
		}
		return return_type(NULL);
	}
	const char ch = op->text[0];
	return return_type(new entity::int_arith_expr(li, ch, ri));
}

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
expr::meta_return_type
relational_expr::check_meta_expr(context& c) const {
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
	if (!li || !ri) {
		static const char err_str[] =
			"ERROR relational_expr expected a pint, but got a ";
		if (!li) {
			cerr << err_str << lo->what(cerr) <<
				" at " << where(*l) << endl;
		}
		if (!ri) {
			cerr << err_str << ro->what(cerr) <<
				" at " << where(*r) << endl;
		}
		return return_type(NULL);
	}
	// else is safe to make entity::relational_expr object
	const string op_str(op->text);
	const entity::pint_relational_expr::op_type*
		o(entity::pint_relational_expr::op_map[op_str]);
	INVARIANT(o);
	if (li->is_static_constant() && ri->is_static_constant()) {
		const int lc = li->static_constant_value();
		const int rc = ri->static_constant_value();
		return return_type(new pbool_const((*o)(lc,rc)));
	} else {
		return return_type(new entity::pint_relational_expr(li, o, ri));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_expr_return_type
relational_expr::check_nonmeta_expr(context& c) const {
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
	if (!li || !ri) {
		static const char err_str[] =
			"ERROR relational_expr expected an int, but got a ";
		if (!li) {
			cerr << err_str << lo->what(cerr) <<
				" at " << where(*l) << endl;
		}
		if (!ri) {
			cerr << err_str << ro->what(cerr) <<
				" at " << where(*r) << endl;
		}
		return return_type(NULL);
	}
	// else is safe to make entity::relational_expr object
	const string op_str(op->text);
	const entity::int_relational_expr::op_type*
		o(entity::int_relational_expr::op_map[op_str]);
	INVARIANT(o);
	return return_type(new entity::int_relational_expr(li, o, ri));
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
logical_expr::check_meta_expr(context& c) const {
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
	if (!lb || !rb) {
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
	// else is safe to make entity::relational_expr object
	const string op_str(op->text);
	entity::pbool_logical_expr::op_type const* const
		o(entity::pbool_logical_expr::op_map[op_str]);
	INVARIANT(o);
	if (lb->is_static_constant() && rb->is_static_constant()) {
		const bool lc = lb->static_constant_value();
		const bool rc = rb->static_constant_value();
		return return_type(new pbool_const((*o)(lc,rc)));
	} else {
		return return_type(new entity::pbool_logical_expr(lb, o, rb));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_expr_return_type
logical_expr::check_nonmeta_expr(context& c) const {
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
	if (!lb || !rb) {
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
	// else is safe to make entity::bool_logical_expr object
	const string op_str(op->text);
	entity::bool_logical_expr::op_type const* const
		o(entity::bool_logical_expr::op_map[op_str]);
	INVARIANT(o);
	return return_type(new entity::bool_logical_expr(lb, o, rb));
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

array_concatenation::array_concatenation(const expr* e) : expr(), parent(e) {
	NEVER_NULL(e);
}

array_concatenation::~array_concatenation() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(array_concatenation)

line_position
array_concatenation::leftmost(void) const {
	return parent::leftmost();
}

line_position
array_concatenation::rightmost(void) const {
	return parent::rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If list contains only a single element, don't bother 
	constructing an aggregate object on the stack, 
	just do the check_build of the lone object.  
 */
expr::meta_return_type
array_concatenation::check_meta_expr(context& c) const {
	if (size() == 1) {
		const const_iterator only(begin());
		return (*only)->check_meta_expr(c);
	} else {
		cerr << "Fang, finish array_concatenation::check_meta_expr()!"
			<< endl;
		return expr::meta_return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_expr_return_type
array_concatenation::check_nonmeta_expr(context& c) const {
	if (size() == 1) {
		const const_iterator only(begin());
		return (*only)->check_nonmeta_expr(c);
	} else {
		cerr << "Fang, finish array_concatenation::check_nonmeta_expr()!"
			<< endl;
		return nonmeta_expr_return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::generic_meta_return_type
array_concatenation::check_meta_generic(context& c) const {
	STACKTRACE("array_concatenation::check_meta_generic()");
	if (size() == 1) {
		const const_iterator only(begin());
		return (*only)->check_meta_generic(c);
	} else {
		cerr << "Fang, finish array_concatenation::check_meta_generic()!" <<
			endl;
		return expr::generic_meta_return_type();
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
loop_concatenation::check_meta_expr(context& c) const {
	cerr << "Fang, finish loop_concatenation::check_meta_expr()!" << endl;
	return expr::meta_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_expr_return_type
loop_concatenation::check_nonmeta_expr(context& c) const {
	cerr << "Fang, finish loop_concatenation::check_nonmeta_expr()!"
		<< endl;
	return nonmeta_expr_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::generic_meta_return_type
loop_concatenation::check_meta_generic(context& c) const {
	cerr << "Fang, finish loop_concatenation::check_meta_generic()!" << endl;
	return expr::generic_meta_return_type();
}

//=============================================================================
// class array_construction method definitions

array_construction::array_construction(const char_punctuation_type* l,
		const expr* e, const char_punctuation_type* r) : 
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
expr::meta_return_type
array_construction::check_meta_expr(context& c) const {
	cerr << "Fang, finish array_construction::check_meta_expr()!" << endl;
	return expr::meta_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_expr_return_type
array_construction::check_nonmeta_expr(context& c) const {
	cerr << "Fang, finish array_construction::check_nonmeta_expr()!"
		<< endl;
	return nonmeta_expr_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::generic_meta_return_type
array_construction::check_meta_generic(context& c) const {
	cerr << "Fang, finish array_construction::check_meta_generic()!" << endl;
	return expr::generic_meta_return_type();
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

#endif	// __AST_HAC_PARSER_EXPR_CC__

