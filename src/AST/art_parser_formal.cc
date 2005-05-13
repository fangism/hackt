/**
	\file "AST/art_parser_formal.cc"
	Class method definitions for ART::parser for formal-related classes.
	$Id: art_parser_formal.cc,v 1.18.2.2 2005/05/13 20:04:12 fang Exp $
 */

#ifndef	__AST_ART_PARSER_FORMAL_CC__
#define	__AST_ART_PARSER_FORMAL_CC__

#define	ENABLE_STACKTRACE			0

// rule-of-thumb for inline directives:
// only inline constructors if you KNOW that they will not be be needed
// outside of this module, because we don't have a means to export
// inline methods other than defining in the header or using
// -fkeep-inline-functions

#include <exception>
#include <iostream>

#include "AST/art_parser_formal.h"
#include "AST/art_parser_expr_base.h"
#include "AST/art_parser_range_list.h"
#include "AST/art_parser_token.h"
#include "AST/art_parser_token_char.h"
#include "AST/art_parser_type.h"
#include "AST/art_parser_node_list.tcc"

#include "Object/art_context.h"
#include "Object/art_object_type_ref_base.h"
#include "Object/art_object_instance_base.h"
#include "Object/art_object_definition.h"
#include "Object/art_object_expr_base.h"

#include "util/what.h"
#include "util/stacktrace.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(ART::parser::data_param_id, "(data-param-id)")
SPECIALIZE_UTIL_WHAT(ART::parser::data_param_decl, "(data-param-decl)")
SPECIALIZE_UTIL_WHAT(ART::parser::port_formal_id, "(port-formal-id)")
SPECIALIZE_UTIL_WHAT(ART::parser::port_formal_decl, "(port-formal-decl)")
SPECIALIZE_UTIL_WHAT(ART::parser::template_formal_id, "(template-formal-id)")
SPECIALIZE_UTIL_WHAT(ART::parser::template_formal_decl, "(template-formal-decl)")
SPECIALIZE_UTIL_WHAT(ART::parser::template_formal_decl_list_pair, "(template-formal-decl-list-pair)")
}

