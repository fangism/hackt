/**
	\file "Object/unroll/meta_instance_reference_connection.cc"
	Method definitions pertaining to connections and assignments.  
	This file was moved from "Object/art_object_connect.cc".
 	$Id: meta_instance_reference_connection.cc,v 1.11.12.1 2006/02/17 05:07:51 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_META_INSTANCE_REFERENCE_CONNECTION_CC__
#define	__HAC_OBJECT_UNROLL_META_INSTANCE_REFERENCE_CONNECTION_CC__

// compilation switches for debugging
#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include <iostream>
#include <vector>
#include <list>

#include "Object/unroll/alias_connection.tcc"
#include "Object/unroll/data_alias_connection_base.h"
#include "Object/unroll/port_connection.h"
#include "Object/unroll/unroll_context.h"	// for debugging
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/simple_datatype_meta_instance_reference_base.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/int_instance_collection.h"
#include "Object/inst/bool_instance_collection.h"
#include "Object/inst/enum_instance_collection.h"
#include "Object/inst/struct_instance_collection.h"
#include "Object/persistent_type_hash.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/struct_traits.h"

#if SUBTYPE_PORT_CONNECTION
#include "Object/unroll/port_connection.tcc"
#endif

#include "util/what.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/packed_array.tcc"
#include "util/stacktrace.h"
// #include "util/binders.h"
// #include "util/compose.h"
// #include "util/dereference.h"
#include "util/reserve.h"

//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::bool_alias_connection, "bool_connection")
SPECIALIZE_UTIL_WHAT(HAC::entity::int_alias_connection, "int_connection")
SPECIALIZE_UTIL_WHAT(HAC::entity::enum_alias_connection, "enum_connection")
SPECIALIZE_UTIL_WHAT(HAC::entity::datastruct_alias_connection, "struct_connection")
SPECIALIZE_UTIL_WHAT(HAC::entity::channel_alias_connection, "channel_connection")
SPECIALIZE_UTIL_WHAT(HAC::entity::process_alias_connection, "process_connection")

#if SUBTYPE_PORT_CONNECTION
SPECIALIZE_UTIL_WHAT(HAC::entity::process_port_connection,
	"process_port_connection")
SPECIALIZE_UTIL_WHAT(HAC::entity::channel_port_connection,
	"channel_port_connection")
SPECIALIZE_UTIL_WHAT(HAC::entity::struct_port_connection,
	"struct_port_connection")
#endif

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::bool_alias_connection, 
		DBOOL_ALIAS_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::int_alias_connection, 
		DINT_ALIAS_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::enum_alias_connection, 
		ENUM_ALIAS_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::datastruct_alias_connection, 
		STRUCT_ALIAS_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_alias_connection, 
		CHANNEL_ALIAS_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::process_alias_connection, 
		PROCESS_ALIAS_CONNECTION_TYPE_KEY, 0)

#if SUBTYPE_PORT_CONNECTION
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::process_port_connection,
	PROCESS_PORT_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_port_connection,
	CHANNEL_PORT_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::struct_port_connection,
	STRUCT_PORT_CONNECTION_TYPE_KEY, 0)
#else
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::port_connection, PORT_CONNECTION_TYPE_KEY, 0)
#endif
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
USING_IO_UTILS
using std::vector;
using util::persistent_traits;
#include "util/using_ostream.h"
using std::mem_fun_ref;
using util::dereference;
USING_UTIL_COMPOSE

//=============================================================================
// class meta_instance_reference_connection method definitions

meta_instance_reference_connection::meta_instance_reference_connection() :
		instance_management_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_instance_reference_connection::~meta_instance_reference_connection() {
}

//=============================================================================
// class aliases_connection_base method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
aliases_connection_base::aliases_connection_base() : 
		meta_instance_reference_connection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
aliases_connection_base::~aliases_connection_base() {
	STACKTRACE_DTOR("~aliases_connection_base()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
aliases_connection_base::what(ostream& o) const {
	return o << "alias-connection";
}

//=============================================================================
#if SUBTYPE_PORT_CONNECTION
port_connection_base::port_connection_base() : parent_type(), inst_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
port_connection_base::~port_connection_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_connection_base::dump_ports(ostream& o, 
		const expr_dump_context& dc) const {
if (!inst_list.empty()) {
	// for_each_if... almost
	inst_list_type::const_iterator iter(inst_list.begin());
	const inst_list_type::const_iterator end(inst_list.end());
	if (*iter)
		(*iter)->dump(o, dc);
	else o << " ";
	for (iter++ ; iter!=end; iter++) {
		o << ", ";
		if (*iter)
			(*iter)->dump(o, dc);
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection_base::reserve(const size_t s) {
	util::reserve(inst_list, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds an instance reference to the back of the connection list.  
	\param i instance reference to connect, may be NULL.
 */
