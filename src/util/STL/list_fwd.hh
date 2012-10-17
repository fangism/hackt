/**
	\file "util/STL/list_fwd.hh"
	Forward declaration of std::list wrapper class.  
	$Id: list_fwd.hh,v 1.5 2006/04/23 07:37:29 fang Exp $
 */

#ifndef	__UTIL_STL_LIST_FWD_H__
#define	__UTIL_STL_LIST_FWD_H__

// may be overridden to just use the std::list without trying
// to split declarations from definitions

#include "util/STL/allocator_fwd.hh"

namespace std {

template <class T, class Alloc>
class list;

template <class T>
struct default_list {
	typedef	list<T, std::allocator<T> >	type;

	template <class T2>
	struct rebind : public default_list<T2> { };
};

}	// end namespace std

#endif	// __UTIL_STL_LIST_FWD_H__

