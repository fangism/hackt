/**
	\file "memory/chunk_map_pool_fwd.h"
	Forward declarations for chunk-allocated mapped memory pool template.  
	$Id: chunk_map_pool_fwd.h,v 1.2 2005/02/27 22:12:01 fang Exp $
 */

#ifndef	__UTIL_MEMORY_CHUNK_MAP_POOL_FWD_H__
#define	__UTIL_MEMORY_CHUNK_MAP_POOL_FWD_H__

namespace util {
namespace memory {

#define	CHUNK_MAP_POOL_TEMPLATE_SIGNATURE				\
template <class T, size_t C, bool Threaded>

template <class T, size_t C, bool Threaded = true>
class chunk_map_pool;

}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_CHUNK_MAP_POOL_FWD_H__

