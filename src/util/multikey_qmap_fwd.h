/**
	\file "multikey_qmap_fwd.h"
	Forward declarations for queryable multidimensional map.  
	$Id: multikey_qmap_fwd.h,v 1.5 2004/12/16 01:08:53 fang Exp $
 */

#ifndef	__MULTIKEY_QMAP_FWD_H__
#define	__MULTIKEY_QMAP_FWD_H__

#include "qmap_fwd.h"
#include "multikey_map_fwd.h"

#define	BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE				\
template <class K, class T>

#define	MULTIKEY_QMAP_TEMPLATE_SIGNATURE				\
template <size_t D, class K, class T>


namespace MULTIKEY_MAP_NAMESPACE {
//=============================================================================
// forward declare specialization
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
class multikey_map<D,K,T,QMAP_NAMESPACE::qmap>;

}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_QMAP_FWD_H__