//=============================================================================
namespace ART {
namespace parser {
#include "util/using_ostream.h"
USING_STACKTRACE

//=============================================================================
// class data_param_id method definitions

data_param_id::data_param_id(const token_identifier* i, 
		const dense_range_list* d) : id(i), dim(d) {
	NEVER_NULL(id);
	// dim is optional
}

data_param_id::~data_param_id() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(data_param_id)

line_position
data_param_id::leftmost(void) const {
	return id->leftmost();
}

line_position
data_param_id::rightmost(void) const {
	if (dim)	return dim->rightmost();
	else		return id->rightmost();
}

never_ptr<const object>
data_param_id::check_build(context& c) const {
	cerr << "Fang, finish data_param_id::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}


//=============================================================================
// class data_param_id_list method definitions

data_param_id_list::data_param_id_list(const data_param_id* d) :
		parent_type(d) { }

data_param_id_list::~data_param_id_list() { }

//=============================================================================
// class data_param_decl method definitions

data_param_decl::data_param_decl(const concrete_type_ref* t, 
		const data_param_id_list* il) : type(t), ids(il) {
	NEVER_NULL(type);
	NEVER_NULL(ids);
}

data_param_decl::~data_param_decl() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(data_param_decl)

line_position
data_param_decl::leftmost(void) const {
	return type->leftmost();
}

line_position
data_param_decl::rightmost(void) const {
	return ids->rightmost();
}

never_ptr<const object>
data_param_decl::check_build(context& c) const {
	cerr << "Fang, finish data_param_decl::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class data_param_decl_list method definitions

data_param_decl_list::data_param_decl_list(const data_param_decl* d) :
		parent_type(d) { }

data_param_decl_list::~data_param_decl_list() { }

//=============================================================================
// class port_formal_id method definitions

CONSTRUCTOR_INLINE
port_formal_id::port_formal_id(const token_identifier* n,
		const dense_range_list* d) : name(n), dim(d) {
	NEVER_NULL(name);
	// dim may be NULL
}

DESTRUCTOR_INLINE
port_formal_id::~port_formal_id() {
}

ostream&
port_formal_id::what(ostream& o) const {
#if 0
	name->what(o << "(port-formal-id): ");
#else
	name->what(o << util::what<port_formal_id>::name() << ": ");
#endif
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

/**
	Should be very similar to instance_base's check_build.  
	// there should be some open definition already
	// type should already be set in the context
	TO DO:
	\return pointer to newly added formal instance.  
 */
never_ptr<const object>
port_formal_id::check_build(context& c) const {
	STACKTRACE("port_formal_id::check_build()");
	never_ptr<const instance_collection_base> t;
		// should be anything but param_instantiation

	if (dim) {
#if 0
		dim->check_build(c);	// useless return value
		const count_ptr<object> o(c.pop_top_object_stack());
		if (!o) {
			cerr << "ERROR in array dimensions " <<
				where(*dim) << endl;
			THROW_EXIT;
		}
		const count_ptr<const range_expr_list>
			d(o.is_a<const range_expr_list>());
		NEVER_NULL(d);
		// attach array dimensions to current instantiation
#else
		const dense_range_list::return_type
			d(dim->check_formal_dense_ranges(c));
		if (!d) {
			cerr << "ERROR in array dimensions " <<
				where(*dim) << endl;
			THROW_EXIT;
		}
#endif
		t = c.add_port_formal(*name, d);
	} else {
		t = c.add_port_formal(*name);
	}
	return t;
}

//=============================================================================
// class port_formal_id_list method definitions

port_formal_id_list::port_formal_id_list(const port_formal_id* p) :
		parent_type(p) { }

port_formal_id_list::~port_formal_id_list() { }

//=============================================================================
// class port_formal_decl method definitions

CONSTRUCTOR_INLINE
port_formal_decl::port_formal_decl(const concrete_type_ref* t, 
		const port_formal_id_list* i) : type(t), ids(i) {
	NEVER_NULL(type); NEVER_NULL(ids);
}

DESTRUCTOR_INLINE
port_formal_decl::~port_formal_decl() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(port_formal_decl)

line_position
port_formal_decl::leftmost(void) const {
	return type->leftmost();
}

line_position
port_formal_decl::rightmost(void) const {
	return ids->rightmost();
}

/**
	Very similar to instance_declaration::check_build.  
	\return just NULL.  
	used to return pointer to the conrete-type used for instantiation
		if there were no problems, else return NULL.  
 */
never_ptr<const object>
port_formal_decl::check_build(context& c) const {
	typedef	never_ptr<const object>		return_type;
	STACKTRACE("port_formal_decl::check_build()");
	type->check_build(c);
	// useless return value
		// should set the current_fundamental_type in context
	const count_ptr<const fundamental_type_reference>
		ftr(c.get_current_fundamental_type());
	c.pop_current_definition_reference();
		// no longer need the base definition
	if (ftr) {
		ids->check_build(c);
		// always returns NULL
		// error catching?
	} else {
		cerr << "ERROR with concrete-type in port formal decl. at "
			<< where(*type) << endl;
		THROW_EXIT;
	}
	c.reset_current_fundamental_type();
	return return_type(NULL);
}

//=============================================================================
// class port_formal_decl_list method definitions

port_formal_decl_list::port_formal_decl_list() : parent_type() { }

port_formal_decl_list::port_formal_decl_list(const port_formal_decl* p) :
		parent_type(p) { }

port_formal_decl_list::~port_formal_decl_list() { }

//=============================================================================
// class template_formal_id method definitions

/**
	Constructor for a template formal identifier, the formal name for a 
	template parameter, which may be an array, declared with
	dimensions in brackets.  
	\param n the name of the template formal.  
	\param d is the (optional) dimension size expression.
	\param e is an '=' token.  
	\param v is the optional default value expression.  
 */
CONSTRUCTOR_INLINE
template_formal_id::template_formal_id(const token_identifier* n, 
		const dense_range_list* d, const char_punctuation_type* e, 
		const expr* v) : 
		name(n), dim(d), eq(e), dflt(v) {
	NEVER_NULL(name);
	// dim may be NULL
	if (eq) NEVER_NULL(dflt);
}

/**
	
 */

DESTRUCTOR_INLINE
template_formal_id::~template_formal_id() {
}

ostream&
template_formal_id::what(ostream& o) const {
#if 0
	name->what(o << "(template-formal-id): ");
#else
	name->what(o << util::what<template_formal_id>::name() << ": ");
#endif
	if (dim) dim->what(o << " with ");
	return o;
}

line_position
template_formal_id::leftmost(void) const {
	return name->leftmost();
}

line_position
template_formal_id::rightmost(void) const {
	if (dflt)	return dflt->rightmost();
	else if (eq)	return eq->rightmost();
	else if (dim)	return dim->rightmost();
	else return	name->rightmost();
}

/**
	Should be very similar to instance_base's check_build.  
	TO DO: register default value in building
	See also make_dense_range_list.  
 */
never_ptr<const object>
template_formal_id::check_build(context& c) const {
	STACKTRACE("template_formal_id::check_build()");
	never_ptr<const instance_collection_base> t;
		// should be param_instantiation
	// there should be some open definition already
	// type should already be set in the context
	count_ptr<const param_expr> default_val;
	if (dflt) {
#if 0
		dflt->check_build(c);
		const count_ptr<object> o(c.pop_top_object_stack());
		if (!o) {
			cerr << "ERROR in default value expression " <<
				where(*dflt) << endl;
			THROW_EXIT;
		}
		const count_ptr<const param_expr>
			p(o.is_a<const param_expr>());
		NEVER_NULL(p);
#else
		count_ptr<const param_expr> p(dflt->check_expr(c));
		if (!p) {
			cerr << "ERROR in default value expression " <<
				where(*dflt) << endl;
			THROW_EXIT;
		}
#endif
		default_val = p;
	}
	if (dim) {
		// attach array dimensions to current instantiation
#if 0
		dim->check_build(c);	// useless return value, check stack
		// should already construct an range_expr_list
		const count_ptr<object> o(c.pop_top_object_stack());
		if (!o) {
			cerr << "ERROR in array dimensions " <<
				where(*dim) << endl;
			THROW_EXIT;
		}
		const count_ptr<const range_expr_list>
			d(o.is_a<const range_expr_list>());
		NEVER_NULL(d);
#else
		const dense_range_list::return_type
			d(dim->check_formal_dense_ranges(c));
		if (!d) {
			cerr << "ERROR in array dimensions " <<
				where(*dim) << endl;
			THROW_EXIT;
		}
#endif
		t = c.add_template_formal(*name, d, default_val);
	} else {
		t = c.add_template_formal(*name, default_val);
	}
	return t;
}

//=============================================================================
// class template_formal_id_list method definitions

template_formal_id_list::template_formal_id_list(const template_formal_id* t) :
		parent_type(t) { }

template_formal_id_list::~template_formal_id_list() { }

//=============================================================================
// class template_formal_decl method definitions

CONSTRUCTOR_INLINE
template_formal_decl::template_formal_decl(
		const token_paramtype* t, 	// why not concrete_type_ref?
		const template_formal_id_list* i) :
		type(t), ids(i) {
	NEVER_NULL(type); NEVER_NULL(ids);
}

DESTRUCTOR_INLINE
template_formal_decl::~template_formal_decl() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(template_formal_decl)

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
	Adds formal parameters to the context's current_prototype.  
	\return the definition found, not particularly useful.
 */
never_ptr<const object>
template_formal_decl::check_build(context& c) const {
	STACKTRACE("template_formal_decl::check_build()");
	type->check_build(c);	// sets_current_definition_reference
	// useless return value, always NULL
	const never_ptr<const definition_base>
		def(c.get_current_definition_reference());
	if (!def) {
		cerr << "ERROR resolving base definition!  " <<
			where(*type) << endl;
		THROW_EXIT;
	}
	c.set_current_fundamental_type(def->make_fundamental_type_reference());
		// don't anticipate any problems here...
		// built-in param types pint and pbool
		// have no template parameters...
		// after type is upgraded to a concrete_type_ref
		// then it will already be set by its check_build()
	ids->check_build(c);	// node_list::check_build: ignore return value
	// catch return errors?
	c.reset_current_fundamental_type();
	return def;
}

//=============================================================================
// class template_formal_decl_list method definitions

template_formal_decl_list::template_formal_decl_list() : parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template_formal_decl_list::template_formal_decl_list(
		const template_formal_decl* t) :
		parent_type(t) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template_formal_decl_list::~template_formal_decl_list() { }

//=============================================================================
// struct template_formal_decl_list_pair method definitions

/**
	\param s the set of strictly matched template parameters, 
		may be empty, but not NULL.
	\param r the set of relaxed template parameters, may be NULL.
 */
template_formal_decl_list_pair::template_formal_decl_list_pair(
		const template_formal_decl_list* s,
		const template_formal_decl_list* r) :
		first(s), second(r) {
	NEVER_NULL(first);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// Default destructor
template_formal_decl_list_pair::~template_formal_decl_list_pair() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARSER_WHAT_DEFAULT_IMPLEMENTATION(template_formal_decl_list_pair)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
template_formal_decl_list_pair::leftmost(void) const {
	return first->leftmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
template_formal_decl_list_pair::rightmost(void) const {
	if (second)	return second->rightmost();
	else		return first->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: implement for real.
	Sequentially check each template formal.  
	Need to distinguish between strict and relaxed parameters for the sake
	of creating the appropriate fundamental type reference?
	DEPENDS: on updating ART::entity::type_reference_base.
 */
never_ptr<const object>
template_formal_decl_list_pair::check_build(context& c) const {
	never_ptr<const object> ret;
	c.strict_template_parameters();
	ret = first->check_build(c);
	if (second) {
		c.relaxed_template_parameters();
		return second->check_build(c);
	} else {
		return ret;
	}
}

//=============================================================================
// explicit class template instantiations

template class node_list<const data_param_id>;
template class node_list<const data_param_decl>;
template class node_list<const port_formal_id>;
template class node_list<const port_formal_decl>;
template class node_list<const template_formal_id>;
template class node_list<const template_formal_decl>;

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __AST_ART_PARSER_FORMAL_CC__

