/**
	\file "util/STL/list_fwd.h"
	Forward declaration of std::list wrapper class.  
	$Id: list_fwd.h,v 1.3.32.1 2006/04/17 03:04:10 fang Exp $
 */

#ifndef	__UTIL_STL_LIST_FWD_H__
#define	__UTIL_STL_LIST_FWD_H__

// may be overridden to just use the std::list without trying
// to split declarations from definitions
#ifndef	USE_STD_LIST
#define USE_STD_LIST	1
#endif

#include "util/STL/allocator_fwd.h"

#if USE_STD_LIST
namespace std {

template <class T, class Alloc>
class list;

template <class T>
struct default_list {
	typedef	list<T, std::allocator<T> >	type;
};

}
#else	// USE_STD_LIST
	#include "util/STL/namespace.h"
	#define LIST_NAMESPACE	util::STL

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

