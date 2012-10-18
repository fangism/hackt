/**
	\file "pooled_thing.cc"
	Example of a simple pool-allocated class, using the util library.  
	This file contains the static member definitions.  
	$Id: pooled_thing.cc,v 1.3 2005/09/04 21:15:13 fang Exp $
 */

#define	DEBUG_LIST_VECTOR_POOL		1

#include "pooled_thing.hh"
#include "util/memory/list_vector_pool.tcc"
#include "util/what.hh"

using util::what;

namespace util {
	SPECIALIZE_UTIL_WHAT(pooled_thing, "pooled_thing")
}

LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(pooled_thing, 4)

