/**
	\file "Object/traits/struct_traits.h"
	Traits and policies for data structs.  
	This file used to be "Object/art_object_struct_traits.h".
	$Id: struct_traits.h,v 1.3 2005/08/08 16:51:11 fang Exp $
 */

#ifndef	__OBJECT_TRAITS_STRUCT_TRAITS_H__
#define	__OBJECT_TRAITS_STRUCT_TRAITS_H__

#include "Object/traits/class_traits.h"

namespace ART {
namespace entity {
template <class> class general_collection_type_manager;
//-----------------------------------------------------------------------------
template <>
struct class_traits<datastruct_tag> {
	typedef	datastruct_tag			tag_type;
	static const char			tag_name[];
	typedef	struct_instance			instance_type;

	typedef	struct_instance_alias_base	instance_alias_base_type;

	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	static const bool		has_substructure = true;
	typedef	instance_alias_info_actuals
					instance_alias_relaxed_actuals_type;
	typedef	datastruct_instance_alias_info	instance_alias_info_type;
	/// defined in "Object/inst/struct_instance_collection.h"
	class state_instance_base;
	template <size_t D>
	struct instance_alias {
		typedef	entity::instance_alias<tag_type,D>	type;
	};

	typedef	struct_instance_collection	instance_collection_generic_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	typedef	general_collection_type_manager<tag_type>
					collection_type_manager_parent_type;
	template <size_t D>
	struct instance_array {
		typedef	entity::instance_array<tag_type,D>	type;
	};

#if 1
	typedef	struct_expr			data_expr_base_type;
	typedef	struct_value_type		data_value_type;
#endif

	// later add instantiation_statement support...

	typedef	simple_datastruct_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_datastruct_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	struct_instance_reference_base	
					nonmeta_instance_reference_base_type;
	typedef	struct_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	struct_instance_reference_base
				nonmeta_instance_reference_parent_type;
	typedef	datastruct_member_meta_instance_reference
				member_simple_meta_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_base_ptr_type>
						alias_collection_type;
	typedef	datastruct_alias_connection	alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
	// need real type here!
	typedef	data_type_reference		type_ref_type;
	typedef	count_ptr<const type_ref_type>	instance_collection_parameter_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
};	// end struct class_traits<datastruct_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TRAITS_STRUCT_TRAITS_H__

