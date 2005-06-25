/**
	\file "Object/art_object_int_traits.h"
	Traits and policies for data type integers.  
	$Id: art_object_int_traits.h,v 1.1.2.1 2005/06/25 21:07:25 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INT_TRAITS_H__
#define	__OBJECT_ART_OBJECT_INT_TRAITS_H__

#include "Object/art_object_classification_details.h"

namespace ART {
namespace entity {
//-----------------------------------------------------------------------------
template <>
struct class_traits<int_tag> {
	typedef	int_tag				tag_type;
	static const char			tag_name[];
	typedef	int_instance			instance_type;
	typedef	int_instance_alias_base		instance_alias_base_type;
	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	template <size_t D>
	struct instance_alias {
		typedef	entity::instance_alias<tag_type,D>	type;
	};

	typedef	int_instance_collection	instance_collection_generic_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	template <size_t D>
	struct instance_array {
		typedef	entity::instance_array<tag_type,D>	type;
	};

	// later add instantiation_statement support...

	/**
		Unit of state storage for integer data.
		TODO: int is only temporary, this will have to become	
		multiprecision to support arbitrary length, 
		like mpz from GMP.  
	 */
	typedef	int_value_type			data_value_type;
	typedef	int_expr			data_expr_base_type;
	typedef	simple_int_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_int_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	int_instance_reference_base	
					nonmeta_instance_reference_base_type;
	typedef	int_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	int_instance_reference_base
				nonmeta_instance_reference_parent_type;
	typedef	int_member_meta_instance_reference
				member_simple_meta_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_base_ptr_type>
						alias_collection_type;
	typedef	int_alias_connection		alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
	typedef	pint_value_type			instance_collection_parameter_type;
	typedef	data_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
};	// end struct class_traits<int_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INT_TRAITS_H__

