/**
	\file "util/null_construct.hh"
	Uses traits to initializes a type with a default value.
	Might consider making this a struct for better specializability.  
	$Id: null_construct.hh,v 1.5 2006/04/27 00:17:08 fang Exp $
 */

#ifndef	__UTIL_NULL_CONSTRUCT_H__
#define	__UTIL_NULL_CONSTRUCT_H__

#include "util/NULL.h"

// #include "util/STL/construct_fwd.hh"
#include "util/memory/pointer_traits.hh"

namespace util {
USING_UTIL_MEMORY_POINTER_TRAITS
//=============================================================================

template <class T>
T
__null_construct(not_a_pointer_tag);

template <class T>
T
__null_construct(not_a_pointer_tag) {
	return T();
//	return std::_Construct<T>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
P
__null_construct(raw_pointer_tag);

template <class P>
P
__null_construct(raw_pointer_tag) {
	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
P
__null_construct(pointer_class_base_tag);

template <class P>
P
__null_construct(pointer_class_base_tag) {
	return P();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
T
null_construct(void);

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

//=============================================================================
}	// end namespace util

#endif	// __UTIL_NULL_CONSTRUCT_H__

