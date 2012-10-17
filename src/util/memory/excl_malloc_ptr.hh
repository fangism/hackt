/**
	\file "util/memory/excl_malloc_ptr.hh"
	Exclusive-ownership pointers that are malloc allocated.
	Provides pointer classes:
		excl_malloc_ptr
		sticky_malloc_ptr
		some_malloc_ptr
		unique_malloc_ptr
	TODO: define the [] operator conditionally for malloc pointers
		in the base "excl_ptr.hh"
	$Id: excl_malloc_ptr.hh,v 1.3 2006/01/22 06:53:42 fang Exp $
 */

#ifndef	__UTIL_MEMORY_EXCL_MALLOC_PTR_H__
#define	__UTIL_MEMORY_EXCL_MALLOC_PTR_H__

#include "util/memory/excl_ptr.hh"
#include "util/memory/deallocation_policy.hh"

namespace util {
namespace memory {

template <class T>
struct excl_malloc_ptr {
	typedef	excl_ptr<T, free_tag>		type;
};

}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_EXCL_MALLOC_PTR_H__

