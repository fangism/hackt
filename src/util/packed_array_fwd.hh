/**
	\file "util/packed_array_fwd.hh"
	Forward declarations for packed array template class.
	$Id: packed_array_fwd.hh,v 1.8 2006/08/24 01:31:18 fang Exp $
 */

#ifndef	__UTIL_PACKED_ARRAY_FWD_H__
#define	__UTIL_PACKED_ARRAY_FWD_H__

#include "util/size_t.h"

#define PACKED_ARRAY_TEMPLATE_SIGNATURE					\
template <size_t D, class K, class T>

#define PACKED_ARRAY_CLASS						\
packed_array<D,K,T>

#define	PACKED_OFFSET_ARRAY_TEMPLATE_SIGNATURE	PACKED_ARRAY_TEMPLATE_SIGNATURE

#define PACKED_OFFSET_ARRAY_CLASS					\
packed_offset_array<D,K,T>

#define PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE				\
template <class K, class T>

#define PACKED_ARRAY_GENERIC_CLASS					\
packed_array_generic<K,T>

#define	PACKED_OFFSET_ARRAY_GENERIC_TEMPLATE_SIGNATURE			\
	PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE

#define PACKED_OFFSET_ARRAY_GENERIC_CLASS				\
packed_offset_array_generic<K,T>


namespace util {

PACKED_ARRAY_TEMPLATE_SIGNATURE
class packed_array;

PACKED_OFFSET_ARRAY_TEMPLATE_SIGNATURE
class packed_offset_array;

PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
class packed_array_generic;

PACKED_OFFSET_ARRAY_GENERIC_TEMPLATE_SIGNATURE
class packed_offset_array_generic;

}	// end namespace util

#endif	// __UTIL_PACKED_ARRAY_FWD_H__

