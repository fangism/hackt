/**
	\file "util/memory/count_array_ptr.tcc"
	Exclusive-ownership pointers that are array allocated (new []).  
	Provides pointer classes:
		count_array_ptr
		count_array_ptr_ref
	TODO: define the [] operator conditionally for array pointers
		in the base "count_ptr.h"
	TODO: technically, raw_count_ptr can be used for all variants
		because it never actually deletes its pointer.  
	$Id: count_array_ptr.tcc,v 1.1.4.1 2006/01/18 06:25:12 fang Exp $
 */

#ifndef	__UTIL_MEMORY_COUNT_ARRAY_PTR_TCC__
#define	__UTIL_MEMORY_COUNT_ARRAY_PTR_TCC__

#include "util/memory/count_array_ptr.h"
#include "util/memory/count_ptr.tcc"

#endif	// __UTIL_MEMORY_COUNT_ARRAY_PTR_TCC__

