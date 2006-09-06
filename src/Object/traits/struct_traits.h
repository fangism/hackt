/**
	\file "Object/traits/struct_traits.h"
	Traits and policies for data structs.  
	This file used to be "Object/art_object_struct_traits.h".
	$Id: struct_traits.h,v 1.15.8.2 2006/09/06 04:20:05 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_STRUCT_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_STRUCT_TRAITS_H__

#include "Object/traits/class_traits.h"
#include "Object/traits/type_tag_enum.h"
#include "Object/traits/classification_tags_fwd.h"

namespace HAC {
namespace entity {
template <class> class general_collection_type_manager;
//-----------------------------------------------------------------------------
template <>
struct class_traits<datastruct_tag> {
	template <class Tag>
	struct rebind {	typedef	class_traits<Tag>	type; };

	typedef	datastruct_tag			tag_type;
	static const char			tag_name[];
	enum { type_tag_enum_value = TYPE_STRUCT };
	typedef	struct_instance			instance_type;

	typedef	struct_instance_alias_base	instance_alias_base_type;

	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	static const bool		has_substructure = true;
	static const bool		can_internally_alias = false;
	static const bool		has_production_rules = false;
	enum {		is_nonmeta_data_lvalue = true		};

	/**
		The substructures that data types may contain.  
	 */
	template <class>
	struct may_contain;

	/**
		Update:
		Datatypes can no longer take relaxed parameters.  
	 */
	typedef	instance_alias_info_empty
					instance_alias_relaxed_actuals_type;
	typedef	datastruct_instance_alias_info	instance_alias_info_type;
	/// defined in "Object/inst/struct_instance.h"
	class state_instance_base;

	template <size_t D>
	struct instance_alias {
		typedef	entity::instance_alias<tag_type,D>	type;
	};
	enum { instance_pool_chunk_size = 64 };

	typedef	struct_instance_collection	instance_collection_generic_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
#if USE_INSTANCE_PLACEHOLDERS
	typedef	datatype_instance_placeholder	instance_placeholder_parent_type;
	typedef	struct_instance_placeholder	instance_placeholder_type;
#endif
	typedef	general_collection_type_manager<tag_type>
					collection_type_manager_parent_type;
	template <size_t D>
	struct instance_array {
		typedef	entity::instance_array<tag_type,D>	type;
	};

	typedef	struct_expr			data_expr_base_type;
	typedef	struct_value_type		data_value_type;

	// later add instantiation_statement support...
	typedef	data_instantiation_statement	instantiation_statement_type;

	typedef	simple_datastruct_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_datastruct_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	struct_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	data_nonmeta_instance_reference
					nonmeta_instance_reference_base_type;
	typedef	datastruct_member_meta_instance_reference
				member_simple_meta_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_base_ptr_type>
						alias_collection_type;
	typedef	datastruct_alias_connection	alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
	// need real type here!
	typedef	data_type_reference		type_ref_type;
	typedef	canonical_user_def_data_type	instance_collection_parameter_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
#if USE_RESOLVED_DATA_TYPES
	typedef	canonical_generic_datatype	resolved_type_ref_type;
	// pointer not necessary
#endif
};	// end struct class_traits<datastruct_tag>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Macro for specialization of may_contains closure. 
 */
#define DATASTRUCTS_MAY_CONTAIN(Tag, _val)				\
template <>								\
struct class_traits<datastruct_tag>::may_contain<Tag> {			\
	enum { value = _val };						\
};

DATASTRUCTS_MAY_CONTAIN(bool_tag, true)
DATASTRUCTS_MAY_CONTAIN(int_tag, true)
DATASTRUCTS_MAY_CONTAIN(enum_tag, true)
DATASTRUCTS_MAY_CONTAIN(datastruct_tag, true)
DATASTRUCTS_MAY_CONTAIN(channel_tag, false)
DATASTRUCTS_MAY_CONTAIN(process_tag, false)

#undef  DATASTRUCTS_MAY_CONTAIN

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_STRUCT_TRAITS_H__

