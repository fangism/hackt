/**
	\file "util/STL/stack_fwd.h"
	Forward declaration of std::stack.
	No wrapping.  
	$Id: stack_fwd.h,v 1.4.32.1 2006/04/17 03:04:10 fang Exp $
 */

#ifndef	__UTIL_STL_STACK_FWD_H__
#define	__UTIL_STL_STACK_FWD_H__

#include "util/STL/deque_fwd.h"

namespace std {

// doesn't like redeclaring default template arguments...
// template <class T, class Alloc = std::allocator<T> >
template <class T, class Seq>
class stack;

template <class T>
struct default_stack {
	typedef	stack<T, typename default_deque<T>::type>	type;
};

}

#endif	// __UTIL_STL_STACK_FWD_H__

