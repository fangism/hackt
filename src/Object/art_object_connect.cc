// "art_object_connect.cc"
// for the instance_reference family of objects

#include <iostream>

#include "art_parser_debug.h"		// need this?
#include "art_parser_base.h"
#include "art_object_connect.h"
#include "art_object_expr.h"
#include "art_object_IO.tcc"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// class param_expression_assignment method definitions
	
param_expression_assignment::param_expression_assignment() :
		object(), instance_management_base(), ex_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_expression_assignment::~param_expression_assignment() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
param_expression_assignment::what(ostream& o) const {
	return o << "param-expr-assignment";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
param_expression_assignment::dump(ostream& o) const {
	assert(ex_list.size() > 1);
	ex_list_type::const_iterator iter = ex_list.begin();
	const ex_list_type::const_iterator end = ex_list.end();
	assert(*iter);
	(*iter)->dump(o);
	for (iter++ ; iter!=end; iter++) {
		assert(*iter);
		(*iter)->dump(o << " = ");
	}
	return o << ';';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_expression_assignment::append_param_expression(
		count_const_ptr<param_expr> e) {
	ex_list.push_back(e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_expression_assignment::prepend_param_expression(
		count_const_ptr<param_expr> e) {
	ex_list.push_front(e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_expression_assignment::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PARAM_EXPR_ASSIGNMENT_TYPE)) {
#if 0
	for_each(ex_list.begin(), ex_list.end(), 
	bind2nd(mem_fun(&ex_list_type::value_type::collect_transient_info), m)
	);
#else
	ex_list_type::const_iterator iter = ex_list.begin();
	const ex_list_type::const_iterator end = ex_list.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
#endif
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
param_expression_assignment::construct_empty(void) {
	return new param_expression_assignment();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_expression_assignment::write_object(persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer_list(f, ex_list);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_expression_assignment::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer_list(f, ex_list);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_expression_assignment::unroll_params(void) const {
	// first make sure rhs is initialized
	// what if rhs expression refers to not-yet-unroll instantiation?
	// need to follow dependencies
	// need a method to search for writers to 

	const ex_list_type::const_iterator last = --ex_list.end();
	const size_t dim = (*last)->dimensions();
	// Evaluate:
	// is the reference valid?
	// is it initialized?
	// what type, what dimension?
	// may be collective, if so, must be dense.  

#if 0
	switch (dim) {
	case 0:
		param_const foo = (*last)->evaluate();

		// Dispatch dimension-specific unrolling sub-routine?

		ex_list_type::iterator iter = ex_list.begin();
		for ( ; iter!=last; iter++) {
			assign
		}
	}
#endif
}

//=============================================================================
// class instance_reference_connection method definitions

instance_reference_connection::instance_reference_connection() :
		object(), instance_management_base(), inst_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i instance reference to connect, may not be NULL.
 */
void
instance_reference_connection::append_instance_reference(
		count_const_ptr<instance_reference_base> i) {
	assert(i);
	inst_list.push_back(i);
}

//=============================================================================
// class aliases_connection method definitions

aliases_connection::aliases_connection() : instance_reference_connection() { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
aliases_connection::what(ostream& o) const {
	return o << "alias-connection";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
aliases_connection::dump(ostream& o) const {
	assert(inst_list.size() > 1);
	inst_list_type::const_iterator iter = inst_list.begin();
	const inst_list_type::const_iterator end = inst_list.end();
	assert(*iter);
	(*iter)->dump(o);
	for (iter++ ; iter!=end; iter++) {
		assert(*iter);
		(*iter)->dump(o << " = ");
	}
	return o << ';';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection::prepend_instance_reference(
		count_const_ptr<instance_reference_base> i) {
	assert(i);
	inst_list.push_front(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, ALIAS_CONNECTION_TYPE)) {
	inst_list_type::const_iterator iter = inst_list.begin();
	const inst_list_type::const_iterator end = inst_list.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
aliases_connection::construct_empty(void) {
	return new aliases_connection();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection::write_object(persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer_list(f, inst_list);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer_list(f, inst_list);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// class port_connection method definitions

/**
	Private empty constructor.
 */
port_connection::port_connection() :
		instance_reference_connection(), inst(NULL) {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initial constructor for a port-connection.  
	\param i an instance of the definition that is to be connected.
 */
port_connection::port_connection(
		count_const_ptr<simple_instance_reference> i) :
		instance_reference_connection(), inst(i) {
	assert(inst);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_connection::what(ostream& o) const {
	return o << "port-connection";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_connection::dump(ostream& o) const {
	assert(inst);
	inst->dump(o) << " (";

	if (!inst_list.empty()) {
		inst_list_type::const_iterator iter = inst_list.begin();
		const inst_list_type::const_iterator end = inst_list.end();
		if (*iter)
			(*iter)->dump(o);
		else o << " ";
		for (iter++ ; iter!=end; iter++) {
			o << ", ";
			if (*iter)
				(*iter)->dump(o);
		}
	}
	return o << ");";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i instance reference to connect, may be NULL.
 */
void
port_connection::append_instance_reference(
		count_const_ptr<instance_reference_base> i) {
	// do not assert, may be NULL.  
	inst_list.push_back(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PORT_CONNECTION_TYPE)) {
	assert(inst);
	inst->collect_transient_info(m);
	inst_list_type::const_iterator iter = inst_list.begin();
	const inst_list_type::const_iterator end = inst_list.end();
	for ( ; iter!=end; iter++) {
		// port connection arguments may be NULL
		if (*iter)
			(*iter)->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
port_connection::construct_empty(void) {
	return new port_connection();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::write_object(persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst);
	m.write_pointer_list(f, inst_list);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, inst);
	m.read_pointer_list(f, inst_list);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
#if 0
NOT READY TO UNVEIL
// class dynamic_connection_assignment method definitions

dynamic_connection_assignment::dynamic_connection_assignment(
		never_const_ptr<scopespace> s) :
		connection_assignment_base(), dscope(s) {
	// check that dscope is actually a loop or conditional
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
//=============================================================================
}	// end namespace entity
}	// end namespace ART

