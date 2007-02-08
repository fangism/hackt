/**
	\file "util/libc_temp.h"
	libc functions related to temporary files.
	$Id: libc_temp.h,v 1.1 2007/02/08 20:34:27 fang Exp $
 */

#ifndef	__UTIL_LIBC_TEMP_H__
#define	__UTIL_LIBC_TEMP_H__

#include "config.h"
#include "util/c_decl.h"
#include "util/FILE_fwd.h"

/***
	On some systems these functions are in <stdlib.h>
	On other systems they sit in <unistd.h>
***/
#ifdef	__cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif

BEGIN_C_DECLS
//=============================================================================
// reminder: else clauses if we want to provide our own workaround-wrapper
#if	defined(HAVE_TMPFILE)
#else
extern	FILE*	tmpfile(void);
#endif

#if	defined(HAVE_TMPNAM)
#else
extern	char*	tmpnam(char*);
#endif

#if	defined(HAVE_TEMPNAME)
#else
extern	char*	tempname(const char*, const char*);
#endif

#if	defined(HAVE_MKTEMP)
#else
extern	char*	mktemp(char* tmplate);
#endif

#if	defined(HAVE_MKSTEMP)
#else
extern	int	mkstemp(char* tmplate);
#endif

#if	defined(HAVE_MKSTEMPS)
#else
extern	int	mkstemps(char* tmplate, int suffixlen);
#endif

#if	defined(HAVE_MKDTEMP)
#else
extern	char*	mkdtemp(char* tmplate);
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// derived macros -- for this project

/**
	Irritating: I really want an fstream, but mkstemp only gives
	me a file descriptor, already opened exclusively, and I'm too lazy
	to write an iostream interface wrapper around FILE* or fd.
	Suggestion, avoid these altogether for now...
	Once we start to use boost, use boost::iostreams::file_descriptor.
 */
#if	defined(HAVE_MKSTEMP)
#define	USE_MKSTEMP			1
#elif	defined(HAVE_TMPNAM)
#define	USE_MKSTEMP			0
#else
#error	"Your system lacks both mkstemp and tmpnam, but I need one of them!"
#endif

//=============================================================================
END_C_DECLS

#endif	// __UTIL_LIBC_TEMP_H__

