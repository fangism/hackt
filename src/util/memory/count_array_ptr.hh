/**
	\file "util/memory/count_array_ptr.hh"
	Exclusive-ownership pointers that are array allocated (new []).  
	Provides pointer classes:
		count_array_ptr
		count_array_ptr_ref
	TODO: define the [] operator conditionally for array pointers
		in the base "count_ptr.hh"
	TODO: technically, count_ptr_ref can be used for all variants
		because it never actually deletes its pointer.  
	$Id: count_array_ptr.hh,v 1.2 2006/01/22 06:53:41 fang Exp $
 */

#ifndef	__UTIL_MEMORY_COUNT_ARRAY_PTR_H__
#define	__UTIL_MEMORY_COUNT_ARRAY_PTR_H__

#include "util/memory/count_ptr.hh"
#include "util/memory/deallocation_policy_fwd.hh"

namespace util {
namespace memory {

template <class T>
struct count_array_ptr {
	typedef	count_ptr<T, delete_array_tag>		type;
};

}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_COUNT_ARRAY_PTR_H__

