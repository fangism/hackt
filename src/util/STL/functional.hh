/**
	\file "util/STL/functional.hh"
	Adds possibly missing (non-standard) definitions.  
	$Id: functional.hh,v 1.3 2006/05/06 22:08:40 fang Exp $
 */

#ifndef	__UTIL_STL_FUNCTIONAL_HH__
#define	__UTIL_STL_FUNCTIONAL_HH__

#include "util/STL/functional_fwd.hh"
#include <functional>

BEGIN_NAMESPACE_STD
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
END_NAMESPACE_STD

#endif	// __UTIL_STL_FUNCTIONAL_HH__

