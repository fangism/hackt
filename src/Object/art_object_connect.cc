/**
	\file "art_object_connect.cc"
	Method definitions pertaining to connections and assignments.  
 	$Id: art_object_connect.cc,v 1.18.16.1.10.7.2.1 2005/02/24 02:03:45 fang Exp $
 */

#ifndef	__ART_OBJECT_CONNECT_CC__
#define	__ART_OBJECT_CONNECT_CC__

// compilation switches for debugging
#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include <iostream>
#include <vector>

#include "art_object_connect.h"
#include "art_object_connect.tcc"
#include "art_object_inst_ref.h"
#include "art_object_inst_ref_data.h"
#include "art_object_instance_int.h"
#include "art_object_instance_bool.h"
#include "art_object_instance_enum.h"
#include "art_object_instance_struct.h"
#include "art_object_instance.h"

#include "what.tcc"
#include "STL/list.tcc"
#include "persistent_object_manager.tcc"
#include "art_object_type_hash.h"
#include "packed_array.tcc"
#include "stacktrace.h"
#include "binders.h"
#include "compose.h"
#include "dereference.h"

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
#if SUBTYPE_ALIASES_CONNECTION
SPECIALIZE_UTIL_WHAT(ART::entity::bool_alias_connection, "bool_connection")
SPECIALIZE_UTIL_WHAT(ART::entity::int_alias_connection, "int_connection")
SPECIALIZE_UTIL_WHAT(ART::entity::enum_alias_connection, "enum_connection")
SPECIALIZE_UTIL_WHAT(ART::entity::datastruct_alias_connection, "struct_connection")
SPECIALIZE_UTIL_WHAT(ART::entity::channel_alias_connection, "channel_connection")
SPECIALIZE_UTIL_WHAT(ART::entity::process_alias_connection, "process_connection")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_alias_connection, 
		DBOOL_ALIAS_CONNECTION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_alias_connection, 
		DINT_ALIAS_CONNECTION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_alias_connection, 
		ENUM_ALIAS_CONNECTION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::datastruct_alias_connection, 
		STRUCT_ALIAS_CONNECTION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_alias_connection, 
		CHANNEL_ALIAS_CONNECTION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_alias_connection, 
		PROCESS_ALIAS_CONNECTION_TYPE_KEY)

#else
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::aliases_connection_base, ALIAS_CONNECTION_TYPE_KEY)
#endif
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::port_connection, PORT_CONNECTION_TYPE_KEY)
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
USING_IO_UTILS
using std::vector;
using util::persistent_traits;
#include "using_ostream.h"
using std::mem_fun_ref;
using util::dereference;
USING_UTIL_COMPOSE
USING_STACKTRACE

//=============================================================================
// class instance_reference_connection method definitions

instance_reference_connection::instance_reference_connection() :
		instance_management_base()
#if !SUBTYPE_ALIASES_CONNECTION
		, inst_list()
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instance_reference_connection::~instance_reference_connection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !SUBTYPE_ALIASES_CONNECTION
/**
	Initializes an instance reference connection with the
	first instance reference.  
	\param i instance reference to connect, may not be NULL.
 */
void
instance_reference_connection::append_instance_reference(
		const generic_inst_ptr_type& i) {
	NEVER_NULL(i);
	inst_list.push_back(i);
}
#endif

//=============================================================================
// class aliases_connection_base method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
aliases_connection_base::aliases_connection_base() : 
		instance_reference_connection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
