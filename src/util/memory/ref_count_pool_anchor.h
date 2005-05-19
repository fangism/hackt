/**
	\file "util/memory/ref_count_pool_anchor.h"
	Interface to reference-count pool allocator.
	$Id: ref_count_pool_anchor.h,v 1.2 2005/05/19 18:43:38 fang Exp $
 */

#ifndef	__UTIL_MEMORY_REF_COUNT_POOL_ANCHOR_H__
#define	__UTIL_MEMORY_REF_COUNT_POOL_ANCHOR_H__

// mutually dependent
#include "util/memory/ref_count_pool.h"

/**
	May want to disable destroying the ref-count allocator via
	reference count because function-local static members may
	never be deallocated, causing the allocator to give warnings
	about live chunks when it is destroyed.  
	Problem is related to stacktrace.
	TODO: figure it out, punting for now...
 */
#define	FREE_REF_COUNT_POOL			0

namespace util {
namespace memory {
//=============================================================================
// try without, let it be un-deallocated, never freed.
/**
	Every translation unit will have it's own static anchor.  
	This must appear before any calls to count_ptr constructors are made.  
	During module construction and initialization, each module's anchor
	will be initialized properly.  The last anchor to be destroyed
	will free the pool.  
 */
#if FREE_REF_COUNT_POOL
static const ref_count_pool_ptr_type
__rc_pool_anchor__(get_ref_count_allocator_anchor());
#endif

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_REF_COUNT_POOL_ANCHOR_H__

