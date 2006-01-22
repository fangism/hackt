/**
	\file "util/STL/vector_fwd.h"
	Forward declaration for std::vector.
	$Id: vector_fwd.h,v 1.4 2006/01/22 06:53:39 fang Exp $
 */

#ifndef	__UTIL_STL_VECTOR_FWD_H__
#define	__UTIL_STL_VECTOR_FWD_H__

#include "util/STL/allocator_fwd.h"

namespace std {

template <class _Tp, class _Alloc>
class vector;

#if 0
// can't to this because <vector> defines the default argument in
// the class definition
template <class _Tp, class _Alloc = std::allocator<_Tp> >
class vector;
#endif

template <class T>
struct default_vector {
	typedef	vector<T, std::allocator<T> >		type;
};

}	// end namespace std

#if 0
#define	USING_VECTOR		using std::vector;
#define	DEFAULT_VECTOR(T)	std::vector<T, std::allocator<T> >
#endif

#endif	// __UTIL_STL_VECTOR_FWD_H__

