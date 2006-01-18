/**
	\file "util/STL/set_fwd.h"
	Forward declarations of std::set.
	$Id: set_fwd.h,v 1.3.54.1 2006/01/18 06:25:11 fang Exp $
 */

#ifndef	__UTIL_STL_SET_FWD_H__
#define	__UTIL_STL_SET_FWD_H__

#include "util/STL/allocator_fwd.h"

namespace std {

template <class K>
struct less;

#if 1
template <class K, class C, class A>
class set;

#else

template <class K, class C = less<K>, class A = allocator<K> >
class set;
#endif

template <class K>
struct default_set {
	typedef	set<K, less<K>, allocator<K> >		type;
};

}	// end namespace std

#endif	// __UTIL_STL_SET_FWD_H__

