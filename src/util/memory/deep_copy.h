/**
	\file "util/memory/deep_copy.h"
	Handy funciton for deep-copying pointers.
	$Id: deep_copy.h,v 1.1.2.1 2008/03/01 08:34:26 fang Exp $
 */

#ifndef	__UTIL_MEMORY_DEEP_COPY_H__
#define	__UTIL_MEMORY_DEEP_COPY_H__

// #include "util/memory/pointer_manipulator.h"
#include "util/memory/pointer_traits.h"

namespace util {
namespace memory {

/**
	\return a new-allocated constructed copy of argument (raw pointer).
	concept requirements:
	indirection (operator *), and copy-constructibility.  
 */
template <class P>
inline
typename pointer_traits<P>::element_type*
deep_copy(const P& p) {
	typedef	typename pointer_traits<P>::element_type	element_type;
	return new element_type(*p);
}

}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_DEEP_COPY_H__

