/**
	\file "util/FILE_fwd.h"
	Forward declaration of FILE type, to avoid including
	<stdio.h> when complete type is not needed.  
	$Id: FILE_fwd.h,v 1.3 2006/04/13 21:45:06 fang Exp $
 */

#ifndef	__UTIL_FILE_FWD_H__
#define	__UTIL_FILE_FWD_H__

#include "config.h"

/**
	known case for Solaris-gcc-3.3,
	some header also conditionally forward-declares the typedef
 */
#ifndef	_FILEDEFED
#define	_FILEDEFED

#if	defined(HAVE_STRUCT__IO_FILE) && HAVE_STRUCT__IO_FILE
struct _IO_FILE;
typedef struct _IO_FILE	FILE;
#elif defined(HAVE_STRUCT___SFILE) && HAVE_STRUCT___SFILE
struct __sFILE;
typedef struct __sFILE	FILE;
#elif defined(HAVE_STRUCT___FILE_TAG) && HAVE_STRUCT___FILE_TAG
struct __FILE_TAG;
typedef	struct __FILE_TAG FILE;
#elif defined(HAVE___FILE_TAG) && HAVE___FILE_TAG
typedef	__FILE_TAG	FILE;
#elif defined(HAVE___FILE) && HAVE___FILE
typedef	__FILE		FILE;

/*
	other KNOWN typedefs:

	NOT forward-declarable:
		alpha-osf-*: typedef struct { ... } FILE;
 */

/* else fallback is to include stdio header */
#elif	defined(__cplusplus)
#include <cstdio>
#else
#include <stdio.h>
#endif

#endif	/* _FILEDEFED */

#endif	/* __UTIL_FILE_FWD_H__ */

