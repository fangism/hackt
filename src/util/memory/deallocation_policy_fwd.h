/**
	\file "util/memory/deallocation_policy_fwd.h"
	Forward declarations of names of deallocation policies.  
	Their definitions appear in "util/memory/deallocation_policy.h".
	$Id: deallocation_policy_fwd.h,v 1.1.2.1 2006/01/17 20:55:28 fang Exp $
 */

#ifndef	__UTIL_MEMORY_DEALLOCATION_POLICY_FWD_H__
#define	__UTIL_MEMORY_DEALLOCATION_POLICY_FWD_H__

namespace util {
namespace memory {
//=============================================================================

struct delete_tag;

struct delete_array_tag;

struct free_tag;
typedef	free_tag	malloc_tag;

struct fclose_tag;
typedef	fclose_tag	FILE_tag;

struct iostream_tag;

template <class T, void (*f)(T*)>
struct custom_ptr_fun_tag;

template <class T, void (f)(T*)>
struct custom_fun_ref_tag;

template <class F>
struct custom_functor_tag;

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_DEALLOCATION_POLICY_FWD_H__

