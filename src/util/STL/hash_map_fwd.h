/**
	\file "STL/hash_map_fwd.h"
	Header-wrapper for gcc-version-specific placement of <hash_map>.
	\todo Make this configuration dependent on ac_cxx_ext_hash_map.
	$Id: hash_map_fwd.h,v 1.1.2.1 2005/02/02 07:59:51 fang Exp $
 */

#ifndef	__STL_HASH_MAP_FWD_H__
#define	__STL_HASH_MAP_FWD_H__

// #include "config.h"

#include "STL/allocator_fwd.h"

#if defined(HASH_MAP_NAMESPACE)
#error	"HASH_MAP_NAMESPACE is already defined, but I need to set it."
#endif

// compiler-version dependent location of hash_map
#ifdef  __GNUC__
#if	(__GNUC__ >= 3)		// works on Mac OS X gcc-3.3, linux gcc-3.2
#define	HASH_MAP_NAMESPACE	__gnu_cxx

#else	// (__GNUC__ <= 2)	// works on FreeBSD gcc-2.95.3
#define	HASH_MAP_NAMESPACE	std
#endif	// gcc version

#else	// __GNUC__
// your guess is as good as mine
#error	"If you know where <hash_map> is for your compiler, add it here."

#endif	// __GNUC__

namespace std {
// forward declaration of default comparator
template <class T>
struct equal_to;
}	// end namespace std

namespace HASH_MAP_NAMESPACE {

template <class K>
struct hash;

template <class K, class T, class H, class E, class A>
class hash_map;

#if 0
// gcc doesn't accept this re-declaration as equivalent to STL header's :(
template <class K, class T,
	class H = hash<K>,
	class E = std::equal_to<K>,
	class A = std::allocator<T> >
class hash_map;
#endif

}	// end namespace HASH_MAP_NAMESPACE

#define	USING_HASH	using HASH_MAP_NAMESPACE::hash;
#define	USING_HASH_MAP	using HASH_MAP_NAMESPACE::hash_map;

#define	DEFAULT_HASH_MAP(K,T)						\
	hash_map<K, T, hash<K>, std::equal_to<K>, std::allocator<T> >

#endif	// __STL_HASH_MAP_FWD_H__

