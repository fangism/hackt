// "art_parser.cc"
// class method definitions for ART::parser

// template instantiations are at the end of the file

// rule-of-thumb for inline directives:
// only inline constructors if you KNOW that they will not be be needed
// outside of this module, because we don't have a means to export
// inline methods other than defining in the header or using
// -fkeep-inline-functions

#include <iostream>
#include <vector>

#include "art_parser_debug.h"
#include "art_macros.h"
#include "art_switches.h"
#include "art_parser_template_methods.h"

#include "art_parser_expr.h"
#include "art_parser_token.h"

#include "art_symbol_table.h"
#include "art_object_base.h"
#include "art_object_expr.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// debug section

//=============================================================================
namespace ART {
namespace parser {
using namespace std;

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

// eventually token keywords here too? or "art_parser_token.cc"

//=============================================================================
// class node method definitions

/// reports location spanned by a node in the source file
inline
line_range
node::where(void) const {
	return line_range(leftmost(), rightmost());
}

/**
	Default type-checker and object builder does nothing.  
	Should be re-implemented in all terminal subclasses.  
	Eventually make this pure virtual.  
	Should really take a context&...
 */
never_const_ptr<object>
node::check_build(never_ptr<context> c) const {
	// We DO want to print this message, even in regression testing. 
	what(cerr << c->auto_indent() << 
		"check_build() not implemented yet for ");
	return c->top_namespace();
}

//=============================================================================
// class root_item method definitions

/// Empty constructor
CONSTRUCTOR_INLINE
root_item::root_item() : node() { }

/// Empty virtual destructor
DESTRUCTOR_INLINE
root_item::~root_item() { }

//-----------------------------------------------------------------------------
// class root_body method definitions

root_body::root_body(const root_item* r) : parent(r) { }

root_body::~root_body() { }

//=============================================================================
// class template_argument_list method definition

CONSTRUCTOR_INLINE
template_argument_list::template_argument_list(expr_list* e) : expr_list() {
	e->release_append(*this);
}

DESTRUCTOR_INLINE
template_argument_list::~template_argument_list() {
}

ostream&
template_argument_list::what(ostream& o) const {
	return o << "(template-arg-list)";
}

/**
	Type checks a expression list in the template argument context.  
	First builds a list of parameter expression objects.  
	Should expr_list do this automatically?
	Can it be used for both template and port arguments?
	TO DO: manipulate context using definition_base.  
	\param c the context object -- its current_definition_reference
		must be set to a valid definition, because this
		uses that definition to type-check.  
	\return NULL always?  How does caller know something went wrong?
 */
never_const_ptr<object>
template_argument_list::check_build(never_ptr<context> c) const {
	excl_ptr<template_param_list> targs(new template_param_list);
	assert(targs);
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() <<
			"template_argument_list::check_build(...): " << endl;
	)
	// enter the template argument context
	// o = expr_list::check_build(c);	// DON'T USE, override
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		count_const_ptr<expr> e(*i);
		assert(e);			// ever blank expression?
		// this should cache parameter expressions
		never_const_ptr<object> eret(e->check_build(c));
		if (eret) {
			never_const_ptr<param_expr>
				exref(eret.is_a<param_expr>());
			assert(exref);
			targs->push_back(exref);
		} else {
			// failed!!!  better error handling later
			cerr << "BAD template argument (not an expression)!"
				<< endl;
			exit(1);
		}
	}
	// set context's template arguments
	c->set_current_template_arguments(targs);
	// leave the template argument context
	return never_const_ptr<object>(NULL);
	// set the current_fundamental_type upon returning from this
}

//=============================================================================
// template class node_list<> method definitions
// had trouble finding reference to template functions defined here...?
// template exporting not implemented in any gcc compiler yet...

// thus all definitions must be in header
// this way, PRS and HSE may use them

//=============================================================================
// class type_base method definitions

CONSTRUCTOR_INLINE
type_base::type_base() : node() { }

DESTRUCTOR_INLINE
type_base::~type_base() { }

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
	return base->what(o << "(type-id): ");
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
	\return pointer to type reference.  
 */
