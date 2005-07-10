/**
	\file "Object/expr/nonmeta_param_value_reference.cc"
	Nonmeta parameter value reference expressions.  
	Like references to arrays of constants with run-time index values.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: nonmeta_param_value_reference.cc,v 1.1.2.2 2005/07/10 19:37:26 fang Exp $
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

#include "Object/art_object_extern_templates.h"
#include "Object/art_object_nonmeta_value_reference.tcc"
#include "Object/art_object_pint_traits.h"
#include "Object/art_object_pbool_traits.h"
#include "Object/art_object_bool_traits.h"
#include "Object/art_object_type_hash.h"
#include "Object/expr/int_expr.h"
#include "Object/expr/bool_expr.h"
#include "Object/art_built_ins.h"

#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"

// these conditional definitions must appear after inclusion of "stacktrace.h"
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_DTOR(x)
#endif

#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)	STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif


//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(ART::entity::simple_pbool_nonmeta_instance_reference,
		"nonmeta-pbool-inst-ref")
SPECIALIZE_UTIL_WHAT(ART::entity::simple_pint_nonmeta_instance_reference,
		"nonmeta-pint-inst-ref")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_pbool_nonmeta_instance_reference, 
		SIMPLE_PBOOL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_pint_nonmeta_instance_reference, 
		SIMPLE_PINT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace ART {
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
		return int32_type_ptr;
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
}	// end namepace ART

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

