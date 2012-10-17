/**
	\file "util/for_all.hh"
	Shorthand for for_each(foo.begin(), foo.end(), ...);
	$Id: for_all.hh,v 1.3 2007/04/26 00:22:45 fang Exp $
 */

#ifndef	__UTIL_FOR_ALL_H__
#define	__UTIL_FOR_ALL_H__

#include <algorithm>

namespace util {
//=============================================================================
/**
	For those times when writing for_each(a.begin(), a.end(), ...)
	is too much typing.  
 */
template <class Cont, class Op>
Op
for_all(Cont& c, Op f) {
#if 1
	return std::for_each(c.begin(), c.end(), f);
#else
	let first(c.begin());
	let last(c.end());
        while (first != last) {
	     f(*first);
        }
#endif
}       // end for_all

//=============================================================================
}	// end namespace util

#endif	// __UTIL_FOR_ALL_H__

