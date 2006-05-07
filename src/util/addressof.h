/**
	\file "util/addressof.h"
	Inspired by boost::utility::addressof.  
	$Id: addressof.h,v 1.1 2006/05/07 20:56:09 fang Exp $
 */

#ifndef	__UTIL_ADDRESSOF_H__
#define	__UTIL_ADDRESSOF_H__

namespace util {
//=============================================================================
/**
	Since some compilers have trouble parsing address of operators.  
	NOTE: this is not as smart as boost's implementation which 
	elides the result of an overloaded unary & (address-of) operator,
	by using a combination of casts.  
	\param T the object type, may be cv-qualified.
	\param t reference whose address is to be taken.  
 */
template <class T>
inline
T*
addressof(T& t) {
	return &t;
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_ADDRESSOF_H__

