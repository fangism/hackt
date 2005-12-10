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
	$Id: count_malloc_ptr.h,v 1.1 2005/12/10 03:56:58 fang Exp $
 */

#ifndef	__UTIL_MEMORY_COUNT_MALLOC_PTR_H__
#define	__UTIL_MEMORY_COUNT_MALLOC_PTR_H__

// Coercively include the template file.
// If the standard template file was already included, 
// remember to restore its inclusion protection at the end of this file.
#ifdef	__UTIL_MEMORY_COUNT_PTR_H__
#undef	__UTIL_MEMORY_COUNT_PTR_H__
// #undef	__UTIL_MEMORY_POINTER_MANIPULATOR_H__
#define	REDEFINE_UTIL_MEMORY_COUNT_PTR_H
#endif

// declare intent to override!
// the real definition is not needed until the .tcc file is included.
#define DELETE_POLICY

// and we substitute names here!!!
#define	count_ptr		count_malloc_ptr
#define	raw_count_ptr		raw_count_malloc_ptr

// and now the magic:
#include "util/memory/count_ptr.h"
// DELETE_POLICY will be undefined by this file when it returns
// NOTE: the definitions in pointer_manipulator will also be affected!

// ... but only for the duration of this file
#undef	count_ptr
#undef	raw_count_ptr

// restore the inclusion protection if necessary
#ifdef	REDEFINE_UTIL_MEMORY_COUNT_PTR_H
#undef	REDEFINE_UTIL_MEMORY_COUNT_PTR_H
#define	__UTIL_MEMORY_COUNT_PTR_H__
// #define	__UTIL_MEMORY_POINTER_MANIPULATOR_H__
#endif

#endif	// __UTIL_MEMORY_COUNT_MALLOC_PTR_H__

