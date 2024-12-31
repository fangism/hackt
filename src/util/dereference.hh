/**
 *	\file "util/dereference.hh"
 *	This file contains definition of deference functor.  
 *	$Id: dereference.hh,v 1.4 2006/02/22 04:45:25 fang Exp $
 */

#ifndef	__UTIL_DEREFERENCE_H__
#define	__UTIL_DEREFERENCE_H__

#include <functional>
#include "util/memory/pointer_traits.hh"

namespace util {
USING_UTIL_MEMORY_POINTER_TRAITS

//=============================================================================
/**
	No const specialization necessary.  
	Is already appropriately given by the pointer type.  
 */
template <class P>
class dereference_t
#if __cplusplus < 201103L
	: public std::unary_function<P, typename internal_reference<P>::type>
#endif
{
public:
	typedef	typename internal_reference<P>::type	reference;

	reference
	operator() (const P& p) const { return *p; }
};	// end class dereference_t

//-----------------------------------------------------------------------------
/**
	Returns a dereference functor.  
	Unfortunately, the type P cannot be deduced from call arguments.  
 */
template <class P>
inline
dereference_t<P>
dereference(void) {
	return dereference_t<P>();
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_DEREFERENCE_H__

