/**
	\file "hash_map.h"
	Header-wrapper for gcc-version-specific placement of <hash_map>.
 */

#ifndef	__HASH_MAP_H__
#define	__HASH_MAP_H__

// compiler-version dependent location of hash_map
#ifdef  __GNUC__
#if	(__GNUC__ >= 3)		// works on Mac OS X gcc-3.3, linux gcc-3.2
#include <ext/hash_map>
using __gnu_cxx::hash_map;

#else	// (__GNUC__ <= 2)	// works on FreeBSD gcc-2.95.3
#include <hash_map>
using std::hash_map;
#endif
#else	// __GNUC__
DIE DIE DIE			// your guess is as good as mine
#endif	// __GNUC__

#endif	// __HASH_MAP_H__

