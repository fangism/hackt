// "art_parser_formal.cc"
// class method definitions for ART::parser
// for formal-related classes

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

#include "art_parser_formal.h"
#include "art_parser_expr.h"
#include "art_parser_token.h"

#include "art_symbol_table.h"
#include "art_object_definition.h"
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
// class data_param_id method definitions

data_param_id::data_param_id(const token_identifier* i, 
		const dense_range_list* d) :
		node(), id(i), dim(d) {
	assert(id);
	// dim is optional
}

data_param_id::~data_param_id() {
}

ostream&
data_param_id::what(ostream& o) const {
	return o << "(data-param-id)";
}

line_position
data_param_id::leftmost(void) const {
	return id->leftmost();
}

line_position
data_param_id::rightmost(void) const {
	if (dim)	return dim->rightmost();
	else		return id->rightmost();
}

//=============================================================================
// class data_param_id_list method definitions

data_param_id_list::data_param_id_list(const data_param_id* d) : parent(d) { }

data_param_id_list::~data_param_id_list() { }

//=============================================================================
// class data_param_decl method definitions

data_param_decl::data_param_decl(const concrete_type_ref* t, 
		const data_param_id_list* il) :
		node(), type(t), ids(il) {
	assert(type);
	assert(ids);
}

data_param_decl::~data_param_decl() {
}

ostream&
data_param_decl::what(ostream& o) const {
	return o << "(data-param-decl)";
}

line_position
data_param_decl::leftmost(void) const {
	return type->leftmost();
}

line_position
data_param_decl::rightmost(void) const {
	return ids->rightmost();
}

//=============================================================================
// class data_param_decl_list method definitions

data_param_decl_list::data_param_decl_list(const data_param_decl* d) :
		parent(d) { }

data_param_decl_list::~data_param_decl_list() { }

//=============================================================================
// class port_formal_id method definitions

CONSTRUCTOR_INLINE
port_formal_id::port_formal_id(const token_identifier* n,
		const dense_range_list* d)
		: node(), name(n), dim(d) {
	assert(name);
	// dim may be NULL
}

DESTRUCTOR_INLINE
port_formal_id::~port_formal_id() {
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

/**
	Should be very similar to instance_base's check_build.  
	// there should be some open definition already
	// type should already be set in the context
	TO DO:
	\return pointer to newly added formal instance.  
 */
never_const_ptr<object>
port_formal_id::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	never_const_ptr<instantiation_base> t;
		// should be anything but param_instantiation
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent()) <<
			"port_formal_id::check_build(...): ";
	)

	if (dim) {
		dim->check_build(c);	// useless return value
		count_ptr<object> o(c->pop_top_object_stack());
		if (!o) {
			cerr << "ERROR in array dimensions " <<
				dim->where() << endl;
			exit(1);
		}
		count_ptr<range_expr_list> d(o.is_a<range_expr_list>());
		assert(d);
		// attach array dimensions to current instantiation
		t = c->add_port_formal(*name, d);
	} else {
		t = c->add_port_formal(*name);
	}
	return t;
}

//=============================================================================
// class port_formal_id_list method definitions

port_formal_id_list::port_formal_id_list(const port_formal_id* p) :
		parent(p) { }

port_formal_id_list::~port_formal_id_list() { }

//=============================================================================
// class port_formal_decl method definitions

CONSTRUCTOR_INLINE
port_formal_decl::port_formal_decl(const concrete_type_ref* t, 
		const port_formal_id_list* i) : 
		node(), type(t), ids(i) {
	assert(type); assert(ids);
}

DESTRUCTOR_INLINE
port_formal_decl::~port_formal_decl() {
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

/**
	Very similar to instance_declaration::check_build.  
	\return pointer to the conrete-type used for instantiation
		if there were no problems, else return NULL.  
 */
never_const_ptr<object>
port_formal_decl::check_build(never_ptr<context> c) const {
	never_const_ptr<object> t;
	t = type->check_build(c);
		// should set the current_fundamental_type in context
	c->reset_current_definition_reference();
		// no longer need the base definition
	if (t) {
		ids->check_build(c);
	} else {
		cerr << "ERROR with concrete-type in port formal decl. at "
			<< type->where() << endl;
		return never_const_ptr<object>(NULL);
	}
	c->reset_current_fundamental_type();
	return t;
}

//=============================================================================
// class port_formal_decl_list method definitions

port_formal_decl_list::port_formal_decl_list() : parent() { }

port_formal_decl_list::port_formal_decl_list(const port_formal_decl* p) :
		parent(p) { }

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
		const dense_range_list* d, const token_char* e, 
		const expr* v) : 
		node(), name(n), dim(d), eq(e), dflt(v) {
	assert(name);
// dim may be NULL
	if (eq) assert(dflt);
}

/**
	
 */

DESTRUCTOR_INLINE
template_formal_id::~template_formal_id() {
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
never_const_ptr<object>
template_formal_id::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	never_const_ptr<instantiation_base> t;
		// should be param_instantiation
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent()) <<
			"template_formal_id::check_build(...): ";
	)
	// there should be some open definition already
	// type should already be set in the context
	if (dim) {
		// attach array dimensions to current instantiation
		dim->check_build(c);	// useless return value, check stack
		// should already construct an range_expr_list
		count_ptr<object> o(c->pop_top_object_stack());
		if (!o) {
			cerr << "ERROR in array dimensions " <<
				dim->where() << endl;
			exit(1);
		}
		count_ptr<range_expr_list> d(o.is_a<range_expr_list>());
		assert(d);
		t = c->add_template_formal(*name, d);
	} else {
		t = c->add_template_formal(*name);
	}
	return t;
//	return never_const_ptr<object>(t);
}

//=============================================================================
// class template_formal_id_list method definitions

template_formal_id_list::template_formal_id_list(const template_formal_id* t) :
		parent(t) { }

template_formal_id_list::~template_formal_id_list() { }

//=============================================================================
// class template_formal_decl method definitions

CONSTRUCTOR_INLINE
template_formal_decl::template_formal_decl(
		const token_paramtype* t, 	// why not concrete_type_ref?
		const template_formal_id_list* i) :
		node(), type(t), ids(i) {
	assert(type); assert(ids);
}

DESTRUCTOR_INLINE
template_formal_decl::~template_formal_decl() {
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
	Adds formal parameters to the context's current_prototype.  
 */
never_const_ptr<object>
template_formal_decl::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent()) <<
			"template_formal_decl::check_build(...): ";
	)
	o = type->check_build(c);	// sets_current_definition_reference
	c->set_current_fundamental_type();
		// built-in param types pint and pbool
		// have no template parameters...
		// after type is upgraded to a concrete_type_ref
		// then it will already be set by its check_build()
	assert(o);
	ids->check_build(c);	// node_list::check_build: ignore return value
	c->reset_current_fundamental_type();
	return o;
}

//=============================================================================
// class template_formal_decl_list method definitions

template_formal_decl_list::template_formal_decl_list(
		const template_formal_decl* t) :
		parent(t) { }

template_formal_decl_list::~template_formal_decl_list() { }

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE


