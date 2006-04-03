/**
	\file "util/STL/allocator_fwd.h"
	Forward declaration of std::allocator.
	$Id: allocator_fwd.h,v 1.2.84.1 2006/04/03 04:58:26 fang Exp $
 */

#ifndef	__UTIL_STL_ALLOCATOR_FWD_H__
#define	__UTIL_STL_ALLOCATOR_FWD_H__

#include "config.h"

namespace std {

template <class>
class allocator;

}

#if	defined(HAVE_EXT_NEW_ALLOCATOR_H)
#define	NEW_ALLOCATOR_NAMESPACE		__gnu_cxx

namespace NEW_ALLOCATOR_NAMEPSACE {

// C++ standard new-allocator
template <typename _Tp>
class new_allocator;

}	// end namespace NEW_ALLOCATOR_NAMESPACE

#endif

#endif	// __UTIL_STL_ALLOCATOR_FWD_H__

