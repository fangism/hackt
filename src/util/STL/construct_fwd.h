/**
	\file "STL/construct_fwd.h"
	Forward declaration of std::_Construct.
	$Id: construct_fwd.h,v 1.1 2005/01/12 03:19:57 fang Exp $
 */

#ifndef	__UTIL_STL_CONSTRUCT_FWD_H__
#define	__UTIL_STL_CONSTRUCT_FWD_H__

namespace std {

template <class _T1, class _T2>
inline void
_Construct(_T1* __p, const _T2& __value);

}

#endif	// __UTIL_STL_CONSTRUCT_FWD_H__

