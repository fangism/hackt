/**
	\file "Object/expr/meta_param_value_reference.cc"
	Template instantiations of meta value references.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: meta_param_value_reference.cc,v 1.14 2011/04/02 01:45:58 fang Exp $
 */

// flags for controlling conditional compilation, mostly for debugging
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS				(0 && ENABLE_STACKTRACE)

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
#include "Object/expr/pstring_const.h"
#include "Object/traits/value_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/expr/const_collection.h"
#include "Object/expr/param_expr_list.h"
#include "Object/inst/pbool_value_collection.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/inst/preal_value_collection.h"
#include "Object/inst/pstring_value_collection.h"
#include "Object/unroll/aliases_connection_base.h"
#include "Object/persistent_type_hash.h"

#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace util {
using HAC::entity::simple_pbool_meta_value_reference;
using HAC::entity::simple_pint_meta_value_reference;
using HAC::entity::simple_preal_meta_value_reference;
using HAC::entity::simple_pstring_meta_value_reference;
using HAC::entity::aggregate_pbool_meta_value_reference;
using HAC::entity::aggregate_pint_meta_value_reference;
using HAC::entity::aggregate_preal_meta_value_reference;
using HAC::entity::aggregate_pstring_meta_value_reference;

SPECIALIZE_UTIL_WHAT(simple_pbool_meta_value_reference, "pbool-val-ref")
SPECIALIZE_UTIL_WHAT(simple_pint_meta_value_reference, "pint-val-ref")
SPECIALIZE_UTIL_WHAT(simple_preal_meta_value_reference, "preal-val-ref")
SPECIALIZE_UTIL_WHAT(simple_pstring_meta_value_reference, "pstring-val-ref")
SPECIALIZE_UTIL_WHAT(aggregate_pbool_meta_value_reference, "agg.-pbool-val-ref")
SPECIALIZE_UTIL_WHAT(aggregate_pint_meta_value_reference, "agg.-pint-val-ref")
SPECIALIZE_UTIL_WHAT(aggregate_preal_meta_value_reference, "agg.-preal-val-ref")
SPECIALIZE_UTIL_WHAT(aggregate_pstring_meta_value_reference, "agg.-pstring-val-ref")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	simple_pbool_meta_value_reference, 
		SIMPLE_PBOOL_META_VALUE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	simple_pint_meta_value_reference, 
		SIMPLE_PINT_META_VALUE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	simple_preal_meta_value_reference, 
		SIMPLE_PREAL_META_VALUE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	simple_pstring_meta_value_reference, 
		SIMPLE_PSTRING_META_VALUE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	aggregate_pbool_meta_value_reference, 
		AGGREGATE_PBOOL_META_VALUE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	aggregate_pint_meta_value_reference, 
		AGGREGATE_PINT_META_VALUE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	aggregate_preal_meta_value_reference, 
		AGGREGATE_PREAL_META_VALUE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	aggregate_pstring_meta_value_reference, 
		AGGREGATE_PSTRING_META_VALUE_REFERENCE_TYPE_KEY, 0)
namespace memory {
	template class count_ptr<HAC::entity::meta_value_reference_base>;
}
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
//=============================================================================
// non-inline dtors
meta_value_reference_base::~meta_value_reference_base() { }
aggregate_meta_value_reference_base::~aggregate_meta_value_reference_base() { }

//=============================================================================
// explicit template instantiations

template class simple_meta_value_reference<pint_tag>;
template class simple_meta_value_reference<pbool_tag>;
template class simple_meta_value_reference<preal_tag>;
template class simple_meta_value_reference<pstring_tag>;

template class aggregate_meta_value_reference<pint_tag>;
template class aggregate_meta_value_reference<pbool_tag>;
template class aggregate_meta_value_reference<preal_tag>;
template class aggregate_meta_value_reference<pstring_tag>;

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

