/**
	\file "util/STL/vector_fwd.h"
	Forward declaration for std::vector.
	$Id: vector_fwd.h,v 1.4.32.1 2006/04/17 03:04:11 fang Exp $
 */

#ifndef	__UTIL_STL_VECTOR_FWD_H__
#define	__UTIL_STL_VECTOR_FWD_H__

#include "util/STL/allocator_fwd.h"

namespace std {

template <class _Tp, class _Alloc>
class vector;

template <class T>
struct default_vector {
	typedef	vector<T, std::allocator<T> >		type;
};

}	// end namespace std

#endif	// __UTIL_STL_VECTOR_FWD_H__

