/**
	\file "multikey_map_fwd.h"
	Forward declarations for multidimensional map.  
	$Id: multikey_map_fwd.h,v 1.5.16.1.2.2 2005/02/16 18:44:23 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_MAP_FWD_H__
#define	__UTIL_MULTIKEY_MAP_FWD_H__

#include "STL/map_fwd.h"

namespace util {

#define MULTIKEY_MAP_TEMPLATE_SIGNATURE					\
template <size_t D, class K, class T, template <class, class> class M>

//=============================================================================

template <size_t D, class K, class T, 
	template <class, class> class M = MAP_NAMESPACE::map >
	class multikey_map;

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIKEY_MAP_FWD_H__

