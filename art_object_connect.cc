// "art_object_connect.cc"
// for the instance_reference family of objects

#include <iostream>

// #include "multidimensional_sparse_set.h"

#include "art_parser_debug.h"		// need this?
#include "art_parser_base.h"
#include "art_object_connect.h"
#include "art_object_expr.h"
// #include "art_built_ins.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// class param_expression_assignment method definitions
	
param_expression_assignment::param_expression_assignment() :
		connection_assignment_base(), ex_list() {
}

param_expression_assignment::~param_expression_assignment() { }

void
param_expression_assignment::append_param_expression(
		count_const_ptr<param_expr> e) {
	ex_list.push_back(e);
}

void
param_expression_assignment::prepend_param_expression(
		count_const_ptr<param_expr> e) {
	ex_list.push_front(e);
}

//=============================================================================
// class instance_reference_connection method definitions

instance_reference_connection::instance_reference_connection() :
		connection_assignment_base(), inst_list() {
}

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

void
aliases_connection::prepend_instance_reference(
		count_const_ptr<instance_reference_base> i) {
	assert(i);
	inst_list.push_front(i);
}

//=============================================================================
// class port_connection method definitions

/**
	Initial constructor for a port-connection.  
	\param i an instance of the definition that is to be connected.
 */
port_connection::port_connection(
		count_const_ptr<simple_instance_reference> i) :
		instance_reference_connection(), inst(i) {
}

/**
	\param i instance reference to connect, may be NULL.
 */
void
port_connection::append_instance_reference(
		count_const_ptr<instance_reference_base> i) {
	// do not assert, may be NULL.  
	inst_list.push_back(i);
}

//=============================================================================
// class dynamic_connection_assignment method definitions

dynamic_connection_assignment::dynamic_connection_assignment(
		never_const_ptr<scopespace> s) :
		connection_assignment_base(), dscope(s) {
	// check that dscope is actually a loop or conditional
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

