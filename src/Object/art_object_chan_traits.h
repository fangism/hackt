/**
	\file "Object/art_object_chan_traits.h"
	Traits and policies for channels.  
	$Id: art_object_chan_traits.h,v 1.1.2.1 2005/06/25 21:07:18 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_CHAN_TRAITS_H__
#define	__OBJECT_ART_OBJECT_CHAN_TRAITS_H__

#include "Object/art_object_classification_details.h"

namespace ART {
namespace entity {
//-----------------------------------------------------------------------------
/**
	NOTE: recently split off channel type references into 
	built-in and user-defined.  
	Instantiation statements and collections now use
	channel_type_reference_base (abstract) (2005-05-28).
 */
template <>
struct class_traits<channel_tag> {
	typedef	channel_tag			tag_type;
	static const char			tag_name[];
	typedef	channel_instance		instance_type;

	typedef	channel_instance_alias_base	instance_alias_base_type;

	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	template <size_t D>
	struct instance_alias {
		typedef	entity::instance_alias<tag_type,D>	type;
	};

	typedef	channel_instance_collection	instance_collection_generic_type;
	typedef	physical_instance_collection	instance_collection_parent_type;
	template <size_t D>
	struct instance_array {
		typedef	entity::instance_array<tag_type,D>	type;
	};

	typedef	instantiation_statement_base
					instantiation_statement_parent_type;
	typedef	channel_instantiation_statement
					instantiation_statement_type;
	// define this elsewhere, in "art_object_inst_stmt_chan.h"
#if 1
	class instantiation_statement_type_ref_base;
#else
	// not until channel_type_reference_base::unroll_resolve is done
	typedef	instantiation_statement_type_ref_default<tag_type>
					instantiation_statement_type_ref_base;
#endif

	typedef	simple_channel_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_channel_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	channel_instance_reference_base	
					nonmeta_instance_reference_base_type;
	typedef	channel_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	channel_instance_reference_base
				nonmeta_instance_reference_parent_type;
	typedef	channel_member_meta_instance_reference
				member_simple_meta_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_base_ptr_type>
						alias_collection_type;
	typedef	channel_alias_connection	alias_connection_type;
	typedef	aliases_connection_base		alias_connection_parent_type;
	// need real type here!
	typedef	channel_type_reference_base	type_ref_type;
	typedef	count_ptr<const type_ref_type>	instance_collection_parameter_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
};	// end struct class_traits<channel_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_CHAN_TRAITS_H__

