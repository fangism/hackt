/**
	\file "util/numeric/abs.h"
	Overloaded abs function that dispatches to cstdlib by type.
	This allows use of abs instead of fabs for real types.
	This might not be necessary if std::abs already provides
	overloads.
	$Id: abs.h,v 1.1.2.2 2011/04/22 01:28:24 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_ABS_H__
#define	__UTIL_NUMERIC_ABS_H__

#include "config.h"
#include <cmath>
#include <cstdlib>

namespace util {
namespace numeric {

/**
	The namespace where cstdlib functions live.
	Could be :: or std::!
 */
#define	STD			std

#ifndef	HAVE_ABS
#error	"Dude, seriously?  see config.log for checking for abs() function."
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// integer types

/**
	Default general template.
 */
template <class T>
inline
T
abs(const T& t) {
	return STD::abs(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if SIZEOF_LONG
// && defined(HAVE_LABS)
template <>
inline
long
abs(const long& t) {
	return labs(t);	// already overloaded for long
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if SIZEOF_LONG_LONG && defined(HAVE_LLABS)
template <>
inline
long long
abs(const long long& t) {
	return llabs(t);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// floating point types

#if SIZEOF_FLOAT && defined(HAVE_FABSF)
template <>
inline
float
abs(const float& t) {
	return fabsf(t);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if SIZEOF_DOUBLE && defined(HAVE_FABS)
template <>
inline
double
abs(const double& t) {
	return fabs(t);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if SIZEOF_LONG_DOUBLE && defined(HAVE_FABSL)
template <>
inline
long double
abs(const long double& t) {
	return fabsl(t);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#undef	STD
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_ABS_H__

