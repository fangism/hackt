/**
	\file "Object/inst/bool_instance_collection.cc"
	Method definitions for boolean data type instance classes.
	This file came from "Object/art_object_instance_bool.cc"
		in a previous life.  
	$Id: bool_instance_collection.cc,v 1.1.2.3 2005/07/22 21:34:18 fang Exp $
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
#include "Object/art_object_type_hash.h"
#include "Object/inst/null_collection_type_manager.tcc"
#include "Object/inst/instance_collection.tcc"

#include "util/memory/list_vector_pool.tcc"

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
#if 1
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::entity::bool_scalar)
#endif
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// class bool_instance method definitions

bool_instance::bool_instance() : persistent(), back_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_instance::~bool_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance::collect_transient_info(persistent_object_manager& m) const {
	// register me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	// write me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance::load_object(const persistent_object_manager& m, 
		istream& i) {
	// load me!
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

