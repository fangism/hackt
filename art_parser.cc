// "art_parser.cc"
// class method definitions for ART::parser

// template instantiations are at the end of the file

#include <iostream>
#include <stdio.h>		// for sprintf
#include <string.h>		// for a few C-string functions

#include "art_macros.h"
#include "art_switches.h"
#include "art_parser_template_methods.h"
#include "art_symbol_table.h"
#include "art_object.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

namespace ART {
namespace parser {

//=============================================================================
// global constants
const char none[] = "";		///< delimiter for node_list template argument
const char comma[] = ",";	///< delimiter for node_list template argument
const char semicolon[] = ";";	///< delimiter for node_list template argument
const char scope[] = "::";	///< delimiter for node_list template argument
const char thickbar[] = "[]";	///< delimiter for node_list template argument
const char colon[] = ":";	///< delimiter for node_list template argument

//=============================================================================
// class node method definitions

/// node's virtual destructor, the mother of all virtual destructors
node::~node() { }

/// reports location spanned by a node in the source file
inline
line_range
node::where(void) const {
	return line_range(leftmost(), rightmost());
}

/**
	Default type-checker and object builder does nothing.  
	Should be re-implemented in all terminal subclasses.  
 */
object*
node::check_build(context* c) const {
	cerr << c->auto_indent() << 
		"check_build() not fully-implmented yet for ";
	what(cerr);
	return NULL;
}

//=============================================================================
// class nonterminal definitions

DESTRUCTOR_INLINE
nonterminal::~nonterminal() { }

//=============================================================================
// class root_item method definitions

/// Empty constructor
CONSTRUCTOR_INLINE
root_item::root_item() : nonterminal() { }

/// Empty virtual destructor
DESTRUCTOR_INLINE
root_item::~root_item() { }

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

CONSTRUCTOR_INLINE
type_id::type_id(node* b, node* t) : type_base(),
	base(b),        // can't restrict type yet...
		// may be id_expr, or chan_type, or data_type
		// or user-defined qualified id...
	temp_spec(dynamic_cast<expr_list*>(t))  // may be NULL
	{ }

DESTRUCTOR_INLINE
type_id::~type_id() {
	SAFEDELETE(base); SAFEDELETE(temp_spec);
}

ostream&
type_id::what(ostream& o) const {
	base->what(o << "(type-id): ");
	if (temp_spec) temp_spec->what(o);
	return o;
}

line_position
type_id::leftmost(void) const {
	return base->leftmost();
}

line_position
type_id::rightmost(void) const {
	return temp_spec->rightmost();
}

//=============================================================================
// class data_type_base method definitions

CONSTRUCTOR_INLINE
data_type_base::data_type_base(node* t, node* l, node* w, node* r) :
		type_base(),
		type(dynamic_cast<token_keyword*>(t)),
		la(dynamic_cast<token_char*>(l)),
		width(dynamic_cast<token_int*>(w)),
		ra(dynamic_cast<token_char*>(r)) {
	assert(type); assert(la); assert(width); assert(ra);
}

CONSTRUCTOR_INLINE
data_type_base::data_type_base(node* t) : type_base(),
		type(dynamic_cast<token_keyword*>(t)),
		la(NULL), width(NULL), ra(NULL) {
	assert(type);
}

DESTRUCTOR_INLINE
data_type_base::~data_type_base() {
	SAFEDELETE(type); SAFEDELETE(la);
	SAFEDELETE(width); SAFEDELETE(ra);
}

ostream&
data_type_base::what(ostream& o) const {
	return o << "(data-type-base)";
}

line_position
data_type_base::leftmost(void) const {
	return type->leftmost();
}

line_position
data_type_base::rightmost(void) const {
	if (ra)         return ra->rightmost();
	else if (width) return width->rightmost();
	else if (la)    return la->rightmost();
	else            return type->rightmost();
}

object*
data_type_base::check_build(context* c) const {
// where do we report the error? in c?
/**
	if (width)
		return c->set_type_def(*type, *width);
	else
**/
		return c->set_type_def(*type);
}

//=============================================================================
// class data_type method definitions

CONSTRUCTOR_INLINE
data_type::data_type(node* df, node* n, node* dp, node* b, node* p,
                node* l, node* s, node* g, node* r) :
		type_base(), 
		def(dynamic_cast<token_keyword*>(df)), 
		name(dynamic_cast<token_identifier*>(n)), 
		dop(dynamic_cast<token_string*>(dp)), 
		bdt(dynamic_cast<data_type_base*>(b)), 
		params(dynamic_cast<data_param_list*>(p)), 
		lb(dynamic_cast<token_char*>(l)), 
		setb(dynamic_cast<language_body*>(s)), 
		getb(dynamic_cast<language_body*>(g)), 
		rb(dynamic_cast<token_char*>(r)) {
	assert(def); assert(name); assert(dop);
	assert(bdt); assert(params); assert(lb);
	assert(setb); assert(getb); assert(rb);
}

DESTRUCTOR_INLINE
data_type::~data_type() {
	SAFEDELETE(def); SAFEDELETE(name); SAFEDELETE(dop);
	SAFEDELETE(bdt); SAFEDELETE(params); SAFEDELETE(lb);
	SAFEDELETE(setb); SAFEDELETE(getb); SAFEDELETE(rb);
}

ostream&
data_type::what(ostream& o) const {
	return o << "(user-data-type)";
}

line_position
data_type::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return name->leftmost();
}

line_position
data_type::rightmost(void) const {
	if (rb)         return rb->rightmost();
	else            return getb->rightmost();
}

//=============================================================================
// class chan_type method definitions

CONSTRUCTOR_INLINE
chan_type::chan_type(node* c, node* d, node* t) : type_base(),
		chan(dynamic_cast<token_keyword*>(c)),
		dir(dynamic_cast<token_char*>(d)),
		dtypes(dynamic_cast<base_data_type_list*>(t)) {
	assert(c);
	if(d) assert(dir);
	if (t) assert(dtypes);
}

DESTRUCTOR_INLINE
chan_type::~chan_type() {
	SAFEDELETE(chan); SAFEDELETE(dir); SAFEDELETE(dtypes);
}

/***
chan_type*
chan_type::attach_data_types(node* t);
***/

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
	return dtypes->rightmost();
}

