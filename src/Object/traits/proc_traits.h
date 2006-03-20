/**
	\file "Object/traits/proc_traits.h"
	Traits and policies for processes.  
	This file used to be "Object/art_object_proc_traits.h".
	$Id: proc_traits.h,v 1.10.4.3 2006/03/20 01:06:22 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_PROC_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_PROC_TRAITS_H__

#include "Object/traits/class_traits.h"
#include "Object/traits/type_tag_enum.h"

namespace HAC {
namespace entity {
template <class> class general_collection_type_manager;
//-----------------------------------------------------------------------------
/**
	Class traits specialization for processes.  
 */
template <>
struct class_traits<process_tag> {
	typedef	process_tag			tag_type;
	static const char			tag_name[];
	enum { type_tag_enum_value = TYPE_PROCESS };
	typedef	process_instance		instance_type;

	typedef	process_instance_alias_base	instance_alias_base_type;

	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	static const bool		has_substructure = true;
	static const bool		can_internally_alias = true;
	static const bool		has_production_rules = true;
	typedef	instance_alias_info_actuals
					instance_alias_relaxed_actuals_type;
	typedef	process_instance_alias_info	instance_alias_info_type;
	/// defined in "Object/inst/process_instance.h"
	class state_instance_base;
	template <size_t D>
	struct instance_alias {
		typedef	entity::instance_alias<tag_type,D>	type;
	};
	enum { instance_pool_chunk_size = 256 };

	typedef	process_instance_collection	instance_collection_generic_type;
	typedef	physical_instance_collection	instance_collection_parent_type;
	typedef	general_collection_type_manager<tag_type>
					collection_type_manager_parent_type;
	template <size_t D>
	struct instance_array {
		typedef	entity::instance_array<tag_type,D>	type;
	};

	typedef	instantiation_statement_base
					instantiation_statement_parent_type;
	typedef	process_instantiation_statement
					instantiation_statement_type;
	typedef	instantiation_statement_type_ref_default<tag_type>
					instantiation_statement_type_ref_base;

	typedef	simple_process_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_process_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	process_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	nonmeta_instance_reference_base
					nonmeta_instance_reference_base_type;
	typedef	process_member_meta_instance_reference
				member_simple_meta_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_base_ptr_type>
						alias_collection_type;
	typedef	process_alias_connection	alias_connection_type;
	typedef	aliases_connection_base		alias_connection_parent_type;
	// need real type here!
	typedef	process_type_reference		type_ref_type;
	typedef	canonical_process_type	instance_collection_parameter_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
};	// end struct class_traits<process_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_PROC_TRAITS_H__

