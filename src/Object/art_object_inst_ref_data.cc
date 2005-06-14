/**
	\file "Object/art_object_inst_ref_data.cc"
	Method definitions for datatype instance reference classes.
	$Id: art_object_inst_ref_data.cc,v 1.8.6.2 2005/06/14 05:38:29 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_REF_DATA_CC__
#define	__OBJECT_ART_OBJECT_INST_REF_DATA_CC__

#include "Object/art_object_instance_bool.h"
#include "Object/art_object_instance_int.h"
#include "Object/art_object_instance_enum.h"
#include "Object/art_object_instance_struct.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_inst_ref.tcc"
#include "Object/art_object_nonmeta_inst_ref.tcc"
#include "Object/art_object_member_inst_ref.tcc"
#include "Object/art_object_nonmeta_value_reference.tcc"
#include "Object/art_built_ins.h"

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

#if 0
SPECIALIZE_UTIL_WHAT(
	ART::entity::simple_datatype_nonmeta_instance_reference,
		"data-nonmeta-inst-ref")
#endif
SPECIALIZE_UTIL_WHAT(
	ART::entity::simple_int_nonmeta_instance_reference,
		"int-nonmeta-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::simple_bool_nonmeta_instance_reference,
		"bool-nonmeta-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::simple_enum_nonmeta_instance_reference,
		"enum-nonmeta-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::simple_datastruct_nonmeta_instance_reference,
		"struct-nonmeta-inst-ref")

SPECIALIZE_UTIL_WHAT(
	ART::entity::int_member_meta_instance_reference,
		"int-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::bool_member_meta_instance_reference,
		"bool-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::enum_member_meta_instance_reference,
		"enum-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::datastruct_member_meta_instance_reference,
		"struct-member-inst-ref")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_int_meta_instance_reference, 
		SIMPLE_DINT_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_bool_meta_instance_reference, 
		SIMPLE_DBOOL_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_enum_meta_instance_reference, 
		SIMPLE_ENUM_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_datastruct_meta_instance_reference, 
		SIMPLE_STRUCT_META_INSTANCE_REFERENCE_TYPE_KEY, 0)

#if 0
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_datatype_nonmeta_instance_reference, 
		SIMPLE_DATATYPE_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
#endif
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_int_nonmeta_instance_reference, 
		SIMPLE_DINT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_bool_nonmeta_instance_reference, 
		SIMPLE_DBOOL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_enum_nonmeta_instance_reference, 
		SIMPLE_ENUM_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_datastruct_nonmeta_instance_reference, 
		SIMPLE_STRUCT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)

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
// policy specializations
// for simple_nonmeta_value_reference

template <>
struct data_type_resolver<bool_tag> {
	typedef	class_traits<bool_tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&) const {
		// easy, no parameters!
		return bool_type_ptr;
	}
};	// end class data_type_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct data_type_resolver<int_tag> {
	typedef	class_traits<int_tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type& d) const {
		// need to do some real work... 
		// extract parameter from collection
		return d.get_inst_base_subtype()->get_type_ref_subtype();
	}
};	// end class data_type_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct data_type_resolver<enum_tag> {
	typedef	class_traits<enum_tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type& d) const {
		// leverange enum_instance_collection?
		return d.get_inst_base_subtype()->get_type_ref_subtype();
	}
};	// end class data_type_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct data_type_resolver<datastruct_tag> {
	typedef	class_traits<datastruct_tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type& d) const {
		// leverange struct_instance_collection?
		return d.get_inst_base_subtype()->get_type_ref_subtype();
	}
};	// end class data_type_resolver

//=============================================================================
// explicit template instantiations

template class simple_meta_instance_reference<bool_tag>;
template class simple_meta_instance_reference<int_tag>;
template class simple_meta_instance_reference<enum_tag>;
template class simple_meta_instance_reference<datastruct_tag>;

// template class simple_nonmeta_instance_reference<datatype_tag>;
// recently upgraded from simple_nonmeta_instance_reference
template class simple_nonmeta_value_reference<bool_tag>;
template class simple_nonmeta_value_reference<int_tag>;
template class simple_nonmeta_value_reference<enum_tag>;
template class simple_nonmeta_value_reference<datastruct_tag>;

template class member_meta_instance_reference<bool_tag>;
template class member_meta_instance_reference<int_tag>;
template class member_meta_instance_reference<enum_tag>;
template class member_meta_instance_reference<datastruct_tag>;

// and my work is done!
//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_REF_DATA_CC__

