/**
	\file "art_parser_base.cc"
	Class method definitions for ART::parser base classes.
	$Id: art_parser_base.cc,v 1.15 2005/02/27 22:11:57 fang Exp $
 */

#ifndef	__ART_PARSER_BASE_CC__
#define	__ART_PARSER_BASE_CC__

// rule-of-thumb for inline directives:
// only inline constructors if you KNOW that they will not be be needed
// outside of this module, because we don't have a means to export
// inline methods other than defining in the header or using
// -fkeep-inline-functions

#include <exception>
#include <iostream>

#include "art_parser_debug.h"
#include "art_switches.h"
#include "art_parser.tcc"

#include "art_parser_expr_base.h"
#include "art_parser_token.h"
#include "art_parser_root_item.h"
#include "art_parser_type.h"
#include "art_parser_identifier.h"
#include "art_parser_statement.h"
#include "art_parser_definition_item.h"

#include "art_context.h"
#include "art_object_definition_base.h"
#include "art_object_type_ref_base.h"
#include "art_object_expr.h"		// for dynamic_param_expr_list
#include "art_object_namespace.h"

#include "indent.h"
#include "what.h"
#include "stacktrace.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(ART::parser::type_id, "(type-id)")
SPECIALIZE_UTIL_WHAT(ART::parser::chan_type, "(chan-type)")
SPECIALIZE_UTIL_WHAT(ART::parser::incdec_stmt, "(inc/dec-stmt)")
SPECIALIZE_UTIL_WHAT(ART::parser::assign_stmt, "(assign-stmt)")
SPECIALIZE_UTIL_WHAT(ART::parser::namespace_body, "namespace-body")
SPECIALIZE_UTIL_WHAT(ART::parser::namespace_id, "(namespace-id)")
SPECIALIZE_UTIL_WHAT(ART::parser::using_namespace, "(using-namespace)")
SPECIALIZE_UTIL_WHAT(ART::parser::concrete_type_ref, "(type-ref)")
}

