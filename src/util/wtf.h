/**
	\file "util/wtf.h"
	Intentionally undefined struct for the sake of seeing
	the name of a resolved type.  
	$Id: wtf.h,v 1.3 2006/01/27 08:07:22 fang Exp $
 */

#ifndef	__UTIL_WTF_H__
#define	__UTIL_WTF_H__

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

