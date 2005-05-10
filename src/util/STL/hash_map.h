/**
	\file "util/STL/hash_map.h"
	Header-wrapper for gcc-version-specific placement of <hash_map>.
	$Id: hash_map.h,v 1.3 2005/05/10 04:51:31 fang Exp $
 */

#ifndef	__UTIL_STL_HASH_MAP_H__
#define	__UTIL_STL_HASH_MAP_H__

#include "util/STL/hash_map_fwd.h"

// compiler-version dependent location of hash_map
#ifdef  __GNUC__
#if	(__GNUC__ >= 3)		// works on Mac OS X gcc-3.3, linux gcc-3.2
#include <ext/hash_map>

#else	// (__GNUC__ <= 2)	// works on FreeBSD gcc-2.95.3
#include <hash_map>
#endif	// gcc version

#else	// __GNUC__
// your guess is as good as mine
#error	"If you know where <hash_map> is for your compiler, add it here."

#endif	// __GNUC__


#endif	// __STL_HASH_MAP_H__

