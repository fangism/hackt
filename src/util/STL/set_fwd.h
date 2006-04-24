/**
	\file "util/STL/set_fwd.h"
	Forward declarations of std::set.
	$Id: set_fwd.h,v 1.6.2.1 2006/04/24 20:15:36 fang Exp $
 */

#ifndef	__UTIL_STL_SET_FWD_H__
#define	__UTIL_STL_SET_FWD_H__

#include "util/STL/allocator_fwd.h"

namespace std {

template <class K>
struct less;

#define	STD_SET_TEMPLATE_SIGNATURE	template <class K, class C, class A>

STD_SET_TEMPLATE_SIGNATURE
class set;

#define	STD_SET_CLASS		std::set<K, C, A >

/**
	Template typedef for default std::set.  
 */
template <class K>
struct default_set {
	typedef	set<K, less<K>, allocator<K> >		type;

	/**
		Template argument rebinder.
	 */
	template <class T>
	struct rebind : public default_set<T> { };
};

}	// end namespace std

#endif	// __UTIL_STL_SET_FWD_H__

