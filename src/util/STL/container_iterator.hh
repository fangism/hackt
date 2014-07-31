/**
	\file "util/STL/container_iterator.hh"
	A different interface to begin and end iterators of containers.
	Why? so we can define specializations to non-iterated containers
	with a consistent interface, e.g. valarray.
	See also "util/STL/valarray_iterator.hh" for overloads.  
	Note: C++11 standard includes std::begin, std::end
	$Id: container_iterator.hh,v 1.2 2007/01/21 06:01:18 fang Exp $
 */

#ifndef	__UTIL_STL_CONTAINER_ITERATOR_HH__
#define	__UTIL_STL_CONTAINER_ITERATOR_HH__

#include "util/STL/valarray_fwd.hh"

BEGIN_NAMESPACE_STD
// forward declarations
template <class>
struct container_iterator;

template <class>
struct container_const_iterator;

// specialization declarations (THIS IS NOT WORKING AS I EXPECT...)
template <class T>
struct container_iterator<valarray<T> >;

template <class T>
struct container_const_iterator<valarray<T> >;

//=============================================================================
template <class C>
struct container_iterator {
	typedef	C				container_type;
	typedef	typename C::iterator		type;

	static
	type
	begin(container_type& c) {
		return c.begin();
	}

	static
	type
	end(container_type& c) {
		return c.end();
	}

};	// end struct container_iterator

template <class C>
struct container_const_iterator {
	typedef	C				container_type;
	typedef	typename C::const_iterator	type;

	static
	type
	begin(const container_type& c) {
		return c.begin();
	}

	static
	type
	end(const container_type& c) {
		return c.end();
	}

};	// end struct container_iterator

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef	HAVE_STD_BEGIN_END
template <class C>
inline
typename container_iterator<C>::type
begin(C& c) {
	return container_iterator<C>::begin(c);
}

template <class C>
inline
typename container_const_iterator<C>::type
begin(const C& c) {
	return container_const_iterator<C>::begin(c);
}

template <class C>
inline
typename container_iterator<C>::type
end(C& c) {
	return container_iterator<C>::end(c);
}

template <class C>
inline
typename container_const_iterator<C>::type
end(const C& c) {
	return container_const_iterator<C>::end(c);
}
#endif	// HAVE_STD_BEGIN_END

//=============================================================================
END_NAMESPACE_STD

#endif	// __UTIL_STL_CONTAINER_ITERATOR_HH__

