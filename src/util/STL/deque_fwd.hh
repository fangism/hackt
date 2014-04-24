/**
	\file "util/STL/deque_fwd.hh"
	Forward declaration of std::deque.
	No wrapping.  
	$Id: deque_fwd.hh,v 1.6 2006/04/23 07:37:29 fang Exp $
 */

#ifndef	__UTIL_STL_DEQUE_FWD_HH__
#define	__UTIL_STL_DEQUE_FWD_HH__

#include "util/STL/allocator_fwd.hh"

BEGIN_NAMESPACE_STD

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

END_NAMESPACE_STD

#endif	// __UTIL_STL_DEQUE_FWD_HH__

