/**
	\file "qmap_fwd.h"
	Forward declarations for queryable map.  
	Full description in "qmap.h".
	$Id: qmap_fwd.h,v 1.7.16.2 2005/02/17 00:10:22 fang Exp $
 */

#ifndef	__UTIL_QMAP_FWD_H__
#define	__UTIL_QMAP_FWD_H__

#include "STL/pair_fwd.h"
#include "STL/allocator_fwd.h"
#include "STL/functional_fwd.h"

#define	QMAP_TEMPLATE_SIGNATURE						\
template <class K, class T, typename C, typename A>

namespace util {

QMAP_TEMPLATE_SIGNATURE
class qmap;

template <class K, class T,
	typename C = std::less<K>,
	typename A = std::allocator<std::pair<const K, T> > >
class qmap;

}	// end namespace util

#endif	// __UTIL_QMAP_FWD_H__

