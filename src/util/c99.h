/**
	\file "util/c99.h"
	Defines __STDC_VERSION__ manually in C++ mode.  
	Useful for C99 sources compiled as C++.  
	$Id: c99.h,v 1.1 2006/07/04 20:46:02 fang Exp $
 */

#ifndef	__UTIL_C99_H__
#define	__UTIL_C99_H__

/**
#include "config.h"
need for __restrict?
**/

#ifdef	__cplusplus
#ifndef	__STDC_VERSION__
#define	__STDC_VERSION__	199901L
#endif
#endif

#endif	/* __UTIL_C99_H__ */

