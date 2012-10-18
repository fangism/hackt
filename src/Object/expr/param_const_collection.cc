/**
	\file "Object/expr/param_const_collection.cc"
	Class instantiations for const collections.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: param_const_collection.cc,v 1.10 2011/04/02 01:46:00 fang Exp $
 */


// flags for controlling conditional compilation, mostly for debugging
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS				(0 && ENABLE_STACKTRACE)

//=============================================================================
// start of static initializations
#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include "Object/common/extern_templates.hh"
#include "Object/expr/const_collection.tcc"
#include "Object/traits/pint_traits.hh"
#include "Object/traits/pbool_traits.hh"
#include "Object/traits/preal_traits.hh"
#include "Object/traits/pstring_traits.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/pbool_const.hh"
#include "Object/expr/preal_const.hh"
#include "Object/expr/pstring_const.hh"
#include "Object/expr/const_index.hh"
#include "Object/expr/const_index_list.hh"
#include "Object/expr/const_range.hh"
#include "Object/expr/const_range_list.hh"
#include "Object/persistent_type_hash.hh"

#include "util/stacktrace.hh"
#include "util/persistent_object_manager.tcc"


//=============================================================================
namespace util {
using HAC::entity::pint_const_collection;
using HAC::entity::pbool_const_collection;
using HAC::entity::preal_const_collection;
using HAC::entity::pstring_const_collection;

SPECIALIZE_UTIL_WHAT(pint_const_collection,
		"pint-const-collection")
SPECIALIZE_UTIL_WHAT(pbool_const_collection,
		"pbool-const-collection")
SPECIALIZE_UTIL_WHAT(preal_const_collection,
		"preal-const-collection")
SPECIALIZE_UTIL_WHAT(pstring_const_collection,
		"pstring-const-collection")

// pint_const_collection requires special treatment:
// it has no empty constructor and requires an int argument
// this example shows how we can register various bound constructor
// functors with the persistent_object_manager type registry.  
using HAC::entity::const_collection;
// macros defined in "expr/const_collection.tcc"
SPECIALIZE_PERSISTENT_TRAITS_CONST_COLLECTION_FULL_DEFINITION(
	HAC::entity::pint_tag, CONST_PINT_COLLECTION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_CONST_COLLECTION_FULL_DEFINITION(
	HAC::entity::pbool_tag, CONST_PBOOL_COLLECTION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_CONST_COLLECTION_FULL_DEFINITION(
	HAC::entity::preal_tag, CONST_PREAL_COLLECTION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_CONST_COLLECTION_FULL_DEFINITION(
	HAC::entity::pstring_tag, CONST_PSTRING_COLLECTION_TYPE_KEY)

namespace memory {
template class count_ptr<const const_collection<HAC::entity::pint_tag> >;
template class count_ptr<const const_collection<HAC::entity::pbool_tag> >;
template class count_ptr<const const_collection<HAC::entity::preal_tag> >;
template class count_ptr<const const_collection<HAC::entity::pstring_tag> >;
}
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
//=============================================================================
// explicit template instantiations

template class const_collection<pint_tag>;
template class const_collection<pbool_tag>;
template class const_collection<preal_tag>;
template class const_collection<pstring_tag>;

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

