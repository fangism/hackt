// "art_parser.cc"
// class method definitions for ART::parser

// template instantiations are at the end of the file

// rule-of-thumb for inline directives:
// only inline constructors if you KNOW that they will not be be needed
// outside of this module, because we don't have a means to export
// inline methods other than defining in the header or using
// -fkeep-inline-functions

#include <iostream>

#include "art_parser_debug.h"
#include "art_macros.h"
#include "art_switches.h"
#include "art_parser_template_methods.h"
#include "art_symbol_table.h"
#include "art_object.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
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
const char alias[] = "=";	///< delimiter for node_list template argument

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
const object*
node::check_build(context* c) const {
	// We DO want to print this message, even in regression testing. 
	cerr << c->auto_indent() << 
		"check_build() not fully-implemented yet for ";
	what(cerr);
//	return NULL;
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

//=============================================================================
// class instance_management method definitions

CONSTRUCTOR_INLINE
instance_management::instance_management() : def_body_item(), root_item() {
}

DESTRUCTOR_INLINE
instance_management::~instance_management() {
}

//=============================================================================
// class alias_list method definitions

/**
	\param e should be a member/index expression, an lvalue.  
 */
CONSTRUCTOR_INLINE
alias_list::alias_list(expr* e) : instance_management(), alias_list_base(e) {
}

DESTRUCTOR_INLINE
alias_list::~alias_list() {
}

ostream&
alias_list::what(ostream& o) const {
	return alias_list_base::what(o);
}

line_position
alias_list::leftmost(void) const {
	return alias_list_base::leftmost();
}

line_position
alias_list::rightmost(void) const {
	return alias_list_base::rightmost();
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

CONSTRUCTOR_INLINE
type_id::type_id(id_expr* b, expr_list* t) : type_base(),
		base(b),
		temp_spec(t)  // may be NULL
		{
	assert(base);
	if (t) assert(temp_spec);
}

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
	if (temp_spec)
		return temp_spec->rightmost();
	else return base->rightmost();
}

const object*
type_id::check_build(context* c) const {
	const object* o;
	o = c->set_type_def(*base);		// will handle errors
	if (temp_spec)
		o = temp_spec->check_build(c);
	return o;
}

const id_expr&
type_id::get_base_type(void) const {
	assert(base);
	return *base;
}

const expr_list*
type_id::get_template_spec(void) const {
	return temp_spec;
}

//=============================================================================
// class data_type_base method definitions

CONSTRUCTOR_INLINE
data_type_base::data_type_base(token_type* t, token_char* l, 
	token_int* w, token_char* r) :
		type_base(),
		type(t), la(l), width(w), ra(r) {
	assert(type); assert(la); assert(width); assert(ra);
}

CONSTRUCTOR_INLINE
data_type_base::data_type_base(token_type* t) : type_base(),
		type(t),
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

/**
	Type checks a single data type.  
	Remember to unset_type_def after the list is done.  
 */
const object*
data_type_base::check_build(context* c) const {
	assert(type);
	if (width)
		return c->set_type_def(*type, *width);
	else
		return c->set_type_def(*type);
}

//=============================================================================
// class user_data_type_def method definitions

CONSTRUCTOR_INLINE
user_data_type_signature::user_data_type_signature(
		token_keyword* df, token_identifier* n, token_string* dp, 
		data_type_base* b, data_param_list* p) :
		def(df), name(n), dop(dp), bdt(b), params(p) {
	assert(def); assert(name); assert(dop);
	assert(bdt); assert(params); 
}

DESTRUCTOR_INLINE
user_data_type_signature::~user_data_type_signature() {
	SAFEDELETE(def); SAFEDELETE(name); SAFEDELETE(dop);
	SAFEDELETE(bdt); SAFEDELETE(params); 
}

//=============================================================================
// class user_data_type_prototype method definitions

CONSTRUCTOR_INLINE
user_data_type_prototype::user_data_type_prototype(token_keyword* df, 
	token_identifier* n, token_string* dp, data_type_base* b, 
	data_param_list* p, token_char* s) :
		prototype(), 
		user_data_type_signature(df, n, dp, b, p), 
		semi(s) {
	assert(semi);
}

DESTRUCTOR_INLINE
user_data_type_prototype::~user_data_type_prototype() {
	SAFEDELETE(semi);
}

ostream&
user_data_type_prototype::what(ostream& o) const {
	return o << "(user-data-type-proto)";
}

line_position
user_data_type_prototype::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return name->leftmost();
}

