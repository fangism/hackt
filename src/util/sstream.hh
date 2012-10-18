/**
	\file "util/sstream.hh"
	Header-wrapper for gcc-version-specific placement of <sstream>

	IMPORTANT NOTE:
	The deprecated <strstream> in older versions of some compilers is 
	known to suck, because of error-prone memory non-management.  
	$Id: sstream.hh,v 1.5 2005/09/04 21:15:08 fang Exp $
 */


#ifndef	__UTIL_SSTREAM_H__
#define	__UTIL_SSTREAM_H__

#include "config.h"

#if defined(HAVE_SSTREAM) && HAVE_SSTREAM
#include <sstream>
#elif defined(HAVE_STRSTREAM) && HAVE_STRSTREAM
#include <strstream>		// BAD
#warn	WARNING: <strstream> is deprecated and suffers from memory mis-management.
#elif defined(HAVE_STRINGSTRING) && HAVE_STRINGSTREAM
// reminiscent of some ancient pre-standard compilers and libraries
#include <stringstream>
#else
#error	You asked for "sstream.h" but I could not find a candidate header.
#endif

#endif	// __UTIL_SSTREAM_H__

