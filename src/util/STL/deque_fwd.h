/**
	\file "util/STL/deque_fwd.h"
	Forward declaration of std::deque.
	No wrapping.  
	$Id: deque_fwd.h,v 1.3 2005/05/10 04:51:31 fang Exp $
 */

#ifndef	__UTIL_STL_DEQUE_FWD_H__
#define	__UTIL_STL_DEQUE_FWD_H__

#include "util/STL/allocator_fwd.h"

namespace std {

// doesn't like redeclaring default template arguments...
// template <class T, class Alloc = std::allocator<T> >
template <class T, class Alloc>
class deque;

}

#define	USING_DEQUE		using std::deque;
#define	DEFAULT_DEQUE(T)	std::deque<T, std::allocator<T> >


#endif	// __UTIL_STL_DEQUE_FWD_H__

