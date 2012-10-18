/**
	\file "Object/inst/struct_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file came from "Object/art_object_instance_struct.cc"
		in a previous life.  
	$Id: struct_instance_collection.cc,v 1.15 2011/04/02 01:46:03 fang Exp $
 */

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/struct_instance_collection.hh"
#include "Object/inst/alias_empty.tcc"
#include "Object/ref/member_meta_instance_reference.hh"
#include "Object/ref/simple_nonmeta_value_reference.hh"
#include "Object/expr/struct_expr.hh"
#include "Object/def/user_def_datatype.hh"
#include "Object/type/data_type_reference.hh"
#include "Object/persistent_type_hash.hh"

#include "Object/inst/instance_collection.tcc"
#include "Object/inst/instance_placeholder.tcc"
#include "Object/inst/instance_alias.tcc"
#include "Object/inst/general_collection_type_manager.tcc"
#include "Object/inst/state_instance.tcc"
#include "Object/unroll/datatype_instantiation_statement.hh"

#include "Object/devel_switches.hh"

#if ENABLE_DATASTRUCTS
// this is a nuisance, so we disable it
namespace util {
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_instance_collection,
		"struct_instance_collection")
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_port_formal_array,
		"struct_port_formal_array")
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_scalar, "struct_scalar")
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_array_1D, "struct_array_1D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_array_2D, "struct_array_2D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_array_3D, "struct_array_3D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::struct_array_4D, "struct_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::struct_instance_placeholder, 
	STRUCT_INSTANCE_PLACEHOLDER_TYPE_KEY, 0)
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
template class port_formal_array<datastruct_tag>;
template class port_actual_collection<datastruct_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC
#endif

