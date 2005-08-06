/**
	\file "Object/inst/bool_instance_collection.cc"
	Method definitions for boolean data type instance classes.
	This file came from "Object/art_object_instance_bool.cc"
		in a previous life.  
	$Id: bool_instance_collection.cc,v 1.2.4.3 2005/08/06 01:32:19 fang Exp $
 */

#ifndef	__OBJECT_INST_BOOL_INSTANCE_COLLECTION_CC__
#define	__OBJECT_INST_BOOL_INSTANCE_COLLECTION_CC__

#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/bool_instance_collection.h"
#include "Object/inst/alias_empty.h"
#include "Object/ref/simple_datatype_meta_instance_reference_base.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/expr/bool_expr.h"
#include "Object/def/built_in_datatype_def.h"
#include "Object/type/data_type_reference.h"
#include "Object/persistent_type_hash.h"
#include "Object/inst/null_collection_type_manager.tcc"
#include "Object/inst/instance_collection.tcc"
#include "util/memory/chunk_map_pool.tcc"

//=============================================================================
// module-local specializations

namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_instance_collection,
		"bool_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_scalar, "bool_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array_1D, "bool_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array_2D, "bool_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array_3D, "bool_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array_4D, "bool_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_scalar, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_array_1D, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_array_2D, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_array_3D, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_array_4D, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 4)

namespace memory {
	// can we still lazy destroy with instance aliases?
	// or will it contain pointers to other things later?  (instances)
#if 0
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::entity::bool_scalar)
#endif
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// class bool_instance method definitions

#if 0
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(bool_instance)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_instance::bool_instance() :
#if 0
		persistent(),
#endif
		back_ref() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_instance::bool_instance(const alias_info_type& b) :
#if 0
		persistent(),
#endif
		back_ref(&b) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_instance::~bool_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ostream&
bool_instance::what(ostream& o) const {
	o << "bool-state";
	if (back_ref) {
		back_ref->dump_hierarchical_name(o << " = ");
	}
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't actually register the back-reference because it is
	not directly managed by the persistent object manager.  
 */
void
bool_instance::collect_transient_info_base(persistent_object_manager& m) const {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
#if USE_INSTANCE_INDEX
	NEVER_NULL(back_ref);
	back_ref->write_next_connection(m, o);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance::load_object_base(const persistent_object_manager& m, 
		istream& i) {
#if USE_INSTANCE_INDEX
	back_ref = never_ptr<const alias_info_type>(
		&alias_info_type::load_alias_reference(m, i));
#endif
}

//=============================================================================
// typedef bool_instance_alias_base function definitions

ostream&
operator << (ostream& o, const bool_instance_alias_base& b) {
	return o << "bool-alias @ " << &b;
}

//=============================================================================
// explicit instantiations

template class instance_alias_info<bool_tag>;
template class instance_collection<bool_tag>;
template class instance_array<bool_tag, 0>;
template class instance_array<bool_tag, 1>;
template class instance_array<bool_tag, 2>;
template class instance_array<bool_tag, 3>;
template class instance_array<bool_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

DEFAULT_STATIC_TRACE_END

#endif	// __OBJECT_INST_BOOL_INSTANCE_COLLECTION_CC__

