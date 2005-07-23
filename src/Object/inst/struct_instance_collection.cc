/**
	\file "Object/inst/struct_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file came from "Object/art_object_instance_struct.cc"
		in a previous life.  
	$Id: struct_instance_collection.cc,v 1.1.2.4 2005/07/23 01:05:56 fang Exp $
 */

#ifndef	__OBJECT_INST_STRUCT_INSTANCE_COLLECTION_CC__
#define	__OBJECT_INST_STRUCT_INSTANCE_COLLECTION_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/struct_instance_collection.h"
#include "Object/inst/alias_actuals.h"
#include "Object/ref/simple_datatype_meta_instance_reference_base.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/expr/struct_expr.h"
#include "Object/def/user_def_datatype.h"
#include "Object/type/data_type_reference.h"
#include "Object/persistent_type_hash.h"

#include "Object/inst/instance_collection.tcc"
#include "Object/inst/general_collection_type_manager.tcc"

namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_instance_collection,
		"struct_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_scalar, "struct_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_array_1D, "struct_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_array_2D, "struct_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_array_3D, "struct_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_array_4D, "struct_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::struct_scalar, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::struct_array_1D, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::struct_array_2D, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::struct_array_3D, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::struct_array_4D, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 4)

}	// end namespace util

namespace ART {
namespace entity {

//=============================================================================
// class struct_instance method definitions

struct_instance::struct_instance() : back_ref(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct_instance::~struct_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
struct_instance::collect_transient_info(persistent_object_manager& m) const {
	// collect pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
struct_instance::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	// write me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
struct_instance::load_object(const persistent_object_manager& m, 
		istream& i) {
	// write me!
}

//=============================================================================
// explicit template class instantiations

template class instance_alias_info<datastruct_tag>;
template class instance_collection<datastruct_tag>;
template class instance_array<datastruct_tag, 0>;
template class instance_array<datastruct_tag, 1>;
template class instance_array<datastruct_tag, 2>;
template class instance_array<datastruct_tag, 3>;
template class instance_array<datastruct_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_STRUCT_INSTANCE_COLLECTION_CC__