//=============================================================================
// class user_chan_type method definitions

CONSTRUCTOR_INLINE
user_chan_type::user_chan_type(node* df, node* n, node* dp, node* b, node* p,
                node* l, node* s, node* g, node* r) :
		type_base(), 
		def(dynamic_cast<token_keyword*>(df)), 
		name(dynamic_cast<token_identifier*>(n)), 
		dop(dynamic_cast<token_string*>(dp)), 
		bct(dynamic_cast<chan_type*>(b)), 
		params(dynamic_cast<data_param_list*>(p)), 
		lb(dynamic_cast<token_char*>(l)), 
		sendb(dynamic_cast<language_body*>(s)), 
		recvb(dynamic_cast<language_body*>(g)), 
		rb(dynamic_cast<token_char*>(r)) {
	assert(def); assert(name); assert(dop);
	assert(bct); assert(params); assert(lb);
	assert(sendb); assert(recvb); assert(rb);
}

DESTRUCTOR_INLINE
user_chan_type::~user_chan_type() {
	SAFEDELETE(def); SAFEDELETE(name); SAFEDELETE(dop);
	SAFEDELETE(bct); SAFEDELETE(params); SAFEDELETE(lb);
	SAFEDELETE(sendb); SAFEDELETE(recvb); SAFEDELETE(rb);
}

ostream&
user_chan_type::what(ostream& o) const {
	return o << "(user-chan-type)";
}

line_position
user_chan_type::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return name->leftmost();
}

line_position
user_chan_type::rightmost(void) const {
	if (rb)         return rb->rightmost();
	else            return recvb->rightmost();
}

