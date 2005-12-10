/**
	\file "util/libc.h"
	Configure-checked header for libstdc functions.  
	Basically all the checks for standard library functions
	and interfaces to their workarounds belong here.  
	Not all desired functions wil necessarily have trivial
	substitution implementations. 
	$Id: libc.h,v 1.1 2005/12/10 03:56:57 fang Exp $
 */

#ifndef	__UTIL_LIBC_H__
#define	__UTIL_LIBC_H__

#include "config.h"

/* can all these checks be skipped if we have STDC_HEADERS? */

/*=============================================================================
 *	<stdlib.h>
 *===========================================================================*/

/* system */
#if	defined(HAVE_SYSTEM) && HAVE_SYSTEM
#else
#error	"Need a substitute for system!"
#endif

/* malloc */
#if	defined(HAVE_MALLOC) && HAVE_MALLOC
#else
#error	"Need a substitute for malloc!"
#endif

/* calloc */
#if	defined(HAVE_CALLOC) && HAVE_CALLOC
#else
#error	"Need a substitute for malloc!"
#endif

/* realloc */
#if	defined(HAVE_REALLOC) && HAVE_REALLOC
#else
#error	"Need a substitute for realloc!"
#endif

/* free */
#if	defined(HAVE_FREE) && HAVE_FREE
#else
#error	"Need a substitute for free!"
#endif

/*=============================================================================
 *	<stdio.h>
 *===========================================================================*/

/* fgets */
#if	defined(HAVE_FGETS) && HAVE_FGETS
#else
#error	"Need a substitute for fgets!"
#endif

/*=============================================================================
 *	<string.h>
 *===========================================================================*/

/* strsep */
#if     defined(HAVE_STRSEP) && HAVE_STRSEP
/* we're good */
#elif   defined(HAVE_STRTOK) && HAVE_STRTOK
/* define a wrapper for strsep that calls strtok */
#else
#error  "Need a suitable substitute for strsep and strtok!"
#endif

/*=============================================================================
 *	<ctype.h>
 *===========================================================================*/
#if	defined(HAVE_CTYPE_H)  && HAVE_CTYPE_H
#include <ctype.h>
#endif

/* isspace */
#if	defined(HAVE_ISSPACE) && HAVE_ISSPACE
#else
#error	"Need a substitute for isspace!"
#endif

/*=============================================================================
 *	<unistd.h>
 *===========================================================================*/
#if	defined(HAVE_UNISTD_H)  && HAVE_UNISTD_H
#include <unistd.h>
#endif

/* getopt */
#if	defined(HAVE_GETOPT) && HAVE_GETOPT
#else
#error	"Need a substitute for getopt!"
#endif

/*=============================================================================
 *	<math.h>
 *	This might go in something like "libm.h".
 *===========================================================================*/

/* sqrt */
#if 	defined(HAVE_SQRT) && HAVE_SQRT
#else
#error	"Need a substitute for sqrt!"
#endif

/*===========================================================================*/
#endif	/* __UTIL_LIBC_H__ */

