/**
	\file "Object/inst/struct_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file came from "Object/art_object_instance_struct.cc"
		in a previous life.  
	$Id: struct_instance_collection.cc,v 1.11.2.1 2006/10/23 06:51:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_STRUCT_INSTANCE_COLLECTION_CC__
#define	__HAC_OBJECT_INST_STRUCT_INSTANCE_COLLECTION_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/struct_instance_collection.h"
// #include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.tcc"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/expr/struct_expr.h"
#include "Object/def/user_def_datatype.h"
#include "Object/type/data_type_reference.h"
#include "Object/persistent_type_hash.h"

#include "Object/inst/instance_collection.tcc"
#include "Object/inst/instance_placeholder.tcc"
#include "Object/inst/instance_alias.tcc"
#include "Object/inst/general_collection_type_manager.tcc"
#include "Object/inst/state_instance.tcc"
#include "Object/unroll/datatype_instantiation_statement.h"

namespace util {
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_instance_collection,
		"struct_instance_collection")
#if DENSE_FORMAL_COLLECTIONS
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_port_formal_array,
		"struct_port_formal_array")
#endif
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_scalar, "struct_scalar")
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_array_1D, "struct_array_1D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_array_2D, "struct_array_2D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_array_3D, "struct_array_3D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_array_4D, "struct_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::struct_instance_placeholder, 
	STRUCT_INSTANCE_PLACEHOLDER_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::struct_scalar, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::struct_array_1D, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::struct_array_2D, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::struct_array_3D, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::struct_array_4D, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 4)
#if DENSE_FORMAL_COLLECTIONS
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::struct_port_formal_array, 
	STRUCT_PORT_FORMAL_ARRAY_TYPE_KEY, 0)
#endif

}	// end namespace util

namespace HAC {
namespace entity {
//=============================================================================
// explicit template class instantiations

template class instance_placeholder<datastruct_tag>;
template class state_instance<datastruct_tag>;
template class instance_pool<state_instance<datastruct_tag> >;
template class instance_alias_info<datastruct_tag>;
template class instance_collection<datastruct_tag>;
template class instance_array<datastruct_tag, 0>;
template class instance_array<datastruct_tag, 1>;
template class instance_array<datastruct_tag, 2>;
template class instance_array<datastruct_tag, 3>;
template class instance_array<datastruct_tag, 4>;
#if DENSE_FORMAL_COLLECTIONS
template class port_formal_array<datastruct_tag>;
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_STRUCT_INSTANCE_COLLECTION_CC__

