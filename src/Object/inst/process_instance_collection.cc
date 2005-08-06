/**
	\file "Object/inst/process_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file came from "Object/art_object_instance_proc.cc"
		in a previous life.  
	$Id: process_instance_collection.cc,v 1.2.4.1 2005/08/06 01:32:20 fang Exp $
 */

#ifndef	__OBJECT_INST_PROCESS_INSTANCE_COLLECTION_CC__
#define	__OBJECT_INST_PROCESS_INSTANCE_COLLECTION_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_PERSISTENTS		0
#define	STACKTRACE_DESTRUCTORS		0

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/process_instance_collection.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/type/process_type_reference.h"
#include "Object/inst/alias_actuals.h"
#include "Object/persistent_type_hash.h"

#include "Object/inst/instance_collection.tcc"
#include "Object/inst/general_collection_type_manager.tcc"


namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::process_instance_collection,
		"process_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_scalar, "process_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_array_1D, "process_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_array_2D, "process_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_array_3D, "process_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_array_4D, "process_array_4D")

#if 0
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_instance, UNIQUE_PROCESS_INSTANCE_TYPE_KEY, 0)
#endif
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_scalar, PROCESS_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_array_1D, PROCESS_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_array_2D, PROCESS_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_array_3D, PROCESS_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_array_4D, PROCESS_INSTANCE_COLLECTION_TYPE_KEY, 4)

}	// end namespace util

namespace ART {
namespace entity {

//=============================================================================
// class proc_instance method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instance::process_instance() : back_ref(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instance::process_instance(const alias_info_type& p) : back_ref(&p) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instance::~process_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_instance::what(ostream& o) const {
	return o << "process_instance";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance::collect_transient_info_base(
		persistent_object_manager& m) const {
#if 0
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	// walk vector of pointers...
}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
#if USE_INSTANCE_INDEX
	NEVER_NULL(back_ref);
	back_ref->write_next_connection(m, o);
#endif
}

void
process_instance::load_object_base(const persistent_object_manager& m,
		istream& i) {
#if USE_INSTANCE_INDEX
	back_ref = never_ptr<const alias_info_type>(
		&alias_info_type::load_alias_reference(m, i));
#endif 
}

//=============================================================================
// explicit template clas instantiations

template class instance_alias_info<process_tag>;
template class instance_collection<process_tag>;
template class instance_array<process_tag, 0>;
template class instance_array<process_tag, 1>;
template class instance_array<process_tag, 2>;
template class instance_array<process_tag, 3>;
template class instance_array<process_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#undef	ENABLE_STACKTRACE

#endif	// __OBJECT_INST_PROCESS_INSTANCE_COLLECTION_CC__