never_const_ptr<object>
type_id::check_build(never_ptr<context> c) const {
//	never_const_ptr<object> o;
//	const definition_base* d;
	never_const_ptr<definition_base> d;
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() <<
			"type_id::check_build(...): " << endl;
	)
	d = c->lookup_definition(*base);
//	o = c->lookup_definition(*base);
//	d = IS_A(const definition_base*, o);
	if (!d) {
//		cerr << "type_id::check_build(never_ptr<context>) : ERROR!" << endl;
		return never_const_ptr<object>(NULL);
	}
	// set type definition reference
	d = d->set_context_definition(*c);	// pure virtual
	// c->set_definition(d);		// don't care which kind...
	return d;
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
	assert(c);
}

DESTRUCTOR_INLINE
chan_type::~chan_type() {
}

ostream&
chan_type::what(ostream& o) const {
	return o << "(chan-type)";
}

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
	dtypes = excl_const_ptr<data_type_ref_list>(t);
	assert(dtypes);
	return this;
}

never_const_ptr<object>
chan_type::check_build(never_ptr<context> c) const {
	cerr << "chan_type::check_build(): FINISH ME!";
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class statement method definitions

CONSTRUCTOR_INLINE
statement::statement() : node() { }

DESTRUCTOR_INLINE
statement::~statement() { }

//=============================================================================
// class incdec_stmt method definitions

CONSTRUCTOR_INLINE
incdec_stmt::incdec_stmt(const expr* n, const terminal* o) : statement(),
		e(n), op(o) {
	assert(e); assert(op);
}

#if 0
CONSTRUCTOR_INLINE
incdec_stmt::incdec_stmt(excl_const_ptr<expr> n, excl_const_ptr<terminal> o) :
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
excl_const_ptr<expr>
incdec_stmt::release_expr(void) {
	return e;
}

excl_const_ptr<terminal>
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

ostream&
incdec_stmt::what(ostream& o) const {
	return o << "(inc/dec-stmt)";
}

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
	assert(lhs); assert(op); assert(rhs);
}

#if 0
CONSTRUCTOR_INLINE
assign_stmt::assign_stmt(excl_const_ptr<expr> left, excl_const_ptr<terminal> o, 
		excl_const_ptr<expr> right) : statement(),
		lhs(left), op(o), rhs(right) {
	assert(lhs); assert(op); assert(rhs);
}
#endif

DESTRUCTOR_INLINE
assign_stmt::~assign_stmt() {
	SAFEDELETE(lhs); SAFEDELETE(op); SAFEDELETE(rhs);
}

#if 0
excl_const_ptr<expr>
assign_stmt::release_lhs(void) {
	return lhs;
}

excl_const_ptr<terminal>
assign_stmt::release_op(void) {
	return op;
}

excl_const_ptr<expr>
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

ostream&
assign_stmt::what(ostream& o) const {
	return o << "(assign-stmt)";
}

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

CONSTRUCTOR_INLINE
def_body_item::def_body_item() : node() { }

DESTRUCTOR_INLINE
def_body_item::~def_body_item() { }

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
	tag = excl_const_ptr<token_keyword>(t);
	assert(tag);
	return this;
}

line_position
language_body::leftmost(void) const {
	// what if untagged?
	assert(tag);
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
namespace_body::
namespace_body(const token_keyword* s, const token_identifier* n, 
		const terminal* l, const root_body* b,
		const terminal* r, const terminal* c) :
		root_item(),       
		ns(s), name(n), lb(l), body(b), rb(r), semi(c) {
	assert(ns); assert(name); assert(lb);
	// body may be NULL
	assert(rb); assert(semi);
}

/// destructor
DESTRUCTOR_INLINE
namespace_body::
~namespace_body() {
}

/// what eeeez it, man?
ostream&
namespace_body::
what(ostream& o) const {
	return o << "(namespace-body: " << *name << ")";
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
never_const_ptr<object>
namespace_body::
check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() << 
			"namespace_body::check_build(...): " << *name;
	)
	// use context lookup: see if namespace already exists in super-scope
		// name_space* ns = c->lookup_namespace(name);
	// if so, open it up, and work with existing namespace
	// otherwise register a new namespace, add it to context
	c->open_namespace(*name);
	if (body)			// may be NULL, which means empty
		body->check_build(c);

//	TRACE_CHECK_BUILD(
//		cerr << c->auto_indent() << "leaving namespace: " << *name;
//	)
	c->close_namespace();
	// if no errors, return pointer to the namespace just processed
	return c->top_namespace();
}

