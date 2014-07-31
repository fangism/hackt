/**
	\file "util/qmap_fwd.hh"
	Forward declarations for std::map wrapper.  
	Full description in "STL/map.hh".
	$Id: map_fwd.hh,v 1.6 2006/04/27 00:17:23 fang Exp $
 */

#ifndef	__UTIL_STL_MAP_FWD_HH__
#define	__UTIL_STL_MAP_FWD_HH__

#include "util/STL/allocator_fwd.hh"		// for std::allocator
#include "util/STL/pair_fwd.hh"			// for std::pair
#include "util/STL/functional_fwd.hh"		// for std::less

BEGIN_NAMESPACE_STD

#define	STD_MAP_TEMPLATE_SIGNATURE					\
template <typename K, typename T, typename C, typename A>

STD_MAP_TEMPLATE_SIGNATURE
class map;

#define	STD_MAP_CLASS		std::map<K, T, C, A >

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

END_NAMESPACE_STD

#endif	//	__UTIL_STL_MAP_FWD_HH__

