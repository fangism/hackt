/**
	\file "multikey_map_fwd.h"
	Forward declarations for multidimensional map.  
	$Id: multikey_map_fwd.h,v 1.4 2004/11/02 07:52:15 fang Exp $
 */

#ifndef	__MULTIKEY_MAP_FWD_H__
#define	__MULTIKEY_MAP_FWD_H__

#ifndef	MULTIKEY_MAP_NAMESPACE
#define	MULTIKEY_MAP_NAMESPACE		multikey_map_namespace
#endif

#include <map>

namespace MULTIKEY_MAP_NAMESPACE {
using namespace std;

//=============================================================================

template <class K = int,
	class T = char* >
	class multikey_map_base;

template <size_t D,
	class K = int,
	class T = char*,
	template <class, class> class M = map >
	class multikey_map;

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_MAP_FWD_H__

