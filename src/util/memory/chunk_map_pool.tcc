/**
	\file "memory/chunk_map_pool.tcc"
	Method definitions for chunk-allocated memory pool.
	$Id: chunk_map_pool.tcc,v 1.1.2.1 2005/02/27 21:57:09 fang Exp $
 */

#ifndef	__UTIL_MEMORY_CHUNK_MAP_POOL_TCC__
#define	__UTIL_MEMORY_CHUNK_MAP_POOL_TCC__

#include <iostream>
#include "memory/chunk_map_pool.h"
#include "memory/destruction_policy.tcc"

namespace util {
namespace memory {
#include "using_ostream.h"

//=============================================================================
// class chunk_map_pool_chunk method definitions

CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE
CHUNK_MAP_POOL_CHUNK_CLASS::chunk_map_pool_chunk() : free_mask(0) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE
CHUNK_MAP_POOL_CHUNK_CLASS::~chunk_map_pool_chunk() {
	if (!this->empty()) {
		cerr << "WARNING: chunk freed while element still live!"
			<< endl;
	}
}


//=============================================================================
// class chunk_map_pool method definitions

CHUNK_MAP_POOL_TEMPLATE_SIGNATURE
CHUNK_MAP_POOL_CLASS::chunk_map_pool() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_TEMPLATE_SIGNATURE
CHUNK_MAP_POOL_CLASS::~chunk_map_pool() {
	if (!this->chunk_map.empty()) {
		cerr << "WARNING: chunk map destroyed while chunks were live!"
			<< endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
CHUNK_MAP_POOL_TEMPLATE_SIGNATURE
pointer
CHUNK_MAP_POOL_CLASS::allocate(void) {
	if (this->avail_set.empty()) {
		// need to create a chunk
	} else {
		// pick any chunk with a free element
	}
}
#endif

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_CHUNK_MAP_POOL_TCC__

