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
	$Id: count_array_ptr.tcc,v 1.1 2005/12/10 03:56:58 fang Exp $
 */

#ifndef	__UTIL_MEMORY_COUNT_ARRAY_PTR_TCC__
#define	__UTIL_MEMORY_COUNT_ARRAY_PTR_TCC__

#include "util/memory/count_array_ptr.h"

// Coercively include the template file.
// If the standard template file was already included, 
// remember to restore its inclusion protection at the end of this file.
#ifdef	__UTIL_MEMORY_COUNT_PTR_TCC__
#undef	__UTIL_MEMORY_COUNT_PTR_TCC__
// #undef	__UTIL_MEMORY_POINTER_MANIPULATOR_TCC__
#define	REDEFINE_UTIL_MEMORY_COUNT_PTR_TCC
#endif

// declare intent to override!
// this is not needed until the .tcc file is included
#define DELETE_POLICY(x)	delete [] x

// and we substitute names here!!!
#define	count_ptr		count_array_ptr
#define	raw_count_ptr		raw_count_array_ptr

// and now the magic:
#include "util/memory/count_ptr.tcc"
// DELETE_POLICY will be undefined by this file when it returns
// NOTE: the definitions in pointer_manipulator will also be affected!

// ... but only for the duration of this file
#undef	count_ptr
#undef	raw_count_ptr

// restore the inclusion protection if necessary
#ifdef	REDEFINE_UTIL_MEMORY_COUNT_PTR_TCC
#undef	REDEFINE_UTIL_MEMORY_COUNT_PTR_TCC
#define	__UTIL_MEMORY_COUNT_PTR_TCC__
// #define	__UTIL_MEMORY_POINTER_MANIPULATOR_TCC__
#endif

#endif	// __UTIL_MEMORY_COUNT_ARRAY_PTR_TCC__

