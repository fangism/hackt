/**
	\file "STL/deque_fwd.h"
	Forward declaration of std::deque.
	No wrapping.  
	$Id: deque_fwd.h,v 1.2 2005/01/28 19:58:50 fang Exp $
 */

#ifndef	__UTIL_STL_DEQUE_FWD_H__
#define	__UTIL_STL_DEQUE_FWD_H__

#include "STL/allocator_fwd.h"

namespace std {

// doesn't like redeclaring default template arguments...
// template <class T, class Alloc = std::allocator<T> >
template <class T, class Alloc>
class deque;

}

#define	USING_DEQUE		using std::deque;
#define	DEFAULT_DEQUE(T)	std::deque<T, std::allocator<T> >


#endif	// __UTIL_STL_DEQUE_FWD_H__

