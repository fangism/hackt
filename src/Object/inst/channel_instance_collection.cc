/**
	\file "Object/inst/channel_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originated from "Object/art_object_instance_chan.cc"
		in a previous life.  
	$Id: channel_instance_collection.cc,v 1.2.4.2 2005/08/06 15:42:27 fang Exp $
 */

#ifndef	__OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_CC__
#define	__OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/channel_instance_collection.h"
#include "Object/inst/alias_actuals.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/def/definition_base.h"
#include "Object/type/channel_type_reference_base.h"
#include "Object/persistent_type_hash.h"
#include "Object/inst/instance_collection.tcc"
#include "Object/inst/general_collection_type_manager.tcc"
#include "Object/inst/state_instance.tcc"

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

#if 0
channel_instance::channel_instance() : back_ref(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance::channel_instance(const alias_info_type& c) : back_ref(&c) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance::~channel_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance::collect_transient_info_base(
		persistent_object_manager& m) const {
	// collect pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
#if USE_INSTANCE_INDEX
	NEVER_NULL(back_ref);
	back_ref->write_next_connection(m, o);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance::load_object_base(const persistent_object_manager& m, 
		istream& i) {
#if USE_INSTANCE_INDEX
	back_ref = never_ptr<const alias_info_type>(
		&alias_info_type::load_alias_reference(m, i));
#endif
}
#endif

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

#endif	// __OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_CC__

