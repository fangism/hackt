/**
	\file "Object/art_object_instance_proc.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_proc.cc,v 1.15.2.1 2005/07/21 05:35:06 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_PROC_CC__
#define	__OBJECT_ART_OBJECT_INSTANCE_PROC_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_PERSISTENTS		0
#define	STACKTRACE_DESTRUCTORS		0

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/art_object_instance_proc.h"
#include "Object/art_object_inst_stmt_base.h"
#include "Object/art_object_inst_ref.h"
#include "Object/art_object_member_inst_ref.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_instance_alias_actuals.h"
#include "Object/art_object_type_hash.h"

#include "Object/art_object_instance_collection.tcc"
#include "Object/inst/general_collection_type_manager.tcc"


namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::process_instance_collection,
		"process_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_scalar, "process_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_array_1D, "process_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_array_2D, "process_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_array_3D, "process_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_array_4D, "process_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_instance, UNIQUE_PROCESS_INSTANCE_TYPE_KEY, 0)
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
process_instance::~process_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_instance::what(ostream& o) const {
	return o << "process_instance";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	// walk vector of pointers...
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance::write_object(const persistent_object_manager& m, 
		ostream& f) const {
#if 0
	write_value(f, state);
	// write pointer sequence...
#endif
}

void
process_instance::load_object(const persistent_object_manager& m, istream& f) {
#if 0
	read_value(f, state);
	// read pointer sequence...
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

#endif	// __OBJECT_ART_OBJECT_INSTANCE_PROC_CC__

