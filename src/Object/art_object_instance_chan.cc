/**
	\file "Object/art_object_instance_chan.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_chan.cc,v 1.14.4.8.2.1 2005/07/11 03:26:56 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_CHAN_CC__
#define	__OBJECT_ART_OBJECT_INSTANCE_CHAN_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/art_object_instance_chan.h"
#include "Object/art_object_instance_alias_actuals.h"
#include "Object/art_object_inst_stmt_base.h"
#include "Object/art_object_inst_ref.h"
#include "Object/art_object_member_inst_ref.h"
#include "Object/art_object_definition_base.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_type_hash.h"

// experimental: suppressing automatic template instantiation
#include "Object/art_object_extern_templates.h"

#include "Object/art_object_instance_collection.tcc"
#include "Object/inst/general_collection_type_manager.tcc"

namespace util {

	SPECIALIZE_UTIL_WHAT(ART::entity::channel_instance_collection,
		"channel_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_scalar, "channel_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_array_1D, "channel_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_array_2D, "channel_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_array_3D, "channel_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_array_4D, "channel_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_scalar, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_array_1D, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_array_2D, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_array_3D, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_array_4D, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 4)
}	// end namespace util

namespace ART {
namespace entity {
//=============================================================================
// class channel_instance method definitions

channel_instance::channel_instance() : back_ref(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance::~channel_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance::collect_transient_info(persistent_object_manager& m) const {
	// collect pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	// write me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance::load_object(const persistent_object_manager& m, 
		istream& i) {
	// write me!
}

//=============================================================================
// explicit template class instantiations

template class instance_alias_info<channel_tag>;
template class instance_collection<channel_tag>;
template class instance_array<channel_tag, 0>;
template class instance_array<channel_tag, 1>;
template class instance_array<channel_tag, 2>;
template class instance_array<channel_tag, 3>;
template class instance_array<channel_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_CHAN_CC__

