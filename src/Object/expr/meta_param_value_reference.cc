/**
	\file "Object/expr/meta_param_value_reference.cc"
	Template instantiations of meta value references.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: meta_param_value_reference.cc,v 1.8.2.3 2006/02/19 03:52:52 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_META_PARAM_VALUE_REFERENCE_CC__
#define	__HAC_OBJECT_EXPR_META_PARAM_VALUE_REFERENCE_CC__

// flags for controlling conditional compilation, mostly for debugging
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS				0 && ENABLE_STACKTRACE

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "Object/common/extern_templates.h"
#include "Object/ref/simple_meta_value_reference.tcc"
#include "Object/ref/aggregate_meta_value_reference.tcc"
#include "Object/ref/meta_value_reference.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/preal_const.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/traits/preal_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/expr/const_collection.h"
#include "Object/expr/param_expr_list.h"
#include "Object/inst/pbool_value_collection.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/inst/preal_value_collection.h"
#include "Object/unroll/aliases_connection_base.h"
#include "Object/persistent_type_hash.h"

#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::simple_pbool_meta_value_reference,
		"pbool-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::simple_pint_meta_value_reference,
		"pint-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::simple_preal_meta_value_reference,
		"preal-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::aggregate_pbool_meta_value_reference,
		"agg.-pbool-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::aggregate_pint_meta_value_reference,
		"agg.-pint-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::aggregate_preal_meta_value_reference,
		"agg.-preal-inst-ref")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_pbool_meta_value_reference, 
		SIMPLE_PBOOL_META_VALUE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_pint_meta_value_reference, 
		SIMPLE_PINT_META_VALUE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_preal_meta_value_reference, 
		SIMPLE_PREAL_META_VALUE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::aggregate_pbool_meta_value_reference, 
		AGGREGATE_PBOOL_META_VALUE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::aggregate_pint_meta_value_reference, 
		AGGREGATE_PINT_META_VALUE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::aggregate_preal_meta_value_reference, 
		AGGREGATE_PREAL_META_VALUE_REFERENCE_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
//=============================================================================
// explicit template instantiations

template class simple_meta_value_reference<pint_tag>;
template class simple_meta_value_reference<pbool_tag>;
template class simple_meta_value_reference<preal_tag>;

template class aggregate_meta_value_reference<pint_tag>;
template class aggregate_meta_value_reference<pbool_tag>;
template class aggregate_meta_value_reference<preal_tag>;

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

DEFAULT_STATIC_TRACE_END

// responsibly undefining macros used
// IDEA: for each header, write an undef header file...

#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR

#endif	// __HAC_OBJECT_EXPR_META_PARAM_VALUE_REFERENCE_CC__

