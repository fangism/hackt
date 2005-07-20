/**
	\file "Object/expr/param_const_collection.cc"
	Class instantiations for const collections.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: param_const_collection.cc,v 1.1.2.3 2005/07/20 18:48:29 fang Exp $
 */

#ifndef	__OBJECT_EXPR_PARAM_CONST_COLLECTION_CC__
#define	__OBJECT_EXPR_PARAM_CONST_COLLECTION_CC__

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
#include "Object/art_object_const_collection.tcc"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
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
SPECIALIZE_UTIL_WHAT(ART::entity::pint_const_collection,
		"pint-const-collection")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_const_collection,
		"pbool-const-collection")

// pint_const_collection requires special treatment:
// it has no empty constructor and requires an int argument
// this example shows how we can register various bound constructor
// functors with the persistent_object_manager type registry.  
using ART::entity::const_collection;
// macros defined in "art_object_const_collection.tcc"
SPECIALIZE_PERSISTENT_TRAITS_CONST_COLLECTION_FULL_DEFINITION(
	ART::entity::pint_tag, CONST_PINT_COLLECTION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_CONST_COLLECTION_FULL_DEFINITION(
	ART::entity::pbool_tag, CONST_PBOOL_COLLECTION_TYPE_KEY)

}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
//=============================================================================
// explicit template instantiations

template class const_collection<pint_tag>;
template class const_collection<pbool_tag>;

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

#endif	// __OBJECT_EXPR_PARAM_CONST_COLLECTION_CC__

