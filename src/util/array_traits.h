/**
	\file "array_traits.h"
	Compile-time traits for multidimensional structures.  
	$Id: array_traits.h,v 1.1.16.1 2005/02/09 04:14:12 fang Exp $
 */

#ifndef	__UTIL_ARRAY_TRAITS_H__
#define	__UTIL_ARRAY_TRAITS_H__

namespace util {

/**
	Purely for the sake of compile-time knowledge of
	(abstract) dimensionality of a structure.  
	Of course, one need not add a dimensions member, 
	the traits may be specialized for any other structure.  
 */
template <class T>
struct array_traits {
	typedef	T		array_type;

	// or default to 0 unless formerly specialized?
	enum { dimensions = array_type::dimensions };

};	// end class array_traits

}	// end namespace util

#endif	// __UTIL_ARRAY_TRAITS_H__

