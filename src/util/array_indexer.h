/**
	\file "util/array_indexer.h"
	$Id: array_indexer.h,v 1.1 2010/04/19 02:46:11 fang Exp $
	Functor for indexing into arrays.
 */

#ifndef	__UTIL_ARRAY_INDEXER_H__
#define	__UTIL_ARRAY_INDEXER_H__

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
	typedef	typename array_type::const_reference	const_reference;

	array_type&					_array;

	explicit
	array_indexer_t(array_type& a) : _array(a) { }

	reference
	operator () (const size_t i) {
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

}	// end namespace util
#endif	// __UTIL_ARRAY_INDEXER_H__

