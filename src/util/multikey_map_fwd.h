/**
	\file "multikey_map_fwd.h"
	Forward declarations for multidimensional map.  
	$Id: multikey_map_fwd.h,v 1.5 2004/12/05 05:07:24 fang Exp $
 */

#ifndef	__MULTIKEY_MAP_FWD_H__
#define	__MULTIKEY_MAP_FWD_H__

#ifndef	MULTIKEY_MAP_NAMESPACE
#define	MULTIKEY_MAP_NAMESPACE		multikey_map_namespace
#endif

#include "STL/map_fwd.h"

namespace MULTIKEY_MAP_NAMESPACE {

#define BASE_MULTIKEY_MAP_TEMPLATE_SIGNATURE	template <class K, class T>

#define MULTIKEY_MAP_TEMPLATE_SIGNATURE					\
template <size_t D, class K, class T, template <class, class> class M>

#define SPECIALIZED_MULTIKEY_MAP_TEMPLATE_SIGNATURE			\
template <class K, class T, template <class, class> class M>

//=============================================================================

template <class K, class T >
	class multikey_map_base;

template <size_t D, class K, class T, 
	template <class, class> class M = MAP_NAMESPACE::map >
	class multikey_map;

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_MAP_FWD_H__

