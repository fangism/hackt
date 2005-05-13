/**
	\file "AST/art_parser_expr.cc"
	Class method definitions for ART::parser, related to expressions.  
	$Id: art_parser_expr.cc,v 1.20.2.5 2005/05/13 06:44:35 fang Exp $
 */

#ifndef	__AST_ART_PARSER_EXPR_CC__
#define	__AST_ART_PARSER_EXPR_CC__

#include <exception>
#include <iostream>

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
#include "Object/art_object_inst_ref_base.h"
#include "Object/art_object_expr.h"

#include "util/what.h"
#include "util/stacktrace.h"

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
using std::transform;
using std::_Select1st;
using std::_Select2nd;

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
//	what(cerr) << endl;
	expr::return_type ret(check_expr(c));
	return generic_return_type(ret,
		ret.is_a<inst_ref_return_type::element_type>());
}

//=============================================================================
// class inst_ref_expr method definitions

#if 0
never_ptr<const object>
inst_ref_expr::check_build(context& c) const {
	cerr << "NEVER supposed to call inst_ref_expr::check_build()!" << endl;
	DIE;
	return never_ptr<const object>(NULL);
}
#endif

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

//=============================================================================
// class expr_list method definitions

expr_list::expr_list() : parent_type() { }

expr_list::expr_list(const expr* e) : parent_type(e) { }

expr_list::~expr_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
PARSER_WHAT_DEFAULT_IMPLEMENTATION(expr_list)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
/**
	PHASE OUT.
	Type-checker checks each individual expression and 
	returns a collection of built object expressions.  
	NULL members will be represented with NULL object 
	expression place-holders.  
	Caller just has to grab the object_list off the stack
	and go from there.  
 */
never_ptr<const object>
expr_list::check_build(context& c) const {
	const count_ptr<object_list> o(new object_list);
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		if (*i) {
			(*i)->check_build(c);
			// ignore useless return values (should be always NULL)
			o->push_back(c.pop_top_object_stack());
			// do error checking on list elsewhere
		} else {
			// add NULL placeholder
			o->push_back(count_ptr<object>(NULL));
		}
	}
	c.push_object_stack(o);
	return never_ptr<const object>(NULL);
}
#endif

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
#if 0
	std::transform(i, e, back_inserter(temp),
	unary_compose(
		bind2nd_argval(mem_fun_ref(&expr::check_generic), c),
		dereference<value_type>()
	)
	);
#else
	// in plain English, the above is equivalent to
	// (without the ?: conditional):
	for ( ; i!=e; i++) {
		temp.push_back((*i) ? (*i)->check_generic(c) :
			checked_generic_type::value_type());
		// else pushes a pair of NULL pointers
	}
#endif
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
#if 0
	std::transform(i, e, back_inserter(temp),
	unary_compose(
		bind2nd_argval(mem_fun_ref(&expr::check_expr), c),
		dereference<value_type>()
	)
	);
#else
	// in plain English, the above is equivalent to:
	for ( ; i!=e; i++) {
		temp.push_back((*i)->check_expr(c));
	}
#endif
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
	UPDATE DOCUMENTATION.
	The qualified_id member's check build can return a definition 
	or instance pointer.  (Only instance pointer now.)
	\param c the context where to begin searching for named object.  
	\return pointer to the found instantiation base instance if found,
		else NULL.
	FIX ME: should return instance_reference!, not instance!
	ACTUALLY: instance_base, caller will wrap into instance reference. 
 */
