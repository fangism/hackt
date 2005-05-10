/**
	\file "util/sstream.h"
	Header-wrapper for gcc-version-specific placement of <sstream>

	IMPORTANT NOTE:
	The deprecated <strstream> in older versions of some compilers is 
	known to suck, because of error-prone memory non-management.  
	$Id: sstream.h,v 1.4 2005/05/10 04:51:30 fang Exp $
 */


#ifndef	__UTIL_SSTREAM_H__
#define	__UTIL_SSTREAM_H__

// should really be configured...
// #include "config.h"

// compiler-version dependent location of sstream
#ifdef  __GNUC__
#if	(__GNUC__ >= 3)
#include <sstream>

#else	// (__GNUC__ <= 2)
#include <strstream>		// BAD
#error	"DO NOT use the deprecated strstream classes!"
#endif
#else	// __GNUC__
DIE DIE DIE			// your guess is as good as mine
#endif	// __GNUC__

#endif	// __UTIL_SSTREAM_H__