//=============================================================================
// class statement method definitions

CONSTRUCTOR_INLINE
statement::statement() : nonterminal() { }

DESTRUCTOR_INLINE
statement::~statement() { }

//=============================================================================
// class incdec_stmt method definitions

CONSTRUCTOR_INLINE
incdec_stmt::incdec_stmt(node* n, node* o) : statement(),
		e(dynamic_cast<expr*>(n)),
		op(dynamic_cast<terminal*>(o)) {
	assert(e); assert(op);
}

DESTRUCTOR_INLINE
incdec_stmt::~incdec_stmt() {
	SAFEDELETE(e); SAFEDELETE(op);
}

/**
	Release operations are needed for destructive transfer of ownership.
	The consumers of the return pointers are thus responsible for the
	memory at their location. 
 */
expr*
incdec_stmt::release_expr(void) {
	expr* r = e; e = NULL; return r;
}

terminal*
incdec_stmt::release_op(void) {
	terminal* r = op; op = NULL; return r;
}

ostream&
incdec_stmt::what(ostream& o) const {
	return o << "(inc/dec-stmt)";
}

line_position
incdec_stmt::leftmost(void) const {
	return e->leftmost();
}

line_position
incdec_stmt::rightmost(void) const {
	return op->rightmost();
}

//=============================================================================
// class assign_stmt method definitions

CONSTRUCTOR_INLINE
assign_stmt::assign_stmt(node* left, node* o, node* right) : statement(),
		lhs(dynamic_cast<expr*>(left)),
		op(dynamic_cast<terminal*>(o)),
		rhs(dynamic_cast<expr*>(right)) {
	assert(lhs); assert(op); assert(rhs);
}

DESTRUCTOR_INLINE
assign_stmt::~assign_stmt() {
	SAFEDELETE(lhs); SAFEDELETE(op); SAFEDELETE(rhs);
}

expr*
assign_stmt::release_lhs(void) {
	expr* r = lhs; lhs = NULL; return r;
}

terminal*
assign_stmt::release_op(void) {
	terminal* r = op; op = NULL; return r;
}

expr*
assign_stmt::release_rhs(void) {
	expr* r = rhs; rhs = NULL; return r;
}

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
def_body_item::def_body_item() : nonterminal() { }

DESTRUCTOR_INLINE
def_body_item::~def_body_item() { }

//=============================================================================
// class language_body methd definitions

CONSTRUCTOR_INLINE
language_body::language_body(node* t) : def_body_item(),
		tag(dynamic_cast<token_keyword*>(t)) {
	if (t) assert(tag);
}

DESTRUCTOR_INLINE
language_body::~language_body() { SAFEDELETE(tag); }

