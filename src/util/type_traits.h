/**
	\file "util/type_traits.h"
	Snippets of type_traits from TR1 / Boost.  
	Don't want to rely on new compiler (gcc4) or Boost Libraries.  
	Really should make this configuration dependent.  
	$Id: type_traits.h,v 1.3 2005/05/10 04:51:30 fang Exp $
 */

#ifndef	__UTIL_TYPE_TRAITS_H__
#define	__UTIL_TYPE_TRAITS_H__

namespace util {

/**
	Template default for non-const type.  
 */
template <class T>
struct remove_const {
	typedef T     type;
};	// end struct remove_const

/**
	Specialization that removes the const from a type.  
 */
template <class T>
struct remove_const<T const> {
	typedef T     type;
};	// end struct remove_const

}	// end namespace util

#endif	// __UTIL_TYPE_TRAITS_H__