line_position
user_data_type_prototype::rightmost(void) const {
	if (semi)	return semi->rightmost();
	else		return params->rightmost();
}

const object*
user_data_type_prototype::check_build(context* c) const {
#if 0
	const object* o;
	c->declare_datatype(*this);	// really only need name
	o = bdt->check_build(c);
	assert(o);
	o = params->check_build(c);
	assert(o);
	c->close_datatype();
#else
	cerr << "TO DO: user_data_type_prototype::check_build();" << endl;
#endif
	return c->top_namespace();
}

//=============================================================================
// class user_data_type_def method definitions

CONSTRUCTOR_INLINE
user_data_type_def::user_data_type_def(token_keyword* df, token_identifier* n, 
	token_string* dp, data_type_base* b, data_param_list* p, 
	token_char* l, language_body* s, language_body* g, token_char* r) :
		definition(), 
		user_data_type_signature(df, n, dp, b, p), 
		lb(l), setb(s), getb(g), rb(r) {
	assert(lb); assert(setb); assert(getb); assert(rb);
}

DESTRUCTOR_INLINE
user_data_type_def::~user_data_type_def() {
	SAFEDELETE(lb); SAFEDELETE(setb); SAFEDELETE(getb); SAFEDELETE(rb);
}

ostream&
user_data_type_def::what(ostream& o) const {
	return o << "(user-data-type-def)";
}

line_position
user_data_type_def::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return name->leftmost();
}

line_position
user_data_type_def::rightmost(void) const {
	if (rb)         return rb->rightmost();
	else            return getb->rightmost();
}

/*** unveil later...
const object*
user_data_type_def::check_build(context* c) const {
	const object* o;
	c->open_datatype(*this);	// really only need name
	o = bdt->check_build(c);
	assert(o);
	o = params->check_build(c);
	assert(o);
//	setb->check_build(c);
//	getb->check_build(c);
	c->close_datatype();
	return c->top_namespace();
}
***/

//=============================================================================
// class chan_type method definitions

CONSTRUCTOR_INLINE
chan_type::chan_type(token_keyword* c, token_char* d, 
		base_data_type_list* t) : type_base(),
		chan(c), dir(d), dtypes(t) {
	assert(c);
	if(d) assert(dir);
	if (t) assert(dtypes);
}

DESTRUCTOR_INLINE
chan_type::~chan_type() {
	SAFEDELETE(chan); SAFEDELETE(dir); SAFEDELETE(dtypes);
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
chan_type::attach_data_types(base_data_type_list* t) {
	assert(t); assert(!dtypes);     // sanity check    
	dtypes = t;
	assert(dtypes);
	return this;
}

//=============================================================================
// class user_chan_type_signature method definitions

CONSTRUCTOR_INLINE
user_chan_type_signature::user_chan_type_signature(
		token_keyword* df, token_identifier* n, token_string* dp, 
		chan_type* b, data_param_list* p) :
		def(df), name(n), dop(dp), bct(b), params(p) {
	assert(def); assert(name); assert(dop);
	assert(bct); assert(params);
}

DESTRUCTOR_INLINE
user_chan_type_signature::~user_chan_type_signature() {
	SAFEDELETE(def); SAFEDELETE(name); SAFEDELETE(dop);
	SAFEDELETE(bct); SAFEDELETE(params);
}

//=============================================================================
// class user_chan_type_prototype method definitions

CONSTRUCTOR_INLINE
user_chan_type_prototype::user_chan_type_prototype(
	token_keyword* df, token_identifier* n, token_string* dp, 
	chan_type* b, data_param_list* p, token_char* s) :
		prototype(), 
		user_chan_type_signature(df, n, dp, b, p), 
		semi(s) {
	assert(semi);
}

DESTRUCTOR_INLINE
user_chan_type_prototype::~user_chan_type_prototype() {
	SAFEDELETE(semi);
}

ostream&
user_chan_type_prototype::what(ostream& o) const {
	return o << "(user-chan-type-proto)";
}

line_position
user_chan_type_prototype::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return name->leftmost();
}

