/**
	\file "new_functor.h"
	Turns new operator for constructing an object into a functor.  
	$Id: new_functor.h,v 1.1 2005/03/03 04:19:58 fang Exp $
 */

#ifndef	__UTIL_NEW_FUNCTOR_H__
#define	__UTIL_NEW_FUNCTOR_H__

#include "new_functor_fwd.h"
#include "nullary_function.h"

namespace util {

//-----------------------------------------------------------------------------
/**
	Pure virtual constructor functor.  
 */
template <class R>
struct new_functor_base : public nullary_function<R> {
virtual ~new_functor_base() { }

virtual	R
	operator () (void) const = 0;
};	// end struct new_functor_base

//-----------------------------------------------------------------------------
/**
	Functor that returns a pointer to newly allocated object,
	and optionally up-casts the return type.  
 */
template <class T, class R>
struct new_functor : public new_functor_base<R*> {
	typedef	R		upcast_type;
	typedef	T		object_type;
	// concept_check: R is superclass of T

	~new_functor() { }

	R*
	operator () (void) const;
};	// end struct new_functor

//-----------------------------------------------------------------------------
}	// end namespace util

#endif	// __UTIL_NEW_FUNCTOR_H__

