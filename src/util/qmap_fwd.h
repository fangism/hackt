/**
	\file "qmap_fwd.h"
	Forward declarations for queryable map.  
	Full description in "qmap.h".
	$Id: qmap_fwd.h,v 1.5 2004/11/02 07:52:18 fang Exp $
 */

#ifndef	__QMAP_FWD_H__
#define	__QMAP_FWD_H__

#ifndef	QMAP_NAMESPACE
#define	QMAP_NAMESPACE		qmap_namespace
#endif

#include <map>

#define	QMAP_TEMPLATE_SIGNATURE						\
template <class K, class T, typename C, typename A>

/**
	Namespace for queryable map, one with const-semantics lookup.  
 */
namespace QMAP_NAMESPACE {
using std::less;
using std::allocator;
using std::pair;

template <class K, class T,
	typename C = less<K>,
	typename A = allocator<pair<const K, T> > >
class qmap;

}	// end namespace

#endif	//	__QMAP_FWD_H__

