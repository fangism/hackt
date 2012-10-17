/**
	\file "Object/traits/node_traits.hh"
	Traits and policies for internal nodes.  
	$Id: node_traits.hh,v 1.3 2008/10/05 23:00:30 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_NODE_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_NODE_TRAITS_H__

#include "Object/traits/class_traits.hh"

namespace HAC {
namespace entity {
template <class> class null_collection_type_manager;
//-----------------------------------------------------------------------------
template <>
struct class_traits<node_tag> {
	template <class Tag>
	struct rebind {	typedef	class_traits<Tag>	type; };

	typedef	node_tag			tag_type;
#if 0
	/// inherits some traits from corresponding meta-type
	typedef	pbool_tag			meta_tag_type;
	typedef	class_traits<meta_tag_type>	meta_traits_type;
	enum { type_tag_enum_value = META_TYPE_BOOL };
#endif
	static const char			tag_name[];
#if 0
	typedef	node_instance			instance_type;
	typedef	instance_alias_info<node_tag>	instance_alias_info_type;
	typedef	never_ptr<instance_alias_info_type>
						instance_alias_info_ptr_type;
	typedef	instance_alias_info_empty
					instance_alias_relaxed_actuals_type;
#endif

	static const bool		has_substructure = false;
	static const bool		can_internally_alias = false;
	static const bool		has_production_rules = false;
	static const bool		has_CHP = false;
	typedef	null_connect_policy		connection_policy;
#if 0
	enum {		is_nonmeta_data_lvalue = true		};
	/// defined in "Object/inst/bool_instance_collection.hh"
	struct state_instance_base;
	template <size_t D>
	struct value_array {
		typedef	typename meta_traits_type::
			template value_array<D>::type		type;
	};
#endif
	enum { instance_pool_chunk_size = 64 };

	typedef	node_instance_collection	instance_collection_generic_type;
	typedef	physical_instance_collection	instance_collection_parent_type;
	typedef	physical_instance_placeholder
					instance_placeholder_parent_type;
	typedef	node_instance_placeholder	instance_placeholder_type;
#if 0
	typedef	null_collection_type_manager<tag_type>
					collection_type_manager_parent_type;
#endif
	template <size_t D>
	struct instance_array {
		typedef	entity::instance_array<tag_type,D>	type;
	};
#if 0
	// TODO: proper subtyping with specialization
	typedef	instantiation_statement_base	instantiation_statement_parent_type;
	typedef	data_instantiation_statement	instantiation_statement_type;
#endif
#if 0
	/**
		Unit of state storage for boolean data.
	 */
	typedef	bool_value_type			data_value_type;
	typedef	bool_expr			data_expr_base_type;
	typedef	meta_traits_type::const_expr_type	const_expr_type;
	typedef	simple_nonmeta_instance_reference<node_tag>
					simple_nonmeta_instance_reference_type;
#endif
	typedef	simple_node_meta_instance_reference
					simple_meta_instance_reference_type;
#if 0
	typedef	meta_instance_reference<node_tag>
				meta_instance_reference_parent_type;
	typedef	data_nonmeta_instance_reference
					nonmeta_instance_reference_base_type;
	typedef	member_meta_instance_reference<node_tag>
				member_simple_meta_instance_reference_type;
#endif
#if 0
	typedef	packed_array_generic<pint_value_type,
			never_ptr<instance_alias_info_type> >
						alias_collection_type;
	typedef	bool_alias_connection		alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
#endif
#if 0
	typedef	null_parameter_type		instance_collection_parameter_type;
#endif
	typedef	data_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
#if 0
	typedef	canonical_generic_datatype	resolved_type_ref_type;
#endif
	enum { print_cflat_leaf = false };	// maybe?

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
#if 0
	static const type_ref_ptr_type&		nonmeta_data_type_ptr;
#endif
};	// end struct class_traits<bool_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_NODE_TRAITS_H__

