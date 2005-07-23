/**
	\file "Object/inst/enum_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originated from "Object/art_object_instance_enum.cc"
		in a previous life.  
	$Id: enum_instance_collection.cc,v 1.1.2.4 2005/07/23 01:05:55 fang Exp $
 */

#ifndef	__OBJECT_INST_ENUM_INSTANCE_COLLECTION_CC__
#define	__OBJECT_INST_ENUM_INSTANCE_COLLECTION_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/enum_instance_collection.h"
#include "Object/inst/alias_empty.h"
#include "Object/ref/simple_datatype_meta_instance_reference_base.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/expr/enum_expr.h"
#include "Object/def/enum_datatype_def.h"
#include "Object/type/data_type_reference.h"
#include "Object/persistent_type_hash.h"

#include "Object/inst/instance_collection.tcc"
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

#endif	// __OBJECT_INST_ENUM_INSTANCE_COLLECTION_CC__

