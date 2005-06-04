/**
	\file "Object/art_object_connect.cc"
	Method definitions pertaining to connections and assignments.  
 	$Id: art_object_connect.cc,v 1.23.4.1 2005/06/04 04:47:53 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_CONNECT_CC__
#define	__OBJECT_ART_OBJECT_CONNECT_CC__

// compilation switches for debugging
#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include <iostream>
#include <vector>

#include "Object/art_object_connect.h"
#include "Object/art_object_connect.tcc"
#include "Object/art_object_inst_ref.h"
#include "Object/art_object_inst_ref_data.h"
#include "Object/art_object_instance_int.h"
#include "Object/art_object_instance_bool.h"
#include "Object/art_object_instance_enum.h"
#include "Object/art_object_instance_struct.h"
#include "Object/art_object_instance.h"
#include "Object/art_object_type_hash.h"

#include "util/what.tcc"
#include "util/STL/list.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/packed_array.tcc"
#include "util/stacktrace.h"
#include "util/binders.h"
#include "util/compose.h"
#include "util/dereference.h"

// conditional defines, after including "stacktrace.h"
#ifndef	STACKTRACE_DTOR
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_DTOR(x)
#endif
#endif

#ifndef	STACKTRACE_PERSISTENT
#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)	STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif
#endif



//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(ART::entity::bool_alias_connection, "bool_connection")
SPECIALIZE_UTIL_WHAT(ART::entity::int_alias_connection, "int_connection")
SPECIALIZE_UTIL_WHAT(ART::entity::enum_alias_connection, "enum_connection")
SPECIALIZE_UTIL_WHAT(ART::entity::datastruct_alias_connection, "struct_connection")
SPECIALIZE_UTIL_WHAT(ART::entity::channel_alias_connection, "channel_connection")
SPECIALIZE_UTIL_WHAT(ART::entity::process_alias_connection, "process_connection")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_alias_connection, 
		DBOOL_ALIAS_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_alias_connection, 
		DINT_ALIAS_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_alias_connection, 
		ENUM_ALIAS_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::datastruct_alias_connection, 
		STRUCT_ALIAS_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_alias_connection, 
		CHANNEL_ALIAS_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_alias_connection, 
		PROCESS_ALIAS_CONNECTION_TYPE_KEY, 0)

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::port_connection, PORT_CONNECTION_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
USING_IO_UTILS
using std::vector;
using util::persistent_traits;
#include "util/using_ostream.h"
using std::mem_fun_ref;
using util::dereference;
USING_UTIL_COMPOSE
USING_STACKTRACE

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
port_connection::dump(ostream& o) const {
	NEVER_NULL(ported_inst);
	ported_inst->dump(o) << " (";

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
port_connection::append_meta_instance_reference(const generic_inst_ptr_type& i) {
	// do not assert, may be NULL.  
	inst_list.push_back(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands and finalizes the connection at unroll time.  
 */
void
port_connection::unroll(unroll_context& c) const {
	cerr << "port_connection::unroll(): "
		"Fang, finish me!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	NEVER_NULL(ported_inst);
	ported_inst->collect_transient_info(m);
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

//=============================================================================
template class alias_connection<int_tag>;
template class alias_connection<bool_tag>;
template class alias_connection<enum_tag>;
template class alias_connection<datastruct_tag>;
template class alias_connection<channel_tag>;
template class alias_connection<process_tag>;

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

#endif	// __OBJECT_ART_OBJECT_CONNECT_CC__

