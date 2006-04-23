/**
	\file "util/qmap_fwd.h"
	Forward declarations for std::map wrapper.  
	Full description in "STL/map.h".
	$Id: map_fwd.h,v 1.5 2006/04/23 07:37:29 fang Exp $
 */

#ifndef	__UTIL_STL_MAP_FWD_H__
#define	__UTIL_STL_MAP_FWD_H__

#include "util/STL/allocator_fwd.h"		// for std::allocator
#include "util/STL/pair_fwd.h"			// for std::pair
#include "util/STL/functional_fwd.h"		// for std::less

namespace std {
template <class K, class T, typename C, typename A>
class map;

/**
	Template typedef for default STL map with only key-value
	template arguments.  
 */
template <class K, class T>
struct default_map {
	typedef	map<K, T, std::less<K>, std::allocator<std::pair<const K, T> > >
					type;

	/**
		Template argument rebinder.  
	 */
	template <class K2, class T2>
	struct rebind : public default_map<K2, T2> { };
};	// end struct default_map

}	// end namespace std

#endif	//	__UTIL_STL_MAP_FWD_H__

