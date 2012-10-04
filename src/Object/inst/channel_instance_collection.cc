/**
	\file "Object/inst/channel_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originated from "Object/art_object_instance_chan.cc"
		in a previous life.  
	$Id: channel_instance_collection.cc,v 1.19 2011/04/02 01:46:00 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_DESTRUCTORS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_CONSTRUCTORS		(0 && ENABLE_STACKTRACE)

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/channel_instance_collection.h"
#include "Object/inst/alias_empty.tcc"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/def/definition_base.h"
#include "Object/def/user_def_chan.h"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/type/builtin_channel_type_reference.h"
#else
#include "Object/type/channel_type_reference.h"
#endif
#include "Object/persistent_type_hash.h"
#include "Object/inst/instance_collection.tcc"
#include "Object/inst/instance_placeholder.tcc"
#include "Object/inst/instance_alias.tcc"
#include "Object/inst/connection_policy.tcc"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/inst/channel_collection_type_manager.tcc"
#else
#include "Object/inst/general_collection_type_manager.tcc"
#endif
#include "Object/inst/state_instance.tcc"
#include "Object/def/datatype_definition_base.h"
#include "Object/inst/alias_empty.h"	// why is this needed?
#include "Object/global_channel_entry.h"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/unroll/channel_instantiation_type_ref_base.h"
#else
#include "Object/unroll/instantiation_statement_type_ref_default.h"
#endif

namespace util {

	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_instance_collection,
		"channel_instance_collection")
	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_port_formal_array,
		"channel_port_formal_array")
	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_scalar, "channel_scalar")
	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_array_1D, "channel_array_1D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_array_2D, "channel_array_2D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_array_3D, "channel_array_3D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_array_4D, "channel_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_instance_placeholder, 
	CHANNEL_INSTANCE_PLACEHOLDER_TYPE_KEY, 0)

// explicit template instantiation
using namespace HAC::entity;
// from Object/traits/chan_traits.h: alias_collection_type
template class packed_array_generic<pint_value_type,
		memory::never_ptr<instance_alias_info<channel_tag> > >;
}	// end namespace util

namespace HAC {
namespace entity {
//=============================================================================
// explicit template class instantiations

template class instance_placeholder<channel_tag>;
template class state_instance<channel_tag>;
template class instance_pool<state_instance<channel_tag> >;
template class instance_alias_info<channel_tag>;
template class instance_collection<channel_tag>;
template class instance_array<channel_tag, 0>;
template class instance_array<channel_tag, 1>;
template class instance_array<channel_tag, 2>;
template class instance_array<channel_tag, 3>;
template class instance_array<channel_tag, 4>;
template class port_formal_array<channel_tag>;
template class port_actual_collection<channel_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_CONSTRUCTORS
#undef	STACKTRACE_DESTRUCTORS

