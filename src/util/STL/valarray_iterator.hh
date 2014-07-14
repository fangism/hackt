/**
	\file "util/STL/valarray_iterator.hh"
	Not really part of the starndard, but oh so useful.  
	Specializaes the begin(container) and end(container) interfaces
	for valarray, which doesn't have its own iterators, but should IMHO.
	Note: C++11 standard includes std::begin, std::end
	$Id: valarray_iterator.hh,v 1.2 2007/01/21 06:01:20 fang Exp $
 */

#ifndef	__UTIL_STL_VALARRAY_ITERATOR_HH__
#define	__UTIL_STL_VALARRAY_ITERATOR_HH__

#include "util/STL/libconfig.hh"
#include <valarray>
// #include "util/STL/container_iterator.hh"
// doesn't work as I expect: helper function does not match the partial specialization...

BEGIN_NAMESPACE_STD
template <class T>
struct container_iterator;
template <class T>
struct container_const_iterator;

/**
	Partial specialization for valarrays which lack iterator types.  
 */
template <class T>
struct container_iterator<valarray<T> > {
	typedef	valarray<T>	container_type;
	typedef	T*		type;

	static
	type
	begin(container_type& c) {
		return &c[0];
	}

	static
	type
	end(container_type& c) {
		return &c[c.size()];
	}
};

/**
	Partial specialization for valarrays which lack iterator types.  
 */
template <class T>
struct container_const_iterator<valarray<T> > {
	typedef	valarray<T>	container_type;
	typedef	const T*		type;

	static
	type
	begin(const container_type& c) {
		return &c[0];
	}

	static
	type
	end(const container_type& c) {
		return &c[c.size()];
	}
};

#ifndef	HAVE_STL_VALARRAY_BEGIN_END
template <class T>
inline
typename container_iterator<valarray<T> >::type
begin(valarray<T>& v) {
	return container_iterator<valarray<T> >::begin(v);
	// return &v[0];
}

template <class T>
inline
typename container_const_iterator<valarray<T> >::type
begin(const valarray<T>& v) {
	return container_const_iterator<valarray<T> >::begin(v);
	// return &v[0];
}

template <class T>
inline
typename container_iterator<valarray<T> >::type
end(valarray<T>& v) {
	return container_iterator<valarray<T> >::end(v);
	// return &v[v.size()];
}

template <class T>
inline
typename container_const_iterator<valarray<T> >::type
end(const valarray<T>& v) {
	return container_const_iterator<valarray<T> >::end(v);
	// return &v[v.size()];
}
#endif	// HAVE_STL_VALARRAY_BEGIN_END

END_NAMESPACE_STD

#endif	// __UTIL_STL_VALARRAY_ITERATOR_HH__

