/**
	\file "Object/traits/enum_traits.hh"
	Traits and policies for enum data types.  
	This file used to be "Object/art_object_enum_traits.h".
	$Id: enum_traits.hh,v 1.20 2008/10/05 23:00:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_ENUM_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_ENUM_TRAITS_H__

#include "Object/traits/class_traits.hh"

namespace HAC {
namespace entity {
template <class> class parameterless_collection_type_manager;
//-----------------------------------------------------------------------------
template <>
struct class_traits<enum_tag> {
	template <class Tag>
	struct rebind {	typedef	class_traits<Tag>	type; };

	typedef	enum_tag			tag_type;
	static const char			tag_name[];
	enum { type_tag_enum_value = META_TYPE_ENUM };
	typedef	enum_instance			instance_type;

	typedef	enum_instance_alias_info	instance_alias_info_type;

	typedef	never_ptr<instance_alias_info_type>
						instance_alias_info_ptr_type;
	static const bool		has_substructure = false;
	static const bool		can_internally_alias = false;
	static const bool		has_production_rules = false;
	static const bool		has_CHP = false;
	typedef	null_connect_policy		connection_policy;
	enum {		is_nonmeta_data_lvalue = true		};
	typedef	instance_alias_info_empty
					instance_alias_relaxed_actuals_type;
	/// defined in "Object/inst/enum_instance_collection.hh"
	struct state_instance_base;

	enum { instance_pool_chunk_size = 256 };

	typedef	enum_instance_collection	instance_collection_generic_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	typedef	datatype_instance_placeholder
					instance_placeholder_parent_type;
	typedef	enum_instance_placeholder	instance_placeholder_type;
	typedef	parameterless_collection_type_manager<tag_type>
					collection_type_manager_parent_type;
	template <size_t D>
	struct instance_array {
		typedef	entity::instance_array<tag_type,D>	type;
	};

	/**
		Temporary, not really used yet.
		TODO: decide how to interpret and pass enums
	 */
	typedef	pint_const			const_expr_type;
	typedef	enum_expr			data_expr_base_type;
	typedef	enum_value_type			data_value_type;

	typedef	instantiation_statement_base	instantiation_statement_parent_type;
	typedef	data_instantiation_statement	instantiation_statement_type;

	typedef	simple_enum_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_enum_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	enum_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	data_nonmeta_instance_reference
					nonmeta_instance_reference_base_type;
	typedef	enum_member_meta_instance_reference
				member_simple_meta_instance_reference_type;
	typedef	packed_array_generic<pint_value_type,
			never_ptr<instance_alias_info_type> >
						alias_collection_type;
	typedef	enum_alias_connection		alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
	typedef	never_ptr<const enum_datatype_def>
						instance_collection_parameter_type;
	typedef	data_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
	typedef	canonical_generic_datatype	resolved_type_ref_type;
	// pointer not necessary

	enum { print_cflat_leaf = false };

};	// end struct class_traits<enum_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_ENUM_TRAITS_H__

