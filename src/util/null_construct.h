/**
	\file "util/null_construct.h"
	Uses traits to initializes a type with a default value.
	Might consider making this a struct for better specializability.  
	$Id: null_construct.h,v 1.4 2006/01/22 06:53:35 fang Exp $
 */

#ifndef	__UTIL_NULL_CONSTRUCT_H__
#define	__UTIL_NULL_CONSTRUCT_H__

#include "util/NULL.h"

// #include "util/STL/construct_fwd.h"
#include "util/memory/pointer_traits.h"

namespace util {
USING_UTIL_MEMORY_POINTER_TRAITS

template <class T>
T
__null_construct(not_a_pointer_tag) {
	return T();
//	return std::_Construct<T>();
}

template <class P>
P
__null_construct(raw_pointer_tag) {
	return NULL;
}

template <class P>
P
__null_construct(pointer_class_base_tag) {
	return P();
}

/**
	Pointer types are guaranteed to return NULL, 
	non pointer-types will default to empty construction.  
	Use this to guarantee that returning an uninitialized pointer
	will be NULL -- because by default they are junk.  
 */
template <class T>
T
null_construct(void) {
	typedef	typename pointer_category<T>::type	pointer_category;
	return __null_construct<T>(pointer_category());
}

}	// end namespace util

#endif	// __UTIL_NULL_CONSTRUCT_H__

