/**
	\file "util/STL/vector_fwd.hh"
	Forward declaration for std::vector.
	$Id: vector_fwd.hh,v 1.6 2006/04/23 07:37:29 fang Exp $
 */

#ifndef	__UTIL_STL_VECTOR_FWD_H__
#define	__UTIL_STL_VECTOR_FWD_H__

#include "util/STL/allocator_fwd.hh"

namespace std {

template <class _Tp, class _Alloc>
class vector;

template <class T>
struct default_vector {
	typedef	vector<T, std::allocator<T> >		type;

	template <class S>
	struct rebind : public default_vector<S> { };
};	// end struct default_vector

}	// end namespace std

#endif	// __UTIL_STL_VECTOR_FWD_H__

