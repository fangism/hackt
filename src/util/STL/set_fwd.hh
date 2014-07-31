/**
	\file "util/STL/set_fwd.hh"
	Forward declarations of std::set.
	$Id: set_fwd.hh,v 1.7 2006/04/27 00:17:23 fang Exp $
 */

#ifndef	__UTIL_STL_SET_FWD_HH__
#define	__UTIL_STL_SET_FWD_HH__

#include "util/STL/allocator_fwd.hh"

BEGIN_NAMESPACE_STD

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

END_NAMESPACE_STD

#endif	// __UTIL_STL_SET_FWD_HH__

