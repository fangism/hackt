/**
	\file "Object/expr/nonmeta_param_value_reference.cc"
	Nonmeta parameter value reference expressions.  
	Like references to arrays of constants with run-time index values.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: nonmeta_param_value_reference.cc,v 1.9.8.1.2.1 2006/09/05 03:55:49 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_NONMETA_PARAM_VALUE_REFERENCE_CC__
#define	__HAC_OBJECT_EXPR_NONMETA_PARAM_VALUE_REFERENCE_CC__

// flags for controlling conditional compilation, mostly for debugging
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS				(0 && ENABLE_STACKTRACE)

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "Object/common/extern_templates.h"
#include "Object/ref/simple_nonmeta_value_reference.tcc"
#include "Object/traits/classification_tags.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/traits/preal_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/persistent_type_hash.h"
#include "Object/expr/int_expr.h"
#include "Object/expr/bool_expr.h"
#include "Object/expr/real_expr.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/preal_const.h"
#include "Object/inst/pint_instance.h"
#include "Object/inst/pbool_instance.h"
#include "Object/inst/preal_instance.h"
#include "common/TODO.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::simple_pbool_nonmeta_instance_reference,
		"nonmeta-pbool-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::simple_pint_nonmeta_instance_reference,
		"nonmeta-pint-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::simple_preal_nonmeta_instance_reference,
		"nonmeta-preal-inst-ref")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_pbool_nonmeta_instance_reference, 
		SIMPLE_PBOOL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_pint_nonmeta_instance_reference, 
		SIMPLE_PINT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_preal_nonmeta_instance_reference, 
		SIMPLE_PREAL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
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
#if USE_UNRESOLVED_DATA_TYPES
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&) const {
		return int_traits::magic_int_type_ptr;
	}
#endif

#if USE_RESOLVED_DATA_TYPES
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&, 
		const unroll_context&) const;
#endif

};      // end struct data_type_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct data_type_resolver<pbool_tag> {
	typedef	class_traits<pbool_tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
#if USE_UNRESOLVED_DATA_TYPES
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&) const {
		return bool_traits::built_in_type_ptr;
	}
#endif

#if USE_RESOLVED_DATA_TYPES
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&, 
		const unroll_context&) const;
#endif
};      // end struct data_type_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct data_type_resolver<preal_tag> {
	typedef	class_traits<preal_tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
#if USE_UNRESOLVED_DATA_TYPES
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&) const {
#if 0
		return real_traits::built_in_type_ptr;
#else
		FINISH_ME(Fang);
		return count_ptr<const data_type_reference>(NULL);
#endif
	}
#endif

#if USE_RESOLVED_DATA_TYPES
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&, 
		const unroll_context&) const;
#endif

};      // end struct data_type_resolver


//=============================================================================
// explicit template instantiations

// maybe this belongs in "Object/art_object_data_expr.cc"?
template class simple_nonmeta_value_reference<pint_tag>;
template class simple_nonmeta_value_reference<pbool_tag>;
template class simple_nonmeta_value_reference<preal_tag>;
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

#endif	// __HAC_OBJECT_EXPR_NONMETA_PARAM_VALUE_REFERENCE_CC__

