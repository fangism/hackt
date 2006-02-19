/**
	\file "Object/unroll/meta_instance_reference_connection.cc"
	Method definitions pertaining to connections and assignments.  
	This file was moved from "Object/art_object_connect.cc".
 	$Id: meta_instance_reference_connection.cc,v 1.11.10.1 2006/02/19 03:53:17 fang Exp $
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
#include "Object/unroll/port_connection.tcc"

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

SPECIALIZE_UTIL_WHAT(HAC::entity::process_port_connection,
	"process_port_connection")
SPECIALIZE_UTIL_WHAT(HAC::entity::channel_port_connection,
	"channel_port_connection")
SPECIALIZE_UTIL_WHAT(HAC::entity::struct_port_connection,
	"struct_port_connection")

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

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::process_port_connection,
	PROCESS_PORT_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_port_connection,
	CHANNEL_PORT_CONNECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::struct_port_connection,
	STRUCT_PORT_CONNECTION_TYPE_KEY, 0)
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
// class port_connection_base method definitions

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
/**
	Pre-allocation.  
 */
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

//=============================================================================
template class alias_connection<int_tag>;
template class alias_connection<bool_tag>;
template class alias_connection<enum_tag>;
template class alias_connection<datastruct_tag>;
template class alias_connection<channel_tag>;
template class alias_connection<process_tag>;

template class port_connection<datastruct_tag>;
template class port_connection<channel_tag>;
template class port_connection<process_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_META_INSTANCE_REFERENCE_CONNECTION_CC__

