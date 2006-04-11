/**
	\file "util/const_traits.h"
	Constification and de-constification traits template.  
	$Id: const_traits.h,v 1.5 2006/04/11 07:54:47 fang Exp $
 */

#ifndef	__UTIL_CONST_TRAITS_H__
#define	__UTIL_CONST_TRAITS_H__

namespace util {

#if 0
/**
	Traits type to distinguish between const and non-const type.  
 */
template <class T>
struct const_traits {
	typedef	const T		const_type;
	typedef	T		non_const_type;
	typedef	T		must_not_be_const;
};

/**
	Specialization for types that are already const.  
 */
template <class T>
struct const_traits<const T> {
	typedef	const T		const_type;
	typedef	T		non_const_type;
	typedef	const T		must_be_const;
};
#else
// #error	This file is obsolete, use "util/type_traits.h" instead.  
#endif

}	// end namespace util

#endif	// __UTIL_CONST_TRAITS_H__

