/**
	\file "ifthenelse_type.h"
	Compile-time selection between two types.  
	Inspired by "traits/ifthenelse.hpp".
	$Id: ifthenelse.h,v 1.1.2.1 2005/01/19 01:21:20 fang Exp $
 */

#ifndef	__UTIL_IFTHENELSE_H__
#define	__UTIL_IFTHENELSE_H__

namespace util {
//=============================================================================
template <bool, class, class>
struct ifthenelse_type;

template <class A, class B>
struct ifthenelse_type<true, A, B> {
	typedef	A	result_type;
};

template <class A, class B>
struct ifthenelse_type<false, A, B> {
	typedef	B	result_type;
};

#if 0
// will this introduce ambiguity? yes
template <bool V, class C>
struct ifthenelse_type<V, C, C> {
	typedef	C	result_type;
};
#endif

//=============================================================================
template <bool, class A, A, A>
struct ifthenelse_value;

template <class A, A x, A y>
struct ifthenelse_value<true,A,x,y> {
	typedef	A			value_type;
	static const value_type		value;
};

template <class A, A x, A y>
const typename ifthenelse_value<true,A,x,y>::value_type
ifthenelse_value<true,A,x,y>::value(x);

template <class A, A x, A y>
struct ifthenelse_value<false,A,x,y> {
	typedef	A			value_type;
	static const value_type		value;
};

template <class A, A x, A y>
const typename ifthenelse_value<false,A,x,y>::value_type
ifthenelse_value<false,A,x,y>::value(y);

//=============================================================================
}	// end namespace util

#endif	// __UTIL_IFTHENELSE_H__

