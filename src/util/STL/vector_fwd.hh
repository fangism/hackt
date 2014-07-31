/**
	\file "util/STL/vector_fwd.hh"
	Forward declaration for std::vector.
	$Id: vector_fwd.hh,v 1.6 2006/04/23 07:37:29 fang Exp $
 */

#ifndef	__UTIL_STL_VECTOR_FWD_HH__
#define	__UTIL_STL_VECTOR_FWD_HH__

#include "util/STL/allocator_fwd.hh"

BEGIN_NAMESPACE_STD

template <class _Tp, class _Alloc>
class vector;

template <class T>
struct default_vector {
	typedef	vector<T, std::allocator<T> >		type;

	template <class S>
	struct rebind : public default_vector<S> { };
};	// end struct default_vector

END_NAMESPACE_STD

#endif	// __UTIL_STL_VECTOR_FWD_HH__

