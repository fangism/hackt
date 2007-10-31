/**
	\file "util/FILE_fwd.h"
	Forward declaration of FILE type, to avoid including
	<stdio.h> when complete type is not needed.  
	This file is also used to test config/cc.m4:FANG_TYPEDEF_FILE.
	$Id: FILE_fwd.h,v 1.4 2007/10/31 23:16:34 fang Exp $
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

#if	defined(FILE_FWD_CONFLICTS_FWD_DECL) && FILE_FWD_CONFLICTS_FWD_DECL
#  if	defined(__cplusplus)
#    include <cstdio>
#  else
#    include <stdio.h>
#  endif
#elif	defined(HAVE_STRUCT___FILE) && HAVE_STRUCT___FILE
struct __FILE;
typedef struct __FILE	FILE;
#elif	defined(HAVE_STRUCT__IO_FILE) && HAVE_STRUCT__IO_FILE
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
#  include <cstdio>
#else
#  include <stdio.h>
#endif

#endif	/* _FILEDEFED */

#endif	/* __UTIL_FILE_FWD_H__ */

