/**
	\file "Object/traits/proc_traits.h"
	Traits and policies for processes.  
	This file used to be "Object/art_object_proc_traits.h".
	$Id: proc_traits.h,v 1.18.4.1 2006/11/16 20:28:47 fang Exp $
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
	template <class Tag>
	struct rebind {	typedef	class_traits<Tag>	type; };

	typedef	process_tag			tag_type;
	static const char			tag_name[];
	enum { type_tag_enum_value = META_TYPE_PROCESS };
	typedef	process_instance		instance_type;

	typedef	process_instance_alias_info	instance_alias_info_type;

	typedef	never_ptr<instance_alias_info_type>
						instance_alias_info_ptr_type;
	static const bool		has_substructure = true;
	static const bool		can_internally_alias = true;
	static const bool		has_production_rules = true;
	static const bool		is_connection_directional = false;

	/**
		closure for may-contains, defined by specialization only.
		Unforunately have to define this by hand.  
	 */
	template <class>
	struct may_contain;

	typedef	instance_alias_info_actuals
					instance_alias_relaxed_actuals_type;
	/// defined in "Object/inst/process_instance.h"
	class state_instance_base;
	enum { instance_pool_chunk_size = 256 };

	typedef	process_instance_collection	instance_collection_generic_type;
	typedef	physical_instance_collection	instance_collection_parent_type;
	typedef	physical_instance_placeholder	instance_placeholder_parent_type;
	typedef	process_instance_placeholder	instance_placeholder_type;
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
	typedef	packed_array_generic<pint_value_type,
			never_ptr<instance_alias_info_type> >
						alias_collection_type;
	typedef	process_alias_connection	alias_connection_type;
	typedef	aliases_connection_base		alias_connection_parent_type;
	// need real type here!
	typedef	process_type_reference		type_ref_type;
	typedef	canonical_process_type	instance_collection_parameter_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
	typedef	canonical_process_type		resolved_type_ref_type;
	// pointer not necessary
};	// end struct class_traits<process_tag>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Handy macro for defining closure properties with specialization.  
	Reminder of standard: 14.7.3/18: can specialize an inner template
		for a specialized outer template.  
	note: is equivalent to:
	template <> template <>
	struct class_traits<process_tag>::may_contain<bool_tag>
 */
#define	PROCESSES_MAY_CONTAIN(Tag, _val)				\
template <>								\
struct process_traits::may_contain<Tag> { enum { value = _val }; };

PROCESSES_MAY_CONTAIN(bool_tag, true)
PROCESSES_MAY_CONTAIN(int_tag, true)
PROCESSES_MAY_CONTAIN(enum_tag, true)
PROCESSES_MAY_CONTAIN(datastruct_tag, true)
PROCESSES_MAY_CONTAIN(channel_tag, true)
PROCESSES_MAY_CONTAIN(process_tag, true)

#undef	PROCESSES_MAY_CONTAIN

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_PROC_TRAITS_H__

