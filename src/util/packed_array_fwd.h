/**
	\file "util/packed_array_fwd.h"
	Forward declarations for packed array template class.
	$Id: packed_array_fwd.h,v 1.5.4.1 2005/06/08 19:00:22 fang Exp $
 */

#ifndef	__UTIL_PACKED_ARRAY_FWD_H__
#define	__UTIL_PACKED_ARRAY_FWD_H__

#include "util/size_t.h"

#define PACKED_ARRAY_TEMPLATE_SIGNATURE					\
template <size_t D, class K, class T>

#define PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE				\
template <class K, class T>


namespace util {

PACKED_ARRAY_TEMPLATE_SIGNATURE
class packed_array;

PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
class packed_array_generic;

}	// end namespace util

#endif	// __UTIL_PACKED_ARRAY_FWD_H__

