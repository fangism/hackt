// "art_parser.cc"
// class method definitions for ART::parser

// template instantiations are at the end of the file

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
// method defitions for node

/**
	Default type-checker and object builder does nothing.  
	Should be re-implemented in all terminal subclasses.  
 */
object*
node::check_build(context* c) const {
	cerr << "check_build() not fully-implmented yet." << endl;
	return NULL;
}


//=============================================================================
// node_list<> method definitions
// had trouble finding reference to template functions defined here...?
// template exporting not implemented in any gcc compiler yet...

// thus all definitions must be in header
// this way, PRS and HSE may use them

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
		body(dynamic_cast<token_identifier*>(b)),     
		rb(dynamic_cast<terminal*>(r)),
		semi(dynamic_cast<terminal*>(c)) {
	assert(ns); assert(name); assert(lb);
	// body may be NULL
	assert(rb); assert(semi);
}

DESTRUCTOR_INLINE
namespace_body::
~namespace_body() {
	SAFEDELETE(ns); SAFEDELETE(name); SAFEDELETE(lb);
	SAFEDELETE(body); SAFEDELETE(rb); SAFEDELETE(semi);
}


// recursive type-checker
object*
namespace_body::
check_build(context* c) const {
	cerr << "entering namespace: " << *name << endl;
	// use context lookup: see if namespace already exists in super-scope
		// name_space* ns = c->lookup_namespace(name);
	// if so, open it up, and work with existing namespace
	// otherwise register a new namespace, add it to context

	if (body)			// may be NULL, which means empty
		body->check_build(c);

	cerr << "leaving namespace: " << *name << endl;
	// if no errors, return pointer to the namespace just processed
	return NULL;
}


//=============================================================================
// class using_namespace method definitions

/**
	Constructor for using_namespace directive.  
	\param o the "open" keyword.  
	\param i the id_expr qualified identifier.  
	\param a the "as" keyword (optional).  
	\param n the alias name (optional).  
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
		semi(dynamic_cast<token_string*>(s)) {
	assert(open); assert(id);
	if (a && !as) delete a;
	if (n && !alias) delete n;
}

/// default destructor
DESTRUCTOR_INLINE
using_namespace::~using_namespace() {
	SAFEDELETE(open); SAFEDELETE(id);
	SAFEDELETE(as); SAFEDELETE(alias);
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

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes
							// also known as...
template class node_list<root_item>;			// root_body
template class node_list<expr,comma>;		// expr_list
template class node_list<token_identifier,scope>;	// id_expr
template class node_list<range,comma>;		// range_list
template class node_list<data_type_base,comma>;	// base_data_type_list
template class node_list<def_body_item>;		// definition_body
template class node_list<declaration_base,comma>;	// declaration_id_list
template 
class node_list<instance_declaration,semicolon>;	// data_param_list
template class node_list<port_formal_id,comma>;	// port_formal_id_list
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