//=============================================================================
// class namespace_id method definitions

namespace_id::namespace_id(qualified_id* i) : node(), qid(i) {
	assert(qid);
}

namespace_id::~namespace_id() {
}

ostream&
namespace_id::what(ostream& o) const {
	return o << "(namespace-id): " << *qid;
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
never_const_ptr<object>
namespace_id::check_build(never_ptr<context> c) const {
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
using_namespace::
using_namespace(const token_keyword* o, const namespace_id* i, 
		const token_char* s) : root_item(),
		open(o), id(i), as(NULL), alias(NULL), semi(s) {
	assert(open); assert(id); assert(semi);
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
using_namespace::
using_namespace(const token_keyword* o, const namespace_id* i, 
		const token_keyword* a, const token_identifier* n, 
		const token_char* s) : root_item(),
		open(o), id(i), as(a), alias(n), semi(s) {
	assert(open); assert(id); assert(as); assert(alias); assert(semi);
}

/// default destructor
DESTRUCTOR_INLINE
using_namespace::~using_namespace() {
}

ostream&
using_namespace::what(ostream& o) const {
	return o << "(using-namespace)";
}

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
never_const_ptr<object>
using_namespace::
check_build(never_ptr<context> c) const {
if (alias) {
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() << 
			"using_namespace::check_build(...) (alias): "
			<< *id;
	)
	c->alias_namespace(*id->get_id(), *alias);
} else {
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() << 
			"using_namespace::check_build(...) (using): "
			<< *id;
	)
	// if aliased... print all, report as error (done inside)
	c->using_namespace(*id->get_id());
}
	return c->top_namespace();
}

//=============================================================================
// class concrete_type_ref method definitions

CONSTRUCTOR_INLINE
concrete_type_ref::concrete_type_ref(const type_base* n, 
		const template_argument_list* t) : 
		node(), base(n), temp_spec(t) {
	assert(base);
}

DESTRUCTOR_INLINE
concrete_type_ref::~concrete_type_ref() {
}

ostream&
concrete_type_ref::what(ostream& o) const {
	return o << "(type-ref)";
}

line_position
concrete_type_ref::leftmost(void) const {
	return base->leftmost();
}

line_position
concrete_type_ref::rightmost(void) const {
	if (temp_spec) return temp_spec->rightmost();
	else return base->rightmost();
}

/**
	Type-check a type reference, a definition with optional template
	arguments.  The type reference is used for creating instantiations.  
	If successful, this sets the current_fundamental_type in the context.  
	\return the current fundamental type reference if successful,
		else NULL.
 */
never_const_ptr<object>
concrete_type_ref::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent()) <<
			"concrete_type_ref::check_build(...): ";
	)

	// sets context's current definition
	o = base->check_build(c);
	never_const_ptr<definition_base> d(o.is_a<definition_base>());
	// and should return reference to definition
	if (!d) {
		cerr << "concrete_type_ref: bad definition reference!  "
			"ERROR! " << base->where() << endl;
		return never_const_ptr<object>(NULL);
	}

	// check template arguments, if given
	if (temp_spec) {
		// using current_definition_reference
		o = temp_spec->check_build(c);
		// remember to check the list of template formals
		// which aren't yet tied to a definition!
		// each iteration should add one more formal to the
		// current_template_formals list.  

		// should return pointer to template arguments, 
		// which is not an object yet...
		if (!o)	{
			cerr << "concrete_type_ref: "
				"bad template args!  ERROR" << endl;
			return never_const_ptr<object>(NULL);
		}
	} else {
		// if no args are supplied, 
		// make sure that the definition doesn't require template args!
		// Now allows default values for unsupplied arguments.  
		if(!d->check_null_template_argument()) {
			cerr << "definition expecting template arguments "
				"where none were given!" << endl;
			return never_const_ptr<object>(NULL);
		}
	}

	// we've made it!  set the fundamental_type_reference for instantiation
	return c->set_current_fundamental_type();
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
	assert(guard); assert(arrow); assert(body);
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


