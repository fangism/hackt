/**
	\file "hash_qmap_fwd.h"
	Forward declarations for queryable hash_map, defined in "hash_qmap.h"
	$Id: hash_qmap_fwd.h,v 1.7 2005/02/27 22:54:22 fang Exp $
 */

#ifndef	__UTIL_HASH_QMAP_FWD_H__
#define	__UTIL_HASH_QMAP_FWD_H__

#include "STL/hash_map_fwd.h"
// needed for reference to default hash function

// these class parameters are explained below
#define HASH_QMAP_TEMPLATE_SIGNATURE					\
template <class K, class T, class H, class E, class A>

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

}	// end namespace util

#endif	// __UTIL_HASH_QMAP_FWD_H__

