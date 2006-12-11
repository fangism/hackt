/**
	"util/STL/container_iterator.h"
	Not really part of the starndard, but oh so useful.  
	Specializaes the begin(container) and end(container) interfaces
	for valarray, which doesn't have its own iterators, but should IMHO.
	$Id: valarray_iterator.h,v 1.1.2.1 2006/12/11 00:40:37 fang Exp $
 */

#ifndef	__UTIL_STL_VALARRAY_ITERATOR_H__
#define	__UTIL_STL_VALARRAY_ITERATOR_H__

#include <valarray>
// #include "util/STL/container_iterator.h"
// doesn't work as I expect: helper function does not match the partial specialization...

namespace std {
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

}

#endif	// __UTIL_STL_VALARRAY_ITERATOR_H__

