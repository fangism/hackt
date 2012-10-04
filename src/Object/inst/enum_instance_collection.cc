/**
	\file "Object/inst/enum_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originated from "Object/art_object_instance_enum.cc"
		in a previous life.  
	$Id: enum_instance_collection.cc,v 1.13 2011/04/02 01:46:01 fang Exp $
 */

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

// explicit template instantiation
using namespace HAC::entity;
// from Object/traits/enum_traits.h: alias_collection_type
template class packed_array_generic<pint_value_type,
		memory::never_ptr<instance_alias_info<enum_tag> > >;
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
template class port_actual_collection<enum_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

