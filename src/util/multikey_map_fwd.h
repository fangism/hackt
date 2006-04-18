/**
	\file "util/multikey_map_fwd.h"
	Forward declarations for multidimensional map.  
	$Id: multikey_map_fwd.h,v 1.10 2006/04/18 18:42:44 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_MAP_FWD_H__
#define	__UTIL_MULTIKEY_MAP_FWD_H__

#include "util/STL/map_fwd.h"
#include "util/size_t.h"
#include "util/multikey_fwd.h"

namespace util {

#define MULTIKEY_MAP_TEMPLATE_SIGNATURE					\
template <size_t D, class K, class T, class M>

//=============================================================================

MULTIKEY_MAP_TEMPLATE_SIGNATURE
	class multikey_map;

/**
	Template typedef for the default map type. 
 */
template <size_t D, class K, class T>
struct default_multikey_map {
	/**
		Note: definition expands ::type::type, so bare STL
		containers (class templates) will not work.
		Use the default_* wrapper policies.  
	 */
	template <template <class, class> class M>
	struct rebind_default_map_type {
		typedef	M<typename multikey<D,K>::simple_type, T>
						wrapper_type;
		typedef	typename wrapper_type::type		type;
	};

	typedef	typename rebind_default_map_type<std::default_map>::type
						default_map_type;

	typedef	multikey_map<D, K, T, default_map_type>		type;

	template <size_t D2, class K2, class T2>
	struct rebind : public default_multikey_map<D2, K2, T2> { };

};	// end struct default_multikey_map

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIKEY_MAP_FWD_H__

