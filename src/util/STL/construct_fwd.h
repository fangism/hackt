/**
	\file "util/STL/construct_fwd.h"
	Forward declaration of std::_Construct, and std::_Destroy.
	$Id: construct_fwd.h,v 1.4.84.1 2006/03/31 06:08:56 fang Exp $
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

/**
	Invoke destructor of referenced object.  
 */
template <class _Tp>
inline
void
_Destroy(_Tp*);


/**
	Invoke destructor over range of objects.  
 */
template <class FwdIter>
inline
void
_Destroy(FwdIter, FwdIter);

}	// end namespace std

#define	USING_CONSTRUCT		using std::_Construct;
#define	USING_DESTROY		using std::_Destroy;

#define	FRIEND_STD_CONSTRUCT						\
	template <class _T1>						\
	friend void std::_Construct(_T1*);
#define	FRIEND_STD_CONSTRUCT2						\
	template <class _T1, class _T2>					\
	friend void std::_Construct(_T1*, const _T2&);

#endif	// __UTIL_STL_CONSTRUCT_FWD_H__

