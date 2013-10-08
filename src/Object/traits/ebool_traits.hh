/**
	\file "Object/traits/ebool_traits.hh"
	Traits and policies for atomic boolean expression data types.  
	Re-use as much of bool_traits as possible
 */

#ifndef	__HAC_OBJECT_TRAITS_EBOOL_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_EBOOL_TRAITS_H__

#include "Object/traits/class_traits.hh"
#include "Object/traits/bool_traits.hh"

namespace HAC {
namespace entity {
template <class> class null_collection_type_manager;

//-----------------------------------------------------------------------------
template <>
struct class_traits<ebool_tag> : public class_traits<bool_tag> {
	template <class Tag>
	struct rebind {	typedef	class_traits<Tag>	type; };

	typedef	ebool_tag			tag_type;
	/// inherits some traits from corresponding meta-type
	typedef	pbool_tag			meta_tag_type;
	typedef	class_traits<meta_tag_type>	meta_traits_type;
//	enum { type_tag_enum_value = META_TYPE_EBOOL };
	static const meta_type_tag_enum		type_tag_enum_value = META_TYPE_EBOOL;
	static const char			tag_name[];
#if 0
	typedef	bool_instance			instance_type;
	typedef	bool_instance_alias_info	instance_alias_info_type;
	typedef	never_ptr<instance_alias_info_type>
						instance_alias_info_ptr_type;
	typedef	instance_alias_info_empty
					instance_alias_relaxed_actuals_type;
	static const bool		has_substructure = false;
	static const bool		can_internally_alias = false;
	static const bool		has_production_rules = false;
	static const bool		has_CHP = false;
	typedef	bool_connect_policy		connection_policy;
	enum {		is_nonmeta_data_lvalue = true		};
	/// defined in "Object/inst/bool_instance_collection.h"
	struct state_instance_base;

	template <size_t D>
	struct value_array {
		typedef	typename meta_traits_type::
			template value_array<D>::type		type;
	};
	enum { instance_pool_chunk_size = 1024 };
#endif
	// yes, use bool, not ebool
	typedef	bool_instance_collection	instance_collection_generic_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	typedef	datatype_instance_placeholder
					instance_placeholder_parent_type;
	typedef	ebool_instance_placeholder	instance_placeholder_type;
	typedef	null_collection_type_manager<bool_tag>
					collection_type_manager_parent_type;
	template <size_t D>
	struct instance_array {
		// yes, use bool_tag
		typedef	entity::instance_array<bool_tag,D>	type;
	};

#if 0
	// TODO: proper subtyping with specialization
	typedef	instantiation_statement_base	instantiation_statement_parent_type;
	typedef	data_instantiation_statement	instantiation_statement_type;
	/**
		Unit of state storage for boolean data.
	 */
	typedef	bool_value_type			data_value_type;
	typedef	bool_expr			data_expr_base_type;
	typedef	meta_traits_type::const_expr_type	const_expr_type;
	typedef	simple_bool_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_bool_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	bool_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	data_nonmeta_instance_reference
					nonmeta_instance_reference_base_type;
	typedef	bool_member_meta_instance_reference
				member_simple_meta_instance_reference_type;
	typedef	packed_array_generic<pint_value_type,
			never_ptr<instance_alias_info_type> >
						alias_collection_type;
	typedef	bool_alias_connection		alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
	typedef	null_parameter_type		instance_collection_parameter_type;
	typedef	data_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
	typedef	canonical_generic_datatype	resolved_type_ref_type;

	enum { print_cflat_leaf = true };
#else
// inherit from bool_traits
#endif

	/**
		Built-in static definition. 
	 */
	static const built_in_datatype_def	built_in_definition;
	/**
		Built-in type pointer.  
		This is defined and initialized in
		"Object/traits/class_traits_types.cc"
	 */
	static const type_ref_ptr_type		built_in_type_ptr;
	static const type_ref_ptr_type&		nonmeta_data_type_ptr;
};	// end struct class_traits<ebool_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_EBOOL_TRAITS_H__

