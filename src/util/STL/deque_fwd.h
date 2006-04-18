/**
	\file "util/STL/deque_fwd.h"
	Forward declaration of std::deque.
	No wrapping.  
	$Id: deque_fwd.h,v 1.5 2006/04/18 18:42:45 fang Exp $
 */

#ifndef	__UTIL_STL_DEQUE_FWD_H__
#define	__UTIL_STL_DEQUE_FWD_H__

#include "util/STL/allocator_fwd.h"

namespace std {

// doesn't like redeclaring default template arguments...
// template <class T, class Alloc = std::allocator<T> >
template <class T, class Alloc>
class deque;

template <class T>
struct default_deque {
	typedef	deque<T, std::allocator<T> >	type;

	template <class T2>
	struct rebind : public default_deque<T2> { };
};

}

#define	USING_DEQUE		using std::deque;
#define	DEFAULT_DEQUE(T)	std::deque<T, std::allocator<T> >


#endif	// __UTIL_STL_DEQUE_FWD_H__

