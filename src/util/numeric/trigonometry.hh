/**
	\file "util/numeric/trigonometry.hh"
	Overloads the sincos function with unified function that
	computes both at the same time efficiently.
	Even when unavailable, some compilers may synthesize
	separate sin/cos calls into a single sincos low-level
	built-in or assembly.  
	$Id: trigonometry.hh,v 1.2 2011/05/03 19:21:11 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_TRIGONOMETRY_H__
#define	__UTIL_NUMERIC_TRIGONOMETRY_H__

#include "config.h"
#include <cmath>

namespace util {
namespace numeric {
//-----------------------------------------------------------------------------
/**
	Generic prototype for sincos.
 */
template <class T>
void
sincos(const T a, T& s, T& c) {
	s = sin(a);
	c = cos(a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
void
sincos(const double a, double& s, double& c) {
#ifdef	HAVE_SINCOS
	::sincos(a, &s, &c);
#else
	s = sin(a);
	c = cos(a);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
void
sincos(const float a, float& s, float& c) {
#ifdef	HAVE_SINCOSF
	::sincosf(a, &s, &c);
#else
	s = sinf(a);
	c = cosf(a);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
void
sincos(const long double a, long double& s, long double& c) {
#ifdef	HAVE_SINCOSF
	::sincosl(a, &s, &c);
#else
	s = sinl(a);
	c = cosl(a);
#endif
}

//-----------------------------------------------------------------------------
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_TRIGONOMETRY_H__
