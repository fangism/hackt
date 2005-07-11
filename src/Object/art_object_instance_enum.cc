/**
	\file "Object/art_object_instance_enum.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_enum.cc,v 1.16.2.7.2.1 2005/07/11 03:26:59 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_ENUM_CC__
#define	__OBJECT_ART_OBJECT_INSTANCE_ENUM_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/art_object_instance_enum.h"
#include "Object/art_object_instance_alias_empty.h"
#include "Object/art_object_inst_ref_data.h"
#include "Object/art_object_member_inst_ref.h"
#include "Object/expr/enum_expr.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_definition_data.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_type_hash.h"
#include "Object/art_object_definition.h"
#include "Object/art_object_nonmeta_value_reference.h"

// experimental: suppressing automatic template instantiation
#include "Object/art_object_extern_templates.h"

#include "Object/art_object_instance_collection.tcc"
#include "Object/inst/parameterless_collection_type_manager.tcc"

namespace util {

	SPECIALIZE_UTIL_WHAT(ART::entity::enum_instance_collection,
		"enum_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::enum_scalar, "enum_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::enum_array_1D, "enum_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::enum_array_2D, "enum_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::enum_array_3D, "enum_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::enum_array_4D, "enum_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_scalar, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_array_1D, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_array_2D, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_array_3D, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_array_4D, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 4)
}	// end namespace util

namespace ART {
namespace entity {

//=============================================================================
// class enum_instance method definitions

enum_instance::enum_instance() : back_ref(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum_instance::~enum_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
enum_instance::collect_transient_info(persistent_object_manager& m) const {
	// collect pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
enum_instance::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	// write me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
enum_instance::load_object(const persistent_object_manager& m, 
		istream& i) {
	// write me!
}

//=============================================================================
// explicit class instantiations

template class instance_alias_info<enum_tag>;
template class instance_collection<enum_tag>;
template class instance_array<enum_tag, 0>;
template class instance_array<enum_tag, 1>;
template class instance_array<enum_tag, 2>;
template class instance_array<enum_tag, 3>;
template class instance_array<enum_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_ENUM_CC__

