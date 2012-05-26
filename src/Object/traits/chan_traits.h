/**
	\file "Object/traits/chan_traits.h"
	Traits and policies for channels.  
	This file used to be "Object/art_object_chan_traits.h".
	$Id: chan_traits.h,v 1.22 2008/10/05 23:00:26 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_CHAN_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_CHAN_TRAITS_H__

#include "Object/traits/class_traits.h"
#include "Object/traits/type_tag_enum.h"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {
template <class> class general_collection_type_manager;
#if BUILTIN_CHANNEL_FOOTPRINTS
template <class> class channel_instantiation_type_ref;
class canonical_fundamental_chan_type;
#endif

//-----------------------------------------------------------------------------
/**
	NOTE: recently split off channel type references into 
	built-in and user-defined.  
	Instantiation statements and collections now use
	channel_type_reference_base (abstract) (2005-05-28).
 */
template <>
struct class_traits<channel_tag> {
	template <class Tag>
	struct rebind {	typedef	class_traits<Tag>	type; };

	typedef	channel_tag			tag_type;
	static const char			tag_name[];
	enum { type_tag_enum_value = META_TYPE_CHANNEL };
	typedef	channel_instance		instance_type;

	typedef	channel_instance_alias_info	instance_alias_info_type;

	typedef	never_ptr<instance_alias_info_type>
						instance_alias_info_ptr_type;
#if BUILTIN_CHANNEL_FOOTPRINTS
	static const bool		has_substructure = false;
	/**
		Channels are not printed in cflat-ing, at least not yet.
		Rationale: No legacy tools ever flattened CHP channels.  
	 */
	enum { print_cflat_leaf = false };
#else
	static const bool		has_substructure = true;
#endif
	static const bool		can_internally_alias = false;
	static const bool		has_production_rules = false;
	static const bool		has_CHP = true;
	/**
		This uses a specialization that adds direction semantics
		to connections, defined in "Object/inst/connection_policy.h".
	 */
	typedef	channel_connect_policy		connection_policy;

	/**
		Closure for containership, defined by specializations only.  
	 */
	template <class>
	struct may_contain;

	/**
		UPDATE: 
		Channels no longer can take relaxed parameters. 
	 */
	typedef	instance_alias_info_empty
					instance_alias_relaxed_actuals_type;
	/// defined in "Object/inst/channel_instance.h"
	struct state_instance_base;
	enum { instance_pool_chunk_size = 128 };

	typedef	channel_instance_collection	instance_collection_generic_type;
	typedef	physical_instance_collection	instance_collection_parent_type;
	typedef	physical_instance_placeholder
					instance_placeholder_parent_type;
	typedef	channel_instance_placeholder	instance_placeholder_type;
#if 0
	// 7.1.5.3 Elaborated type specifiers.
	// If the identifier resolves to a typedef name or template type
	// parameter, the elaborated type specifier is ill-formed.
	typedef	general_collection_type_manager<tag_type>
					collection_type_manager_parent_type;
#else
	// will be derived trivially from general_collection_type_manager
	// this is now defined in "Object/inst/channel_instance_collection.h"
	struct collection_type_manager_parent_type;
#endif
	template <size_t D>
	struct instance_array {
		typedef	entity::instance_array<tag_type,D>	type;
	};

	typedef	instantiation_statement_base
					instantiation_statement_parent_type;
	typedef	channel_instantiation_statement
					instantiation_statement_type;
#if BUILTIN_CHANNEL_FOOTPRINTS
	typedef	channel_instantiation_type_ref<tag_type>
#else
	typedef	instantiation_statement_type_ref_default<tag_type>
#endif
					instantiation_statement_type_ref_base;

	typedef	simple_channel_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_channel_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	channel_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	nonmeta_instance_reference_base
					nonmeta_instance_reference_base_type;
	typedef	channel_member_meta_instance_reference
				member_simple_meta_instance_reference_type;
	typedef	packed_array_generic<pint_value_type,
			never_ptr<instance_alias_info_type> >
						alias_collection_type;
	typedef	channel_alias_connection	alias_connection_type;
	typedef	aliases_connection_base		alias_connection_parent_type;
	// need real type here!
#if BUILTIN_CHANNEL_FOOTPRINTS
	typedef	builtin_channel_type_reference	type_ref_type;
#else
	typedef	channel_type_reference_base	type_ref_type;
#endif
	/**
		Note: may need to split-off built-in and user-defined
		channel types: built-in types have no template
		parameters, they are folded into their sub-data types.  
		It is possible however to derive them both from a common 
		abstract base, like channel_type_reference_base, 
		but that complicates persistence... may have to fake it.  
		TODO: figure this out.
	 */
#if BUILTIN_CHANNEL_FOOTPRINTS
	typedef	canonical_fundamental_chan_type	instance_collection_parameter_type;
#else
	typedef	canonical_generic_chan_type	instance_collection_parameter_type;
//	typedef	canonical_user_def_chan_type	instance_collection_parameter_type;
#endif

	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;

	// same type info as parameter_type
#if BUILTIN_CHANNEL_FOOTPRINTS
	typedef	instance_collection_parameter_type	resolved_type_ref_type;
#else
	typedef	canonical_generic_chan_type	resolved_type_ref_type;
#endif
	// pointer not necessary
};	// end struct class_traits<channel_tag>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define CHANNELS_MAY_CONTAIN(Tag, _val)					\
template <>								\
struct channel_traits::may_contain<Tag> { enum { value = _val }; };

CHANNELS_MAY_CONTAIN(bool_tag, true)
CHANNELS_MAY_CONTAIN(int_tag, true)
CHANNELS_MAY_CONTAIN(enum_tag, true)
CHANNELS_MAY_CONTAIN(datastruct_tag, true)	// false!
CHANNELS_MAY_CONTAIN(channel_tag, true)		// false!
CHANNELS_MAY_CONTAIN(process_tag, false)

#undef  CHANNELS_MAY_CONTAIN

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_CHAN_TRAITS_H__

