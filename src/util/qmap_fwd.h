/**
	\file "qmap_fwd.h"
	Forward declarations for queryable map.  
	Full description in "qmap.h".
	$Id: qmap_fwd.h,v 1.6 2004/12/05 05:07:25 fang Exp $
 */

#ifndef	__QMAP_FWD_H__
#define	__QMAP_FWD_H__

#ifndef	QMAP_NAMESPACE
#define	QMAP_NAMESPACE		qmap_namespace
#endif

#include "STL/pair_fwd.h"
#include "STL/allocator_fwd.h"
#include "STL/functional_fwd.h"

#define	QMAP_TEMPLATE_SIGNATURE						\
template <class K, class T, typename C, typename A>

/**
	Namespace for queryable map, one with const-semantics lookup.  
 */
namespace QMAP_NAMESPACE {

template <class K, class T,
	typename C = std::less<K>,
	typename A = std::allocator<std::pair<const K, T> > >
class qmap;

}	// end namespace

#endif	//	__QMAP_FWD_H__

