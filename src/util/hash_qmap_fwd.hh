/**
	\file "util/hash_qmap_fwd.hh"
	Forward declarations for queryable hash_map, defined in "hash_qmap.hh"
	$Id: hash_qmap_fwd.hh,v 1.12 2008/11/25 21:52:44 fang Exp $
 */

#ifndef	__UTIL_HASH_QMAP_FWD_H__
#define	__UTIL_HASH_QMAP_FWD_H__

#if __cplusplus >= 201103L
#error "Do not use this header in C++11 or newer.  Rewrite the code."
#endif

#include <functional>  // for std::equal_to, std::hash
#include <memory>  // for std::allocator
#include <unordered_map>

// these class parameters are explained below
#include <utility>  // for std::pair
#define HASH_QMAP_TEMPLATE_SIGNATURE				\
template <class K, class T, class H, class E, class A>
#define	HASH_QMAP_CLASS		hash_qmap<K, T, H, E, A>

namespace util {

// apologies for the short parameter names
HASH_QMAP_TEMPLATE_SIGNATURE
class hash_qmap;

// default arguments, only key and value type are needed
template <class K, class T, 
          class H = std::hash<K>,
          class E = std::equal_to<K>,
          class A = std::allocator<std::pair<const K, T> > >
class hash_qmap;

/**
        Template typedef.  
 */
template <class K, class T>
struct default_hash_qmap {
	typedef hash_qmap<K, T, std::hash<K>,
		std::equal_to<K>, std::allocator<std::pair<const K, T> > >
					type;

	template <class K2, class T2>
	struct rebind : public default_hash_qmap<K2, T2> { };
};      // end struct default_qmap

}	// end namespace util

#endif	// __UTIL_HASH_QMAP_FWD_H__

