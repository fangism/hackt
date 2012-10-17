/**
	\file "util/wtf.hh"
	Intentionally undefined struct for the sake of seeing
	the name of a resolved type.  
	TODO: extend this to work with function, member functions, 
		and their pointers.  
	$Id: wtf.hh,v 1.5 2006/04/23 07:37:28 fang Exp $
 */

#ifndef	__UTIL_WTF_H__
#define	__UTIL_WTF_H__

#if 0
#error	This file should not be #included in released code.  
#endif

#include "util/cppcat.h"

namespace util {
//=============================================================================
class friend_of_wtf;

/**
	Useful for finding out the canonical type behind a name.  
 */
template <class T>
class wtf {
	// to silence that warning that wtf has no public members nor friends
	friend class friend_of_wtf;
private:
	// undefined
	explicit
	wtf(const wtf&);
};

/**
	Alternate macro for WTF (for types).  
 */
#define	WTF_TYPE(T)	const util::wtf<T >	UNIQUIFY(__wtf_);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Useful for finding out the type of an object.  
 */
template <class T>
inline void
wtf_is(T t) {
	const wtf<T> the_type_is(t);
}

/**
	Alternate macro for WTF (for objects).
 */
#define	WTF_OBJECT(t)	util::wtf_is(t);

//=============================================================================
}	// end namespace util

#endif	// __UTIL_WTF_H__

