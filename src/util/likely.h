/**
	\file "util/likely.h"
	Macros for static branch prediction.  
	Idea from linux kernal code.  

	$Id: likely.h,v 1.2 2005/05/10 04:51:25 fang Exp $
 */
#ifndef	__UTIL_LIKELY_H__
#define	__UTIL_LIKELY_H__


/***
	I've also seen definition using !!(x)
***/
#if defined(__GNU_C__) && __GNU_C__ >= 3
	#define	LIKELY(x)	__builtin_expect(!!(x), 1)
	#define UNLIKELY(x)	__builtin_expect(!!(x), 0)
#else
	#define	LIKELY(x)	(x)
	#define UNLIKELY(x)	(x)
#endif

#endif	// __UTIL_LIKELY_H__

