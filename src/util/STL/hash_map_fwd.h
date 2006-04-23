/**
	\file "util/STL/hash_map_fwd.h"
	Header-wrapper for gcc-version-specific placement of <hash_map>.
	\todo Make this configuration dependent on ac_cxx_ext_hash_map.
	$Id: hash_map_fwd.h,v 1.9 2006/04/23 07:37:29 fang Exp $
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

#define	HASH_MAP_TEMPLATE_SIGNATURE					\
template <class K, class T, class H, class E, class A>

#define	DEFAULT_HASH_MAP_TEMPLATE_SIGNATURE	template <class K, class T>

#define	HASH_MAP_CLASS			hash_map<K,T,H,E,A >

HASH_MAP_TEMPLATE_SIGNATURE
class hash_map;

DEFAULT_HASH_MAP_TEMPLATE_SIGNATURE
struct default_hash_map {
	typedef	hash_map<K, T, hash<K>, std::equal_to<K>, std::allocator<T> >
					type;
	template <class K2, class T2>
	struct rebind : public default_hash_map<K2, T2> { };
};	// end struct default_hash_map

}	// end namespace HASH_MAP_NAMESPACE

#define	USING_HASH	using HASH_MAP_NAMESPACE::hash;
#define	USING_HASH_MAP	using HASH_MAP_NAMESPACE::hash_map;


#endif	// __UTIL_STL_HASH_MAP_FWD_H__

