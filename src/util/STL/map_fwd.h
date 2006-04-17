/**
	\file "util/qmap_fwd.h"
	Forward declarations for std::map wrapper.  
	Full description in "STL/map.h".
	$Id: map_fwd.h,v 1.3.32.1 2006/04/17 03:04:10 fang Exp $
 */

#ifndef	__UTIL_STL_MAP_FWD_H__
#define	__UTIL_STL_MAP_FWD_H__

#ifndef	USE_STD_MAP
#define	USE_STD_MAP	1
#endif

#include "util/STL/allocator_fwd.h"		// for std::allocator
#include "util/STL/pair_fwd.h"			// for std::pair
#include "util/STL/functional_fwd.h"		// for std::less

#if USE_STD_MAP
namespace std {
template <class K, class T, typename C, typename A>
class map;

template <class K, class T>
struct default_map {
	typedef	map<K, T, std::less<K>, std::allocator<std::pair<const K, T> > >
					type;
};

}	// end namespace STL
#else	// USE_STD_MAP
	#include "util/STL/namespace.h"
	#define	MAP_NAMESPACE	util::STL


#define	STL_MAP_TEMPLATE_SIGNATURE					\
template <class K, class T, typename C, typename A>

namespace util {
namespace STL {

template <class K, class T,
	typename C = std::less<K>,
	typename A = std::allocator<std::pair<const K, T> > >
class map;

}	// end namespace STL
}	// end namespace util

#endif	// USE_STD_MAP

#define	USING_MAP		using MAP_NAMESPACE::map;

#endif	//	__UTIL_STL_MAP_FWD_H__

