/**
	\file "STL/deque_fwd.h"
	Forward declaration of std::deque.
	No wrapping.  
	$Id: deque_fwd.h,v 1.1 2004/12/06 07:13:16 fang Exp $
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
#define	DEFAULT_DEQUE(T)	deque<T, std::allocator<T> >


#endif	// __UTIL_STL_DEQUE_FWD_H__

