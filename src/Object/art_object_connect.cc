/**
	\file "art_object_connect.cc"
	Method definitions pertaining to connections and assignments.  
 	$Id: art_object_connect.cc,v 1.17 2005/01/13 05:28:28 fang Exp $
 */

#ifndef	__ART_OBJECT_CONNECT_CC__
#define	__ART_OBJECT_CONNECT_CC__

#include <iostream>

#include "art_object_connect.h"
#include "art_object_inst_ref_base.h"

#include "STL/list.tcc"
#include "persistent_object_manager.tcc"
#include "art_object_type_hash.h"

//=============================================================================
namespace ART {
namespace entity {
USING_IO_UTILS
#include "using_ostream.h"

//=============================================================================
// class instance_reference_connection method definitions

instance_reference_connection::instance_reference_connection() :
		object(), instance_management_base(), inst_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instance_reference_connection::~instance_reference_connection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes an instance reference connection with the
	first instance reference.  
	\param i instance reference to connect, may not be NULL.
 */
void
instance_reference_connection::append_instance_reference(
		const count_ptr<const instance_reference_base>& i) {
	assert(i);
	inst_list.push_back(i);
}

//=============================================================================
// class aliases_connection method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(aliases_connection,
	ALIAS_CONNECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
aliases_connection::aliases_connection() : instance_reference_connection() { }

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
/**
	Adds an instance reference to the front of the connection list.  
	\param i the instance reference to add.  
 */
void
aliases_connection::prepend_instance_reference(
		const count_ptr<const instance_reference_base>& i) {
	assert(i);
	inst_list.push_front(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection::unroll(void) const {
	cerr << "aliases_connection::unroll(): "
		"Fang, finish me!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, ALIAS_CONNECTION_TYPE_KEY)) {
	inst_list_type::const_iterator iter = inst_list.begin();
	const inst_list_type::const_iterator end = inst_list.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
aliases_connection::construct_empty(const int i) {
	return new aliases_connection();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection::write_object(const persistent_object_manager& m) const {
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

DEFAULT_PERSISTENT_TYPE_REGISTRATION(port_connection, PORT_CONNECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
		const count_ptr<const simple_instance_reference>& i) :
		instance_reference_connection(), inst(i) {
	assert(inst);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.
 */
port_connection::~port_connection() {
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
	Adds an instance reference to the back of the connection list.  
	\param i instance reference to connect, may be NULL.
 */
void
port_connection::append_instance_reference(
		const count_ptr<const instance_reference_base>& i) {
	// do not assert, may be NULL.  
	inst_list.push_back(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands and finalizes the connection at unroll time.  
 */
void
port_connection::unroll(void) const {
	cerr << "port_connection::unroll(): "
		"Fang, finish me!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PORT_CONNECTION_TYPE_KEY)) {
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
persistent*
port_connection::construct_empty(const int i) {
	return new port_connection();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::write_object(const persistent_object_manager& m) const {
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
		never_ptr<const scopespace> s) :
		connection_assignment_base(), dscope(s) {
	// check that dscope is actually a loop or conditional
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_CONNECT_CC__

