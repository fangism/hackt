/**
	\file "Object/inst/enum_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originated from "Object/art_object_instance_enum.cc"
		in a previous life.  
	$Id: enum_instance_collection.cc,v 1.11.2.1 2006/10/28 03:03:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ENUM_INSTANCE_COLLECTION_CC__
#define	__HAC_OBJECT_INST_ENUM_INSTANCE_COLLECTION_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/enum_instance_collection.h"
#include "Object/inst/alias_empty.tcc"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/expr/enum_expr.h"
#include "Object/def/enum_datatype_def.h"
#include "Object/type/data_type_reference.h"
#include "Object/persistent_type_hash.h"

#include "Object/def/user_def_datatype.h"	// needed by canonical_type

#include "Object/inst/instance_collection.tcc"
#include "Object/inst/instance_placeholder.tcc"
#include "Object/inst/instance_alias.tcc"
#include "Object/inst/parameterless_collection_type_manager.tcc"
#include "Object/inst/state_instance.tcc"
#include "Object/unroll/datatype_instantiation_statement.h"

namespace util {

	SPECIALIZE_UTIL_WHAT(HAC::entity::enum_instance_collection,
		"enum_instance_collection")
	SPECIALIZE_UTIL_WHAT(HAC::entity::enum_port_formal_array,
		"enum_port_formal_array")
	SPECIALIZE_UTIL_WHAT(HAC::entity::enum_scalar, "enum_scalar")
	SPECIALIZE_UTIL_WHAT(HAC::entity::enum_array_1D, "enum_array_1D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::enum_array_2D, "enum_array_2D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::enum_array_3D, "enum_array_3D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::enum_array_4D, "enum_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::enum_instance_placeholder, 
	ENUM_INSTANCE_PLACEHOLDER_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::enum_scalar, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::enum_array_1D, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::enum_array_2D, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::enum_array_3D, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::enum_array_4D, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 4)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::enum_port_formal_array, 
	ENUM_PORT_FORMAL_ARRAY_TYPE_KEY, 0)
#if ENABLE_PORT_ACTUAL_COLLECTIONS
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::port_actual_collection<HAC::entity::enum_tag>, 
	ENUM_PORT_ACTUAL_COLLECTION_TYPE_KEY, 0)
#endif
}	// end namespace util

namespace HAC {
namespace entity {
//=============================================================================
// explicit class instantiations

template class instance_placeholder<enum_tag>;
template class state_instance<enum_tag>;
template class instance_pool<state_instance<enum_tag> >;
template class instance_alias_info<enum_tag>;
template class instance_collection<enum_tag>;
template class instance_array<enum_tag, 0>;
template class instance_array<enum_tag, 1>;
template class instance_array<enum_tag, 2>;
template class instance_array<enum_tag, 3>;
template class instance_array<enum_tag, 4>;
template class port_formal_array<enum_tag>;
#if ENABLE_PORT_ACTUAL_COLLECTIONS
template class port_actual_collection<enum_tag>;
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ENUM_INSTANCE_COLLECTION_CC__

