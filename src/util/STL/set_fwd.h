/**
	\file "util/STL/set_fwd.h"
	Forward declarations of std::set.
	$Id: set_fwd.h,v 1.5 2006/04/12 08:53:23 fang Exp $
 */

#ifndef	__UTIL_STL_SET_FWD_H__
#define	__UTIL_STL_SET_FWD_H__

#include "util/STL/allocator_fwd.h"

namespace std {

template <class K>
struct less;

template <class K, class C, class A>
class set;

/**
	Template typedef for default std::set.  
 */
template <class K>
struct default_set {
	typedef	set<K, less<K>, allocator<K> >		type;
};

}	// end namespace std

#endif	// __UTIL_STL_SET_FWD_H__

