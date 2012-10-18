/**
	\file "util/memory/ref_count_pool.cc"
	Implementation of reference count pool allocator.  
	$Id: ref_count_pool.cc,v 1.5 2007/08/15 01:08:22 fang Exp $
 */

/**
	For this module only, force use of default allocator for the
	reference counts to the reference count pool.  
	Otherwise, end up with cyclic dependence between the chunk map pool
	and reference count pointers thereof.  

	NOTE: critical to delete count FIRST before ptr in count_ptr.tcc!
 */
// #define	USE_REF_COUNT_POOL	0
#define	ENABLE_STATIC_TRACE		0

#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include "util/memory/ref_count_pool_anchor.hh"
#include "util/memory/chunk_map_pool.tcc"
#include "util/memory/count_ptr.tcc"

// #include <iostream>

namespace util {
namespace memory {
// #include "util/using_ostream.hh"
//-----------------------------------------------------------------------------
/**
	Since reference count are likely to be needed during 
	global static initialization, we use the 'robust' method of
	initializing depedent objects and data structures.  
**/

//-----------------------------------------------------------------------------
/**
	Bootstrapping!
	This method returns a reference count pointer to the refernce count
	pool, which is needed to guarantee that the count pool at 
	least out-lives every reference-count pointer.  
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
	static const size_t	zero __ATTRIBUTE_UNUSED__ = (*count = 0);

	// the following is no good because the method of deletion depends
	// on who the last count-holder is!
	// static size_t* const			count = new size_t(0);

//	ostream not necessarily ready during static initialization :(
//	ostream& o(std::cerr << "ptr = " << count << std::endl);
//	assert(!count);		// observing this address with gdb :S
	return ref_count_pool_ref_type(pool, count);
}

//=============================================================================
// explicit template instantiations

// should match ref_count_pool_type in "ref_count_pool.h"
template class chunk_map_pool<size_t, 32>;

//=============================================================================
}	// end namespace memory
}	// end namespace util

DEFAULT_STATIC_TRACE_END

