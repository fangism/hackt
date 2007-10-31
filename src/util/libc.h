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

	$Id: libc.h,v 1.10 2007/10/31 23:16:35 fang Exp $
 */

#ifndef	__UTIL_LIBC_H__
#define	__UTIL_LIBC_H__

#include "config.h"
#include "util/c_decl.h"
#include "util/FILE_fwd.h"
/* #include "util/size_t.h" */

/* can all these checks be skipped if we have STDC_HEADERS? */

/*=============================================================================
 *	<stdlib.h>
 *===========================================================================*/
BEGIN_C_DECLS
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

/*-------------------- random number generators -----------------------------*/
#if	!defined(HAVE_CXX_RAND48_FAMILY)
#if	defined(HAVE_DRAND48)
extern	double	drand48(void);
#endif

#if	defined(HAVE_ERAND48)
extern	double	erand48(unsigned short[3]);
#endif

#if	defined(HAVE_LRAND48)
extern	long	lrand48(void);
#endif

#if	defined(HAVE_NRAND48)
extern	long	nrand48(unsigned short[3]);
#endif

#if	defined(HAVE_MRAND48)
extern	long	mrand48(void);
#endif

#if	defined(HAVE_JRAND48)
extern	long	jrand48(unsigned short[3]);
#endif

#if	defined(HAVE_SRAND48)
extern	void	srand48(long sd);
#endif

#if	defined(HAVE_SEED48)
extern	unsigned short*	seed48(unsigned short xs[3]);
#endif

#if	defined(HAVE_LCONG48)
extern	void	lcong48(unsigned short p[7]);
#endif
#endif	/* HAVE_CXX_RAND48_FAMILY */

END_C_DECLS

/*=============================================================================
 *	<stdio.h>
 *===========================================================================*/
BEGIN_C_DECLS
#if	defined(HAVE_FGETS) && HAVE_FGETS
#else
extern char*	fgets(char*, int, FILE*);
#endif

#if	defined(HAVE_FPUTS) && HAVE_FPUTS
#else
extern int	fputs(const char*, FILE*);
#endif

#if	defined(HAVE_FGETC) && HAVE_FGETC
#else
extern int	fgetc(FILE*);
#endif

#if	defined(HAVE_FPUTC) && HAVE_FPUTC
#else
extern int	fputc(const int, FILE*);
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
extern int	setvbuf(FILE*, char*, int, size_t);
#endif

#if 0
#if	defined(HAVE_TMPFILE) && HAVE_TMPFILE
#else
extern FILE*	tmpfile(void);
#endif

#if	defined(HAVE_TMPNAM)
#else
extern char*	tmpnam(char*);
#endif

#if	defined(HAVE_TEMPNAME)
#else
extern char*	tempname(const char*, const char*);
#endif
#endif

#if	defined(HAVE_FEOF) && HAVE_FEOF
#else
extern int	feof(const FILE*);
#endif

#if	defined(HAVE_FERROR) && HAVE_FERROR
#else
extern int	ferror(const FILE*);
#endif

#if	defined(HAVE_CLEARERR) && HAVE_CLEARERR
#else
extern void	clearerr(FILE*);
#endif

#if	defined(HAVE_FSEEK) && HAVE_FSEEK
#else
extern int	fseek(FILE*, const long, const int);
#endif

#if	defined(HAVE_FTELL) && HAVE_FTELL
#else
extern long	ftell(const FILE*);
#endif

#if	defined(HAVE_REWIND) && HAVE_REWIND
#else
extern void	rewind(FILE*);
#endif

/* may need <sys/types.h> */
#if	defined(HAVE_FSETPOS) && HAVE_FSETPOS
#else
extern int	fsetpos(FILE*, fpos_t*);
#endif

#if	defined(HAVE_FGETPOS) && HAVE_FGETPOS
#else
extern int	fgetpos(FILE*, const fpos_t*);
#endif

#if	defined(HAVE_REMOVE) && HAVE_REMOVE
#else
extern int	remove(const char*);
#endif

#if	defined(HAVE_RENAME) && HAVE_RENAME
#else
extern int	rename(const char*, const char*);
#endif

/* non-ANSI functions that may be missing in strict-ansi modes */
#if	defined(HAVE_FDOPEN) && !defined(HAVE_CXX_FDOPEN)
extern	FILE*	fdopen(int, const char*);
#endif

#if	defined(HAVE_POPEN) && !defined(HAVE_CXX_POPEN)
extern	FILE*	popen(const char*, const char*);
#endif

END_C_DECLS

/*=============================================================================
 *	<string.h>
 *===========================================================================*/
BEGIN_C_DECLS
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
END_C_DECLS

/*=============================================================================
 *	<ctype.h>
 *===========================================================================*/
#include "util/ctype.h"

/*=============================================================================
 *	<unistd.h>
 *===========================================================================*/
#if	defined(HAVE_UNISTD_H)  && HAVE_UNISTD_H
#include <unistd.h>
#endif

BEGIN_C_DECLS
/* getopt */
#if	defined(HAVE_GETOPT) && HAVE_GETOPT
#else
#error	"Need a substitute for getopt!"
#endif
END_C_DECLS

/*=============================================================================
 *	<math.h>
 *	This might go in something like "libm.h".
 *===========================================================================*/
BEGIN_C_DECLS
#if 0
/* sqrt */
#if 	defined(HAVE_SQRT) && HAVE_SQRT
#else
#error	"Need a substitute for sqrt!"
#endif
#endif
END_C_DECLS

/*===========================================================================*/
#endif	/* __UTIL_LIBC_H__ */

