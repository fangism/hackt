/**
	\file "nullary_function.h"
	Generic generator functor base.
	$Id: nullary_function.h,v 1.1 2005/03/03 04:19:59 fang Exp $
 */

#ifndef	__UTIL_NULLARY_FUNCTION_H__
#define	__UTIL_NULLARY_FUNCTION_H__

namespace util {

/**
	Template for functor that takes no arguments.  
	Oddly enough this isn't in <bits/stl_function.h>.
 */
template <class _Result>
struct nullary_function {
	/// the return type
	typedef	_Result		result_type;
};	// end struct nullary_function

}	// end namespace util

#endif	// __UTIL_NULLARY_FUNCTION_H__

