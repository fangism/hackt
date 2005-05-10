/**
	\file "util/new_functor_fwd.h"
	Turns new operator for constructing an object into a functor.  
	$Id: new_functor_fwd.h,v 1.3 2005/05/10 04:51:28 fang Exp $
 */

#ifndef	__UTIL_NEW_FUNCTOR_FWD_H__
#define	__UTIL_NEW_FUNCTOR_FWD_H__

namespace util {

template <class T, class R = T>
struct new_functor;

template <class, class, class>
struct binder_new_functor;

//-----------------------------------------------------------------------------
}	// end namespace util

#endif	// __UTIL_NEW_FUNCTOR_FWD_H__

