/**
	\file "util/memory/count_malloc_ptr.h"
	Exclusive-ownership pointers that are malloc allocated (new []).  
	Provides pointer classes:
		count_malloc_ptr
		count_malloc_ptr_ref
	TODO: define the [] operator conditionally for malloc pointers
		in the base "count_ptr.h"
	TODO: technically, count_ptr_ref can be used for all variants
		because it never actually deletes its pointer.  
	$Id: count_malloc_ptr.h,v 1.2 2006/01/22 06:53:41 fang Exp $
 */

#ifndef	__UTIL_MEMORY_COUNT_MALLOC_PTR_H__
#define	__UTIL_MEMORY_COUNT_MALLOC_PTR_H__

#include "util/memory/count_ptr.h"
#include "util/memory/deallocation_policy_fwd.h"

namespace util {
namespace memory {

template <class T>
struct count_malloc_ptr {
	typedef	count_ptr<T, free_tag>		type;
};

}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_COUNT_MALLOC_PTR_H__

