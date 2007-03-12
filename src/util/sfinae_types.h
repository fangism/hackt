/**
	\file "util/sfinae_types.h"
	Meta-programming helpers for the 'substitution-failure-is-not-an-error'
	paradigm.  
	$Id: sfinae_types.h,v 1.1 2007/03/12 07:38:15 fang Exp $
 */

#ifndef	__UTIL_SFINAE_TYPES_H__
#define	__UTIL_SFINAE_TYPES_H__

namespace util {
//=============================================================================
/**
	Now we are resorting to Jedi C++ techniques...
	Helper base class with two types of different size, used to 
	deduce type-traits of classes.  
	Derive from this type to get convenient access to the type members.  
 */
struct sfinae_types {
	/// has size 1
	typedef	char		one;

	/// has size 2
	struct two {
		char		dummy[2];
	};
};	// end struct sfinae_types

//=============================================================================
}	// end namespace util

#endif	// __UTIL_SFINAE_TYPES_H__

