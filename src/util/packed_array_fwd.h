/**
	\file "util/packed_array_fwd.h"
	Forward declarations for packed array template class.
	$Id: packed_array_fwd.h,v 1.6.92.1 2006/07/04 04:41:13 fang Exp $
 */

#ifndef	__UTIL_PACKED_ARRAY_FWD_H__
#define	__UTIL_PACKED_ARRAY_FWD_H__

#include "util/size_t.h"

#define PACKED_ARRAY_TEMPLATE_SIGNATURE					\
template <size_t D, class K, class T>

#define PACKED_ARRAY_CLASS						\
packed_array<D,K,T>

#define PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE				\
template <class K, class T>

#define PACKED_ARRAY_GENERIC_CLASS					\
packed_array_generic<K,T>


namespace util {

PACKED_ARRAY_TEMPLATE_SIGNATURE
class packed_array;

PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
class packed_array_generic;

}	// end namespace util

#endif	// __UTIL_PACKED_ARRAY_FWD_H__

