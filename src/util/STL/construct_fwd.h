/**
	\file "STL/construct_fwd.h"
	Forward declaration of std::_Construct.
	$Id: construct_fwd.h,v 1.1.12.1.2.1 2005/01/27 02:47:56 fang Exp $
 */

#ifndef	__UTIL_STL_CONSTRUCT_FWD_H__
#define	__UTIL_STL_CONSTRUCT_FWD_H__

namespace std {

template <class _T1>
inline
void
_Construct(_T1*);

/**
	Forward declaration for placement new construction. 
	NOTES: do not write the declarations with any formal identifiers, 
		gcc-3.3.x dies trying to use them in the context of the
		identifiers that appear in the actual definition
		in <bits/stl_construct.h>.
 */
template <class _T1, class _T2>
inline
void
_Construct(_T1*, const _T2&);

}

#define	USING_CONSTRUCT		using std::_Construct;

#endif	// __UTIL_STL_CONSTRUCT_FWD_H__