line_position
user_chan_type_prototype::rightmost(void) const {
	if (semi)	return semi->rightmost();
	else		return params->rightmost();
}

//=============================================================================
// class user_chan_type_def method definitions

CONSTRUCTOR_INLINE
user_chan_type_def::user_chan_type_def(token_keyword* df, token_identifier* n, 
	token_string* dp, chan_type* b, data_param_list* p, token_char* l, 
	language_body* s, language_body* g, token_char* r) :
		definition(), 
		user_chan_type_signature(df, n, dp, b, p), 
		lb(l), sendb(s), recvb(g), rb(r) {
	assert(lb); assert(sendb); assert(recvb); assert(rb);
}

DESTRUCTOR_INLINE
user_chan_type_def::~user_chan_type_def() {
	SAFEDELETE(lb); SAFEDELETE(sendb); SAFEDELETE(recvb); SAFEDELETE(rb);
}

ostream&
user_chan_type_def::what(ostream& o) const {
	return o << "(user-chan-type-def)";
}

line_position
user_chan_type_def::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return name->leftmost();
}

line_position
user_chan_type_def::rightmost(void) const {
	if (rb)         return rb->rightmost();
	else            return recvb->rightmost();
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
incdec_stmt::incdec_stmt(expr* n, terminal* o) : statement(),
		e(n), op(o) {
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
assign_stmt::assign_stmt(expr* left, terminal* o, expr* right) : statement(),
		lhs(left), op(o), rhs(right) {
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
def_body_item::def_body_item() : node() { }

DESTRUCTOR_INLINE
def_body_item::~def_body_item() { }

//=============================================================================
// class language_body methd definitions

CONSTRUCTOR_INLINE
language_body::language_body(token_keyword* t) : def_body_item(),
		tag(t) {
	if (t) assert(tag);
}

DESTRUCTOR_INLINE
language_body::~language_body() { SAFEDELETE(tag); }

language_body*
language_body::attach_tag(token_keyword* t) {
	// need to safe-delete first?  nah...
	tag = t;
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
namespace_body(token_keyword* s, token_identifier* n, terminal* l, 
	root_body* b, terminal* r, terminal* c) :
		root_item(),       
		ns(s), name(n), lb(l), body(b), rb(r), semi(c) {
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
const object*
namespace_body::
check_build(context* c) const {
	DEBUG(TRACE_CHECK_BUILD, 
		cerr << c->auto_indent() << "entering namespace: " << *name)
	// use context lookup: see if namespace already exists in super-scope
		// name_space* ns = c->lookup_namespace(name);
	// if so, open it up, and work with existing namespace
	// otherwise register a new namespace, add it to context
	c->open_namespace(*name);
	if (body)			// may be NULL, which means empty
		body->check_build(c);

	DEBUG(TRACE_CHECK_BUILD, 
		cerr << c->auto_indent() << "leaving namespace: " << *name)
	c->close_namespace();
	// if no errors, return pointer to the namespace just processed
	return c->top_namespace();
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
using_namespace(token_keyword* o, id_expr* i, token_char* s) :
		root_item(),
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
using_namespace(token_keyword* o, id_expr* i, token_keyword* a, 
	token_identifier* n, token_char* s) :
		root_item(),
		open(o), id(i), as(a), alias(n), semi(s) {
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
const object*
using_namespace::
check_build(context* c) const {
	if (alias) {
		DEBUG(TRACE_CHECK_BUILD, 
			cerr << c->auto_indent() << 
				"aliasing namespace: " << *id)
		c->alias_namespace(*id, *alias);
	} else {
		DEBUG(TRACE_CHECK_BUILD, 
			cerr << c->auto_indent() << 
				"using namespace: " << *id)
		// if aliased... print all, report as error (done inside)
		c->using_namespace(*id);
	}
	return c->top_namespace();
}

//=============================================================================
// class actuals_base method definitions

CONSTRUCTOR_INLINE
actuals_base::actuals_base(expr_list* a) : 
		instance_management(), actuals(a) {
	assert(actuals);
}

DESTRUCTOR_INLINE
actuals_base::~actuals_base() {
	SAFEDELETE(actuals);
}

//=============================================================================
// class instance_base method definitions

CONSTRUCTOR_INLINE
instance_base::instance_base(token_identifier* i) :
		instance_management(), id(i) {
	assert(id);
}

DESTRUCTOR_INLINE
instance_base::~instance_base() {
	SAFEDELETE(id);
}

ostream&
instance_base::what(ostream& o) const {
	return id->what(o << "(declaration-id): ");
}

line_position
instance_base::leftmost(void) const {
	return id->leftmost();
}

line_position
instance_base::rightmost(void) const {
	return id->rightmost();
}

/***
line_range
instance_base::where(void) const {
	return node::where();
}
***/

const object*
instance_base::check_build(context* c) const {
	DEBUG(TRACE_CHECK_BUILD, 
		what(cerr << c->auto_indent()) << ": ")
	c->add_type_instance(*id);		// ignored return value
	return c->top_namespace();
}

//=============================================================================
// class instance_array method definitions

CONSTRUCTOR_INLINE
instance_array::instance_array(token_identifier* i, range_list* rl) :
		instance_base(i), ranges(rl) {
	// ranges may be NULL, equivalent to declaration base
}

DESTRUCTOR_INLINE
instance_array::~instance_array() {
	SAFEDELETE(ranges);
}

ostream&
instance_array::what(ostream& o) const {
	return ranges->what(id->what(o << "(declaration-array): "));
}

line_position
instance_array::rightmost(void) const {
	return ranges->rightmost();
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
instance_declaration::instance_declaration(type_base* t, 
	instance_id_list* i, terminal* s) :
		instance_management(),
		type(t), ids(i), semi(s) {
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

const object*
instance_declaration::check_build(context* c) const {
	const object* t;
	DEBUG(TRACE_CHECK_BUILD, 
		what(cerr << c->auto_indent()) << ": ")
	t = type->check_build(c);
	if (t) {
		ids->check_build(c);
	} 
	c->unset_type_def();
//	return t;
	return c->top_namespace();
}

//=============================================================================
// class instance_connection method definitions

CONSTRUCTOR_INLINE
instance_connection::instance_connection(token_identifier* i, expr_list* a, 
		terminal* s) :
		instance_base(i), actuals_base(a), semi(s) {
	if (s) assert(semi);
}

DESTRUCTOR_INLINE
instance_connection::~instance_connection() {
	SAFEDELETE(semi);
}

// remember to check for declaration context when checking id

ostream&
instance_connection::what(ostream& o) const {
	return o << "(actuals-connection)";
}

line_position
instance_connection::leftmost(void) const {
	return instance_base::leftmost();
}

line_position
instance_connection::rightmost(void) const {
	if (semi) return semi->rightmost();
	else return actuals->rightmost();
}

/***
line_range
instance_connection::where(void) const {
	return node::where();
}
***/

const object*
instance_connection::check_build(context* c) const {
	return node::check_build(c);
}

//=============================================================================
// class connection_statement method definitions

CONSTRUCTOR_INLINE
connection_statement::connection_statement(expr* l, expr_list* a, 
		terminal* s) : actuals_base(a), lvalue(l), semi(s) {
	assert(lvalue);
	if (s) assert(semi);
}

DESTRUCTOR_INLINE
connection_statement::~connection_statement() {
	SAFEDELETE(lvalue);
	SAFEDELETE(semi);
}

ostream&
connection_statement::what(ostream& o) const {
	return o << "(connection-statement)";
}

line_position
connection_statement::leftmost(void) const {
	return lvalue->leftmost();
}

line_position
connection_statement::rightmost(void) const {
	if (semi) return semi->rightmost();
	else return actuals->rightmost();
}

//=============================================================================
// class instance_alias method definitions

CONSTRUCTOR_INLINE
instance_alias::instance_alias(token_identifier* i, alias_list* a, 
	terminal* s) :
		instance_base(i), aliases(a), semi(s) {
	assert(aliases);
	if (s) assert(semi);
}

DESTRUCTOR_INLINE
instance_alias::~instance_alias() {
	SAFEDELETE(aliases); SAFEDELETE(semi);
}

ostream&
instance_alias::what(ostream& o) const {
	return o << "(alias-assign)";
}

line_position
instance_alias::leftmost(void) const {
	return instance_base::leftmost();
}

line_position
instance_alias::rightmost(void) const {
	return semi->rightmost();
}

/***
line_range
instance_alias::where(void) const {
	return node::where();
}
***/

//=============================================================================
// class loop_instantiation method definitions

CONSTRUCTOR_INLINE
loop_instantiation::loop_instantiation(terminal* l, terminal* d, 
	token_identifier* i, terminal* c1, range* g, terminal* c2, 
	definition_body* b, terminal* r) :
		instance_management(),
		lp(l), delim(d), index(i), colon1(c1), 
		rng(g), colon2(c2), body(b), rp(r) {
	assert(lp); assert(delim); assert(index);
	assert(colon); assert(rng); assert(body); assert(lp);
}

DESTRUCTOR_INLINE
loop_instantiation::~loop_instantiation() {
	SAFEDELETE(lp); SAFEDELETE(delim); SAFEDELETE(index);
	SAFEDELETE(colon1); SAFEDELETE(rng); SAFEDELETE(colon2);
	SAFEDELETE(body); SAFEDELETE(rp);
}

ostream&
loop_instantiation::what(ostream& o) const {
	return o << "(loop-instance)";
}

line_position
loop_instantiation::leftmost(void) const {
	if (lp) return lp->leftmost();
	else return delim->leftmost();
}

line_position
loop_instantiation::rightmost(void) const {
	if (rp) return rp->rightmost();
	else if (body) return body->rightmost();
	else if (colon2) return colon2->rightmost();
	else return rng->rightmost();
}

//=============================================================================
// class port_formal_id method definitions

CONSTRUCTOR_INLINE
port_formal_id::port_formal_id(token_identifier* n, range_list* d) : node(),
		name(n), dim(d) {
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
port_formal_decl::port_formal_decl(type_base* t, port_formal_id_list* i) : 
		node(), type(t), ids(i) {
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

/**
	Constructor for a template formal identifier, the formal name for a 
	template parameter, which may be an array, declared with
	dimensions in brackets.  
	\param n the name of the template formal.  
	\param d is the (optional) dimension size expression.
 */
CONSTRUCTOR_INLINE
template_formal_id::template_formal_id(token_identifier* n, range_list* d) : 
		node(), name(n), dim(d) {
	assert(name);
// dim may be NULL
	if (d) assert(dim);
}

DESTRUCTOR_INLINE
template_formal_id::~template_formal_id() {
	SAFEDELETE(name); SAFEDELETE(dim);
}

ostream&
template_formal_id::what(ostream& o) const {
	name->what(o << "(template-formal-id): ");
	if (dim) dim->what(o << " with ");
	return o;
}

line_position
template_formal_id::leftmost(void) const {
	return name->leftmost();
}

line_position
template_formal_id::rightmost(void) const {
	if (dim) return dim->rightmost();
	else return name->rightmost();
}

const object*
template_formal_id::check_build(context* c) const {
	const object* o;
	const type_instantiation* t;
	// type should already be set in the context
	t = c->add_template_formal(*name);
	if (dim) {
		// attach array dimensions to current instantiation
		o = dim->check_build(c);
		assert(o);
	}
	return t;
}

//=============================================================================
// class template_formal_decl method definitions

CONSTRUCTOR_INLINE
template_formal_decl::template_formal_decl(type_base* t, 
	template_formal_id_list* i) :
		node(), type(t), ids(i) {
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

/**
	Type-checks a list of template formals with the same type.  
 */
const object*
template_formal_decl::check_build(context* c) const {
	const object* o;
	o = type->check_build(c);
	assert(o);
	ids->check_build(c);	// node_list::check_build: ignore return value
	c->unset_type_def();	// don't forget to unset!
	return o;
}

//=============================================================================
// class def_type_id method definitions

CONSTRUCTOR_INLINE
def_type_id::def_type_id(token_identifier* n, template_formal_decl_list* t) : 
		type_base(), name(n), temp_spec(t) {
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

const token_identifier&
def_type_id::get_name(void) const {
	assert(name);
	return *name;
}

const template_formal_decl_list*
def_type_id::get_template_formals(void) const {
	return temp_spec;
}

const object*
def_type_id::check_build(context* c) const {
	const object* o;
//	const type_definition* t;
	assert(name);
// don't check name again, should already be checked by process_proto, etc...
//	t = c->set_type_def(*name);
//	assert(t);
	if (temp_spec) {
		o = temp_spec->check_build(c);
		assert(o);
	}
	return NULL;
}

//=============================================================================
// class definition method definitions

CONSTRUCTOR_INLINE
definition::definition() : root_item() {
}

DESTRUCTOR_INLINE
definition::~definition() {
}

//=============================================================================
// class process_signature method definitions

CONSTRUCTOR_INLINE
process_signature::process_signature(token_keyword* d, def_type_id* i, 
	port_formal_decl_list* p) :
		node(), def(d), idt(i), ports(p) {
	assert(def); assert(idt); assert(ports);
}

DESTRUCTOR_INLINE
process_signature::~process_signature() {
	SAFEDELETE(def); SAFEDELETE(idt); SAFEDELETE(ports);
}

const token_identifier&
process_signature::get_name(void) const {
	return idt->get_name();
}

const template_formal_decl_list*
process_signature::get_template_formals(void) const {
	return idt->get_template_formals();
}

const port_formal_decl_list*
process_signature::get_port_formals(void) const {
	return ports;
}

//=============================================================================
// class process_prototype method definitions

CONSTRUCTOR_INLINE
process_prototype::process_prototype(token_keyword* d, def_type_id* i, 
	port_formal_decl_list* p, token_char* s) :
		prototype(),
		process_signature(d, i, p), 
		semi(s) {
	assert(semi);
}

DESTRUCTOR_INLINE
process_prototype::~process_prototype() {
	SAFEDELETE(semi);
}

ostream&
process_prototype::what(ostream& o) const {
	return o << "(process-prototype)";
}

line_position
process_prototype::leftmost(void) const {
	return def->leftmost();
}

line_position
process_prototype::rightmost(void) const {
	return semi->rightmost();
}

const object*
process_prototype::check_build(context* c) const {
	const object* o;
	DEBUG(TRACE_CHECK_BUILD, 
		idt->what(cerr << c->auto_indent() << "process prototype: "))
	c->declare_process(get_name());		// will handle errors
	o = idt->check_build(c);		// always returns NULL
	o = ports->check_build(c);		// ignore return value
	c->unset_type_def();			// unset port type
	c->close_process();
	// nothing better to do
	return c->top_namespace();
}

//=============================================================================
// class process_def method definitions

CONSTRUCTOR_INLINE
process_def::process_def(token_keyword* d, def_type_id* i, 
	port_formal_decl_list* p, definition_body* b) :
		definition(),
		process_signature(d, i, p), 
		body(b) {
	assert(body);		// body may be empty, is is not NULL
}

DESTRUCTOR_INLINE
process_def::~process_def() {
	SAFEDELETE(body);
}

ostream&
process_def::what(ostream& o) const {
	return o << "(process-definition)";
}

line_position
process_def::leftmost(void) const {
	return def->leftmost();
}

line_position
process_def::rightmost(void) const {
	return body->rightmost();
}

/*** unveil later...
const object*
process_def::check_build(context* c) const {
	const object* o;
	DEBUG(TRACE_CHECK_BUILD, 
		idt->what(cerr << c->auto_indent() << "process prototype: "))
	c->open_process(get_name());		// will handle errors
	o = idt->check_build(c);
	assert(o);
	o = ports->check_build(c);
	assert(o);
	o = body->check_build(c);
	assert(o);
	c->close_process();
	// nothing better to do
	return c->top_namespace();
}
***/


//=============================================================================
// class guarded_definition_body method definitions

CONSTRUCTOR_INLINE
guarded_definition_body::guarded_definition_body(expr* e, terminal* a, 
	definition_body* b) :
		instance_management(), guard(e), arrow(a), body(b) {
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
conditional_instantiation::conditional_instantiation(
	guarded_definition_body_list* n) :
		instance_management(), gd(n) {
	assert(gd);
}

DESTRUCTOR_INLINE
conditional_instantiation::~conditional_instantiation() {
	SAFEDELETE(gd);
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
// abstract class prototype method definitions

inline
prototype::prototype() : root_item() { }

DESTRUCTOR_INLINE
prototype::~prototype() { }


//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes
							// also known as...
template class node_list<root_item>;			// root_body
template class node_list<data_type_base,comma>;		// base_data_type_list
template class node_list<def_body_item>;		// definition_body
template class node_list<instance_base,comma>;		// instance_id_list
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
};	// end namespace parser
};	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE


