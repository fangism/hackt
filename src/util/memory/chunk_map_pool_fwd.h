/**
	\file "memory/chunk_map_pool_fwd.h"
	Forward declarations for chunk-allocated mapped memory pool template.  
	$Id: chunk_map_pool_fwd.h,v 1.2.10.1 2005/03/06 00:52:05 fang Exp $
 */

#ifndef	__UTIL_MEMORY_CHUNK_MAP_POOL_FWD_H__
#define	__UTIL_MEMORY_CHUNK_MAP_POOL_FWD_H__

namespace util {
namespace memory {

#define CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE				\
template <class T, size_t C>

CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE
class chunk_map_pool_chunk;


#define	CHUNK_MAP_POOL_TEMPLATE_SIGNATURE				\
template <class T, size_t C, bool Threaded>

template <class T, size_t C, bool Threaded = true>
class chunk_map_pool;

}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_CHUNK_MAP_POOL_FWD_H__

