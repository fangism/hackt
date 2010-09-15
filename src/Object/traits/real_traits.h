/**
	\file "Object/traits/real_traits.h"
	Traits and policies for data type reals.  
	$Id: real_traits.h,v 1.3.30.1 2010/09/15 00:57:57 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_REAL_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_REAL_TRAITS_H__

#include "Object/traits/class_traits.h"
#include "Object/traits/preal_traits.h"

namespace HAC {
namespace entity {
// unless we parameterize by exponent and mantissa bits?
template <class> class null_collection_type_manager;
//-----------------------------------------------------------------------------
template <>
struct class_traits<real_tag> {
	template <class Tag>
	struct rebind {	typedef	class_traits<Tag>	type; };

	typedef	real_tag				tag_type;
	/// inherits some traits from corresponding meta-type
	typedef	preal_tag			meta_tag_type;
	typedef	class_traits<meta_tag_type>	meta_traits_type;
	static const char			tag_name[];
	enum { type_tag_enum_value = META_TYPE_INT };
	typedef	real_instance			instance_type;
	typedef	real_instance_alias_info	instance_alias_info_type;
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
	/// defined in "Object/inst/int_instance_collection.h"
	class state_instance_base;

	template <size_t D>
	struct value_array {
		typedef typename meta_traits_type::
			template value_array<D>::type	type;
	};

	enum { instance_pool_chunk_size = 512 };

	typedef	real_instance_collection
					instance_collection_generic_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	typedef	datatype_instance_placeholder
					instance_placeholder_parent_type;
	typedef	real_instance_placeholder	instance_placeholder_type;
	typedef	null_collection_type_manager<tag_type>
					collection_type_manager_parent_type;
	template <size_t D>
	struct instance_array {
		typedef	entity::instance_array<tag_type,D>	type;
	};

	typedef	instantiation_statement_base	instantiation_statement_parent_type;
	typedef	data_instantiation_statement	instantiation_statement_type;

	/**
		Unit of state storage for floating-point data.
		TODO: real is only temporary, this will have to become	
		multiprecision to support arbitrary length, 
		like mpz from GMP.  
	 */
	typedef	real_value_type			data_value_type;
	typedef	real_expr			data_expr_base_type;
	typedef	meta_traits_type::const_expr_type	const_expr_type;
	typedef	simple_real_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_real_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	real_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	data_nonmeta_instance_reference
					nonmeta_instance_reference_base_type;
	typedef	real_member_meta_instance_reference
				member_simple_meta_instance_reference_type;
	typedef	packed_array_generic<pint_value_type,
			never_ptr<instance_alias_info_type> >
						alias_collection_type;
	typedef	real_alias_connection		alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
	/**
		
	 */
	typedef	null_parameter_type	instance_collection_parameter_type;
	typedef	data_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
	typedef	canonical_generic_datatype	resolved_type_ref_type;
	// pointer not necessary

	enum { print_cflat_leaf = false };

	/**
		Built-in static floating-point datatype definition.
		NOTE (2005-07-10): its type cache will updated
		as integers are instantiated.
	 */
	static const built_in_datatype_def	built_in_definition;
	static const type_ref_ptr_type		built_in_type_ptr;
	static const type_ref_ptr_type&		nonmeta_data_type_ptr;
};	// end struct class_traits<real_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_REAL_TRAITS_H__

