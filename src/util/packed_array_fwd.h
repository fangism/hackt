/**
	\file "packed_array_fwd.h"
	Forward declarations for packed array template class.
	$Id: packed_array_fwd.h,v 1.2 2005/01/28 19:58:47 fang Exp $
 */

#ifndef	__UTIL_PACKED_ARRAY_FWD_H__
#define	__UTIL_PACKED_ARRAY_FWD_H__

#define PACKED_ARRAY_TEMPLATE_SIGNATURE					\
template <size_t D, class T>

#define PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE				\
template <class T>


namespace util {

template <class T>
class packed_array_base;

PACKED_ARRAY_TEMPLATE_SIGNATURE
class packed_array;

PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
class packed_array_generic;

}	// end namespace util

#endif	// __UTIL_PACKED_ARRAY_FWD_H__

