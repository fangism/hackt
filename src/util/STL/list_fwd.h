/**
	\file "list_fwd.h"
	Forward declaration of std::list wrapper class.  
	$Id: list_fwd.h,v 1.1 2004/12/05 05:07:57 fang Exp $
 */

#ifndef	__UTIL_STL_LIST_FWD_H__
#define	__UTIL_STL_LIST_FWD_H__

// may be overridden to just use the std::list without trying
// to split declarations from definitions
#ifndef	USE_STD_LIST
#define USE_STD_LIST	1
#endif

#if USE_STD_LIST
	#include <list>
	#define LIST_NAMESPACE	std
	/// use this macro to select which list to use
	// in this case, just ignore the wrapper class entirely
#else	// USE_STD_LIST
	#include "STL/namespace.h"
	#define LIST_NAMESPACE	util::STL

#include "STL/allocator_fwd.h"

namespace util {
namespace STL {

template <class T, class Alloc = std::allocator<T> >
class list;

}	// end namespace STL
}	// end namespace util

#endif	// USE_STD_LIST

/// use this macro directive to automatically switch between implementations
#define	USING_LIST	using LIST_NAMESPACE::list;

#endif	// __UTIL_STL_LIST_FWD_H__