void
port_connection_base::append_meta_instance_reference(
		const generic_inst_ptr_type& i) {
	inst_list.push_back(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	m.collect_pointer_list(inst_list);
#if 0
	inst_list_type::const_iterator iter(inst_list.begin());
	const inst_list_type::const_iterator end(inst_list.end());
	for ( ; iter!=end; iter++) {
		// port connection arguments may be NULL
		if (*iter)
			(*iter)->collect_transient_info(m);
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection_base::write_object_base(const persistent_object_manager& m, 
		ostream& f) const {
	m.write_pointer_list(f, inst_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection_base::load_object_base(const persistent_object_manager& m,
		istream& f) {
	m.read_pointer_list(f, inst_list);
}

#else	// !SUBTYPE_PORT_CONNECTIONS
//=============================================================================
// class port_connection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
port_connection::port_connection() :
		parent_type(), ported_inst(NULL) , inst_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initial constructor for a port-connection.  
	\param i an instance of the definition that is to be connected.
 */
port_connection::port_connection(const ported_inst_ptr_type& i) :
		parent_type(), ported_inst(i), inst_list() {
	NEVER_NULL(ported_inst);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.
 */
port_connection::~port_connection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_connection::what(ostream& o) const {
	return o << "port-connection";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_connection::dump(ostream& o, const expr_dump_context& dc) const {
	NEVER_NULL(ported_inst);
	ported_inst->dump(o, dc) << " (";
	if (!inst_list.empty()) {
		inst_list_type::const_iterator iter(inst_list.begin());
		const inst_list_type::const_iterator end(inst_list.end());
		if (*iter)
			(*iter)->dump(o, dc);
		else o << " ";
		for (iter++ ; iter!=end; iter++) {
			o << ", ";
			if (*iter)
				(*iter)->dump(o, dc);
		}
	}
	return o << ");";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::reserve(const size_t s) {
	util::reserve(inst_list, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds an instance reference to the back of the connection list.  
	\param i instance reference to connect, may be NULL.
 */
void
port_connection::append_meta_instance_reference(const generic_inst_ptr_type& i) {
	// do not assert, may be NULL.  
	inst_list.push_back(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !UNIFY_UNROLL_PASS
/**
	Expands and finalizes the connection at unroll time.  
 */
good_bool
port_connection::unroll_meta_connect(const unroll_context& c) const {
	return unroll(c);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands and finalizes the connection at unroll time.  
 */
good_bool
port_connection::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "context c @ " << &c << endl;
	c.dump(cerr) << endl;
#endif
	NEVER_NULL(ported_inst);
	const never_ptr<substructure_alias>
		parent_instance(
			ported_inst->unroll_scalar_substructure_reference(c));
	if (!parent_instance) {
		cerr << "ERROR: resolving super instance of port connection: ";
		ported_inst->dump(cerr, 
			expr_dump_context::default_value) << endl;
		return good_bool(false);
	}
	// iterators point to meta_instance_reference_base
	return parent_instance->connect_ports(inst_list, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	NEVER_NULL(ported_inst);
	ported_inst->collect_transient_info(m);
	inst_list_type::const_iterator iter(inst_list.begin());
	const inst_list_type::const_iterator end(inst_list.end());
	for ( ; iter!=end; iter++) {
		// port connection arguments may be NULL
		if (*iter)
			(*iter)->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	m.write_pointer(f, ported_inst);
	m.write_pointer_list(f, inst_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::load_object(const persistent_object_manager& m, istream& f) {
	m.read_pointer(f, ported_inst);
	m.read_pointer_list(f, inst_list);
}
#endif	// SUBTYPE_PORT_CONNECTION

//=============================================================================
template class alias_connection<int_tag>;
template class alias_connection<bool_tag>;
template class alias_connection<enum_tag>;
template class alias_connection<datastruct_tag>;
template class alias_connection<channel_tag>;
template class alias_connection<process_tag>;

#if SUBTYPE_PORT_CONNECTION
template class port_connection<datastruct_tag>;
template class port_connection<channel_tag>;
template class port_connection<process_tag>;
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_META_INSTANCE_REFERENCE_CONNECTION_CC__

