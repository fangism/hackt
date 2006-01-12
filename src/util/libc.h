/**
	\file "util/libc.h"

	Include this file where there may be references to missing 
	libc functions.  

	Configure-checked header for libstdc functions.  
	Basically all the checks for standard library functions
	and interfaces to their workarounds belong here.  
	Not all desired functions wil necessarily have trivial
	substitution implementations. 
	NOTE: that these declarations are not extern "C", 
	because we will compile libc.c in C++.  

	$Id: libc.h,v 1.1.4.2 2006/01/12 21:31:49 fang Exp $
 */

#ifndef	__UTIL_LIBC_H__
#define	__UTIL_LIBC_H__

#include "config.h"
#include "util/FILE_fwd.h"
/* #include "util/size_t.h" */

/* can all these checks be skipped if we have STDC_HEADERS? */

/*=============================================================================
 *	<stdlib.h>
 *===========================================================================*/

/* system */
#if	defined(HAVE_SYSTEM) && HAVE_SYSTEM
#else
#error	"Need a substitute for system!"
#endif

/*-------------------- memory management functions --------------------------*/

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

/*------------------- environment variable functions ------------------------*/

#if	defined(HAVE_GETENV) && HAVE_GETENV
#else
extern char*	getenv(const char*);
#endif

#if	defined(HAVE_SETENV) && HAVE_SETENV
#else
extern int	setenv(const char*, const char*, int);
#endif

#if	defined(HAVE_PUTENV) && HAVE_PUTENV
#else
extern int	putenv(const char*);
#endif

#if	defined(HAVE_UNSETENV) && HAVE_UNSETENV
#else
extern void	unsetenv(const char*);
#endif


/*=============================================================================
 *	<stdio.h>
 *===========================================================================*/

/* fgets */
#if	defined(HAVE_FGETS) && HAVE_FGETS
#else
#error	"Need a substitute for fgets!"
extern char*	fgets(char*, int, FILE*);
#endif

#if	defined(HAVE_SETBUF) && HAVE_SETBUF
#else
extern void	setbuf(FILE*, char*);
#endif

#if	defined(HAVE_SETBUFFER) && HAVE_SETBUFFER
#else
extern void	setbuffer(FILE*, char*, int);
#endif

#if	defined(HAVE_SETLINEBUF) && HAVE_SETLINEBUF
#else
extern int	setlinebuf(FILE*);
#endif

#if	defined(HAVE_SETVBUF) && HAVE_SETVBUF
#else
extern int	setvbuf(FILE*, char&, int, size_t);
#endif

#if	defined(HAVE_TMPFILE) && HAVE_TMPFILE
#else
extern FILE*	tmpfile(void);
#endif

#if	defined(HAVE_TMPNAME) && HAVE_TMPNAME
#else
extern char*	tmpname(char*);
#endif

#if	defined(HAVE_TEMPNAME) && HAVE_TEMPNAME
#else
extern char*	tempname(const char*, const char*);
#endif

/*=============================================================================
 *	<string.h>
 *===========================================================================*/

/* strdup */
#if	defined(HAVE_STRDUP) && HAVE_STRDUP
#else
/* consider templating for other char types */
extern char*	strdup(const char*);
#endif

/* strsep */
#if     defined(HAVE_STRSEP) && HAVE_STRSEP
#else
/* define a wrapper for strsep that calls strtok */
extern char*	strsep(char**, const char*);
#endif

/* strtok: note this is obsolete, we don't actually want to use it */
#if   defined(HAVE_STRTOK) && HAVE_STRTOK
#else
extern char*	strtok(char*, const char*);
#endif

/* strtok_r: re-entrant strtok */
#if   defined(HAVE_STRTOK_R) && HAVE_STRTOK_R
#else
extern char*	strtok_r(char*, const char*, char**);
#endif

/*=============================================================================
 *	<ctype.h>
 *===========================================================================*/
#if	defined(__cplusplus) && defined(HAVE_CTYPE_H)  && HAVE_CTYPE_H
#include <cctype>
#elif	defined(HAVE_CTYPE_H)  && HAVE_CTYPE_H
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

