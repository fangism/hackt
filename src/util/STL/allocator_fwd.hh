/**
	\file "util/STL/allocator_fwd.hh"
	Forward declaration of std::allocator.
	$Id: allocator_fwd.hh,v 1.3 2006/04/03 05:30:39 fang Exp $
 */

#ifndef	__UTIL_STL_ALLOCATOR_FWD_HH__
#define	__UTIL_STL_ALLOCATOR_FWD_HH__

#include "util/STL/libconfig.hh"

BEGIN_NAMESPACE_STD

template <class>
class allocator;

END_NAMESPACE_STD

#if	defined(HAVE_EXT_NEW_ALLOCATOR_H)
#define	NEW_ALLOCATOR_NAMESPACE		__gnu_cxx

namespace NEW_ALLOCATOR_NAMEPSACE {

// C++ standard new-allocator
template <typename _Tp>
class new_allocator;

}	// end namespace NEW_ALLOCATOR_NAMESPACE

#endif

#endif	// __UTIL_STL_ALLOCATOR_FWD_HH__

