/**
	\file "new_functor.tcc"
	Default implementation of new functor.  
	$Id: new_functor.tcc,v 1.1 2005/03/03 04:19:59 fang Exp $
 */

#ifndef	__UTIL_NEW_FUNCTOR_TCC__
#define	__UTIL_NEW_FUNCTOR_TCC__

#include "new_functor.h"

namespace util {

template <class T, class R>
R*
new_functor<T,R>::operator () (void) const {
	return new T();
}

}	// end namespace util

#endif	// __UTIL_NEW_FUNCTOR_TCC__