//=============================================================================
namespace ART {
namespace parser {
#include "using_ostream.h"
using util::indent;
using util::auto_indent;
USING_STACKTRACE

//=============================================================================
// global constants
// need to be kept here for explicit template instantiations (?)
const char none[] = "";		///< delimiter for node_list template argument
const char comma[] = ",";	///< delimiter for node_list template argument
const char semicolon[] = ";";	///< delimiter for node_list template argument
const char scope[] = "::";	///< delimiter for node_list template argument
const char thickbar[] = "[]";	///< delimiter for node_list template argument
const char colon[] = ":";	///< delimiter for node_list template argument
const char alias[] = "=";	///< delimiter for node_list template argument
const char pound[] = "#";	///< delimiter for node_list template argument

// eventually token keywords here too? or "art_parser_token.cc"

//=============================================================================
// class node method definitions

#if 0
/** 
	Destructor kept here because vtable is not generated on 
	darwin-gcc-3.3 if it is inlined in the header.  
 */
node::~node() { }
#endif

void
node::bogus(void) const {
}

/// reports location spanned by a node in the source file
inline
line_range
node::where(void) const {
	return line_range(leftmost(), rightmost());
}

#if 0
/**
	Default type-checker and object builder does nothing.  
	Should be re-implemented in all terminal subclasses.  
	Eventually make this pure virtual.  
	Should really take a context&...
 */
never_ptr<const object>
node::check_build(context& c) const {
	// We DO want to print this message, even in regression testing. 
	what(cerr << c.auto_indent() << 
		"check_build() not implemented yet for ");
	return c.top_namespace();
}
#endif

//=============================================================================
// class root_item method definitions

#if 0
/// Empty constructor
CONSTRUCTOR_INLINE
root_item::root_item() : node() { }

/// Empty virtual destructor
DESTRUCTOR_INLINE
root_item::~root_item() { }
#endif

//-----------------------------------------------------------------------------
// class root_body method definitions

root_body::root_body(const root_item* r) : parent(r) { }

root_body::~root_body() { }

//=============================================================================
// class type_base method definitions

#if 0
CONSTRUCTOR_INLINE
type_base::type_base() : node() { }

DESTRUCTOR_INLINE
type_base::~type_base() { }
#endif

//=============================================================================
// class type_id method definitions

/**
	Builds a type-identifier.  
	Also deletes expression list argument after transfering list.  
 */
CONSTRUCTOR_INLINE
type_id::type_id(const qualified_id* b) : node(),
		base(b) {
	assert(base);
}

DESTRUCTOR_INLINE
type_id::~type_id() {
}

ostream&
type_id::what(ostream& o) const {
	return base->what(o << util::what<type_id>::name() << ": ");
}

line_position
type_id::leftmost(void) const {
	return base->leftmost();
}

line_position
type_id::rightmost(void) const {
	return base->rightmost();
}

/**
	The base name of the type can refer to either user-defined
	data, channel, or process type for instantiation.  
	Use context object to lookup the actual type.  
	\return pointer to type reference, else NULL if failure.  
 */
never_ptr<const object>
type_id::check_build(context& c) const {
	STACKTRACE("type_id::check_build()");
	TRACE_CHECK_BUILD(
		cerr << c.auto_indent() <<
			"type_id::check_build(...): " << endl;
	)
	const never_ptr<const definition_base>
		d(c.lookup_definition(*base));
	if (!d) {
//		cerr << "type_id::check_build(context&) : ERROR!" << endl;
		return never_ptr<const object>(NULL);
	}
	// set type definition reference
	return c.push_current_definition_reference(*d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// non-member functions
ostream& operator << (ostream& o, const type_id& id) {
	return o << *id.base;
}

//=============================================================================
// class chan_type method definitions

CONSTRUCTOR_INLINE
chan_type::chan_type(const token_keyword* c, const token_char* d, 
		const data_type_ref_list* t) : type_base(),
		chan(c), dir(d), dtypes(t) {
	NEVER_NULL(c);
}

DESTRUCTOR_INLINE
chan_type::~chan_type() {
}

#if 0
ostream&
chan_type::what(ostream& o) const {
	return o << "(chan-type)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(chan_type)
#endif

line_position
chan_type::leftmost(void) const {
	return chan->leftmost();
}

line_position
chan_type::rightmost(void) const {
	if (dtypes)
		return dtypes->rightmost();
	else if (dir)
		return dir->rightmost();
	else return chan->rightmost();
}

/**
	Associates a channel or port with a data type, such as a list of 
	ints and bools.  
	\param t is the type list for the channel.  
 */
chan_type*
chan_type::attach_data_types(const data_type_ref_list* t) {
	assert(t); assert(!dtypes);     // sanity check    
	dtypes = excl_ptr<const data_type_ref_list>(t);
	assert(dtypes);
	return this;
}

never_ptr<const object>
chan_type::check_build(context& c) const {
	STACKTRACE("chan_type::check_build()");
	cerr << "chan_type::check_build(): FINISH ME!";
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class statement method definitions

#if 0
CONSTRUCTOR_INLINE
statement::statement() : node() { }

DESTRUCTOR_INLINE
statement::~statement() { }
#endif

//=============================================================================
// class incdec_stmt method definitions

CONSTRUCTOR_INLINE
incdec_stmt::incdec_stmt(const expr* n, const terminal* o) : statement(),
		e(n), op(o) {
	NEVER_NULL(e); NEVER_NULL(op);
}

#if 0
CONSTRUCTOR_INLINE
incdec_stmt::incdec_stmt(excl_ptr<const expr> n, excl_const_ptr<terminal> o) :
		statement(), e(n), op(o) {
	assert(e); assert(op);
}
#endif

DESTRUCTOR_INLINE
incdec_stmt::~incdec_stmt() {
	SAFEDELETE(e); SAFEDELETE(op);
}

/**
	Release operations are needed for destructive transfer of ownership.
	The consumers of the return pointers are thus responsible for the
	memory at their location. 
 */
#if 0
excl_ptr<const expr>
incdec_stmt::release_expr(void) {
	return e;
}

excl_ptr<const terminal>
incdec_stmt::release_op(void) {
	return op;
}
#else
const expr*
incdec_stmt::release_expr(void) {
	const expr* ret = e;
	e = NULL;
	return ret;
}

const terminal*
incdec_stmt::release_op(void) {
	const terminal* ret = op;
	op = NULL;
	return ret;
}
#endif

#if 0
ostream&
incdec_stmt::what(ostream& o) const {
	return o << "(inc/dec-stmt)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(incdec_stmt)
#endif

line_position
incdec_stmt::leftmost(void) const {
	// if (e)
	return e->leftmost();
}

line_position
incdec_stmt::rightmost(void) const {
	// if (op)
	return op->rightmost();
}

//=============================================================================
// class assign_stmt method definitions

CONSTRUCTOR_INLINE
assign_stmt::assign_stmt(const expr* left, const terminal* o, 
		const expr* right) : statement(),
		lhs(left), op(o), rhs(right) {
	NEVER_NULL(lhs); NEVER_NULL(op); NEVER_NULL(rhs);
}

#if 0
CONSTRUCTOR_INLINE
assign_stmt::assign_stmt(excl_ptr<const expr> left, excl_const_ptr<terminal> o, 
		excl_ptr<const expr> right) : statement(),
		lhs(left), op(o), rhs(right) {
	assert(lhs); assert(op); assert(rhs);
}
#endif

DESTRUCTOR_INLINE
assign_stmt::~assign_stmt() {
	SAFEDELETE(lhs); SAFEDELETE(op); SAFEDELETE(rhs);
}

#if 0
excl_ptr<const expr>
assign_stmt::release_lhs(void) {
	return lhs;
}

excl_ptr<const terminal>
assign_stmt::release_op(void) {
	return op;
}

excl_ptr<const expr>
assign_stmt::release_rhs(void) {
	return rhs;
}
#else
const expr*
assign_stmt::release_lhs(void) {
	const expr* ret = lhs;
	lhs = NULL;
	return ret;
}

const terminal*
assign_stmt::release_op(void) {
	const terminal* ret = op;
	op = NULL;
	return ret;
}

const expr*
assign_stmt::release_rhs(void) {
	const expr* ret = rhs;
	rhs = NULL;
	return ret;
}
#endif

#if 0
ostream&
assign_stmt::what(ostream& o) const {
	return o << "(assign-stmt)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(assign_stmt)
#endif

line_position
assign_stmt::leftmost(void) const {
	return lhs->leftmost();
}

line_position
assign_stmt::rightmost(void) const {
	return rhs->rightmost();
}

//=============================================================================
// class def_body_item method definitions

#if 0
CONSTRUCTOR_INLINE
def_body_item::def_body_item() : node() { }

DESTRUCTOR_INLINE
def_body_item::~def_body_item() { }
#endif

//=============================================================================
// class definition_body method definitions

definition_body::definition_body() : parent() { }

definition_body::definition_body(const def_body_item* d) : parent(d) { }

definition_body::~definition_body() { }

//=============================================================================
// class language_body methd definitions

CONSTRUCTOR_INLINE
language_body::language_body(const token_keyword* t) :
		def_body_item(), tag(t) {
}

DESTRUCTOR_INLINE
language_body::~language_body() { }

language_body*
language_body::attach_tag(token_keyword* t) {
	// need to safe-delete first?  nah...
	tag = excl_ptr<const token_keyword>(t);
	NEVER_NULL(tag);
	return this;
}

line_position
language_body::leftmost(void) const {
	// what if untagged?
	NEVER_NULL(tag);
	return tag->leftmost();
}


//=============================================================================
// class namespace_body method definitions

/**
	Namespace body constructor.  
	\param s the "namespace" keyword. 
	\param n the identifier for the name of the namespace.  
	\param l the left brace.  
	\param b the body (contents), may be NULL.  
	\param r the right brace.  
	\param c the semicolon.  
 */
CONSTRUCTOR_INLINE
namespace_body::namespace_body(
		const token_keyword* s, const token_identifier* n, 
		const terminal* l, const root_body* b,
		const terminal* r, const terminal* c) :
		root_item(),       
		ns(s), name(n), lb(l), body(b), rb(r), semi(c) {
	NEVER_NULL(ns); NEVER_NULL(name); NEVER_NULL(lb);
	// body may be NULL
	NEVER_NULL(rb); NEVER_NULL(semi);
}

/// destructor
DESTRUCTOR_INLINE
namespace_body::~namespace_body() {
}

/// what eeeez it, man?
ostream&
namespace_body::what(ostream& o) const {
#if 0
	return o << "(namespace-body: " << *name << ")";
#else
	return o << '(' << util::what<namespace_body>::name() <<
		": " << *name << ')';
#endif
}

line_position
namespace_body::leftmost(void) const {
	if (ns)		return ns->leftmost();
	else		return name->rightmost();
}

line_position
namespace_body::rightmost(void) const {
	if (semi)	return semi->rightmost();
	else if (rb)	return rb->rightmost();
	else if (body)	return body->rightmost();
	else if (lb)	return lb->rightmost();
	else		return name->rightmost();
}

// recursive type-checker
never_ptr<const object>
namespace_body::check_build(context& c) const {
	STACKTRACE("namespace_body::check_build()");
	TRACE_CHECK_BUILD(
		cerr << c.auto_indent() << 
			"namespace_body::check_build(...): " << *name;
	)
	// use context lookup: see if namespace already exists in super-scope
		// name_space* ns = c.lookup_namespace(name);
	// if so, open it up, and work with existing namespace
	// otherwise register a new namespace, add it to context
	c.open_namespace(*name);
	// if there was error, would've exited...

	if (body)			// may be NULL, which means empty
		body->check_build(c);

//	TRACE_CHECK_BUILD(
//		cerr << c.auto_indent() << "leaving namespace: " << *name;
//	)
	c.close_namespace();

	// if no errors, return pointer to the namespace just processed
	return c.top_namespace();
}

//=============================================================================
// class namespace_id method definitions

namespace_id::namespace_id(qualified_id* i) : node(), qid(i) {
	NEVER_NULL(qid);
}

namespace_id::~namespace_id() {
}

ostream&
namespace_id::what(ostream& o) const {
#if 0
	return o << "(namespace-id): " << *qid;
#else
	return o << util::what<namespace_id>::name() << ": " << *qid;
#endif
}

line_position
namespace_id::leftmost(void) const {
	return qid->leftmost();
}

line_position
namespace_id::rightmost(void) const {
	return qid->rightmost();
}

qualified_id*
namespace_id::force_absolute(const token_string* s) {
	return qid->force_absolute(s);
}

bool
namespace_id::is_absolute(void) const {
	return qid->is_absolute();
}

/*** NOT USED... yet
never_ptr<const object>
namespace_id::check_build(context& c) const {
}
***/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// non-member functions

ostream&
operator << (ostream& o, const namespace_id& id) {
	return o << *id.qid;
}

//=============================================================================
// class using_namespace method definitions

/**
	Constructor for using_namespace directive.  
	\param o the "open" keyword.  
	\param i the id_expr qualified identifier.  
	\param s the terminating semicolon.  
 */
CONSTRUCTOR_INLINE
using_namespace::using_namespace(
		const token_keyword* o, const namespace_id* i, 
		const token_char* s) : root_item(),
		open(o), id(i), as(NULL), alias(NULL), semi(s) {
	NEVER_NULL(open); NEVER_NULL(id); NEVER_NULL(semi);
}

/**
	Constructor for using_namespace directive.  
	\param o the "open" keyword.  
	\param i the id_expr qualified identifier.  
	\param a the "as" keyword.  
	\param n the alias name.  
	\param s the terminating semicolon.  
 */
CONSTRUCTOR_INLINE
using_namespace::using_namespace(
		const token_keyword* o, const namespace_id* i, 
		const token_keyword* a, const token_identifier* n, 
		const token_char* s) : root_item(),
		open(o), id(i), as(a), alias(n), semi(s) {
	NEVER_NULL(open); NEVER_NULL(id); NEVER_NULL(as);
	NEVER_NULL(alias); NEVER_NULL(semi);
}

/// default destructor
DESTRUCTOR_INLINE
using_namespace::~using_namespace() {
}

#if 0
ostream&
using_namespace::what(ostream& o) const {
	return o << "(using-namespace)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(using_namespace)
#endif

line_position
using_namespace::leftmost(void) const {
	return open->leftmost();
}

line_position
using_namespace::rightmost(void) const {
	if (semi)	return semi->rightmost();
	else if (alias)	return alias->rightmost();
	else if (as)	return as->rightmost();
	else		return id->rightmost();
}

/// returns a pointer to a valid namespace that's now mapped in this scope
never_ptr<const object>
using_namespace::check_build(context& c) const {
	STACKTRACE("using_namespace::check_build()");
if (alias) {
	TRACE_CHECK_BUILD(
		cerr << c.auto_indent() << 
			"using_namespace::check_build(...) (alias): "
			<< *id;
	)
	c.alias_namespace(*id->get_id(), *alias);
} else {
	TRACE_CHECK_BUILD(
		cerr << c.auto_indent() << 
			"using_namespace::check_build(...) (using): "
			<< *id;
	)
	// if aliased... print all, report as error (done inside)
	c.using_namespace(*id->get_id());
}
	return c.top_namespace();
}

//=============================================================================
// class concrete_type_ref method definitions

CONSTRUCTOR_INLINE
concrete_type_ref::concrete_type_ref(const type_base* n, const expr_list* t) : 
		node(), base(n), temp_spec(t) {
	NEVER_NULL(base);
}

DESTRUCTOR_INLINE
concrete_type_ref::~concrete_type_ref() {
}

#if 0
ostream&
concrete_type_ref::what(ostream& o) const {
	return o << "(type-ref)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(concrete_type_ref)
#endif

line_position
concrete_type_ref::leftmost(void) const {
	return base->leftmost();
}

line_position
concrete_type_ref::rightmost(void) const {
	if (temp_spec)	return temp_spec->rightmost();
	else		return base->rightmost();
}

never_ptr<const type_base>
concrete_type_ref::get_base_def(void) const {
	return base;
}

never_ptr<const expr_list>
concrete_type_ref::get_temp_spec(void) const {
	return temp_spec;
}

/**
	Type-check a type reference, a definition with optional template
	arguments.  The type reference is used for creating instantiations.  
	If successful, this sets the current_fundamental_type in the context.  
	\return NULL, caller needs to check the current_fundamental_type
		set in the context.  
	used to return the current fundamental type reference if successful,
		else NULL.
 */
never_ptr<const object>
concrete_type_ref::check_build(context& c) const {
	typedef	never_ptr<const object>		return_type;
	STACKTRACE("concrete_type_ref::check_build()");

	never_ptr<const object> o;
	TRACE_CHECK_BUILD(
		what(cerr << c.auto_indent()) <<
			"concrete_type_ref::check_build(...): ";
	)

	// sets context's current definition
	o = base->check_build(c);
	const never_ptr<const definition_base>
		d(o.is_a<const definition_base>());
	// and should return reference to definition
	if (!d) {
		cerr << "concrete_type_ref: bad definition reference!  "
			"ERROR! " << base->where() << endl;
		THROW_EXIT;		// temporary
		return return_type(NULL);
	}

	// check template arguments, if given
	if (temp_spec) {
		// FINISH ME!!!!!!!!!!
		// using current_definition_reference
		temp_spec->check_build(c);
		// useless return value, grab object_list off the stack
		count_ptr<object> o(c.pop_top_object_stack());

		// remember to check the list of template formals
		// which aren't yet tied to a definition!
		// each iteration should add one more formal to the
		// current_template_formals list.  

		// should return pointer to template arguments, 
		// which is not an object yet...
		if (!o)	{
			cerr << "concrete_type_ref: "
				"bad template args!  ERROR " 
				<< temp_spec->where() << endl;
			THROW_EXIT;		// temporary
			return return_type(NULL);
		} 
		const count_ptr<object_list>
			ol(o.is_a<object_list>());
		NEVER_NULL(ol);
		excl_ptr<dynamic_param_expr_list>
			tpl = ol->make_param_expr_list();
		if (!tpl) {
			cerr << "ERROR building template parameter "
				"expression list.  " << temp_spec->where()
				<< endl;
			THROW_EXIT;		// temporary
		}
		const count_ptr<const fundamental_type_reference>
			type_ref(d->make_fundamental_type_reference(tpl));
		if (!type_ref) {
			cerr << "ERROR making complete type reference.  "
				<< where() << endl;
			THROW_EXIT;
		}
		c.set_current_fundamental_type(type_ref);
	} else {
		// if no args are supplied, 
		// make sure that the definition doesn't require template args!
		// Now allows default values for unsupplied arguments.  
		if(!d->check_null_template_argument()) {
			cerr << "definition expecting template arguments "
				"where none were given!  " << where() << endl;
			THROW_EXIT;		// temporary
			return never_ptr<const object>(NULL);
		} else {
			const count_ptr<const fundamental_type_reference>
				type_ref(d->make_fundamental_type_reference());
			if (!type_ref) {
				cerr << "ERROR making complete type reference.  "
					<< where() << endl;
				THROW_EXIT;
			}
			c.set_current_fundamental_type(type_ref);
		}
	}
	return return_type(NULL);

// we've made it!  set the fundamental_type_reference for instantiation
//	return c.set_current_fundamental_type();
	// who should reset_current_fundamental_type?
	// the decl_lists? or their containers?
}

//=============================================================================
// class data_type_ref_list method definitions

data_type_ref_list::data_type_ref_list(const concrete_type_ref* c) :
		parent(c) { }

data_type_ref_list::~data_type_ref_list() { }

//=============================================================================
#if 0
// moved to "art_parser_instance.cc"
// class guarded_definition_body method definitions

CONSTRUCTOR_INLINE
guarded_definition_body::guarded_definition_body(const expr* e, 
		const terminal* a, const definition_body* b) :
		instance_management(), guard(e), arrow(a), body(b) {
	NEVER_NULL(guard); NEVER_NULL(arrow); NEVER_NULL(body);
}

DESTRUCTOR_INLINE
guarded_definition_body::~guarded_definition_body() {
}

ostream&
guarded_definition_body::what(ostream& o) const {
	return o << "(guarded-def-body)";
}

line_position
guarded_definition_body::leftmost(void) const {
	return guard->leftmost();
}

line_position
guarded_definition_body::rightmost(void) const {
	return body->rightmost();
}

//=============================================================================
// class guarded_definition_body_list method definitions

guarded_definition_body_list::guarded_definition_body_list(
		const guarded_definition_body* g) :
		parent(g) { }

guarded_definition_body_list::~guarded_definition_body_list() { }
#endif

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __ART_PARSER_BASE_CC__

