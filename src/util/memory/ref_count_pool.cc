/**
	\file "util/memory/ref_count_pool.cc"
	Implementation of reference count pool allocator.  
	$Id: ref_count_pool.cc,v 1.1.2.1 2005/05/17 21:48:46 fang Exp $
 */

/**
	For this module only, force use of default allocator for the
	reference counts to the reference count pool.  
	Otherwise, end up with cyclic dependence between the chunk map pool
	and reference count pointers thereof.  

	NOTE: critical to delete count FIRST before ptr in count_ptr.tcc!
 */
// #define	USE_REF_COUNT_POOL	0

#include "util/static_trace.h"
STATIC_TRACE_BEGIN("util/memory/ref_count_pool.cc")

#include "util/memory/ref_count_pool_anchor.h"
#include "util/memory/chunk_map_pool.tcc"
#include "util/memory/count_ptr.tcc"

// #include <iostream>

namespace util {
namespace memory {
// #include "util/using_ostream.h"
//-----------------------------------------------------------------------------
/**
	Since reference count are likely to be needed during 
	global static initialization, we use the 'robust' method of
	initializing depedent objects and data structures.  
**/

//-----------------------------------------------------------------------------
/**
	Bootstrapping!
 */
ref_count_pool_ref_type
get_ref_count_allocator_anchor(void) {
	// without count_ptrs to consume this pointer, this pool
	// will never be deallocated, *GASP*
	static ref_count_pool_type* const	pool = new ref_count_pool_type();

	// this results in mutual recursion between this and allocate_ref_count.
	// static size_t* const			count = NEW_SIZE_T;	// BAD

	// this is good, uses new pool to get it's own count!
	static size_t* const			count = pool->allocate();
	static const size_t			zero = (*count = 0);

	// the following is no good because the method of deletion depends
	// on who the last count-holder is!
	// static size_t* const			count = new size_t(0);

//	ostream not necessarily ready during static initialization :(
//	ostream& o(std::cerr << "ptr = " << count << std::endl);
//	assert(!count);		// observing this address with gdb :S
	return ref_count_pool_ref_type(pool, count);
}

//-----------------------------------------------------------------------------

}	// end namespace memory
}	// end namespace util

STATIC_TRACE_END("util/memory/ref_count_pool.cc")

