/**
	\file "what.tcc"
	Default implementation for user-defined type-names.
	$Id: what.tcc,v 1.1 2005/01/06 17:44:58 fang Exp $
 */

#ifndef	__UTIL_WHAT_TCC__
#define	__UTIL_WHAT_TCC__

#include "what.h"
#include <typeinfo>

namespace util {

/**
	Default static initializer of type name.  
	Depending on implementation of type_info::name, may or may not
	be human-readable.  
 */
UTIL_WHAT_TEMPLATE_SIGNATURE
const typename what<T>::name_type
what<T>::name(typeid(T).name());


}	// end namespace util

#endif	// __UTIL_WHAT_TCC__

