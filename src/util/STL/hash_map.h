/**
	\file "util/STL/hash_map.h"
	Header-wrapper for gcc-version-specific placement of <hash_map>.
	$Id: hash_map.h,v 1.5.140.1 2008/11/19 05:45:03 fang Exp $
 */

#ifndef	__UTIL_STL_HASH_MAP_H__
#define	__UTIL_STL_HASH_MAP_H__

#include "util/STL/hash_map_fwd.h"

#if defined(HAVE_UNORDERED_MAP)
#include <unordered_map>		// C++0x
// #elif defined(HAVE_TR1_UNORDERED_MAP)
// #include <tr1/unordered_map>
#elif defined(HAVE_EXT_HASH_MAP) && HAVE_EXT_HASH_MAP
#include <ext/hash_map>
#elif defined(HAVE_HASH_MAP) && HAVE_HASH_MAP
#include <hash_map>
#else
// your guess is as good as mine
#error	"If you know where <hash_map> is for your compiler, add it here."
#endif

#endif	// __STL_HASH_MAP_H__

