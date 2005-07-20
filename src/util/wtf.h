/**
	\file "util/wtf.h"
	Intentionally undefined struct for the sake of seeing
	the name of a resolved type.  
	$Id: wtf.h,v 1.2 2005/07/20 21:01:02 fang Exp $
 */

#ifndef	__UTIL_WTF_H__
#define	__UTIL_WTF_H__

namespace util {
//=============================================================================

/**
	Useful for finding out the canonical type behind a name.  
 */
template <class T>
class wtf {
	// undefined
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Useful for finding out the type of an object.  
 */
template <class T>
inline void
wtf_is(T t) {
	const wtf<T> the_type_is(t);
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_WTF_H__

