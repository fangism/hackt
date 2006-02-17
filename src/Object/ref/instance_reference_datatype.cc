/**
	\file "Object/ref/instance_reference_datatype.cc"
	Method definitions for datatype instance reference classes.
	This file was reincarnated from "Object/art_object_inst_ref_data.cc".
	$Id: instance_reference_datatype.cc,v 1.5.18.1 2006/02/17 05:07:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_INSTANCE_REFERENCE_DATATYPE_CC__
#define	__HAC_OBJECT_REF_INSTANCE_REFERENCE_DATATYPE_CC__

#define	ENABLE_STACKTRACE			0

#include "Object/inst/alias_empty.h"
#include "Object/inst/bool_instance_collection.h"
#include "Object/inst/int_instance_collection.h"
#include "Object/inst/enum_instance_collection.h"
#include "Object/inst/struct_instance_collection.h"
#include "Object/unroll/alias_connection.h"
#include "Object/unroll/data_alias_connection_base.h"
#include "Object/ref/simple_meta_instance_reference.tcc"
#include "Object/ref/simple_nonmeta_instance_reference.tcc"
#include "Object/ref/member_meta_instance_reference.tcc"
#include "Object/ref/simple_nonmeta_value_reference.tcc"
#include "Object/expr/int_expr.h"
#include "Object/expr/bool_expr.h"
#include "Object/expr/enum_expr.h"
#include "Object/expr/struct_expr.h"
#include "Object/expr/const_range.h"

// introduced by canonical_type
#include "Object/def/user_def_datatype.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/process_definition.h"

#include "Object/persistent_type_hash.h"
#include "util/persistent_object_manager.tcc"
#include "Object/traits/class_traits.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/inst/null_collection_type_manager.h"
#include "Object/inst/int_collection_type_manager.h"
#include "Object/inst/parameterless_collection_type_manager.h"
#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
#include "Object/ref/meta_instance_reference_subtypes.tcc"
#endif

#if SUBTYPE_PORT_CONNECTION
#include "Object/unroll/port_connection_base.h"
#endif

namespace util {
using HAC::entity::int_tag;
using HAC::entity::bool_tag;

SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_int_meta_instance_reference, "int-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_bool_meta_instance_reference, "bool-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_enum_meta_instance_reference, "enum-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_datastruct_meta_instance_reference,
							"struct-inst-ref")

#if 0
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_datatype_nonmeta_instance_reference,
		"data-nonmeta-inst-ref")
#endif
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_int_nonmeta_instance_reference,
		"int-nonmeta-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_bool_nonmeta_instance_reference,
		"bool-nonmeta-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_enum_nonmeta_instance_reference,
		"enum-nonmeta-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_datastruct_nonmeta_instance_reference,
		"struct-nonmeta-inst-ref")

SPECIALIZE_UTIL_WHAT(
	HAC::entity::int_member_meta_instance_reference,
		"int-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::bool_member_meta_instance_reference,
		"bool-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::enum_member_meta_instance_reference,
		"enum-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::datastruct_member_meta_instance_reference,
		"struct-member-inst-ref")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_int_meta_instance_reference, 
		SIMPLE_DINT_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_bool_meta_instance_reference, 
		SIMPLE_DBOOL_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_enum_meta_instance_reference, 
		SIMPLE_ENUM_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_datastruct_meta_instance_reference, 
		SIMPLE_STRUCT_META_INSTANCE_REFERENCE_TYPE_KEY, 0)

#if 0
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_datatype_nonmeta_instance_reference, 
		SIMPLE_DATATYPE_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
#endif
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_int_nonmeta_instance_reference, 
		SIMPLE_DINT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_bool_nonmeta_instance_reference, 
		SIMPLE_DBOOL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_enum_nonmeta_instance_reference, 
		SIMPLE_ENUM_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_datastruct_nonmeta_instance_reference, 
		SIMPLE_STRUCT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::int_member_meta_instance_reference, 
		MEMBER_DINT_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::bool_member_meta_instance_reference, 
		MEMBER_DBOOL_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::enum_member_meta_instance_reference, 
		MEMBER_ENUM_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::datastruct_member_meta_instance_reference, 
		MEMBER_STRUCT_INSTANCE_REFERENCE_TYPE_KEY, 0)
}	// end namespace util

namespace HAC {
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
		return bool_traits::built_in_type_ptr;
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

#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
template class meta_instance_reference<bool_tag>;
template class meta_instance_reference<int_tag>;
template class meta_instance_reference<enum_tag>;
template class meta_instance_reference<datastruct_tag>;
#endif

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
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_INSTANCE_REFERENCE_DATATYPE_CC__

