/**
	\file "AST/art_parser_expr.cc"
	Class method definitions for ART::parser, related to expressions.  
	$Id: art_parser_expr.cc,v 1.23.2.2.2.1 2005/06/03 21:43:47 fang Exp $
 */

#ifndef	__AST_ART_PARSER_EXPR_CC__
#define	__AST_ART_PARSER_EXPR_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#define	ENABLE_STACKTRACE		0

#include "AST/art_parser_token.h"
#include "AST/art_parser_token_char.h"
#include "AST/art_parser_expr.h"
#include "AST/art_parser_expr_list.h"
#include "AST/art_parser_range_list.h"
#include "AST/art_parser_node_list.tcc"
#include "util/sublist.tcc"

// will need these come time for type-checking
#include "Object/art_object_instance_base.h"
#include "Object/art_object_definition_base.h"
#include "Object/art_object_inst_ref_data.h"
#include "Object/art_object_expr.h"
#include "Object/art_object_PRS.h"
// to dynamic_cast bool_instance_reference
#include "Object/art_object_inst_ref.h"
#include "Object/art_object_classification_details.h"

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
SPECIALIZE_UTIL_WHAT(ART::parser::expr, "(expr)")
// SPECIALIZE_UTIL_WHAT(ART::parser::expr_list, "(expr-list)")
SPECIALIZE_UTIL_WHAT(ART::parser::qualified_id, "(qualified-id)")
SPECIALIZE_UTIL_WHAT(ART::parser::id_expr, "(id-expr)")
SPECIALIZE_UTIL_WHAT(ART::parser::prefix_expr, "(prefix-expr)")
SPECIALIZE_UTIL_WHAT(ART::parser::member_expr, "(member-expr)")
SPECIALIZE_UTIL_WHAT(ART::parser::index_expr, "(index-expr)")
SPECIALIZE_UTIL_WHAT(ART::parser::arith_expr, "(arith-expr)")
SPECIALIZE_UTIL_WHAT(ART::parser::relational_expr, "(relational-expr)")
SPECIALIZE_UTIL_WHAT(ART::parser::logical_expr, "(logical-expr)")
SPECIALIZE_UTIL_WHAT(ART::parser::array_concatenation, "(array-concatenation)")
SPECIALIZE_UTIL_WHAT(ART::parser::loop_concatenation, "(loop-concatenation)")
SPECIALIZE_UTIL_WHAT(ART::parser::array_construction, "(array-construction)")
}

