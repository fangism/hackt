/**
	\file "array_traits.h"
	Compile-time traits for multidimensional structures.  
	$Id: array_traits.h,v 1.1 2004/12/15 23:31:13 fang Exp $
 */

#ifndef	__ARRAY_TRAITS_H__
#define	__ARRAY_TRAITS_H__

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
	static const size_t	dimensions = array_type::dimensions;

};	// end class array_traits

}	// end namespace util

#endif	// __ARRAY_TRAITS_H__

