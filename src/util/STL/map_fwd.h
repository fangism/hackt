/**
	\file "qmap_fwd.h"
	Forward declarations for std::map wrapper.  
	Full description in "STL/map.h".
	$Id: map_fwd.h,v 1.1 2004/12/05 05:07:57 fang Exp $
 */

#ifndef	__UTIL_STL_MAP_FWD_H__
#define	__UTIL_STL_MAP_FWD_H__

#ifndef	USE_STD_MAP
#define	USE_STD_MAP	1
#endif

#if USE_STD_MAP
	#include <map>
	#define	MAP_NAMESPACE	std
#else	// USE_STD_MAP
	#include "STL/namespace.h"
	#define	MAP_NAMESPACE	util::STL

#include "STL/allocator_fwd.h"			// for std::allocator
#include "STL/pair_fwd.h"			// for std::pair
#include "STL/functional_fwd.h"			// for std::less

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

