/**
	\file "packed_array_fwd.h"
	Forward declarations for packed array template class.
	$Id: packed_array_fwd.h,v 1.2.18.1 2005/02/21 19:48:11 fang Exp $
 */

#ifndef	__UTIL_PACKED_ARRAY_FWD_H__
#define	__UTIL_PACKED_ARRAY_FWD_H__

#define	USE_PACKED_ARRAY_BASE		0

#define PACKED_ARRAY_TEMPLATE_SIGNATURE					\
template <size_t D, class T>

#define PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE				\
template <class T>


namespace util {

#if USE_PACKED_ARRAY_BASE
template <class T>
class packed_array_base;
#endif

PACKED_ARRAY_TEMPLATE_SIGNATURE
class packed_array;

PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
class packed_array_generic;

}	// end namespace util

#endif	// __UTIL_PACKED_ARRAY_FWD_H__

