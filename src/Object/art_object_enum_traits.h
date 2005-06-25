/**
	\file "Object/art_object_enum_traits.h"
	Traits and policies for enum data types.  
	$Id: art_object_enum_traits.h,v 1.1.2.1 2005/06/25 21:07:20 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_ENUM_TRAITS_H__
#define	__OBJECT_ART_OBJECT_ENUM_TRAITS_H__

#include "Object/art_object_classification_details.h"

namespace ART {
namespace entity {
//-----------------------------------------------------------------------------
template <>
struct class_traits<enum_tag> {
	typedef	enum_tag			tag_type;
	static const char			tag_name[];
	typedef	enum_instance			instance_type;

	typedef	enum_instance_alias_base	instance_alias_base_type;

	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	template <size_t D>
	struct instance_alias {
		typedef	entity::instance_alias<tag_type,D>	type;
	};

	typedef	enum_instance_collection	instance_collection_generic_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	template <size_t D>
	struct instance_array {
		typedef	entity::instance_array<tag_type,D>	type;
	};

#if 1
	typedef	enum_expr			data_expr_base_type;
#endif
	typedef	enum_value_type			data_value_type;

	// later add instantiation_statement support...

	typedef	simple_enum_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_enum_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	enum_instance_reference_base	
					nonmeta_instance_reference_base_type;
	typedef	enum_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	enum_instance_reference_base
				nonmeta_instance_reference_parent_type;
	typedef	enum_member_meta_instance_reference
				member_simple_meta_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_base_ptr_type>
						alias_collection_type;
	typedef	enum_alias_connection		alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
	typedef	never_ptr<const enum_datatype_def>
						instance_collection_parameter_type;
	typedef	data_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
};	// end struct class_traits<enum_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_ENUM_TRAITS_H__

