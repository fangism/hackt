/**
	\file "util/STL/functional.hh"
	Adds possibly missing (non-standard) definitions.  
	$Id: functional.hh,v 1.3 2006/05/06 22:08:40 fang Exp $
 */

#ifndef	__UTIL_STL_FUNCTIONAL_H__
#define	__UTIL_STL_FUNCTIONAL_H__

#include "config.h"
#include "util/STL/functional_fwd.hh"
#include <functional>

namespace std {
//=============================================================================
// make up for possible deficiencies in <functional> header...

#if	!defined(HAVE_STD__IDENTITY)
template <class _Tp>
struct _Identity : public unary_function<_Tp,_Tp> {
_Tp&
operator()(_Tp& __x) const { return __x; }

const _Tp&
operator()(const _Tp& __x) const { return __x; }
};	// end struct _Identity
#endif	// HAVE_STD__IDENTITY
 
#if	!defined(HAVE_STD__SELECT1ST)
template <class _Pair>
struct _Select1st : public unary_function<_Pair, typename _Pair::first_type> {
	typename _Pair::first_type&
	operator()(_Pair& __x) const { return __x.first; }

	const typename _Pair::first_type&
	operator()(const _Pair& __x) const { return __x.first; }
};	// end struct _Select1st
#endif	// HAVE_STD__SELECT1ST

#if	!defined(HAVE_STD__SELECT2ND)
template <class _Pair>
struct _Select2nd : public unary_function<_Pair, typename _Pair::second_type> {
	typename _Pair::second_type&
	operator()(_Pair& __x) const { return __x.second; }

	const typename _Pair::second_type&
	operator()(const _Pair& __x) const { return __x.second; }
};	// end struct _Select2nd
#endif	// HAVE_STD__SELECT2ND

//=============================================================================
// well, I want a functor with 3 arguments!

template <class _Arg1, class _Arg2, class _Arg3, class _Result>
struct ternary_function {
	typedef _Arg1 first_argument_type;
	typedef _Arg2 second_argument_type;
	typedef _Arg3 third_argument_type;
	typedef _Result result_type;
};

template <class _Arg1, class _Arg2, class _Arg3, class _Result>
class pointer_to_ternary_function
	: public ternary_function<_Arg1, _Arg2, _Arg3, _Result> {
protected:
	_Result (*_M_ptr)(_Arg1, _Arg2, _Arg3);
public:
	pointer_to_ternary_function() {}

	explicit
	pointer_to_ternary_function(_Result (*__x)(_Arg1, _Arg2, _Arg3))
	: _M_ptr(__x) {}

	_Result
	operator()(_Arg1 __x, _Arg2 __y, _Arg3 __z) const
		{ return _M_ptr(__x, __y, __z); }
};	// end struct ternary function

template <class _Arg1, class _Arg2, class _Arg3, class _Result>
inline pointer_to_ternary_function<_Arg1, _Arg2, _Arg3, _Result>
ptr_fun(_Result (*__x)(_Arg1, _Arg2, _Arg3)) {
	return pointer_to_ternary_function<_Arg1, _Arg2, _Arg3, _Result>(__x);
}

//=============================================================================
}	// end namespace std

#endif	// __UTIL_STL_FUNCTIONAL_H__

