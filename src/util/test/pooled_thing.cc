/**
	\file "pooled_thing.cc"
	Example of a simple pool-allocated class, using the util library.  
	This file contains the static member definitions.  
	$Id: pooled_thing.cc,v 1.1.12.1 2005/01/25 20:34:39 fang Exp $
 */

#define	DEBUG_LIST_VECTOR_POOL		1

#include "pooled_thing.h"
#include "memory/list_vector_pool.tcc"
#include "what.h"

using util::what;

namespace util {
	SPECIALIZE_UTIL_WHAT(pooled_thing, "pooled_thing")
}

LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(pooled_thing, 4)

