/**
	\file "util/memory/count_malloc_ptr.tcc"
	Exclusive-ownership pointers that are malloc allocated (new []).  
	Provides pointer classes:
		count_malloc_ptr
		count_malloc_ptr_ref
	TODO: define the [] operator conditionally for malloc pointers
		in the base "count_ptr.h"
	TODO: technically, raw_count_ptr can be used for all variants
		because it never actually deletes its pointer.  
	$Id: count_malloc_ptr.tcc,v 1.3 2006/01/22 06:53:41 fang Exp $
 */

#ifndef	__UTIL_MEMORY_COUNT_MALLOC_PTR_TCC__
#define	__UTIL_MEMORY_COUNT_MALLOC_PTR_TCC__

#include "util/memory/count_malloc_ptr.h"
#include "util/memory/count_ptr.tcc"
#include "util/memory/deallocation_policy.h"

#endif	// __UTIL_MEMORY_COUNT_MALLOC_PTR_TCC__

