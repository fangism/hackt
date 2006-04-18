/**
	\file "util/hash_qmap_fwd.h"
	Forward declarations for queryable hash_map, defined in "hash_qmap.h"
	$Id: hash_qmap_fwd.h,v 1.10 2006/04/18 18:42:42 fang Exp $
 */

#ifndef	__UTIL_HASH_QMAP_FWD_H__
#define	__UTIL_HASH_QMAP_FWD_H__

#include "util/STL/hash_map_fwd.h"
// needed for reference to default hash function

// these class parameters are explained below
#define HASH_QMAP_TEMPLATE_SIGNATURE					\
template <class K, class T, class H, class E, class A>

#define	HASH_QMAP_CLASS			hash_qmap<K, T, H, E, A >

namespace util {

// apologies for the short parameter names
HASH_QMAP_TEMPLATE_SIGNATURE
class hash_qmap;

// default arguments, only key and value type are needed
template <class K, class T, 
          class H = HASH_MAP_NAMESPACE::hash<K>,
          class E = std::equal_to<K>,
          class A = std::allocator<T> >
class hash_qmap;

/**
        Template typedef.  
 */
template <class K, class T>
struct default_hash_qmap {
	typedef hash_qmap<K, T, HASH_MAP_NAMESPACE::hash<K>,
		std::equal_to<K>, std::allocator<T> >
					type;

	template <class K2, class T2>
	struct rebind : public default_hash_qmap<K2, T2> { };
};      // end struct default_qmap

}	// end namespace util

#endif	// __UTIL_HASH_QMAP_FWD_H__

