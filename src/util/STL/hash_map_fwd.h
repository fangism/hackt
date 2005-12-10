/**
	\file "util/STL/hash_map_fwd.h"
	Header-wrapper for gcc-version-specific placement of <hash_map>.
	\todo Make this configuration dependent on ac_cxx_ext_hash_map.
	$Id: hash_map_fwd.h,v 1.5 2005/12/10 03:56:58 fang Exp $
 */

#ifndef	__UTIL_STL_HASH_MAP_FWD_H__
#define	__UTIL_STL_HASH_MAP_FWD_H__

#include "config.h"
#include "util/STL/allocator_fwd.h"


#if defined(HASH_MAP_NAMESPACE)
#error	"HASH_MAP_NAMESPACE is already defined, but I need to set it."
#endif

// compiler-version dependent location of hash_map
#if	defined(HAVE_EXT_HASH_MAP) && HAVE_EXT_HASH_MAP
#define	HASH_MAP_NAMESPACE	__gnu_cxx
#elif	defined(HAVE_HASH_MAP) && HAVE_HASH_MAP
#define	HASH_MAP_NAMESPACE	std
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
	hash_map<K, T, HASH_MAP_NAMESPACE::hash<K>,			\
		std::equal_to<K>, std::allocator<T> >

#endif	// __UTIL_STL_HASH_MAP_FWD_H__

