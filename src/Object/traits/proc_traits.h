/**
	\file "Object/traits/proc_traits.h"
	Traits and policies for processes.  
	This file used to be "Object/art_object_proc_traits.h".
	$Id: proc_traits.h,v 1.1.4.2 2005/07/15 03:49:26 fang Exp $
 */

#ifndef	__OBJECT_TRAITS_PROC_TRAITS_H__
#define	__OBJECT_TRAITS_PROC_TRAITS_H__

#include "Object/traits/class_traits.h"

namespace ART {
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
	typedef	process_instance		instance_type;

	typedef	process_instance_alias_base	instance_alias_base_type;

	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	static const bool		has_substructure = true;
	typedef	instance_alias_info_actuals
					instance_alias_relaxed_actuals_type;
	template <size_t D>
	struct instance_alias {
		typedef	entity::instance_alias<tag_type,D>	type;
	};

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
	// define this elsewhere, in "traits/inst_stmt_proc.h"
#if 0
	class instantiation_statement_type_ref_base;
#else
	typedef	instantiation_statement_type_ref_default<tag_type>
					instantiation_statement_type_ref_base;
#endif

	typedef	simple_process_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_process_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	process_instance_reference_base	
					nonmeta_instance_reference_base_type;
	typedef	process_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	process_instance_reference_base
				nonmeta_instance_reference_parent_type;
	typedef	process_member_meta_instance_reference
				member_simple_meta_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_base_ptr_type>
						alias_collection_type;
	typedef	process_alias_connection	alias_connection_type;
	typedef	aliases_connection_base		alias_connection_parent_type;
	// need real type here!
	typedef	process_type_reference		type_ref_type;
	typedef	count_ptr<const type_ref_type>	instance_collection_parameter_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
};	// end struct class_traits<process_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TRAITS_PROC_TRAITS_H__

