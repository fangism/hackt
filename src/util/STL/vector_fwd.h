/**
	\file "vector_fwd.h"
	Forward declaration for std::vector.
	$Id: vector_fwd.h,v 1.2 2005/01/28 19:04:00 fang Exp $
 */

#ifndef	__UTIL_STL_VECTOR_FWD_H__
#define	__UTIL_STL_VECTOR_FWD_H__

#include "STL/allocator_fwd.h"

namespace std {

template <class _Tp, class _Alloc>
class vector;

}	// end namespace std

#define	USING_VECTOR		using std::vector;
#define	DEFAULT_VECTOR(T)	std::vector<T, std::allocator<T> >

#endif	// __UTIL_STL_VECTOR_FWD_H__

