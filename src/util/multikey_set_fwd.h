/**
	"multikey_set_fwd.h"
	Forward declarations for multikey set.
	$Id: multikey_set_fwd.h,v 1.2 2005/02/27 22:54:25 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_SET_FWD_H__
#define	__UTIL_MULTIKEY_SET_FWD_H__

#include "STL/set_fwd.h"

#define	MULTIKEY_SET_TEMPLATE_SIGNATURE					\
template <size_t D, class T, template <class> class S>

#define	MULTIKEY_SET_ELEMENT_TEMPLATE_SIGNATURE				\
template <size_t D, class K, class T>

namespace util {

template <size_t D, class T, template <class> class S = std::set >
class multikey_set;

MULTIKEY_SET_ELEMENT_TEMPLATE_SIGNATURE
class multikey_set_element;

}	// end namespace util

#endif	// __UTIL_MULTIKEY_SET_FWD_H__

