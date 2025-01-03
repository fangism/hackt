/**
	\file "util/bitwise_functional.hh"
	Functors for bitwise operations.  
	$Id: bitwise_functional.hh,v 1.1 2010/04/19 02:46:12 fang Exp $
 */

#ifndef	__UTIL_BITWISE_FUNCTIONAL_H__
#define	__UTIL_BITWISE_FUNCTIONAL_H__

#include <functional>

// was debating injecting these into the std namespace...
namespace util {
//=============================================================================
#if __cplusplus < 201103L
using std::unary_function;
using std::binary_function;
#endif

template <class _Tp>
struct bitwise_and
#if __cplusplus < 201103L
: public binary_function<_Tp, _Tp, _Tp>
#endif
{
	_Tp
	operator() (const _Tp& __x, const _Tp& __y) const
		{ return __x & __y; }
};	// end struct bitwise_and

template <class _Tp>
struct bitwise_or
#if __cplusplus < 201103L
: public binary_function<_Tp, _Tp, _Tp>
#endif
{
	_Tp
	operator() (const _Tp& __x, const _Tp& __y) const
		{ return __x | __y; }
};	// end struct bitwise_or

template <class _Tp>
struct bitwise_xor
#if __cplusplus < 201103L
: public binary_function<_Tp, _Tp, _Tp>
#endif
{
	_Tp
	operator() (const _Tp& __x, const _Tp& __y) const
		{ return __x ^ __y; }
};	// end struct bitwise_xor

template <class _Tp>
struct bitwise_not
#if __cplusplus < 201103L
: public unary_function<_Tp, _Tp>
#endif
{
	_Tp
	operator() (const _Tp& __x) const
		{ return ~__x; }
};	// end struct bitwise_not

//=============================================================================
}	// end namespace util

#endif	// __UTIL_BITWISE_FUNCTIONAL_H__

