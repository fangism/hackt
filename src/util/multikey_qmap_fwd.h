/**
	\file "multikey_qmap_fwd.h"
	Forward declarations for queryable multidimensional map.  
	$Id: multikey_qmap_fwd.h,v 1.3 2004/11/02 07:52:15 fang Exp $
 */

#ifndef	__MULTIKEY_QMAP_FWD_H__
#define	__MULTIKEY_QMAP_FWD_H__

#include "multikey_map_fwd.h"

namespace MULTIKEY_MAP_NAMESPACE {

//=============================================================================
template <class K, class T>
class multikey_qmap_base;

//=============================================================================
template <size_t D, class K, class T>
class multikey_qmap;

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_QMAP_FWD_H__

