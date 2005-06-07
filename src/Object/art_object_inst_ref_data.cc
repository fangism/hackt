/**
	\file "Object/art_object_inst_ref_data.cc"
	Method definitions for datatype instance reference classes.
	$Id: art_object_inst_ref_data.cc,v 1.8.8.4 2005/06/07 03:01:25 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_REF_DATA_CC__
#define	__OBJECT_ART_OBJECT_INST_REF_DATA_CC__

#include "Object/art_object_instance_bool.h"
#include "Object/art_object_instance_int.h"
#include "Object/art_object_instance_enum.h"
#include "Object/art_object_instance_struct.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_inst_ref.tcc"
#include "Object/art_object_inst_ref_data.tcc"
#include "Object/art_object_member_inst_ref.tcc"

#include "Object/art_object_type_hash.h"
#include "util/persistent_object_manager.tcc"
#include "Object/art_object_classification_details.h"

namespace util {
using ART::entity::int_tag;
using ART::entity::bool_tag;

SPECIALIZE_UTIL_WHAT(
	ART::entity::simple_int_meta_instance_reference, "int-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::simple_bool_meta_instance_reference, "bool-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::simple_enum_meta_instance_reference, "enum-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::simple_datastruct_meta_instance_reference,
							"struct-inst-ref")

SPECIALIZE_UTIL_WHAT(
	ART::entity::int_member_meta_instance_reference, "int-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::bool_member_meta_instance_reference, "bool-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::enum_member_meta_instance_reference, "enum-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::datastruct_member_meta_instance_reference,
		"struct-member-inst-ref")

#if USE_DATA_REFERENCE
// not really used -- bogus type keys -- 
// but needed for complete class instantiation
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::meta_instance_reference<int_tag>, "BOGUSint", 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::meta_instance_reference<bool_tag>, "BOGUSbl", 0)
#endif
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_int_meta_instance_reference, 
		SIMPLE_DINT_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_bool_meta_instance_reference, 
		SIMPLE_DBOOL_INSTANCE_REFERENCE_TYPE_KEY, 0)

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_enum_meta_instance_reference, 
		SIMPLE_ENUM_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_datastruct_meta_instance_reference, 
		SIMPLE_STRUCT_INSTANCE_REFERENCE_TYPE_KEY, 0)

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_member_meta_instance_reference, 
		MEMBER_DINT_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_member_meta_instance_reference, 
		MEMBER_DBOOL_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_member_meta_instance_reference, 
		MEMBER_ENUM_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::datastruct_member_meta_instance_reference, 
		MEMBER_STRUCT_INSTANCE_REFERENCE_TYPE_KEY, 0)
}	// end namespace util

namespace ART {
namespace entity {
//=============================================================================
// class int_meta_instance_reference method definitions
// class bool_meta_instance_reference method definitions
// class enum_meta_instance_reference method definitions
// class datastruct_meta_instance_reference method definitions
//	... all have been replaced with template definitions!

//=============================================================================
// explicit template instantiations

#if USE_DATA_REFERENCE
template class data_reference<bool_tag>;
template class data_reference<int_tag>;
#else
template class simple_meta_instance_reference<bool_tag>;
template class simple_meta_instance_reference<int_tag>;
#endif
template class simple_meta_instance_reference<enum_tag>;
template class simple_meta_instance_reference<datastruct_tag>;

template class member_meta_instance_reference<bool_tag>;
template class member_meta_instance_reference<int_tag>;
template class member_meta_instance_reference<enum_tag>;
template class member_meta_instance_reference<datastruct_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_REF_DATA_CC__