aliases_connection_base::~aliases_connection_base() {
	STACKTRACE_DTOR("~aliases_connection_base()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
aliases_connection_base::what(ostream& o) const {
	return o << "alias-connection";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !SUBTYPE_ALIASES_CONNECTION
ostream&
aliases_connection_base::dump(ostream& o) const {
	INVARIANT(inst_list.size() > 1);
	const_iterator iter = inst_list.begin();
	const const_iterator end = inst_list.end();
	NEVER_NULL(*iter);
	(*iter)->dump(o);
	for (iter++ ; iter!=end; iter++) {
		NEVER_NULL(*iter);
		(*iter)->dump(o << " = ");
	}
	return o << ';';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Adds an instance reference to the front of the connection list.  
	\param i the instance reference to add.  
 */
void
aliases_connection_base::prepend_instance_reference(
		const generic_inst_ptr_type& i) {
	NEVER_NULL(i);
	inst_list.push_front(i);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Requires precise type-checking!
 */
void
aliases_connection_base::unroll(unroll_context& c) const {
#if 1
	cerr << "aliases_connection_base::unroll(): "
		"Fang, finish me!" << endl;
#else
	// want to collect packed_array<instance_aliases>
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection_base::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	const_iterator iter = inst_list.begin();
	const const_iterator end = inst_list.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
aliases_connection_base::construct_empty(const int i) {
	return new aliases_connection_base();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection_base::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer_list(f, inst_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection_base::load_object(
		const persistent_object_manager& m, istream& f) {
	m.read_pointer_list(f, inst_list);
}

#endif	// SUBTYPE_ALIASES_CONNECTION

//=============================================================================
#if 0
// class alias_connection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
ALIAS_CONNECTION_CLASS::alias_connection() :
		parent_type(), inst_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
ALIAS_CONNECTION_CLASS::~alias_connection() {
	STACKTRACE_DTOR("~alias_connection<>()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
ostream&
ALIAS_CONNECTION_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
ostream&
ALIAS_CONNECTION_CLASS::dump(ostream& o) const {
	INVARIANT(inst_list.size() > 1);
	const_iterator iter = inst_list.begin();
	const const_iterator end = inst_list.end();
	NEVER_NULL(*iter);
	(*iter)->dump(o);
	for (iter++; iter!=end; iter++) {
		NEVER_NULL(*iter);
		(*iter)->dump(o << " = ");
	}
	return o << ';';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::append_instance_reference(
		const generic_inst_ptr_type& i) {
	NEVER_NULL(i);
	// need dynamic cast
	const inst_ref_ptr_type
		irp(i.template is_a<const instance_reference_type>());
		// gcc-3.3 slightly crippled, needs template keyword :(
	NEVER_NULL(irp);
	inst_list.push_back(irp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Connects the referenced instance aliases.  
 */
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::unroll(unroll_context& c) const {
	typedef	vector<alias_collection_type>	alias_collection_array_type;
	what(cerr << "Fang, finish ") << "::unroll()!" << endl;
//	Create a vector of alias_collection_type (packed_array_generic)
	alias_collection_array_type ref_array(inst_list.size());
	const_iterator iter = inst_list.begin();
	const const_iterator end = inst_list.end();
	typename alias_collection_array_type::iterator
		ref_iter = ref_array.begin();
	bool err = false;
	for ( ; iter != end; iter++, ref_iter++) {
		NEVER_NULL(*iter);
		if ((*iter)->unroll_references(c, *ref_iter))
			err = true;
	}
	if (err) {
		what(cerr << "ERROR: unrolling instance references in ") <<
			"::unroll()." << endl;
		return;
	}
/***
	Make sure each packed array has the same dimensions.  
	Type-check.
		Collectible vs. connectible!  Different semantics!
		Collectible !=> connectible!  Must check each reference!
	Use vector of iterators to walk?
	Fancy: cache type-checking...
***/
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
persistent*
ALIAS_CONNECTION_CLASS::construct_empty(const int) {
	return new this_type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	// improper key!!!
	STACKTRACE_PERSISTENT("alias_connection<>::collect_transients()");
//	cerr << persistent_traits<this_type>::type_key << endl;
#if 1
	const_iterator iter = inst_list.begin();
	const const_iterator end = inst_list.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
#else
	for_each(inst_list.begin(), inst_list.end(),
	unary_compose_void(
		bind2nd_argval_void(mem_fun_ref(
			&instance_reference_type::collect_transient_info), m),
		dereference<inst_ref_ptr_type>()
	)
	);
#endif
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::write_object(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT("alias_connection<>::write_object()");
	m.write_pointer_list(o, inst_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::load_object(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_PERSISTENT("alias_connection<>::load_object()");
	m.read_pointer_list(i, inst_list);
}

#endif	// SUBTYPE_ALIASES_CONNECTION

//=============================================================================
// class port_connection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
port_connection::port_connection() : parent_type(), ported_inst(NULL)
#if SUBTYPE_ALIASES_CONNECTION
		, inst_list()
#endif
		{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initial constructor for a port-connection.  
	\param i an instance of the definition that is to be connected.
 */
port_connection::port_connection(const ported_inst_ptr_type& i) :
		parent_type(), ported_inst(i)
#if SUBTYPE_ALIASES_CONNECTION
		, inst_list()
#endif
		{
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
port_connection::append_instance_reference(const generic_inst_ptr_type& i) {
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
persistent*
port_connection::construct_empty(const int i) {
	return new port_connection();
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
#if SUBTYPE_ALIASES_CONNECTION
#if USE_CLASSIFICATION_TAGS
template class alias_connection<int_tag>;
template class alias_connection<bool_tag>;
template class alias_connection<enum_tag>;
template class alias_connection<datastruct_tag>;
template class alias_connection<channel_tag>;
template class alias_connection<process_tag>;
#else
template class alias_connection<
	int_instance_reference, data_alias_connection_base>;
template class alias_connection<
	bool_instance_reference, data_alias_connection_base>;
template class alias_connection<
	enum_instance_reference, data_alias_connection_base>;
template class alias_connection<
	datastruct_instance_reference, data_alias_connection_base>;
template class alias_connection<
	channel_instance_reference, aliases_connection_base>;
template class alias_connection<
	process_instance_reference, aliases_connection_base>;
#endif
#endif

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

