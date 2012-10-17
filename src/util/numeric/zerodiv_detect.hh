/**
	\file "util/numeric/zerodiv_detect.hh"
	Template function overloads for detecting divide by zero.  
	$Id: zerodiv_detect.hh,v 1.1 2006/07/16 03:35:00 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_ZERODIV_DETECT_H__
#define	__UTIL_NUMERIC_ZERODIV_DETECT_H__

#include <limits>

namespace util {
namespace numeric {
//=============================================================================
/**
	\param T the number type
	\param IsInteger if type T is integral (true)
 */
template <typename T, bool IsInteger>
struct zerodiv_detect_t {
	bool
	operator () (const T v) const {
		return !v;
	}
};	// end struct zerodiv_detect_t

/**
	Partial specialization for non-integers.  
 */
template <typename T>
struct zerodiv_detect_t<T, false> {
	bool
	operator () (const T& v) const {
		return v < std::numeric_limits<T>::min();
	}
};	// end struct zerodiv_detect_t

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Use this function, instead of struct functors, 
	this automatically proxies.  
	\param T is integral and can be compared with 0.  
 */
template <typename T>
inline
bool
zerodiv_detect(const T v) {
	typedef zerodiv_detect_t<T, std::numeric_limits<T>::is_integer>
						__detector_type;
	return __detector_type()(v);
}

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_ZERODIV_DETECT_H__

