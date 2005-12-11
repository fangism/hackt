/**
	\file "Object/inst/bool_instance_collection.cc"
	Method definitions for boolean data type instance classes.
	This file came from "Object/art_object_instance_bool.cc"
		in a previous life.  
	$Id: bool_instance_collection.cc,v 1.5.20.1 2005/12/11 00:45:33 fang Exp $
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
#include "Object/inst/alias_empty.tcc"
#include "Object/ref/simple_datatype_meta_instance_reference_base.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/expr/bool_expr.h"
#include "Object/def/built_in_datatype_def.h"
#include "Object/type/data_type_reference.h"
#include "Object/persistent_type_hash.h"
#include "Object/inst/null_collection_type_manager.tcc"
#include "Object/inst/instance_collection.tcc"
#include "Object/inst/instance_alias.tcc"
#include "Object/inst/state_instance.tcc"
// #include "util/memory/chunk_map_pool.tcc"

//=============================================================================
// module-local specializations

namespace util {
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_instance_collection,
		"bool_instance_collection")
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_scalar, "bool_scalar")
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_array_1D, "bool_array_1D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_array_2D, "bool_array_2D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_array_3D, "bool_array_3D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_array_4D, "bool_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::bool_scalar, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::bool_array_1D, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::bool_array_2D, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::bool_array_3D, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::bool_array_4D, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 4)

namespace memory {
	// can we still lazy destroy with instance aliases?
	// or will it contain pointers to other things later?  (instances)
#if 0
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(HAC::entity::bool_scalar)
#endif
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {

//=============================================================================
// typedef bool_instance_alias_base function definitions

ostream&
operator << (ostream& o, const bool_instance_alias_base& b) {
	return o << "bool-alias @ " << &b;
}

//=============================================================================
// explicit instantiations

template class state_instance<bool_tag>;
template class instance_pool<state_instance<bool_tag> >;
template class instance_alias_info<bool_tag>;
template class instance_collection<bool_tag>;
template class instance_array<bool_tag, 0>;
template class instance_array<bool_tag, 1>;
template class instance_array<bool_tag, 2>;
template class instance_array<bool_tag, 3>;
template class instance_array<bool_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

#endif	// __OBJECT_INST_BOOL_INSTANCE_COLLECTION_CC__

