/**
	\file "const_traits.h"
	Constification and de-constification traits template.  
	$Id: const_traits.h,v 1.1.4.1 2005/02/09 04:14:13 fang Exp $
 */

#ifndef	__UTIL_CONST_TRAITS_H__
#define	__UTIL_CONST_TRAITS_H__

namespace util {

/**
	Traits type to distinguish between const and non-const type.  
 */
template <class T>
struct const_traits {
	typedef	const T		const_type;
	typedef	T		non_const_type;
};

/**
	Specialization for types that are already const.  
 */
template <class T>
struct const_traits<const T> {
	typedef	const T		const_type;
	typedef	T		non_const_type;
};

}	// end namespace util

#endif	// __UTIL_CONST_TRAITS_H__