#if 1
never_ptr<const object>
id_expr::check_build(context& c) const {
	const never_ptr<const object>
		o(qid->check_build(c));		// will lookup_object
	if (o) {
		const never_ptr<const instance_collection_base>
			inst(o.is_a<const instance_collection_base>());
		if (inst) {
			// we found an instance which may be single
			// or collective... info is in inst.
			c.push_object_stack(inst->make_instance_reference());
			// pushes the created reference onto
			// context's instance_reference_stack.
			// if indexed, check in the caller, and modify
			//	in index_expr...

			// doesn't have to be a parameter, does it?
		} else {
			// push NULL or error object to continue?
			cerr << "object \"" << *qid <<
				"\" is not an instance, ERROR!  "
				<< where(*qid) << endl;
			THROW_EXIT;
		}
	} else {
		// push NULL or error object to continue?
		cerr << "object \"" << *qid << "\" not found, ERROR!  "
			<< where(*qid) << endl;
		THROW_EXIT;
	}
	return never_ptr<const object>(NULL);
//	return c.lookup_instance(*qid);
// also accomplishes same thing?
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
#if 0
expr::return_type
id_expr::check_expr(context& c) const {
	inst_ref_expr::return_type inst_ref(check_reference(c));
	NEVER_NULL(inst_ref);	// else would've exited
	const expr::return_type
		param_ref(inst_ref.is_a<expr::return_type::element_type>());
	if (param_ref) {
		// accepted
		return param_ref;
	} else {
		cerr << "object \"" << *qid <<
			"\" does not refer to a parameter, ERROR!  "
			<< where(*qid) << endl;
		THROW_EXIT;
		return expr::return_type(NULL);
	}
}
#endif

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
unary_expr::unary_expr(const expr* n, const terminal* o) : expr(), 
		e(n), op(o) {
}

DESTRUCTOR_INLINE
unary_expr::~unary_expr() {
}

//=============================================================================
// class prefix_expr method definitions

CONSTRUCTOR_INLINE
prefix_expr::prefix_expr(const terminal* o, const expr* n) :
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
	Does basic checking on expression.  
	Grabs last expression off top of stack and replaces it.  
	Always returns NULL, rather useless.  
 */
never_ptr<const object>
prefix_expr::check_build(context& c) const {
	typedef	never_ptr<const object>		return_type;
	e->check_build(c);	// useless return value
	const count_ptr<object> o(c.pop_top_object_stack());
	if (!o) {
		// error propagates up the stack
		cerr << "ERROR building expression at " << where(*e) << endl;
		c.push_object_stack(count_ptr<object>(NULL));
		return return_type(NULL);
	}
	const count_ptr<param_expr> pe(o.is_a<param_expr>());
	NEVER_NULL(pe);	// must be a param expression!
	const count_ptr<pint_expr> ie(pe.is_a<pint_expr>());
	const count_ptr<pbool_expr> be(pe.is_a<pbool_expr>());

	const int ch = op.is_a<const token_char>()->get_char();
	switch(ch) {
		case '-':
			// integer negation
			if (!ie) {
				cerr << "Unary \'-\' operator requires a "
					"pint argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				c.push_object_stack(count_ptr<object>(NULL));
				break;
			}
			if (ie->is_static_constant()) {
				// constant simplification
				c.push_object_stack(count_ptr<pint_const>(
					new pint_const(- ie->static_constant_value())));
			} else {
				c.push_object_stack(count_ptr<pint_unary_expr>(
					new pint_unary_expr(ch, ie)));
			}
			break;
		case '!':
			// integer logical negation
			if (!ie) {
				cerr << "Unary \'!\' operator requires a "
					"pint argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				c.push_object_stack(count_ptr<object>(NULL));
				break;
			}
			if (ie->is_static_constant()) {
				// constant simplification
				c.push_object_stack(count_ptr<pint_const>(
					new pint_const(! ie->static_constant_value())));
			} else {
				c.push_object_stack(count_ptr<pint_unary_expr>(
					new pint_unary_expr(ch, ie)));
			}
			break;
		case '~':
			// context-dependent? in PRS or not?
			// is bit-wise negation for ints, 
			// logical negation for bools?
			// for now, restrict to bools only...
			if (!be) {
				cerr << "Unary \'~\' operator requires a "
					"pint argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< where(*e) << endl;
				c.push_object_stack(count_ptr<object>(NULL));
				break;
			}
			if (be->is_static_constant()) {
				// constant simplification
				c.push_object_stack(count_ptr<pbool_const>(
					new pbool_const(
						!be->static_constant_value())));
			} else {
				c.push_object_stack(
					count_ptr<pbool_unary_expr>(
						new pbool_unary_expr(be, ch)));
			}
			break;
		default:
			cerr << "Bad operator char \'" << ch << "\' in "
				"prefix_expr::check_build()!" << endl;
			DIE;
	}
	return return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	const int ch = op.is_a<const token_char>()->get_char();
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
			// context-dependent? in PRS or not?
			// is bit-wise negation for ints, 
			// logical negation for bools?
			// for now, restrict to bools only...
			if (!be) {
				cerr << "Unary \'~\' operator requires a "
					"pint argument, but got a ";
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

//=============================================================================
// class member_expr method definitions

CONSTRUCTOR_INLINE
member_expr::member_expr(const inst_ref_expr* l,
//		const char_punctuation_type* o, 
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
#if 1
/**
	Type-check of member reference.  
	Current restriction: left expression must be scalar 0-dimensional.
	\return NULL, but places an instance_reference object on the
		context's object stack.  
 */
never_ptr<const object>
member_expr::check_build(context& c) const {
	owner->check_build(c);
	// useless return value
	// expect: simple_instance_reference on object stack
	const count_ptr<const object> o(c.pop_top_object_stack());
	if (!o) {
		cerr << "ERROR in base instance reference of member expr at "
			<< where(*owner) << endl;
		THROW_EXIT;
	}
	const count_ptr<const simple_instance_reference>
		inst_ref(o.is_a<const simple_instance_reference>());
	assert(inst_ref);
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
	c.push_current_definition_reference(*base_def);
	// this should return a reference to an instance
	// of some type that has members, such as data, channel, process.  
	// should resolve to a *single* instance of something, 
	// cannot be an array.  

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
	c.push_object_stack(
		member_inst->make_member_instance_reference(inst_ref));
	// useless return value: NULL
	// will result in member_instance_reference on object_stack

	// reset definition reference in context
	c.pop_current_definition_reference();

	// what should this return?  the same thing it expects:
	// a reference to an instance of some type.  
	// Problem: instances aren't concrete until they are unrolled.
	// What is available in type-check phase?
	// Maybe we don't care about the instances themselves, 
	// rather the *type* returned.  
	// after all this is type-checking, not range checking.  

	return never_ptr<const object>(NULL);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
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

	// CHECK THIS:
	// is this needed? context is not passed down
	// before definition reference is popped off stack.
	c.push_current_definition_reference(*base_def);
	// this should return a reference to an instance
	// of some type that has members, such as data, channel, process.  
	// should resolve to a *single* instance of something, 
	// cannot be an array.  

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
//	const return_type ret(ret_inst_ref.is_a<param_expr>());

	// reset definition reference in context
	c.pop_current_definition_reference();

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
#if 0
expr::return_type
member_expr::check_expr(context& c) const {
	inst_ref_expr::return_type ex(check_reference(c));
	expr::return_type
		ret(ex.is_a<expr::return_type::element_type>());
	if (ex)	INVARIANT(ret);
	return ret;
}
#endif

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
#if 1
/**
	TO DO: FINISH ME
	Check index expression first, must be an integer type.  
	Checking identifier should place an instance_reference 
	on the context's object stack.  
	For an indexed instance reference, we need to take it off the 
	stack, modify it, and replace it back onto the stack.  
 */
never_ptr<const object>
index_expr::check_build(context& c) const {
#if 1
	ranges->check_build(c);		// useless return value
	// should result in a ART::entity::index_list on the stack
	const count_ptr<object>
		index_obj(c.pop_top_object_stack());
	// see range_list::check_build()
	if (!index_obj) {
		cerr << "ERROR in indices!  " << where(*ranges) << endl;
		THROW_EXIT;
	}
	const count_ptr<object_list>
		ol(index_obj.is_a<object_list>());
	NEVER_NULL(ol);
	// would rather have excl_ptr...
	excl_ptr<index_list> index_list_obj = ol->make_index_list();
	if (!index_list_obj) {
		cerr << "ERROR in index list!  " << where(*ranges) << endl;
		THROW_EXIT;
	}
	NEVER_NULL(index_list_obj);

	base->check_build(c);
	// should result in an instance_reference on the stack.  
	const count_ptr<object> base_obj(c.pop_top_object_stack());
	if (!base_obj) {
		cerr << "ERROR in base instance_reference!  "
			<< where(*base) << endl;
		THROW_EXIT;
	}

	// later this may be a member_instance_reference...
	// should cast to instance_reference_base instead, 
	// abstract attach_indices
	const count_ptr<simple_instance_reference>
		base_inst(base_obj.is_a<simple_instance_reference>());
	NEVER_NULL(base_inst);

	const bad_bool ai(base_inst->attach_indices(index_list_obj));
	if (ai.bad) {
		cerr << where(*ranges) << endl;
		THROW_EXIT;
	}
	// push indexed instance reference back onto stack
	c.push_object_stack(base_inst);
#else
	// PROBLEM: if called by alias_list, then need either
	// param_expr or instance_reference, 
	// but cannot know apriori, requires context!
	c.push_object_stack(check_expr(c));
	OR
	c.push_object_stack(check_reference(c));
	// ???
#endif
	return never_ptr<const object>(NULL);
}
#endif

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
#if 0
/**
	Ensures that what's returned is a valid parameter expression.  
 */
expr::return_type
index_expr::check_expr(context& c) const {
	inst_ref_expr::return_type ref(check_reference(c));
	const expr::return_type
		ret(ref.is_a<expr::return_type::element_type>());
	if (ref) INVARIANT(ret);
	return ret;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
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
binary_expr::binary_expr(const expr* left, const terminal* o, 
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
arith_expr::arith_expr(const expr* left, const terminal* o, 
		const expr* right) :
		binary_expr(left, o, right) {
}

DESTRUCTOR_INLINE
arith_expr::~arith_expr() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(arith_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
arith_expr::check_build(context& c) const {
	typedef	never_ptr<const object>		return_type;
	l->check_build(c);	// useless return value
	r->check_build(c);	// useless return value
	const count_ptr<object> ro(c.pop_top_object_stack());
	const count_ptr<object> lo(c.pop_top_object_stack());
	if (!ro || !lo) {
		if (!lo)
			cerr << "ERROR building expression at " << 
				where(*l) << endl;
		if (!ro)
			cerr << "ERROR building expression at " << 
				where(*r) << endl;
		c.push_object_stack(count_ptr<object>(NULL));
		return return_type(NULL);
	}
	const count_ptr<pint_expr> li(lo.is_a<pint_expr>());
	const count_ptr<pint_expr> ri(ro.is_a<pint_expr>());
	if (!li || !ri) {
		if (!li) {
			cerr << "ERROR arith_expr expected a pint, but got a ";
			lo->what(cerr) << " at " << where(*l) << endl;;
		}
		if (!ri) {
			cerr << "ERROR arith_expr expected a pint, but got a ";
			ro->what(cerr) << " at " << where(*r) << endl;;
		}
		c.push_object_stack(count_ptr<object>(NULL));
		return return_type(NULL);
	}
	// else is safe to make arith_expr object
	const char ch = op.is_a<const token_char>()->get_char();
	if (li->is_static_constant() && ri->is_static_constant()) {
		const int lc = li->static_constant_value();
		const int rc = ri->static_constant_value();
		switch(ch) {
			case '+':
				c.push_object_stack(count_ptr<pint_const>(
					new pint_const(lc +rc)));
				break;
			case '-':
				c.push_object_stack(count_ptr<pint_const>(
					new pint_const(lc -rc)));
				break;
			case '*':
				c.push_object_stack(count_ptr<pint_const>(
					new pint_const(lc *rc)));
				break;
			case '/':
				c.push_object_stack(count_ptr<pint_const>(
					new pint_const(lc /rc)));
				break;
			case '%':
				c.push_object_stack(count_ptr<pint_const>(
					new pint_const(lc %rc)));
				break;
			default:
				cerr << "Bad operator char \'" << ch << "\' in "
					"arith_expr::check_build()!" << endl;
				DIE;
		}
	} else {
		c.push_object_stack(count_ptr<entity::arith_expr>(
			new entity::arith_expr(li, ch, ri)));
	}
	return return_type(NULL);
}

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
	const char ch = op.is_a<const token_char>()->get_char();
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
relational_expr::relational_expr(const expr* left, const terminal* o, 
		const expr* right) :
		binary_expr(left, o, right) {
}

DESTRUCTOR_INLINE
relational_expr::~relational_expr() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(relational_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
relational_expr::check_build(context& c) const {
	// temporary
	cerr << "Fang, finish relational_expr::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::return_type
relational_expr::check_expr(context& c) const {
	// same idea as arith expr
	cerr << "Fang, finish relational_expr::check_expr()!" << endl;
	return expr::return_type(NULL);
}

//=============================================================================
// class logical_expr method definitions

CONSTRUCTOR_INLINE
logical_expr::logical_expr(const expr* left, const terminal* o, 
		const expr* right) :
		binary_expr(left, o, right) {
}

DESTRUCTOR_INLINE
logical_expr::~logical_expr() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(logical_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
logical_expr::check_build(context& c) const {
	cerr << "Fang, finish relational_expr::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr::return_type
logical_expr::check_expr(context& c) const {
	// same idea as arith expr
	cerr << "Fang, finish relational_expr::check_expr()!" << endl;
	return expr::return_type(NULL);
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
never_ptr<const object>
array_concatenation::check_build(context& c) const {
	if (size() == 1) {
		const const_iterator only = begin();
		return (*only)->check_build(c);
	} else {
		cerr << "Fang, finish array_concatenation::check_build()!" <<
			endl;
		return never_ptr<const object>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
//		const char_punctuation_type* h,
//		const char_punctuation_type* c1, 
		const token_identifier* i,   
//		const char_punctuation_type* c2, 
		const range* rng,
//		const char_punctuation_type* c3, 
		const expr* e,
		const char_punctuation_type* r) :
		lp(l), id(i), bounds(rng), ex(e), rp(r) {
//	NEVER_NULL(lp);
	NEVER_NULL(id); NEVER_NULL(bounds); NEVER_NULL(ex);
//	NEVER_NULL(rp);
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
never_ptr<const object>
loop_concatenation::check_build(context& c) const {
	cerr << "Fang, finish loop_concatenation::check_build()!" << endl;
	return never_ptr<const object>(NULL);
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
never_ptr<const object>
array_construction::check_build(context& c) const {
	cerr << "Fang, finish array_construction::check_build()!" << endl;
	return never_ptr<const object>(NULL);
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
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes
							// also known as...
template class node_list<const expr>;			// expr_list
template class node_list<const token_identifier>;	// qualified_id_base

//=============================================================================
};	// end namespace parser
};	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __AST_ART_PARSER_EXPR_CC__

