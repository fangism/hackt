/**
	\file "util/STL/stack_fwd.h"
	Forward declaration of std::stack.
	No wrapping.  
	$Id: stack_fwd.h,v 1.3.54.1 2006/01/18 06:25:11 fang Exp $
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

#if 0
#define	USING_STACK		using std::stack;
#define	DEFAULT_STACK(T)	std::stack<T, DEFAULT_DEQUE(T) >
#endif


#endif	// __UTIL_STL_STACK_FWD_H__

