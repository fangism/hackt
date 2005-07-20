/**
	\file "Object/expr/meta_param_value_reference.cc"
	Template instantiations of meta value references.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: meta_param_value_reference.cc,v 1.2 2005/07/20 21:00:45 fang Exp $
 */

#ifndef	__OBJECT_EXPR_META_PARAM_VALUE_REFERENCE_CC__
#define	__OBJECT_EXPR_META_PARAM_VALUE_REFERENCE_CC__

// flags for controlling conditional compilation, mostly for debugging
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS				0 && ENABLE_STACKTRACE

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "Object/art_object_extern_templates.h"
#include "Object/art_object_value_reference.tcc"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/art_object_const_collection.h"
#include "Object/art_object_value_collection.h"
#include "Object/art_object_instance_param.h"
#include "Object/art_object_connect.h"	// for ~aliases_connection_base
#include "Object/art_object_type_hash.h"

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
SPECIALIZE_UTIL_WHAT(ART::entity::simple_pbool_meta_instance_reference,
		"pbool-inst-ref")
SPECIALIZE_UTIL_WHAT(ART::entity::simple_pint_meta_instance_reference,
		"pint-inst-ref")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_pbool_meta_instance_reference, 
		SIMPLE_PBOOL_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::simple_pint_meta_instance_reference, 
		SIMPLE_PINT_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
//=============================================================================
// explicit template instantiations

template class simple_meta_value_reference<pint_tag>;
template class simple_meta_value_reference<pbool_tag>;

//=============================================================================
}	// end namepace entity
}	// end namepace ART

DEFAULT_STATIC_TRACE_END

// responsibly undefining macros used
// IDEA: for each header, write an undef header file...

#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR

#endif	// __OBJECT_EXPR_META_PARAM_VALUE_REFERENCE_CC__

