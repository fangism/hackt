/**
	\file "STL/set_fwd.h"
	Forward declarations of std::set.
	$Id: set_fwd.h,v 1.1.2.1.2.1 2005/02/06 02:22:11 fang Exp $
 */

#ifndef	__UTIL_STL_SET_FWD_H__
#define	__UTIL_STL_SET_FWD_H__

#include "STL/allocator_fwd.h"

namespace std {

template <class K>
struct less;

template <class K, class C, class A>
class set;

template <class K, class C = less<K>, class A = allocator<K> >
class set;

}	// end namespace std

#endif	// __UTIL_STL_SET_FWD_H__

