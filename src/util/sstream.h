// "sstream.h"
// header-wrapper for gcc-version-specific placement of <sstream>

// IMPORTANT NOTE:
// The deprecated <strstream> in older versions of some compilers is 
// known to suck, because of error-prone memory non-management.  

#ifndef	__SSTREAM_H__
#define	__SSTREAM_H__

// compiler-version dependent location of hash_map
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

#endif	// __SSTREAM_H__

