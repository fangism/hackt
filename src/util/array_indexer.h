/**
	\file "util/array_indexer.h"
	$Id: array_indexer.h,v 1.2 2010/05/23 17:44:59 fang Exp $
	Functor for indexing into arrays.
 */

#ifndef	__UTIL_ARRAY_INDEXER_H__
#define	__UTIL_ARRAY_INDEXER_H__

#include "util/size_t.h"
#include <functional>

namespace util {

/**
	This functor binds the container.
	\param A is a container type with array-like interface.
 */
template <class A>
struct array_indexer_t :
	public std::unary_function<size_t, typename A::reference> {
	typedef	A					array_type;
	typedef	typename array_type::value_type		value_type;
	typedef	typename array_type::reference		reference;
//	typedef	typename array_type::const_reference	const_reference;

	array_type&					_array;

	explicit
	array_indexer_t(array_type& a) : _array(a) { }

	reference
	operator () (const size_t& i) const {
		return _array[i];
	}
};	// end struct array_indexer_t

template <class A>
struct const_array_indexer_t :
	public std::unary_function<size_t, typename A::const_reference> {
	typedef	A					array_type;
	typedef	typename array_type::value_type		value_type;
//	typedef	typename array_type::reference		reference;
	typedef	typename array_type::const_reference	const_reference;

	const array_type&				_array;

	explicit
	const_array_indexer_t(const array_type& a) : _array(a) { }

	const_reference
	operator () (const size_t& i) const {
		return _array[i];
	}
};	// end struct array_indexer_t

/**
	Returns a functor.
 */
template <class A>
array_indexer_t<A>
array_indexer(A& a) {
	return array_indexer_t<A>(a);
}

template <class A>
const_array_indexer_t<A>
array_indexer(const A& a) {
	return const_array_indexer_t<A>(a);
}

}	// end namespace util
#endif	// __UTIL_ARRAY_INDEXER_H__

