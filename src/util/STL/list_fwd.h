/**
	\file "util/STL/list_fwd.h"
	Forward declaration of std::list wrapper class.  
	$Id: list_fwd.h,v 1.4 2006/04/18 18:42:45 fang Exp $
 */

#ifndef	__UTIL_STL_LIST_FWD_H__
#define	__UTIL_STL_LIST_FWD_H__

// may be overridden to just use the std::list without trying
// to split declarations from definitions

#include "util/STL/allocator_fwd.h"

namespace std {

template <class T, class Alloc>
class list;

template <class T>
struct default_list {
	typedef	list<T, std::allocator<T> >	type;

	template <class T2>
	struct rebind : public default_list<T2> { };
};

}

#endif	// __UTIL_STL_LIST_FWD_H__

