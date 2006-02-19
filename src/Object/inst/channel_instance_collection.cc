/**
	\file "Object/inst/channel_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originated from "Object/art_object_instance_chan.cc"
		in a previous life.  
	$Id: channel_instance_collection.cc,v 1.7.16.1 2006/02/19 03:52:57 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_CC__
#define	__HAC_OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/channel_instance_collection.h"
#include "Object/inst/alias_actuals.tcc"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/def/definition_base.h"
#include "Object/def/user_def_chan.h"
#include "Object/type/channel_type_reference.h"
#include "Object/persistent_type_hash.h"
#include "Object/inst/instance_collection.tcc"
#include "Object/inst/instance_alias.tcc"
#include "Object/inst/general_collection_type_manager.tcc"
#include "Object/inst/state_instance.tcc"
#include "Object/def/datatype_definition_base.h"
#include "Object/unroll/instantiation_statement_type_ref_default.h"

namespace util {

	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_instance_collection,
		"channel_instance_collection")
	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_scalar, "channel_scalar")
	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_array_1D, "channel_array_1D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_array_2D, "channel_array_2D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_array_3D, "channel_array_3D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::channel_array_4D, "channel_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_scalar, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_array_1D, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_array_2D, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_array_3D, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_array_4D, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 4)
}	// end namespace util

namespace HAC {
namespace entity {
//=============================================================================
// explicit template class instantiations

template class state_instance<channel_tag>;
template class instance_pool<state_instance<channel_tag> >;
template class instance_alias_info<channel_tag>;
template class instance_collection<channel_tag>;
template class instance_array<channel_tag, 0>;
template class instance_array<channel_tag, 1>;
template class instance_array<channel_tag, 2>;
template class instance_array<channel_tag, 3>;
template class instance_array<channel_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_CC__

