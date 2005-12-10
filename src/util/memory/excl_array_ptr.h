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
	$Id: excl_array_ptr.h,v 1.1 2005/12/10 03:56:59 fang Exp $
 */

#ifndef	__UTIL_MEMORY_EXCL_ARRAY_PTR_H__
#define	__UTIL_MEMORY_EXCL_ARRAY_PTR_H__

// Coercively include the template file.
// If the standard template file was already included, 
// remember to restore its inclusion protection at the end of this file.
#ifdef	__UTIL_MEMORY_EXCL_PTR_H__
#undef	__UTIL_MEMORY_EXCL_PTR_H__
// #undef	__UTIL_MEMORY_POINTER_MANIPULATOR_H__
#define	REDEFINE_UTIL_MEMORY_EXCL_PTR_H
#endif

// declare intent to override!
#define DELETE_POLICY(x)	delete [] x

// and we substitute names here!!!
#define	excl_ptr		excl_array_ptr
#define	excl_ptr_ref		excl_array_ptr_ref
#define	sticky_ptr		sticky_array_ptr
#define	some_ptr		some_array_ptr
#define	unique_ptr		unique_array_ptr
#define	never_ptr		never_array_ptr

// some forward declarations to help us along
namespace util {
namespace memory {
template <class> class never_array_ptr;
template <class> class some_array_ptr;
}	// end namespace memory
}	// end namespace util
// template <class, class> class excl_array_ptr;
// #define	EXCL_PTR_TEMPLATE_CLASS		excl_array_ptr<T,TP>

// and now the magic:
#include "util/memory/excl_ptr.h"
// DELETE_POLICY will be undefined by this file when it returns
// NOTE: the definitions in pointer_manipulator will also be affected!

// ... but only for the duration of this file
#undef	excl_ptr
#undef	excl_ptr_ref
#undef	sticky_ptr
#undef	some_ptr
#undef	unique_ptr
#undef	never_ptr

// restore the inclusion protection if necessary
#ifdef	REDEFINE_UTIL_MEMORY_EXCL_PTR_H
#undef	REDEFINE_UTIL_MEMORY_EXCL_PTR_H
#define	__UTIL_MEMORY_EXCL_PTR_H__
// #define	__UTIL_MEMORY_POINTER_MANIPULATOR_H__
#endif

#endif	// __UTIL_MEMORY_EXCL_ARRAY_PTR_H__

