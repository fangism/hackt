/**
	\file "multikey_map_fwd.h"
	Forward declarations for multidimensional map.  
	$Id: multikey_map_fwd.h,v 1.5.24.1 2005/02/06 16:23:45 fang Exp $
 */

#ifndef	__MULTIKEY_MAP_FWD_H__
#define	__MULTIKEY_MAP_FWD_H__

#ifndef	MULTIKEY_MAP_NAMESPACE
#define	MULTIKEY_MAP_NAMESPACE		util
#endif

#include "STL/map_fwd.h"

namespace MULTIKEY_MAP_NAMESPACE {

// #define BASE_MULTIKEY_MAP_TEMPLATE_SIGNATURE	template <class K, class T>

#define MULTIKEY_MAP_TEMPLATE_SIGNATURE					\
template <size_t D, class K, class T, template <class, class> class M>

#define SPECIALIZED_MULTIKEY_MAP_TEMPLATE_SIGNATURE			\
template <class K, class T, template <class, class> class M>

//=============================================================================

#if 0
template <class K, class T >
	class multikey_map_base;
#endif

template <size_t D, class K, class T, 
	template <class, class> class M = MAP_NAMESPACE::map >
	class multikey_map;

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_MAP_FWD_H__

