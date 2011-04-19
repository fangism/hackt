/**
	\file "util/numeric/constants.h"
	Mathematical constants.
	Here they are defined as preprocessor macros, 
	might later decide to define them as const doubles.
	$Id: constants.h,v 1.1.2.1 2011/04/19 22:31:22 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_CONSTANTS_H__
#define	__UTIL_NUMERIC_CONSTANTS_H__


/**
	The following constants stolen from /usr/include/math.h
	Probably not a bad idea to include <math.h> first.
	These constants only have sufficient precision for double, 
	but not enough for long double.
 */
#ifndef	M_E
#define	M_E		2.7182818284590452354	/* e */
#endif

#ifndef	M_LOG2E
#define	M_LOG2E		1.4426950408889634074	/* log_2 e */
#endif

#ifndef	M_LOG10E
#define	M_LOG10E	0.43429448190325182765	/* log_10 e */
#endif

#ifndef	M_LN2
#define	M_LN2		0.69314718055994530942	/* log_e 2 */
#endif

#ifndef	M_LN10
#define	M_LN10		2.30258509299404568402	/* log_e 10 */
#endif

#ifndef	M_PI
#define	M_PI		3.14159265358979323846	/* pi */
#endif

#ifndef	M_PI_2
#define	M_PI_2		1.57079632679489661923	/* pi/2 */
#endif

#ifndef	M_PI_4
#define	M_PI_4		0.78539816339744830962	/* pi/4 */
#endif

#ifndef	M_1_PI
#define	M_1_PI		0.31830988618379067154	/* 1/pi */
#endif

#ifndef	M_2_PI
#define	M_2_PI		0.63661977236758134308	/* 2/pi */
#endif

#ifndef	M_2_SQRTPI
#define	M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#endif

#ifndef	M_SQRT2
#define	M_SQRT2		1.41421356237309504880	/* sqrt(2) */
#endif

#ifndef	M_SQRT1_2
#define	M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */
#endif

// the following constants I provided from elsewhere

#ifndef	M_TAU
#define	M_TAU		1.6180339887498948482	/* golden ratio */
#endif

#ifndef	M_GAMMA
#define	M_GAMMA		0.57721566490153286061	/* gamma */
#endif

#endif	// __UTIL_NUMERIC_CONSTANTS_H__

