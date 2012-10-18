/**
	\file "util/qmap_fwd.hh"
	Forward declarations for queryable map.  
	Full description in "qmap.hh".
	$Id: qmap_fwd.hh,v 1.10 2006/04/18 18:42:44 fang Exp $
 */

#ifndef	__UTIL_QMAP_FWD_H__
#define	__UTIL_QMAP_FWD_H__

#include "util/STL/pair_fwd.hh"
#include "util/STL/allocator_fwd.hh"
#include "util/STL/functional_fwd.hh"

#define	QMAP_TEMPLATE_SIGNATURE						\
template <class K, class T, typename C, typename A>

namespace util {

QMAP_TEMPLATE_SIGNATURE
class qmap;

template <class K, class T,
	typename C = std::less<K>,
	typename A = std::allocator<std::pair<const K, T> > >
class qmap;

/**
	Template typedef.  
 */
template <class K, class T>
struct default_qmap {
	typedef	qmap<K, T, std::less<K>,
		std::allocator<std::pair<const K, T> > >
					type;

	template <class K2, class T2>
	struct rebind : public default_qmap<K2, T2> { };
};	// end struct default_qmap

}	// end namespace util

#endif	// __UTIL_QMAP_FWD_H__

