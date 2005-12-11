/**
	\file "Object/expr/nonmeta_param_value_reference.cc"
	Nonmeta parameter value reference expressions.  
	Like references to arrays of constants with run-time index values.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: nonmeta_param_value_reference.cc,v 1.4.26.1 2005/12/11 00:45:29 fang Exp $
 */

#ifndef	__OBJECT_EXPR_NONMETA_PARAM_VALUE_REFERENCE_CC__
#define	__OBJECT_EXPR_NONMETA_PARAM_VALUE_REFERENCE_CC__

// flags for controlling conditional compilation, mostly for debugging
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS				0 && ENABLE_STACKTRACE

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "Object/common/extern_templates.h"
#include "Object/ref/simple_nonmeta_value_reference.tcc"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/persistent_type_hash.h"
#include "Object/expr/int_expr.h"
#include "Object/expr/bool_expr.h"

#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::simple_pbool_nonmeta_instance_reference,
		"nonmeta-pbool-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::simple_pint_nonmeta_instance_reference,
		"nonmeta-pint-inst-ref")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_pbool_nonmeta_instance_reference, 
		SIMPLE_PBOOL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_pint_nonmeta_instance_reference, 
		SIMPLE_PINT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
//=============================================================================
// specializations

/**
        Specialized data type reference resolved for parameter ints, 
        which are promoted to int<32> in data context.  
	TODO: this is wrong, fix me!
	constant parameters can have arbitrary width.  
 */
template <>
struct data_type_resolver<pint_tag> {
	typedef	class_traits<pint_tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&) const {
		return int_traits::int32_type_ptr;
	}
};      // end struct data_type_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct data_type_resolver<pbool_tag> {
	typedef	class_traits<pbool_tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&) const {
		return bool_traits::built_in_type_ptr;
	}
};      // end struct data_type_resolver


//=============================================================================
// explicit template instantiations

// maybe this belongs in "Object/art_object_data_expr.cc"?
template class simple_nonmeta_value_reference<pint_tag>;
template class simple_nonmeta_value_reference<pbool_tag>;
// used to be simple_nonmeta_instance_reference<...>

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

DEFAULT_STATIC_TRACE_END

// responsibly undefining macros used
// IDEA: for each header, write an undef header file...

#undef	DEBUG_LIST_VECTOR_POOL
#undef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR

#endif	// __OBJECT_EXPR_NONMETA_PARAM_VALUE_REFERENCE_CC__

