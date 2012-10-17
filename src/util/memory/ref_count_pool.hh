/**
	\file "util/memory/ref_count_pool.hh"
	Interface to reference-count pool allocator.
	$Id: ref_count_pool.hh,v 1.5 2006/01/22 06:53:44 fang Exp $
 */

#ifndef	__UTIL_MEMORY_REF_COUNT_POOL_H__
#define	__UTIL_MEMORY_REF_COUNT_POOL_H__

#include "util/memory/chunk_map_pool_fwd.hh"
#include "util/memory/pointer_classes_fwd.hh"

namespace util {
namespace memory {
//=============================================================================
// forward declarations

#if 0
template <class>
class count_ptr;
template <class>
class raw_count_ptr;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// typedefs

typedef	chunk_map_pool<size_t, 32>		ref_count_pool_type;
typedef	count_ptr<ref_count_pool_type, default_deallocation_policy>
						ref_count_pool_ptr_type;
typedef	raw_count_ptr<ref_count_pool_type, default_deallocation_policy>
						ref_count_pool_ref_type;

//=============================================================================
extern	ref_count_pool_ref_type
	get_ref_count_allocator_anchor(void);
	
#if 0
extern	size_t*
	allocate_ref_count(void);

extern	void
	deallocate_ref_count(size_t*);
#endif

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_REF_COUNT_POOL_H__

