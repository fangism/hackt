/**
	\file "util/STL/stack_fwd.h"
	Forward declaration of std::stack.
	No wrapping.  
	$Id: stack_fwd.h,v 1.3 2005/05/10 04:51:32 fang Exp $
 */

#ifndef	__UTIL_STL_STACK_FWD_H__
#define	__UTIL_STL_STACK_FWD_H__

#include "util/STL/deque_fwd.h"

namespace std {

// doesn't like redeclaring default template arguments...
// template <class T, class Alloc = std::allocator<T> >
template <class T, class Seq>
class stack;

}

#define	USING_STACK		using std::stack;
#define	DEFAULT_STACK(T)	std::stack<T, DEFAULT_DEQUE(T) >


#endif	// __UTIL_STL_STACK_FWD_H__

