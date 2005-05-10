/**
 *	\file "util/dereference.h"
 *	This file contains definition of deference functor.  
 *	$Id: dereference.h,v 1.3 2005/05/10 04:51:23 fang Exp $
 */

#ifndef	__UTIL_DEREFERENCE_H__
#define	__UTIL_DEREFERENCE_H__

#include <functional>
#include "util/memory/pointer_traits.h"

namespace util {
USING_UTIL_MEMORY_POINTER_TRAITS

//=============================================================================
template <class P>
class dereference_t :
	public std::unary_function<P, typename internal_reference<P>::type> {
public:
	typedef	typename internal_reference<P>::type	reference;

	reference
	operator() (const P& p) const { return *p; }
};	// end class dereference_t

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
template <template <class> class P, class T>
class const_dereference_t : public unary_function<P<T>, const T&> {
public:
	const T& operator() (const P<T>& p) const { return *p; }
};	// end class const_dereference_t
#endif

//-----------------------------------------------------------------------------
/**
	Returns a dereference functor.  
 */
template <class P>
inline
dereference_t<P>
dereference(void) {
	return dereference_t<P>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
template <template <class> class P, class T>
inline
const_dereference_t<P,T>
const_dereference () {
	return const_dereference_t<P,T>();
}
#endif

//=============================================================================
}	// end namespace util

#endif	// __UTIL_DEREFERENCE_H__

