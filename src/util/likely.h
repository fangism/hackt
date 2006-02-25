/**
	\file "util/likely.h"
	Macros for static branch prediction.  
	Idea from linux kernel code.  

	$Id: likely.h,v 1.3 2006/02/25 04:55:03 fang Exp $
 */
#ifndef	__UTIL_LIKELY_H__
#define	__UTIL_LIKELY_H__

#include "config.h"

/***
	Useful macro definition for static branch predictions.
	Use in case where one direction clearly dominates.  
	e.g. error-handling is "unlikely".
	Known to be supported in gcc-3.x and higher.  
	I've also seen definition using !!(x)
***/
#ifdef HAVE_BUILTIN_EXPECT
	#define	LIKELY(x)	__builtin_expect(!!(x), 1)
	#define UNLIKELY(x)	__builtin_expect(!!(x), 0)
#else
	#define	LIKELY(x)	(x)
	#define UNLIKELY(x)	(x)
#endif

#endif	// __UTIL_LIKELY_H__

