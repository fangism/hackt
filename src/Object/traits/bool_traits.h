/**
	\file "Object/traits/bool_traits.h"
	Traits and policies for boolean data types.  
	This file used to be "Object/art_object_bool_traits.h".
	$Id: bool_traits.h,v 1.1.4.2 2005/07/15 03:49:23 fang Exp $
 */

#ifndef	__OBJECT_TRAITS_BOOL_TRAITS_H__
#define	__OBJECT_TRAITS_BOOL_TRAITS_H__

#include "Object/traits/class_traits.h"

namespace ART {
namespace entity {
template <class> class null_collection_type_manager;
//-----------------------------------------------------------------------------
template <>
struct class_traits<bool_tag> {
	typedef	bool_tag			tag_type;
	static const char			tag_name[];
	typedef	bool_instance			instance_type;
	typedef	bool_instance_alias_base	instance_alias_base_type;
	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	typedef	instance_alias_info_empty
					instance_alias_relaxed_actuals_type;
	static const bool		has_substructure = false;
	template <size_t D>
	struct instance_alias {
		typedef	entity::instance_alias<tag_type,D>	type;
	};

	typedef	bool_instance_collection	instance_collection_generic_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	typedef	null_collection_type_manager<tag_type>
					collection_type_manager_parent_type;
	template <size_t D>
	struct instance_array {
		typedef	entity::instance_array<tag_type,D>	type;
	};

	// later add instantiation_statement support...
	/**
		Unit of state storage for boolean data.
	 */
	typedef	bool_value_type			data_value_type;
	typedef	bool_expr			data_expr_base_type;
	typedef	simple_bool_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_bool_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	bool_instance_reference_base	
					nonmeta_instance_reference_base_type;
	typedef	bool_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	bool_instance_reference_base
				nonmeta_instance_reference_parent_type;
	typedef	bool_member_meta_instance_reference
				member_simple_meta_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_base_ptr_type>
						alias_collection_type;
	typedef	bool_alias_connection		alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
	typedef	null_parameter_type		instance_collection_parameter_type;
	typedef	data_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;

	/**
		Built-in static definition. 
	 */
	static const built_in_datatype_def	built_in_definition;
	/**
		Built-in type pointer.  
		This is defined and initialized in
		"Object/art_built_in.cc"
	 */
	static const type_ref_ptr_type		built_in_type_ptr;
};	// end struct class_traits<bool_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TRAITS_BOOL_TRAITS_H__

