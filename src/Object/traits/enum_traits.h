/**
	\file "Object/traits/enum_traits.h"
	Traits and policies for enum data types.  
	This file used to be "Object/art_object_enum_traits.h".
	$Id: enum_traits.h,v 1.4.4.1 2005/10/05 23:10:21 fang Exp $
 */

#ifndef	__OBJECT_TRAITS_ENUM_TRAITS_H__
#define	__OBJECT_TRAITS_ENUM_TRAITS_H__

#include "Object/traits/class_traits.h"

namespace ART {
namespace entity {
template <class> class parameterless_collection_type_manager;
//-----------------------------------------------------------------------------
template <>
struct class_traits<enum_tag> {
	typedef	enum_tag			tag_type;
	static const char			tag_name[];
	typedef	enum_instance			instance_type;

	typedef	enum_instance_alias_base	instance_alias_base_type;

	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	static const bool		has_substructure = false;
	static const bool		can_internally_alias = false;
	static const bool		has_production_rules = false;
	typedef	instance_alias_info_empty
					instance_alias_relaxed_actuals_type;
	typedef	enum_instance_alias_info	instance_alias_info_type;
	/// defined in "Object/inst/enum_instance_collection.h"
	class state_instance_base;
	template <size_t D>
	struct instance_alias {
		typedef	entity::instance_alias<tag_type,D>	type;
	};
	enum { instance_pool_chunk_size = 256 };

	typedef	enum_instance_collection	instance_collection_generic_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	typedef	parameterless_collection_type_manager<tag_type>
					collection_type_manager_parent_type;
	template <size_t D>
	struct instance_array {
		typedef	entity::instance_array<tag_type,D>	type;
	};

	typedef	enum_expr			data_expr_base_type;
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

#endif	// __OBJECT_TRAITS_ENUM_TRAITS_H__

