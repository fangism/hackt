/**
	\file "art_object_inst_ref_data.cc"
	Method definitions for datatype instance reference classes.
	$Id: art_object_inst_ref_data.cc,v 1.4.16.2.2.3 2005/02/23 21:12:36 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_REF_DATA_CC__
#define	__ART_OBJECT_INST_REF_DATA_CC__

#include "art_object_inst_ref_data.h"
#include "art_object_instance_bool.h"
#include "art_object_instance_int.h"
#include "art_object_instance_enum.h"
#include "art_object_instance_struct.h"
#include "art_object_connect.h"
#include "art_object_inst_ref.tcc"
#include "art_object_member_inst_ref.tcc"

#include "art_object_type_hash.h"
#include "persistent_object_manager.tcc"

#if USE_CLASSIFICATION_TAGS
#include "art_object_classification_details.h"
#endif

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_instance_reference, 
		SIMPLE_DINT_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_instance_reference, 
		SIMPLE_DBOOL_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_instance_reference, 
		SIMPLE_ENUM_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::datastruct_instance_reference, 
		SIMPLE_STRUCT_INSTANCE_REFERENCE_TYPE_KEY)

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_member_instance_reference, 
		MEMBER_DINT_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_member_instance_reference, 
		MEMBER_DBOOL_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_member_instance_reference, 
		MEMBER_ENUM_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::datastruct_member_instance_reference, 
		MEMBER_STRUCT_INSTANCE_REFERENCE_TYPE_KEY)

SPECIALIZE_UTIL_WHAT(
	ART::entity::int_instance_reference, "int-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::bool_instance_reference, "bool-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::enum_instance_reference, "enum-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::datastruct_instance_reference, "struct-inst-ref")

SPECIALIZE_UTIL_WHAT(
	ART::entity::int_member_instance_reference, "int-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::bool_member_instance_reference, "bool-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::enum_member_instance_reference, "enum-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::datastruct_member_instance_reference,
		"struct-member-inst-ref")

}	// end namespace util

namespace ART {
namespace entity {
//=============================================================================
// class int_instance_reference method definitions
// class bool_instance_reference method definitions
// class enum_instance_reference method definitions
// class datastruct_instance_reference method definitions
//	... all have been replaced with template definitions!

//=============================================================================
// explicit template instantiations

template class
instance_reference<bool_instance_collection, datatype_instance_reference>;
template class
instance_reference<int_instance_collection, datatype_instance_reference>;
template class
instance_reference<enum_instance_collection, datatype_instance_reference>;
template class
instance_reference<struct_instance_collection, datatype_instance_reference>;

#if USE_CLASSIFICATION_TAGS
template class member_instance_reference<bool_tag>;
template class member_instance_reference<int_tag>;
template class member_instance_reference<enum_tag>;
template class member_instance_reference<datastruct_tag>;
#else
template class member_instance_reference<bool_instance_reference>;
template class member_instance_reference<int_instance_reference>;
template class member_instance_reference<enum_instance_reference>;
template class member_instance_reference<datastruct_instance_reference>;
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_REF_DATA_CC__

