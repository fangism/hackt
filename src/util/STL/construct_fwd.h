/**
	\file "STL/construct_fwd.h"
	Forward declaration of std::_Construct.
	$Id: construct_fwd.h,v 1.1.12.1 2005/01/25 22:33:43 fang Exp $
 */

#ifndef	__UTIL_STL_CONSTRUCT_FWD_H__
#define	__UTIL_STL_CONSTRUCT_FWD_H__

namespace std {

template <class _T1>
inline
void
_Construct(_T1* __p);

template <class _T1, class _T2>
inline
void
_Construct(_T1* __p, const _T2& __value);

}

#define	USING_CONSTRUCT		using std::_Construct;

#endif	// __UTIL_STL_CONSTRUCT_FWD_H__

