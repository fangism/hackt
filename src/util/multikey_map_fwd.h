/**
	\file "multikey_map_fwd.h"
	Forward declarations for multidimensional map.  
	$Id: multikey_map_fwd.h,v 1.5.24.2 2005/02/06 18:25:36 fang Exp $
 */

#ifndef	__MULTIKEY_MAP_FWD_H__
#define	__MULTIKEY_MAP_FWD_H__

#ifndef	MULTIKEY_MAP_NAMESPACE
#define	MULTIKEY_MAP_NAMESPACE		util
#endif

#include "STL/map_fwd.h"

namespace MULTIKEY_MAP_NAMESPACE {

#define MULTIKEY_MAP_TEMPLATE_SIGNATURE					\
template <size_t D, class K, class T, template <class, class> class M>

#define SPECIALIZED_MULTIKEY_MAP_TEMPLATE_SIGNATURE			\
template <class K, class T, template <class, class> class M>

//=============================================================================

template <size_t D, class K, class T, 
	template <class, class> class M = MAP_NAMESPACE::map >
	class multikey_map;

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_MAP_FWD_H__

