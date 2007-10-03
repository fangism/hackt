/**
	\file "Object/inst/node_instance_collection.cc"
	Method definitions for boolean data type instance classes.
	This file came from "Object/art_object_instance_bool.cc"
		in a previous life.  
	$Id: node_instance_collection.cc,v 1.1.2.2 2007/10/03 06:43:59 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_NODE_INSTANCE_COLLECTION_CC__
#define	__HAC_OBJECT_INST_NODE_INSTANCE_COLLECTION_CC__

#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

// #include "Object/inst/node_instance_collection.h"
// #include "Object/inst/alias_empty.tcc"
// #include "Object/ref/member_meta_instance_reference.h"
// #include "Object/ref/simple_nonmeta_value_reference.h"
// #include "Object/expr/bool_expr.h"
// #include "Object/def/built_in_datatype_def.h"
#include "Object/traits/node_traits.h"
#include "Object/type/data_type_reference.h"
#include "Object/persistent_type_hash.h"
#include "Object/inst/null_collection_type_manager.tcc"
// #include "Object/inst/instance_collection.tcc"
#include "Object/inst/dummy_placeholder.tcc"
// #include "Object/inst/instance_alias.tcc"
// #include "Object/inst/state_instance.tcc"
#include "Object/unroll/datatype_instantiation_statement.h"
#include "Object/unroll/null_parameter_type.h"

//=============================================================================
// module-local specializations

namespace util {
#if 0
	SPECIALIZE_UTIL_WHAT(HAC::entity::node_instance_collection,
		"node_instance_collection")
// node cannot be formal
//	SPECIALIZE_UTIL_WHAT(HAC::entity::node_port_formal_array,
//		"node_port_formal_array")
	SPECIALIZE_UTIL_WHAT(HAC::entity::node_scalar, "node_scalar")
	SPECIALIZE_UTIL_WHAT(HAC::entity::node_array_1D, "node_array_1D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::node_array_2D, "node_array_2D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::node_array_3D, "node_array_3D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::node_array_4D, "node_array_4D")
#endif

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::node_instance_placeholder, 
	NODE_INSTANCE_PLACEHOLDER_TYPE_KEY, 0)

}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {

//=============================================================================
// explicit instantiations

template class dummy_placeholder<node_tag>;
// template class state_instance<node_tag>;
// template class instance_pool<state_instance<node_tag> >;
// template class instance_alias_info<node_tag>;
#if 0
template class instance_collection<node_tag>;
template class instance_array<node_tag, 0>;
template class instance_array<node_tag, 1>;
template class instance_array<node_tag, 2>;
template class instance_array<node_tag, 3>;
template class instance_array<node_tag, 4>;
#endif
// template class port_formal_array<node_tag>;
// template class port_actual_collection<node_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

#endif	// __HAC_OBJECT_INST_NODE_INSTANCE_COLLECTION_CC__

