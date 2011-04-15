/**
	\file "util/numeric/zero.h"
	What we need is a <limit> member for zero.
	For a consistent type-safe way of getting 0 or 0.0.
	$Id: zero.h,v 1.1.2.1 2011/04/15 00:52:09 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_ZERO_H__
#define	__UTIL_NUMERIC_ZERO_H__

#include <limits>

namespace util {
namespace numeric {

//-----------------------------------------------------------------------------
/**
	\param B true if type is_integer
 */
template <bool B>
struct __zero_impl {

	template <class T>
	static
	inline
	T
	value(void) {
		return 0;
	}
};	// end struct __zero_impl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// partial specialization for floating-point types
template <>
struct __zero_impl<false> {

	template <class T>
	static
	inline
	T
	value(void) {
		return 0.0;
	}
};	// end struct __zero_impl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
T
zero(void) {
	return __zero_impl<std::numeric_limits<T>::is_integer>::template value<T>();
}

//-----------------------------------------------------------------------------
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_ZERO_H__

