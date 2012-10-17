/**
	\file "Object/inst/bool_instance_collection.cc"
	Method definitions for boolean data type instance classes.
	This file came from "Object/art_object_instance_bool.cc"
		in a previous life.  
	$Id: bool_instance_collection.cc,v 1.17 2011/04/02 01:46:00 fang Exp $
 */

#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)

#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/bool_instance_collection.hh"
#include "Object/inst/alias_empty.tcc"
#include "Object/ref/member_meta_instance_reference.hh"
#include "Object/ref/simple_nonmeta_value_reference.hh"
#include "Object/expr/bool_expr.hh"
#include "Object/def/built_in_datatype_def.hh"
#include "Object/type/data_type_reference.hh"
#include "Object/persistent_type_hash.hh"
#include "Object/inst/null_collection_type_manager.tcc"
#include "Object/inst/instance_collection.tcc"
#include "Object/inst/instance_placeholder.tcc"
#include "Object/inst/instance_alias.tcc"
#include "Object/inst/connection_policy.tcc"
#include "Object/inst/state_instance.tcc"
#include "Object/unroll/datatype_instantiation_statement.hh"
#include "Object/unroll/null_parameter_type.hh"

//=============================================================================
// module-local specializations

namespace util {
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_instance_collection,
		"bool_instance_collection")
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_port_formal_array,
		"bool_port_formal_array")
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_scalar, "bool_scalar")
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_array_1D, "bool_array_1D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_array_2D, "bool_array_2D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_array_3D, "bool_array_3D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::bool_array_4D, "bool_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::bool_instance_placeholder, 
	DBOOL_INSTANCE_PLACEHOLDER_TYPE_KEY, 0)

// explicit template instantiation
using namespace HAC::entity;
// from Object/traits/bool_traits.hh: alias_collection_type
template class packed_array_generic<pint_value_type,
		memory::never_ptr<instance_alias_info<bool_tag> > >;

namespace memory {
	// can we still lazy destroy with instance aliases?
	// or will it contain pointers to other things later?  (instances)
#if 0
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(HAC::entity::bool_scalar)
#endif
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {

//=============================================================================
// typedef bool_instance_alias_info function definitions

ostream&
operator << (ostream& o, const bool_instance_alias_info& b) {
	return o << "bool-alias @ " << &b;
}

//=============================================================================
// explicit instantiations

template class instance_placeholder<bool_tag>;
template class state_instance<bool_tag>;
template class instance_pool<state_instance<bool_tag> >;
template class instance_alias_info<bool_tag>;
template class instance_collection<bool_tag>;
template class instance_array<bool_tag, 0>;
template class instance_array<bool_tag, 1>;
template class instance_array<bool_tag, 2>;
template class instance_array<bool_tag, 3>;
template class instance_array<bool_tag, 4>;
template class port_formal_array<bool_tag>;
template class port_actual_collection<bool_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

