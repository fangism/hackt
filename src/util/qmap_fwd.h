/**
	\file "qmap_fwd.h"
	Forward declarations for queryable map.  
	Full description in "qmap.h".
	$Id: qmap_fwd.h,v 1.7.16.1 2005/02/09 04:14:19 fang Exp $
 */

#ifndef	__UTIL_QMAP_FWD_H__
#define	__UTIL_QMAP_FWD_H__

#ifndef	QMAP_NAMESPACE
#define	QMAP_NAMESPACE		util
#endif

#include "STL/pair_fwd.h"
#include "STL/allocator_fwd.h"
#include "STL/functional_fwd.h"

#define	QMAP_TEMPLATE_SIGNATURE						\
template <class K, class T, typename C, typename A>

namespace QMAP_NAMESPACE {

QMAP_TEMPLATE_SIGNATURE
class qmap;

template <class K, class T,
	typename C = std::less<K>,
	typename A = std::allocator<std::pair<const K, T> > >
class qmap;

}	// end namespace QMAP_NAMESPACE

#endif	// __UTIL_QMAP_FWD_H__

