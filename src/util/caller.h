/**
	\file "util/caller.h"
	Functional calling utility.
	Useful when the seuence being iterated over is a function or
	function object.  
	$Id: caller.h,v 1.1.2.1 2007/03/27 06:20:46 fang Exp $
 */

#ifndef	__UTIL_CALLER_H__
#define	__UTIL_CALLER_H__

namespace util {
//=============================================================================
/**
	Functor that invokes argument like a function.  
	TODO: add specializations for arguments and binders...
 */
struct caller {
	template <class F>
	void
	operator () (F f) const {
		f();
	}
};	// end struct caller

//=============================================================================
}	// end namespace util

#endif	// __UTIL_CALLER_H__

