/**
	\file "new_functor_fwd.h"
	Turns new operator for constructing an object into a functor.  
	$Id: new_functor_fwd.h,v 1.1 2005/03/03 04:19:59 fang Exp $
 */

#ifndef	__UTIL_NEW_FUNCTOR_FWD_H__
#define	__UTIL_NEW_FUNCTOR_FWD_H__

namespace util {

template <class R>
struct new_functor_base;

template <class T, class R = T>
struct new_functor;

//-----------------------------------------------------------------------------
}	// end namespace util

#endif	// __UTIL_NEW_FUNCTOR_FWD_H__

