/**
	\file "Object/inst/process_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file came from "Object/art_object_instance_proc.cc"
		in a previous life.  
	$Id: process_instance_collection.cc,v 1.19 2011/04/02 01:46:03 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_DESTRUCTORS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_CONSTRUCTORS		(0 && ENABLE_STACKTRACE)

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/process_instance_collection.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/def/process_definition.h"
#include "Object/type/process_type_reference.h"
#include "Object/inst/alias_actuals.tcc"
#include "Object/persistent_type_hash.h"
#include "Object/inst/connection_policy.tcc"
#include "Object/inst/instance_collection.tcc"
#include "Object/inst/instance_placeholder.tcc"
#include "Object/inst/instance_alias.tcc"
#include "Object/inst/general_collection_type_manager.tcc"
#include "Object/inst/state_instance.tcc"
#include "Object/inst/internal_aliases_policy.tcc"
#include "Object/inst/alias_empty.h"		// why is this needed?
#include "Object/unroll/instantiation_statement_type_ref_default.h"

namespace util {
	SPECIALIZE_UTIL_WHAT(HAC::entity::process_instance_collection,
		"process_instance_collection")
	SPECIALIZE_UTIL_WHAT(HAC::entity::process_port_formal_array,
		"process_port_formal_array")
	SPECIALIZE_UTIL_WHAT(HAC::entity::process_scalar, "process_scalar")
	SPECIALIZE_UTIL_WHAT(HAC::entity::process_array_1D, "process_array_1D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::process_array_2D, "process_array_2D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::process_array_3D, "process_array_3D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::process_array_4D, "process_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::process_instance_placeholder, 
	PROCESS_INSTANCE_PLACEHOLDER_TYPE_KEY, 0)
}	// end namespace util

namespace HAC {
namespace entity {
//=============================================================================
// explicit template class instantiations

template 
ostream&
instance_alias_info_actuals::dump_complete_type(
	const instance_alias_info<process_tag>&,
	ostream&, const footprint* const);

template class instance_placeholder<process_tag>;
template class state_instance<process_tag>;
template class instance_pool<state_instance<process_tag> >;
template class instance_alias_info<process_tag>;
template class instance_collection<process_tag>;
template class instance_array<process_tag, 0>;
template class instance_array<process_tag, 1>;
template class instance_array<process_tag, 2>;
template class instance_array<process_tag, 3>;
template class instance_array<process_tag, 4>;
template class port_formal_array<process_tag>;
template class port_actual_collection<process_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef	ENABLE_STACKTRACE

