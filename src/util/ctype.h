/**
	\file "util/ctype.h"
	Configure-dependent ctype.h standard header.  
	We uncomment each definition as it is used.  
	Recommended: Include this instead of <cctype> or <ctype.h> directly.
	$Id: ctype.h,v 1.1.4.3 2006/01/18 06:25:04 fang Exp $
 */

#ifndef	__UTIL_CTYPE_H__
#define	__UTIL_CTYPE_H__

#include "config.h"
#include "util/c_decl.h"

#if	defined(__cplusplus) && defined(HAVE_CTYPE_H)  && HAVE_CTYPE_H
#include <cctype>
#elif	defined(HAVE_CTYPE_H) && HAVE_CTYPE_H
#include <ctype.h>
#endif

BEGIN_C_DECLS
/* isspace */
#if	defined(HAVE_ISSPACE) && HAVE_ISSPACE
#else
#error	"Need a substitute for isspace!"
#endif

/* isascii */
#if	defined(HAVE_ISASCII) && HAVE_ISASCII
#else
#error	"Need a substitute for isascii!"
#endif

/* isdigit */
#if	defined(HAVE_ISDIGIT) && HAVE_ISDIGIT
#else
#error	"Need a substitute for isdigit!"
#endif

/* isalpha */
#if	defined(HAVE_ISALPHA) && HAVE_ISALPHA
#else
#error	"Need a substitute for isalpha!"
#endif

/* isalnum */
#if	defined(HAVE_ISALNUM) && HAVE_ISALNUM
#else
#error	"Need a substitute for isalnum!"
#endif

/* islower */
#if	defined(HAVE_ISLOWER) && HAVE_ISLOWER
#else
#error	"Need a substitute for islower!"
#endif

/* isupper */
#if	defined(HAVE_ISUPPER) && HAVE_ISUPPER
#else
#error	"Need a substitute for isupper!"
#endif

#if 0
/* iscntrl */
#if	defined(HAVE_ISCNTRL) && HAVE_ISCNTRL
#else
#error	"Need a substitute for iscntrl!"
#endif

/* isgraph */
#if	defined(HAVE_ISGRAPH) && HAVE_ISGRAPH
#else
#error	"Need a substitute for isgraph!"
#endif

/* ishexnumber */
#if	defined(HAVE_ISHEXNUMBER) && HAVE_ISHEXNUMBER
#else
#error	"Need a substitute for ishexnumber!"
#endif

/* isideogram */
#if	defined(HAVE_ISIDEOGRAM) && HAVE_ISIDEOGRAM
#else
#error	"Need a substitute for isideogram!"
#endif

/* isnumber */
#if	defined(HAVE_ISNUMBER) && HAVE_ISNUMBER
#else
#error	"Need a substitute for isnumber!"
#endif

/* isphonogram */
#if	defined(HAVE_ISPHONOGRAM) && HAVE_ISPHONOGRAM
#else
#error	"Need a substitute for isphonogram!"
#endif

/* isspecial */
#if	defined(HAVE_ISSPECIAL) && HAVE_ISSPECIAL
#else
#error	"Need a substitute for isspecial!"
#endif
#endif	/* disabled until needed */

/* isprint */
#if	defined(HAVE_ISPRINT) && HAVE_ISPRINT
#else
#error	"Need a substitute for isprint!"
#endif

/* ispunct */
#if	defined(HAVE_ISPUNCT) && HAVE_ISPUNCT
#else
#error	"Need a substitute for ispunct!"
#endif

#if 0
/* isrune */
#if	defined(HAVE_ISRUNE) && HAVE_ISRUNE
#else
#error	"Need a substitute for isrune!"
#endif

/* isxdigit */
#if	defined(HAVE_ISXDIGIT) && HAVE_ISXDIGIT
#else
#error	"Need a substitute for isxdigit!"
#endif
#endif	/* disabled until needed */

/* tolower */
#if	defined(HAVE_TOLOWER) && HAVE_TOLOWER
#else
#error	"Need a substitute for tolower!"
#endif

/* toupper */
#if	defined(HAVE_TOUPPER) && HAVE_TOUPPER
#else
#error	"Need a substitute for toupper!"
#endif

/* I think that just about covers it */
END_C_DECLS

#endif	/* __UTIL_CTYPE_H__ */

