/**
	\file "util/STL/set_fwd.h"
	Forward declarations of std::set.
	$Id: set_fwd.h,v 1.3 2005/05/10 04:51:32 fang Exp $
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

}	// end namespace std

#endif	// __UTIL_STL_SET_FWD_H__