language_body*
language_body::attach_tag(node* t) {
	tag = dynamic_cast<token_keyword*>(t);
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
namespace_body(node* s, node* n, node* l, node* b, node* r, node* c) :
		root_item(),       
		ns(dynamic_cast<token_keyword*>(s)),
		name(dynamic_cast<token_identifier*>(n)),
		lb(dynamic_cast<terminal*>(l)),
		body(dynamic_cast<root_body*>(b)),     
		rb(dynamic_cast<terminal*>(r)),
		semi(dynamic_cast<terminal*>(c)) {
	assert(ns); assert(name); assert(lb);
	if (b) assert(body);		// body may be NULL
	assert(rb); assert(semi);
}

/// destructor
DESTRUCTOR_INLINE
namespace_body::
~namespace_body() {
	SAFEDELETE(ns); SAFEDELETE(name); SAFEDELETE(lb);
	SAFEDELETE(body); SAFEDELETE(rb); SAFEDELETE(semi);
}

/// what eeeez it, man?
ostream&
namespace_body::
what(ostream& o) const {
	return o << "(namespace-body: " << *name << ")";
}

line_position
namespace_body::leftmost(void) const {
	return ns->leftmost();
}

line_position
namespace_body::rightmost(void) const {
	return semi->rightmost();
}

// recursive type-checker
object*
namespace_body::
check_build(context* c) const {
	cerr << c->auto_indent() << "entering namespace: " << *name;
	// use context lookup: see if namespace already exists in super-scope
		// name_space* ns = c->lookup_namespace(name);
	// if so, open it up, and work with existing namespace
	// otherwise register a new namespace, add it to context
	c->open_namespace(*name);
	if (body)			// may be NULL, which means empty
		body->check_build(c);

	cerr << c->auto_indent() << "leaving namespace: " << *name;
	c->close_namespace();
	// if no errors, return pointer to the namespace just processed
	return NULL;
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
using_namespace(node* o, node* i, node* s) :
		root_item(),
		open(dynamic_cast<token_keyword*>(o)),
		id(dynamic_cast<id_expr*>(i)),     
		as(NULL), alias(NULL), 
		semi(dynamic_cast<token_char*>(s)) {
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
using_namespace(node* o, node* i, node* a, node* n, node* s) :
		root_item(),
		open(dynamic_cast<token_keyword*>(o)),
		id(dynamic_cast<id_expr*>(i)),     
		as(dynamic_cast<token_keyword*>(a)),		// optional
		alias(dynamic_cast<token_identifier*>(n)),	// optional
		semi(dynamic_cast<token_char*>(s)) {
	assert(open); assert(id); assert(as); assert(alias); assert(semi);
}

/// default destructor
DESTRUCTOR_INLINE
using_namespace::~using_namespace() {
	SAFEDELETE(open); SAFEDELETE(id);
	SAFEDELETE(as); SAFEDELETE(alias);
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
	return semi->rightmost();
}

/// returns a pointer to a valid namespace that's now mapped in this scope
object*
using_namespace::
check_build(context* c) const {
	if (alias) {
		cerr << c->auto_indent() << "aliasing namespace: " << *id;
		return c->alias_namespace(*id, *alias);
	} else {
		cerr << c->auto_indent() << "using namespace: " << *id;
		// if aliased... print all, report as error (done inside)
		return c->using_namespace(*id);
	}
}

//=============================================================================
// chan_type methods
/**
	Associates a channel or port with a data type, such as a list of 
	ints and bools.  
	\param t is the type list for the channel.  
 */
chan_type*
chan_type::attach_data_types(node* t) {
	assert(t); assert(!dtypes);     // sanity check    
	dtypes = dynamic_cast<base_data_type_list*>(t);
	assert(dtypes);
	return this;
}

//=============================================================================
// class declaration_base method definitions

CONSTRUCTOR_INLINE
declaration_base::declaration_base(node* i) :
		def_body_item(), root_item(),
		id(dynamic_cast<expr*>(i)) {
	assert(id);
}

DESTRUCTOR_INLINE
declaration_base::~declaration_base() {
	SAFEDELETE(id);
}

ostream&
declaration_base::what(ostream& o) const {
	return id->what(o << "(declaration-id): ");
}

line_position
declaration_base::leftmost(void) const {
	return id->leftmost();
}

line_position
declaration_base::rightmost(void) const {
	return id->rightmost();
}

line_range
declaration_base::where(void) const {
	return node::where();
}

//=============================================================================
// class declaration_array method definitions

CONSTRUCTOR_INLINE
declaration_array::declaration_array(node* i, node* rl) :
		declaration_base(i),
		ranges(dynamic_cast<range_list*>(rl)) {
	// ranges may be NULL, equivalent to declaration base
}

DESTRUCTOR_INLINE
declaration_array::~declaration_array() {
	SAFEDELETE(ranges);
}

ostream&
declaration_array::what(ostream& o) const {
	return ranges->what(id->what(o << "(declaration-array): "));
}

line_position
declaration_array::rightmost(void) const {
	return ranges->rightmost();
}

//=============================================================================
// class instance_base method definitions

CONSTRUCTOR_INLINE
instance_base::instance_base() : def_body_item(), root_item() { }

DESTRUCTOR_INLINE
instance_base::~instance_base() { }

line_range
instance_base::where(void) const {
	return node::where();
}

//=============================================================================
// class instance_declaration method definitions

/**
	Creates an instance declaration, which may contain a list of 
	identifiers to instantiation.  
	\param t the base type (no array).  
	\param i the identifier list (may contain arrays).
	\param s the terminating semicolon.  
 */
CONSTRUCTOR_INLINE
instance_declaration::instance_declaration(node* t, node* i, node* s) :
		instance_base(),
		type(dynamic_cast<type_base*>(t)),
		ids(dynamic_cast<declaration_id_list*>(i)),
		semi(dynamic_cast<terminal*>(s)) {
	assert(type);
	assert(ids);
	if(s) assert(semi);
}

DESTRUCTOR_INLINE
instance_declaration::~instance_declaration() {
	SAFEDELETE(type); SAFEDELETE (ids); SAFEDELETE(semi);
}

ostream&
instance_declaration::what(ostream& o) const {
	return o << "(instance-decl)";
}

line_position
instance_declaration::leftmost(void) const {
	return type->leftmost();
}

line_position
instance_declaration::rightmost(void) const {
	return semi->rightmost();
}

object*
instance_declaration::check_build(context* c) const {
	object* t;
	what(cerr << c->auto_indent()) << ": ";
	t = type->check_build(c);
	if (t) {
		ids->check_build(c);
	} 
	c->unset_type_def();
	return t;
}

//=============================================================================
// class actuals_connection method definitions

CONSTRUCTOR_INLINE
actuals_connection::actuals_connection(node* i, node* a, node* s) :
		instance_base(), declaration_base(i),
		actuals(dynamic_cast<expr_list*>(a)),
		semi(dynamic_cast<terminal*>(s)) {
	assert(actuals);
	if (s) assert(semi);
}

DESTRUCTOR_INLINE
actuals_connection::~actuals_connection() {
	SAFEDELETE(actuals); SAFEDELETE(semi);
}

// remember to check for declaration context when checking id

ostream&
actuals_connection::what(ostream& o) const {
	return o << "(actuals-connection)";
}

line_position
actuals_connection::leftmost(void) const {
	return declaration_base::leftmost();
}

line_position
actuals_connection::rightmost(void) const {
	return semi->rightmost();
}

line_range
actuals_connection::where(void) const {
	return node::where();
}

//=============================================================================
// class alias_assign method definitions

CONSTRUCTOR_INLINE
alias_assign::alias_assign(node* i, node* o, node* r, node* s) :
		instance_base(), declaration_base(i),
		op(dynamic_cast<terminal*>(o)),
		rhs(dynamic_cast<expr*>(r)),
		semi(dynamic_cast<terminal*>(s)) {
	assert(op); assert(rhs);
	if (s) assert(semi);
}

DESTRUCTOR_INLINE
alias_assign::~alias_assign() {
	SAFEDELETE(op); SAFEDELETE(rhs); SAFEDELETE(semi);
}

ostream&
alias_assign::what(ostream& o) const {
	return o << "(alias-assign)";
}

line_position
alias_assign::leftmost(void) const {
	return declaration_base::leftmost();
}

line_position
alias_assign::rightmost(void) const {
	return semi->rightmost();
}

line_range
alias_assign::where(void) const {
	return node::where();
}

//=============================================================================
// class loop_instantiation method definitions

CONSTRUCTOR_INLINE
loop_instantiation::loop_instantiation(node* l, node* d, node* i, node* c, 
		node* g, node* b, node* r) :
		instance_base(),
		lp(dynamic_cast<terminal*>(l)),
		delim(dynamic_cast<terminal*>(d)),
		index(dynamic_cast<token_identifier*>(i)),
		colon(dynamic_cast<terminal*>(c)),
		rng(dynamic_cast<range*>(g)),
		body(dynamic_cast<definition_body*>(b)),
		rp(dynamic_cast<terminal*>(r)) {
	assert(lp); assert(delim); assert(index);
	assert(colon); assert(rng); assert(body); assert(lp);
}

DESTRUCTOR_INLINE
loop_instantiation::~loop_instantiation() {
	SAFEDELETE(lp); SAFEDELETE(delim); SAFEDELETE(index);
	SAFEDELETE(colon); SAFEDELETE(rng); SAFEDELETE(body);
	SAFEDELETE(rp);
}

ostream&
loop_instantiation::what(ostream& o) const {
	return o << "(loop-instance)";
}

line_position
loop_instantiation::leftmost(void) const {
	return lp->leftmost();
}

line_position
loop_instantiation::rightmost(void) const {
	return rp->rightmost();
}

//=============================================================================
// class port_formal_id method definitions

CONSTRUCTOR_INLINE
port_formal_id::port_formal_id(node* n, node* d) : nonterminal(),
		name(dynamic_cast<token_identifier*>(n)),
		dim(dynamic_cast<range_list*>(d)) {
	assert(name);
	// dim may be NULL
}

DESTRUCTOR_INLINE
port_formal_id::~port_formal_id() {
	SAFEDELETE(name); SAFEDELETE(dim);
}

ostream&
port_formal_id::what(ostream& o) const {
	name->what(o << "(port-formal-id): ");
	if (dim) dim->what(o);
	return o;
}

line_position
port_formal_id::leftmost(void) const {
	return name->leftmost();
}

line_position
port_formal_id::rightmost(void) const {
	return dim->rightmost();
}

//=============================================================================
// class port_formal_decl method definitions

CONSTRUCTOR_INLINE
port_formal_decl::port_formal_decl(node* t, node* i) : nonterminal(),
		type(dynamic_cast<type_id*>(t)),
		ids(dynamic_cast<port_formal_id_list*>(i)) {
	assert(type); assert(ids);
}

DESTRUCTOR_INLINE
port_formal_decl::~port_formal_decl() {
	SAFEDELETE(type); SAFEDELETE(ids);
}

ostream&
port_formal_decl::what(ostream& o) const {
	return o << "(port-formal-decl)";
}

line_position
port_formal_decl::leftmost(void) const {
	return type->leftmost();
}

line_position
port_formal_decl::rightmost(void) const {
	return ids->rightmost();
}

//=============================================================================
// class template_formal_id method definitions

CONSTRUCTOR_INLINE
template_formal_id::template_formal_id(node* n, node* d) : nonterminal(),
		name(dynamic_cast<token_identifier*>(n)),
		dim(dynamic_cast<range_list*>(d)) {
	assert(name);
// dim may be NULL
}

DESTRUCTOR_INLINE
template_formal_id::~template_formal_id() {
	SAFEDELETE(name); SAFEDELETE(dim);
}

ostream&
template_formal_id::what(ostream& o) const {
	name->what(o << "(template-formal-id): ");
	if (dim) dim->what(o);
	return o;
}

line_position
template_formal_id::leftmost(void) const {
	return name->leftmost();
}

line_position
template_formal_id::rightmost(void) const {
	return dim->rightmost();
}

//=============================================================================
// class template_formal_decl method definitions

CONSTRUCTOR_INLINE
template_formal_decl::template_formal_decl(node* t, node* i) :
		nonterminal(),
		type(dynamic_cast<type_id*>(t)),
		ids(dynamic_cast<template_formal_id_list*>(i)) {
	assert(type); assert(ids);
}

DESTRUCTOR_INLINE
template_formal_decl::~template_formal_decl() {
	SAFEDELETE(type); SAFEDELETE(ids);
}

ostream&
template_formal_decl::what(ostream& o) const {
	return o << "(template-formal-decl)";
}

line_position
template_formal_decl::leftmost(void) const {
	return type->leftmost();
}

line_position
template_formal_decl::rightmost(void) const {
	return ids->rightmost();
}

//=============================================================================
// class def_type_id method definitions

CONSTRUCTOR_INLINE
def_type_id::def_type_id(node* n, node* t) : type_base(),
		name(dynamic_cast<token_identifier*>(n)),
		temp_spec(dynamic_cast<template_formal_decl_list*>(t)) {
	assert(name);
	if (t) assert(temp_spec);
}

DESTRUCTOR_INLINE
def_type_id::~def_type_id() {
	SAFEDELETE(name); SAFEDELETE(temp_spec);
}

ostream&
def_type_id::what(ostream& o) const {
	return o << "(def-type-id)";
}

line_position
def_type_id::leftmost(void) const {
	return name->leftmost();
}

line_position
def_type_id::rightmost(void) const {
	if (temp_spec) return temp_spec->rightmost();
	else return name->rightmost();
}

//=============================================================================
// class definition method definitions

CONSTRUCTOR_INLINE
definition::definition(node* b) : root_item(),
		body(dynamic_cast<definition_body*>(b)) {
	assert(body);
}

DESTRUCTOR_INLINE
definition::~definition() {
	SAFEDELETE(body);
}

ostream&
definition::what(ostream& o) const {
	return o << "(definition)";
}

line_position
definition::leftmost(void) const {
	return body->leftmost();
}

line_position
definition::rightmost(void) const {
	return body->rightmost();
}

//=============================================================================
// class process_def method definitions

CONSTRUCTOR_INLINE
process_def::process_def(node* d, node* i, node* p, node* b) :
		definition(b),
		def(dynamic_cast<token_keyword*>(d)),
		idt(dynamic_cast<def_type_id*>(i)),
		ports(dynamic_cast<port_formal_decl_list*>(p)) {
	assert(def); assert(idt); assert(ports);
}

DESTRUCTOR_INLINE
process_def::~process_def() {
	SAFEDELETE(def); SAFEDELETE(idt); SAFEDELETE(ports);
}

ostream&
process_def::what(ostream& o) const {
	return o << "(process-definition)";
}

line_position
process_def::leftmost(void) const {
	return def->leftmost();
}

//=============================================================================
// class guarded_definition_body method definitions

CONSTRUCTOR_INLINE
guarded_definition_body::guarded_definition_body(node* e, node* a, node* b) :
		instance_base(),
		guard(dynamic_cast<expr*>(e)),
		arrow(dynamic_cast<terminal*>(a)),
		body(dynamic_cast<definition_body*>(b)) {
	assert(guard); assert(arrow); assert(body);
}

DESTRUCTOR_INLINE
guarded_definition_body::~guarded_definition_body() {
	SAFEDELETE(guard); SAFEDELETE(arrow); SAFEDELETE(body);
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
// class conditional_instantiation method definitions

CONSTRUCTOR_INLINE
conditional_instantiation::conditional_instantiation(node* n) :
		instance_base(),
		gd(dynamic_cast<guarded_definition_body_list*>(n)) {
	assert(gd);
}

DESTRUCTOR_INLINE
conditional_instantiation::~conditional_instantiation() {

}

ostream&
conditional_instantiation::what(ostream& o) const {
	return o << "(conditional-instance)";
}

line_position
conditional_instantiation::leftmost(void) const {
	return gd->leftmost();
}

line_position
conditional_instantiation::rightmost(void) const {
	return gd->rightmost();
}


//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes
							// also known as...
template class node_list<root_item>;			// root_body
template class node_list<data_type_base,comma>;		// base_data_type_list
template class node_list<def_body_item>;		// definition_body
template class node_list<declaration_base,comma>;	// declaration_id_list
template 
class node_list<instance_declaration,semicolon>;	// data_param_list
template class node_list<port_formal_id,comma>;		// port_formal_id_list
template 
class node_list<port_formal_decl,semicolon>;		// port_formal_decl_list
template 
class node_list<template_formal_id,comma>;	// template_formal_id_list
template 
class node_list<template_formal_decl,semicolon>; // template_formal_decl_list
template 
class node_list<guarded_definition_body,thickbar>;
						// guarded_definition_body_list

//=============================================================================
};
};


#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE


