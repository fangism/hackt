/**
	\file "multikey_qmap_fwd.h"
	Forward declarations for queryable multidimensional map.  
	$Id: multikey_qmap_fwd.h,v 1.5.16.1 2005/02/09 04:14:17 fang Exp $
 */

#ifndef	__MULTIKEY_QMAP_FWD_H__
#define	__MULTIKEY_QMAP_FWD_H__

#include "qmap_fwd.h"
#include "multikey_map_fwd.h"

#ifndef SPECIALIZE_MULTIKEY_QMAP
#define SPECIALIZE_MULTIKEY_QMAP		0
#endif


#define	MULTIKEY_QMAP_TEMPLATE_SIGNATURE				\
template <size_t D, class K, class T>

namespace MULTIKEY_MAP_NAMESPACE {
//=============================================================================
// forward declare specialization
#if SPECIALIZE_MULTIKEY_QMAP
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
class multikey_map<D,K,T,QMAP_NAMESPACE::qmap>;
#endif

}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_QMAP_FWD_H__

