/**
	\file "hash_map.h"
	Header-wrapper for gcc-version-specific placement of <hash_map>.
 */

#ifndef	__HASH_MAP_H__
#define	__HASH_MAP_H__

#if defined(HASH_MAP_NAMESPACE)
#error	"HASH_MAP_NAMESPACE is already defined, but I need to set it."
#endif

// compiler-version dependent location of hash_map
#ifdef  __GNUC__
#if	(__GNUC__ >= 3)		// works on Mac OS X gcc-3.3, linux gcc-3.2
#include <ext/hash_map>
#define	HASH_MAP_NAMESPACE	__gnu_cxx

#else	// (__GNUC__ <= 2)	// works on FreeBSD gcc-2.95.3
#include <hash_map>
#define	HASH_MAP_NAMESPACE	std
#endif	// gcc version

#else	// __GNUC__
// your guess is as good as mine
#error	"If you know where <hash_map> is for your compiler, add it here."

#endif	// __GNUC__


using HASH_MAP_NAMESPACE::hash_map;

#endif	// __HASH_MAP_H__

