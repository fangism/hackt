/**
	\file "new_functor.h"
	Turns new operator for constructing an object into a functor.  
	$Id: new_functor.h,v 1.2 2005/03/04 03:17:40 fang Exp $
 */

#ifndef	__UTIL_NEW_FUNCTOR_H__
#define	__UTIL_NEW_FUNCTOR_H__

#include "new_functor_fwd.h"
#include "nullary_function.h"

namespace util {

//-----------------------------------------------------------------------------
#if 0
/**
	Pure virtual constructor functor.  
 */
template <class R>
struct new_functor_base : public nullary_function<R> {
virtual ~new_functor_base() { }

virtual	R
	operator () (void) const = 0;
};	// end struct new_functor_base
#endif

//-----------------------------------------------------------------------------
/**
	Functor that returns a pointer to newly allocated object,
	and optionally up-casts the return type.  
 */
template <class T, class R>
struct new_functor : public nullary_function_virtual<R*> {
	typedef	R		upcast_type;
	typedef	T		object_type;
	// concept_check: R is superclass of T

	~new_functor() { }

	R*
	operator () (void) const;
};	// end struct new_functor

//-----------------------------------------------------------------------------
/**
	Constructor functor that binds a single argument.  
 */
template <class T, class R, class A>
struct binder_new_functor : public nullary_function_virtual<R*> {
public:
	typedef	R*		result_type;
protected:
	typedef A		argument_type;

protected:
	argument_type   value;
public:
	explicit
	binder_new_functor(const argument_type& x) : value(x) { }

	~binder_new_functor() { }

	R*
	operator () (void) const;

};	// end struct binder_new_functor

//-----------------------------------------------------------------------------
/**
	Helper inline function for binding an argument to a constructor
	functor.  
 */
template <class T, class R, class A>
inline
binder_new_functor<T,R,A>
bind_new_functor(const A& a) {
	return binder_new_functor<T,R,A>(a);
}

//-----------------------------------------------------------------------------
}	// end namespace util

#endif	// __UTIL_NEW_FUNCTOR_H__

