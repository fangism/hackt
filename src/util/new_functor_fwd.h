/**
	\file "new_functor_fwd.h"
	Turns new operator for constructing an object into a functor.  
	$Id: new_functor_fwd.h,v 1.2 2005/03/04 03:17:40 fang Exp $
 */

#ifndef	__UTIL_NEW_FUNCTOR_FWD_H__
#define	__UTIL_NEW_FUNCTOR_FWD_H__

namespace util {

#if 0
template <class R>
struct new_functor_base;
#endif

template <class T, class R = T>
struct new_functor;

template <class, class, class>
struct binder_new_functor;

//-----------------------------------------------------------------------------
}	// end namespace util

#endif	// __UTIL_NEW_FUNCTOR_FWD_H__

