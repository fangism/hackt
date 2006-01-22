/**
	\file "util/multikey_map_fwd.h"
	Forward declarations for multidimensional map.  
	$Id: multikey_map_fwd.h,v 1.9 2006/01/22 06:53:34 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_MAP_FWD_H__
#define	__UTIL_MULTIKEY_MAP_FWD_H__

#include "util/STL/map_fwd.h"
#include "util/size_t.h"

namespace util {

#define MULTIKEY_MAP_TEMPLATE_SIGNATURE					\
template <size_t D, class K, class T, template <class, class> class M>

//=============================================================================

template <size_t D, class K, class T, 
	template <class, class> class M = std::map >
	class multikey_map;

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIKEY_MAP_FWD_H__

