/**
	\file "Object/traits/int_traits.h"
	Traits and policies for data type integers.  
	This file used to be "Object/art_object_int_traits.h".
	$Id: int_traits.h,v 1.3 2005/08/08 16:51:11 fang Exp $
 */

#ifndef	__OBJECT_TRAITS_INT_TRAITS_H__
#define	__OBJECT_TRAITS_INT_TRAITS_H__

#include "Object/traits/class_traits.h"

namespace ART {
namespace entity {
template <class> class int_collection_type_manager;
//-----------------------------------------------------------------------------
template <>
struct class_traits<int_tag> {
	typedef	int_tag				tag_type;
	static const char			tag_name[];
	typedef	int_instance			instance_type;
	typedef	int_instance_alias_base		instance_alias_base_type;
	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	static const bool		has_substructure = false;
	typedef	instance_alias_info_empty
					instance_alias_relaxed_actuals_type;
	typedef	int_instance_alias_info	instance_alias_info_type;
	/// defined in "Object/inst/int_instance_collection.h"
	class state_instance_base;
	template <size_t D>
	struct instance_alias {
		typedef	entity::instance_alias<tag_type,D>	type;
	};

	typedef	int_instance_collection	instance_collection_generic_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	typedef	int_collection_type_manager<tag_type>
					collection_type_manager_parent_type;
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

	/**
		Built-in static integer datatype definition.
		NOTE (2005-07-10): its type cache will updated
		as integers are instantiated.
	 */
	static const built_in_datatype_def	built_in_definition;

	/**
		One hard-coded copy of the default integer pointer type.  
		This built-in type pointer is not a substitute
		for 'built_in_type_ptr' because the definition is a template.  
	 */
	static const type_ref_ptr_type		int32_type_ptr;
};	// end struct class_traits<int_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TRAITS_INT_TRAITS_H__

