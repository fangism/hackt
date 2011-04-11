/**
	\file "util/vector_ops.h"
	Common vector/valarray operations, using a single interface.
	Purpose is to mimic std::valarray functions for 
		non-valarray containers.
	TODO: specialize these for vector operations on machines.
	See MacSTL, for example.
	Should with with std::valarray (specialized), 
		and any sequence containers like std::vector, util::array.
	$Id: vector_ops.h,v 1.1.2.1 2011/04/11 18:38:47 fang Exp $
 */

#ifndef	__UTIL_VECTOR_OPS_H__
#define	__UTIL_VECTOR_OPS_H__

#include <valarray>
#include <functional>
#include <algorithm>
#include <numeric>
#include "util/macros.h"

namespace util {
namespace vector_ops {
//=============================================================================
// operator overloads

#define	UTIL_VECTOR_OVERLOAD_BINARY_OPERATOR(OP, functor)		\
template <class T>							\
inline									\
T									\
operator OP (const T& l, const T& r) {					\
	typedef	typename T::value_type	value_type;			\
	INVARIANT(l.size() == r.size());				\
	T ret(l.size());						\
	std::transform(l.begin(), l.end(), r.begin(), ret.begin(), 	\
		functor<value_type>());					\
	return ret;							\
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	UTIL_VECTOR_OVERLOAD_BINARY_OPERATOR_SCALAR(OP, functor)	\
template <class T>							\
inline									\
T									\
operator OP (const T& l, const typename T::value_type& r) {		\
	typedef	typename T::value_type	value_type;			\
	T ret(l.size());						\
	std::transform(l.begin(), l.end(), ret.begin(),		 	\
		std::bind2nd(functor<value_type>(), r));		\
	return ret;							\
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	vector-vector arithmetic
	Explicitly forward to class operator.
 */
#define	UTIL_VECTOR_SPECIALIZE_FORWARD_BINARY_OPERATOR(OP, tclass)	\
template <class T>							\
inline									\
tclass<T>								\
operator OP (const tclass<T>& l, const tclass<T>& r) {			\
	return tclass<T>::operator OP (l, r);				\
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	UTIL_VECTOR_SPECIALIZE_FORWARD_BINARY_OPERATOR_SCALAR(OP, tclass) \
template <class T>							\
inline									\
tclass<T>								\
operator OP (const tclass<T>& l, const T& r) {				\
	return tclass<T>::operator OP (l, r);				\
}

//-----------------------------------------------------------------------------
/**
	Assign operator with arithmetic operator.
 */
#define	UTIL_VECTOR_OVERLOAD_ASSIGN_OPERATOR(OP, functor)		\
template <class T>							\
inline									\
T&									\
operator OP (T& l, const T& r) {					\
	typedef	typename T::value_type	value_type;			\
	INVARIANT(l.size() == r.size());				\
	std::transform(l.begin(), l.end(), r.begin(), l.begin(), 	\
		functor<value_type>());					\
	return l;							\
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	UTIL_VECTOR_OVERLOAD_ASSIGN_OPERATOR_SCALAR(OP, functor)	\
template <class T>							\
inline									\
T&									\
operator OP (T& l, const typename T::value_type& r) {			\
	typedef	typename T::value_type	value_type;			\
	std::transform(l.begin(), l.end(), l.begin(),		 	\
		std::bind2nd(functor<value_type>(), r));		\
	return l;							\
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialize for valarray class.
 */
#define	UTIL_VECTOR_SPECIALIZE_FORWARD_ASSIGN_OPERATOR(OP, tclass)	\
template <class T>							\
inline									\
tclass<T>&								\
operator OP (tclass<T>& l, const tclass<T>& r) {			\
	return l.operator OP (r);					\
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	UTIL_VECTOR_SPECIALIZE_FORWARD_ASSIGN_OPERATOR_SCALAR(OP, tclass) \
template <class T>							\
inline									\
tclass<T>&								\
operator OP (tclass<T>& l, const T& r) {				\
	return l.operator OP (r);					\
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
inline
T&
fill(T& l, const typename T::value_type& r) {
	std::fill(l.begin(), l.end(), r);
	return l;
}

template <class T>
inline
std::valarray<T>&
fill(std::valarray<T>& l, const T& r) {
	l = r;
}

//=============================================================================
#define	OVERLOAD_SPECIALIZE_ARITH_OPERATOR_FAMILY(OP, functor)		\
UTIL_VECTOR_OVERLOAD_BINARY_OPERATOR(OP, functor)			\
UTIL_VECTOR_OVERLOAD_BINARY_OPERATOR_SCALAR(OP, functor)		\
UTIL_VECTOR_SPECIALIZE_FORWARD_BINARY_OPERATOR(OP, std::valarray)	\
UTIL_VECTOR_SPECIALIZE_FORWARD_BINARY_OPERATOR_SCALAR(OP, std::valarray)

#define	OVERLOAD_SPECIALIZE_ASSIGN_OPERATOR_FAMILY(OP, functor)		\
UTIL_VECTOR_OVERLOAD_ASSIGN_OPERATOR(OP, functor)			\
UTIL_VECTOR_OVERLOAD_ASSIGN_OPERATOR_SCALAR(OP, functor)		\
UTIL_VECTOR_SPECIALIZE_FORWARD_ASSIGN_OPERATOR(OP, std::valarray)	\
UTIL_VECTOR_SPECIALIZE_FORWARD_ASSIGN_OPERATOR_SCALAR(OP, std::valarray)

//=============================================================================
OVERLOAD_SPECIALIZE_ARITH_OPERATOR_FAMILY(+, std::plus)
OVERLOAD_SPECIALIZE_ARITH_OPERATOR_FAMILY(-, std::minus)
OVERLOAD_SPECIALIZE_ARITH_OPERATOR_FAMILY(*, std::multiplies)
OVERLOAD_SPECIALIZE_ARITH_OPERATOR_FAMILY(/, std::divides)
OVERLOAD_SPECIALIZE_ARITH_OPERATOR_FAMILY(%, std::modulus)

OVERLOAD_SPECIALIZE_ASSIGN_OPERATOR_FAMILY(+=, std::plus)
OVERLOAD_SPECIALIZE_ASSIGN_OPERATOR_FAMILY(-=, std::minus)
OVERLOAD_SPECIALIZE_ASSIGN_OPERATOR_FAMILY(*=, std::multiplies)
OVERLOAD_SPECIALIZE_ASSIGN_OPERATOR_FAMILY(/=, std::divides)
OVERLOAD_SPECIALIZE_ASSIGN_OPERATOR_FAMILY(%=, std::modulus)
// and more...

//=============================================================================
// algorithm function wrappers

#define	DEFINE_REDUCE_ALGO_WRAPPER(func, algo)				\
template <class T>							\
inline									\
typename T::value_type							\
func(const T& t) {							\
	return algo(t.begin(), t.end());				\
}

#define	SPECIALIZE_REDUCE_MEMFUN_WRAPPER(func, tclass)			\
template <class T>							\
inline									\
T									\
func(const tclass<T>& t) {						\
	return t.func();						\
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	DEFINE_AGGREGATE_ALGO_WRAPPER(func, algo, ival)		\
template <class T>							\
inline									\
typename T::value_type							\
func(const T& t) {							\
	return algo(t.begin(), t.end(), ival);				\
}

#define	SPECIALIZE_AGGREGATE_MEMFUN_WRAPPER(func, tclass)		\
template <class T>							\
inline									\
T									\
func(const tclass<T>& t) {						\
	return t.func();						\
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define	DEFINE_REDUCE_ALGO_FAMILY(func, algo)				\
DEFINE_REDUCE_ALGO_WRAPPER(func, algo)					\
SPECIALIZE_REDUCE_MEMFUN_WRAPPER(func, std::valarray)

#define	DEFINE_AGGREGATE_ALGO_FAMILY(func, algo, ival)			\
DEFINE_AGGREGATE_ALGO_WRAPPER(func, algo, ival)				\
SPECIALIZE_AGGREGATE_MEMFUN_WRAPPER(func, std::valarray)


DEFINE_REDUCE_ALGO_FAMILY(max, *std::max_element)
DEFINE_REDUCE_ALGO_FAMILY(min, *std::min_element)

DEFINE_AGGREGATE_ALGO_FAMILY(sum, std::accumulate, 0)
// valarray does not officially have product

//=============================================================================
// #undefs
//=============================================================================
}	// end namespace vector_ops
}	// end namespace util

#endif	// __UTIL_VECTOR_OPS_H__
