/**
	\file "util/memory/excl_array_ptr.h"
	Exclusive-ownership pointers that are array allocated (new []).  
	Provides pointer classes:
		excl_array_ptr
		sticky_array_ptr
		some_array_ptr
		unique_array_ptr
	TODO: define the [] operator conditionally for array pointers
		in the base "excl_ptr.h"
	$Id: excl_array_ptr.h,v 1.1.4.1 2006/01/18 06:25:13 fang Exp $
 */

#ifndef	__UTIL_MEMORY_EXCL_ARRAY_PTR_H__
#define	__UTIL_MEMORY_EXCL_ARRAY_PTR_H__


#include "util/memory/excl_ptr.h"
#include "util/memory/deallocation_policy_fwd.h"

namespace util {
namespace memory {

template <class T>
struct excl_array_ptr {
	typedef	excl_ptr<T, delete_array_tag>		type;
};

}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_EXCL_ARRAY_PTR_H__

