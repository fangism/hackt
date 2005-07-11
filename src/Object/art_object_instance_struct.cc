/**
	\file "Object/art_object_instance_struct.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_struct.cc,v 1.16.2.8.2.1 2005/07/11 03:27:03 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_STRUCT_CC__
#define	__OBJECT_ART_OBJECT_INSTANCE_STRUCT_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/art_object_instance_struct.h"
#include "Object/art_object_instance_alias_actuals.h"
#include "Object/art_object_inst_ref_data.h"
#include "Object/art_object_member_inst_ref.h"
#include "Object/expr/struct_expr.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_definition_data.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_type_hash.h"
#include "Object/art_object_nonmeta_value_reference.h"

// experimental: suppressing automatic template instantiation
#include "Object/art_object_extern_templates.h"

#include "Object/art_object_instance_collection.tcc"
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

#endif	// __OBJECT_ART_OBJECT_INSTANCE_STRUCT_CC__

