/**
	\file "Object/inst/int_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file came from "Object/art_object_instance_int.cc"
		in a previous life.  
	$Id: int_instance_collection.cc,v 1.14 2011/04/02 01:46:01 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)

//=============================================================================
#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/int_instance_collection.hh"
#include "Object/inst/alias_empty.tcc"
#include "Object/ref/member_meta_instance_reference.hh"
#include "Object/ref/simple_nonmeta_value_reference.hh"
#include "Object/expr/int_expr.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/def/built_in_datatype_def.hh"
#include "Object/type/data_type_reference.hh"
#include "Object/persistent_type_hash.hh"
#include "Object/inst/instance_collection.tcc"
#include "Object/inst/instance_placeholder.tcc"
#include "Object/inst/instance_alias.tcc"
#include "Object/inst/int_collection_type_manager.tcc"
#include "Object/inst/state_instance.tcc"
#include "Object/unroll/datatype_instantiation_statement.hh"

//=============================================================================
// module-local specializations

namespace util {
	SPECIALIZE_UTIL_WHAT(HAC::entity::int_instance_collection,
		"int_instance_collection")
	SPECIALIZE_UTIL_WHAT(HAC::entity::int_port_formal_array,
		"int_port_formal_array")
	SPECIALIZE_UTIL_WHAT(HAC::entity::int_scalar, "int_scalar")
	SPECIALIZE_UTIL_WHAT(HAC::entity::int_array_1D, "int_array_1D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::int_array_2D, "int_array_2D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::int_array_3D, "int_array_3D")
	SPECIALIZE_UTIL_WHAT(HAC::entity::int_array_4D, "int_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::int_instance_placeholder, 
	DINT_INSTANCE_PLACEHOLDER_TYPE_KEY, 0)

// explicit template instantiation
using namespace HAC::entity;
// from Object/traits/int_traits.hh: alias_collection_type
template class packed_array_generic<pint_value_type,
		memory::never_ptr<instance_alias_info<int_tag> > >;
}	// end namespace util


namespace HAC {
namespace entity {
//=============================================================================
// typedef int_instance_alias_info function definitions

ostream&
operator << (ostream& o, const int_instance_alias_info& i) {
	return o << "int-alias @ " << &i;
}

//=============================================================================
// explicit template instantiations

template class instance_placeholder<int_tag>;
template class state_instance<int_tag>;
template class instance_pool<state_instance<int_tag> >;
template class instance_alias_info<int_tag>;
template class instance_collection<int_tag>;
template class instance_array<int_tag, 0>;
template class instance_array<int_tag, 1>;
template class instance_array<int_tag, 2>;
template class instance_array<int_tag, 3>;
template class instance_array<int_tag, 4>;
template class port_formal_array<int_tag>;
template class port_actual_collection<int_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

