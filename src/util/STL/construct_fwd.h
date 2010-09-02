/**
	\file "util/STL/construct_fwd.h"
	Forward declaration of std::_Construct, and std::_Destroy.
	$Id: construct_fwd.h,v 1.7 2010/09/02 00:34:44 fang Exp $
 */

#ifndef	__UTIL_STL_CONSTRUCT_FWD_H__
#define	__UTIL_STL_CONSTRUCT_FWD_H__

#include "config.h"

#ifdef	HAVE_BITS_STL_CONSTRUCT_H
#include <iterator>		// for iterator_traits
#include <bits/stl_construct.h>
#else
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
#endif	// HAVE_BITS_STL_CONSTRUCT_H

#if	!defined(HAVE_STL_CONSTRUCT_DEFAULT)
/**
	Stolen from gcc-4.0 libstdc++...
 */
namespace std {
template <class _T1>
inline
void
_Construct(_T1* __p) {
	::new(static_cast<void*>(__p)) _T1();
}
}
#endif

#if	!defined(HAVE_STL_CONSTRUCT_COPY)
/**
	This copy construct-or might have disappeared with C++0x...
 */
namespace std {
template <class _T1, class _T2>
inline
void
_Construct(_T1* __p, const _T2& __v) {
	::new(static_cast<void*>(__p)) _T1(__v);
}
}
#endif

#define	USING_CONSTRUCT		using std::_Construct;
#define	USING_DESTROY		using std::_Destroy;

#define	FRIEND_STD_CONSTRUCT						\
	template <class _T1>						\
	friend void std::_Construct(_T1*);
#define	FRIEND_STD_CONSTRUCT2						\
	template <class _T1, class _T2>					\
	friend void std::_Construct(_T1*, const _T2&);

#endif	// __UTIL_STL_CONSTRUCT_FWD_H__