//=============================================================================
namespace ART {
using namespace entity;

namespace parser {
#include "util/using_ostream.h"
using std::copy;
using std::transform;
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
	the result of check_expr to an instance reference.  
	This is overridden by inst_ref_expr::check_generic.
	\return pair of typed pointers.  
 */
expr::generic_return_type
expr::check_generic(context& c) const {
	STACKTRACE("expr::check_generic()");
	expr::return_type ret(check_expr(c));
	return generic_return_type(ret,
		ret.is_a<inst_ref_return_type::element_type>());
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
	the result of check_reference to an parameter expression.  
	\return pair of typed pointers.  
 */
expr::generic_return_type
inst_ref_expr::check_generic(context& c) const {
	STACKTRACE("inst_ref_expr::check_generic()");
	return_type ret(check_reference(c));
	return generic_return_type(
		ret.is_a<expr::return_type::element_type>(), ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should really not be called...
 */
expr::return_type
inst_ref_expr::check_expr(context& c) const {
	STACKTRACE("inst_ref_expr::check_expr() (should not be called)");
#if 0
	return check_reference(c).is_a<expr::return_type::element_type>();
#else
	const inst_ref_expr::return_type inst_ref(check_reference(c));
	const expr::return_type
		param_ref(inst_ref.is_a<expr::return_type::element_type>());
	if (param_ref) {
		// accepted
		return param_ref;
	} else {
		cerr << "object \"" // << *qid <<
			"\" does not refer to a parameter, ERROR!  "
			<< where(*this) << endl;
		THROW_EXIT;
		return expr::return_type(NULL);
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	After checking an instance_reference, this checks to make sure
	that a bool is referenced, appropriate for PRS.  
 */
prs_literal_ptr_type
inst_ref_expr::check_prs_literal(context& c) const {
	return_type ref(check_reference(c));
	count_ptr<bool_instance_reference>
		bool_ref(ref.is_a<bool_instance_reference>());
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
expr_list::postorder_check_generic(checked_generic_type& temp,
		context& c) const {
	STACKTRACE("expr_list::postorder_check_generic()");
	INVARIANT(temp.empty());
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		temp.push_back((*i) ? (*i)->check_generic(c) :
			checked_generic_type::value_type());
		// else pushes a pair of NULL pointers
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just collects the result of type-checking of items in list.
	\param temp the type-checked result list.
	\param c the context.
 */
void
expr_list::postorder_check_exprs(checked_exprs_type& temp,
		context& c) const {
	STACKTRACE("expr_list::postorder_check_exprs()");
	INVARIANT(temp.empty());
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		temp.push_back((*i) ? (*i)->check_expr(c) :
			checked_exprs_type::value_type(NULL));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
expr_list::select_checked_exprs(const checked_generic_type& src, 
		checked_exprs_type& dst) {
	INVARIANT(dst.empty());
	transform(src.begin(), src.end(), back_inserter(dst),
		_Select1st<checked_generic_type::value_type>()
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
expr_list::select_checked_refs(const checked_generic_type& src, 
		checked_refs_type& dst) {
	INVARIANT(dst.empty());
	transform(src.begin(), src.end(), back_inserter(dst),
		_Select2nd<checked_generic_type::value_type>()
	);
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
		Consumer should wrap in instance_reference?
			might be collective, in the case of an array
 */
never_ptr<const object>
qualified_id::check_build(context& c) const {
	return c.lookup_object(*this);
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
		qualified_id::const_iterator i = id.begin();
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
		qualified_id_slice::const_iterator i = id.begin();
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
	The qualified_id member's check build can return a definition 
	or instance pointer.  
	A different method will be used to lookup definition/type identifiers.
	\param c the context where to begin searching for named object.  
	\return pointer to the found instantiation base instance if found,
		else NULL.
 */
inst_ref_expr::return_type
id_expr::check_reference(context& c) const {
	STACKTRACE("id_expr::check_reference()");
	const never_ptr<const object>
		o(qid->check_build(c));		// will lookup_object
	if (o) {
		const never_ptr<const instance_collection_base>
			inst(o.is_a<const instance_collection_base>());
		if (inst) {
			STACKTRACE("valid instance collection found");
			// we found an instance which may be single
			// or collective... info is in inst.
			return inst->make_instance_reference();
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
	return inst_ref_expr::return_type(NULL);
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
expr::return_type
prefix_expr::check_expr(context& c) const {
	typedef	expr::return_type		return_type;
	const return_type pe(e->check_expr(c));
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
				"prefix_expr::check_expr()!" << endl;
			DIE;
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
		cerr << "FATAL: Invalid unary operator: \'" << op->text[0] <<
			"\' at " << where(*op) <<
			".  Aborting... have a nice day." << endl;
		DIE;
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
	\return type-checked instance_reference or null.  
	Really, should never be able to refer to param_expr
	member of an instance.
 */
inst_ref_expr::return_type
member_expr::check_reference(context& c) const {
	typedef	inst_ref_expr::return_type	return_type;
	const inst_ref_expr::return_type o(owner->check_reference(c));
	// useless return value
	// expect: simple_instance_reference on object stack
	if (!o) {
		cerr << "ERROR in base instance reference of member expr at "
			<< where(*owner) << endl;
		THROW_EXIT;
	}
	const count_ptr<const simple_instance_reference>
		inst_ref(o.is_a<const simple_instance_reference>());
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

	// use that instance_reference, get its referenced definition_base, 
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

	const count_ptr<instance_reference_base>
	ret_inst_ref(member_inst->make_member_instance_reference(inst_ref));

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
range_list::checked_indices_type
index_expr::intercept_indices_error(context& c) const {
	const range_list::checked_indices_type
		checked_indices(ranges->check_indices(c));
	// should result in a ART::entity::index_list
	// what happened to object_list::make_index_list() ?
	if (!checked_indices) {
		cerr << "ERROR in index list!  " << where(*ranges) << endl;
		THROW_EXIT;
	}
	return checked_indices;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For convenience, exit upon error.  
 */
inst_ref_expr::return_type
index_expr::intercept_base_ref_error(context& c) const {
	// should result in an instance_reference
	const inst_ref_expr::return_type
		base_expr(base->check_reference(c));
	if (!base_expr) {
		cerr << "ERROR in base instance_reference!  "
			<< where(*base) << endl;
		THROW_EXIT;
	}
	return base_expr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Build's an indexed reference from base and index.  
	Check index expression first, must be an integer type.  
	\return pointer to instance_reference_base.  
 */
inst_ref_expr::return_type
index_expr::check_reference(context& c) const {
	range_list::checked_indices_type
		checked_indices(intercept_indices_error(c));
	const inst_ref_expr::return_type
		base_expr(intercept_base_ref_error(c));

	// later this may be a member_instance_reference...
	// should cast to instance_reference_base instead, 
	// abstract attach_indices
	const count_ptr<simple_instance_reference>
		base_inst(base_expr.is_a<simple_instance_reference>());
	NEVER_NULL(base_inst);

	excl_ptr<range_list::checked_indices_type::element_type>
		passing_indices(checked_indices.exclusive_release());
	const bad_bool ai(base_inst->attach_indices(passing_indices));
	if (ai.bad) {
		cerr << where(*ranges) << endl;
		THROW_EXIT;
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
expr::return_type
arith_expr::check_expr(context& c) const {
	typedef	expr::return_type	return_type;
	return_type lo(l->check_expr(c));
	return_type ro(r->check_expr(c));
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
		return return_type(new entity::arith_expr(li, ch, ri));
	}
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
expr::return_type
relational_expr::check_expr(context& c) const {
	typedef	expr::return_type	return_type;
	return_type lo(l->check_expr(c));
	return_type ro(r->check_expr(c));
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
	const entity::relational_expr::op_type*
		o(entity::relational_expr::op_map[op_str]);
	INVARIANT(o);
	if (li->is_static_constant() && ri->is_static_constant()) {
		const int lc = li->static_constant_value();
		const int rc = ri->static_constant_value();
		return return_type(new pbool_const((*o)(lc,rc)));
	} else {
		return return_type(new entity::relational_expr(li, o, ri));
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
expr::return_type
logical_expr::check_expr(context& c) const {
	typedef	expr::return_type	return_type;
	return_type lo(l->check_expr(c));
	return_type ro(r->check_expr(c));
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
	entity::logical_expr::op_type const* const
		o(entity::logical_expr::op_map[op_str]);
	INVARIANT(o);
	if (lb->is_static_constant() && rb->is_static_constant()) {
		const bool lc = lb->static_constant_value();
		const bool rc = rb->static_constant_value();
		return return_type(new pbool_const((*o)(lc,rc)));
	} else {
		return return_type(new entity::logical_expr(lb, o, rb));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
		count_ptr<entity::PRS::and_expr>
			l_and(lo.is_a<entity::PRS::and_expr>());
		count_ptr<entity::PRS::and_expr>
			r_and(ro.is_a<entity::PRS::and_expr>());
		if (l_and) {
			if (r_and) {
				copy(r_and->begin(), r_and->end(), 
					back_inserter(*l_and));
			} else {
				l_and->push_back(ro);
			}
			return l_and;
		} else if (r_and) {
			r_and->push_front(lo);
			return r_and;
		} else {
			count_ptr<entity::PRS::and_expr>
				ret(new entity::PRS::and_expr);
			ret->push_back(lo);
			ret->push_back(ro);
//			ret->check();	// paranoia
			return ret;
		}
	} else if (op_char == '|') {
		typedef	entity::PRS::or_expr::iterator		iterator;
		typedef	entity::PRS::or_expr::const_iterator	const_iterator;
		count_ptr<entity::PRS::or_expr>
			l_or(lo.is_a<entity::PRS::or_expr>());
		count_ptr<entity::PRS::or_expr>
			r_or(ro.is_a<entity::PRS::or_expr>());
		if (l_or) {
			if (r_or) {
				copy(r_or->begin(), r_or->end(), 
					back_inserter(*l_or));
			} else {
				l_or->push_back(ro);
			}
			return l_or;
		} else if (r_or) {
			r_or->push_front(lo);
			return r_or;
		} else {
			count_ptr<entity::PRS::or_expr>
				ret(new entity::PRS::or_expr);
			ret->push_back(lo);
			ret->push_back(ro);
			return ret;
		}
	} else {
		cerr << "FATAL: Invalid PRS operor: \'" << op_char << "\' at "
			<< where(*op) << ".  Aborting... have a nice day."
			<< endl;
		DIE;
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
expr::return_type
array_concatenation::check_expr(context& c) const {
	if (size() == 1) {
		const const_iterator only = begin();
		return (*only)->check_expr(c);
	} else {
		cerr << "Fang, finish array_concatenation::check_expr()!" <<
			endl;
		return expr::return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::generic_return_type
array_concatenation::check_generic(context& c) const {
	STACKTRACE("array_concatenation::check_generic()");
	if (size() == 1) {
		const const_iterator only = begin();
		return (*only)->check_generic(c);
	} else {
		cerr << "Fang, finish array_concatenation::check_generic()!" <<
			endl;
		return expr::generic_return_type();
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
expr::return_type
loop_concatenation::check_expr(context& c) const {
	cerr << "Fang, finish loop_concatenation::check_expr()!" << endl;
	return return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::generic_return_type
loop_concatenation::check_generic(context& c) const {
	cerr << "Fang, finish loop_concatenation::check_generic()!" << endl;
	return expr::generic_return_type();
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
expr::return_type
array_construction::check_expr(context& c) const {
	cerr << "Fang, finish array_construction::check_expr()!" << endl;
	return return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::generic_return_type
array_construction::check_generic(context& c) const {
	cerr << "Fang, finish array_construction::check_generic()!" << endl;
	return expr::generic_return_type();
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS

template
ostream&
node_list<const token_identifier>::what(ostream&) const;

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __AST_ART_PARSER_EXPR_CC__

