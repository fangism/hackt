/**
	\file "Object/inst/channel_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originated from "Object/art_object_instance_chan.cc"
		in a previous life.  
	$Id: channel_instance_collection.cc,v 1.14 2006/11/07 06:34:36 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_CC__
#define	__HAC_OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/channel_instance_collection.h"
#include "Object/inst/alias_empty.tcc"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/def/definition_base.h"
#include "Object/def/user_def_chan.h"
#include "Object/type/channel_type_reference.h"
#include "Object/persistent_type_hash.h"
#include "Object/inst/instance_collection.tcc"
#include "Object/inst/instance_placeholder.tcc"
#include "Object/inst/instance_alias.tcc"
#include "Object/inst/general_collection_type_manager.tcc"
#include "Object/inst/state_instance.tcc"
#include "Object/def/datatype_definition_base.h"
#include "Object/inst/alias_empty.h"	// why is this needed?
#include "Object/unroll/instantiation_statement_type_ref_default.h"

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

#endif	// __HAC_OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_CC__

