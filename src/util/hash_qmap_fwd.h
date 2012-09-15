/**
	\file "util/hash_qmap_fwd.h"
	Forward declarations for queryable hash_map, defined in "hash_qmap.h"
	$Id: hash_qmap_fwd.h,v 1.12 2008/11/25 21:52:44 fang Exp $
 */

#ifndef	__UTIL_HASH_QMAP_FWD_H__
#define	__UTIL_HASH_QMAP_FWD_H__

#include "util/STL/hash_map_fwd.h"
// needed for reference to default hash function

// these class parameters are explained below
#if	USING_UNORDERED_MAP
// #include "util/STL/pair_fwd.h"
	#define HASH_QMAP_TEMPLATE_SIGNATURE				\
	template <class K, class T, class H, class E, class A>
	#define	HASH_QMAP_CLASS		hash_qmap<K, T, H, E, A>
#elif	defined(HASH_MAP_SGI_STYLE)
	#define HASH_QMAP_TEMPLATE_SIGNATURE				\
	template <class K, class T, class H, class E, class A>
	#define	HASH_QMAP_CLASS		hash_qmap<K, T, H, E, A >
#elif	defined(HASH_MAP_INTEL_STYLE)
	#define HASH_QMAP_TEMPLATE_SIGNATURE				\
	template <class K, class T, class HC, class A>
	#define	HASH_QMAP_CLASS		hash_qmap<K, T, HC, A >
#else
#error	Screw this hash_map sh--!
#endif


namespace util {

// apologies for the short parameter names
HASH_QMAP_TEMPLATE_SIGNATURE
class hash_qmap;

// default arguments, only key and value type are needed
#if	USING_UNORDERED_MAP
template <class K, class T, 
          class H = HASH_MAP_NAMESPACE::hash<K>,
          class E = std::equal_to<K>,
          class A = std::allocator<T> >
#elif	defined(HASH_MAP_SGI_STYLE)
template <class K, class T, 
          class H = HASH_MAP_NAMESPACE::hash<K>,
          class E = std::equal_to<K>,
          class A = std::allocator<T> >
#elif	defined(HASH_MAP_INTEL_STYLE)
template <class K, class T, 
          class HC = std::hash_compare<K, std::less<K> >,
          class A = std::allocator<T> >
#endif
class hash_qmap;

/**
        Template typedef.  
 */
template <class K, class T>
struct default_hash_qmap {
#if	USING_UNORDERED_MAP
	typedef hash_qmap<K, T, HASH_MAP_NAMESPACE::hash<K>,
		std::equal_to<K>, std::allocator<T> >
#elif	defined(HASH_MAP_SGI_STYLE)
	typedef hash_qmap<K, T, HASH_MAP_NAMESPACE::hash<K>,
		std::equal_to<K>, std::allocator<T> >
#elif	defined(HASH_MAP_INTEL_STYLE)
	typedef hash_qmap<K, T, std::hash_compare<K, std::less<K> >,
		std::allocator<T> >
#endif
					type;

	template <class K2, class T2>
	struct rebind : public default_hash_qmap<K2, T2> { };
};      // end struct default_qmap

}	// end namespace util

#endif	// __UTIL_HASH_QMAP_FWD_H__

