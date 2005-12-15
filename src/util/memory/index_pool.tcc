/**
	\file "util/memory/index_pool.tcc"
	$Id: index_pool.tcc,v 1.1.2.1 2005/12/15 04:47:01 fang Exp $
 */

#ifndef	__UTIL_MEMORY_INDEX_POOL_TCC__
#define	__UTIL_MEMORY_INDEX_POOL_TCC__

#include "util/memory/index_pool.h"

namespace util {
namespace memory {
//=============================================================================
// class index_pool method definitions

template <class Pool>
index_pool<Pool>::index_pool() : array_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Pool>
index_pool<Pool>::~index_pool() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return index into the array for newly allocated (available) entry.
 */
template <class Pool>
size_t
index_pool<Pool>::allocate(void) {
	const size_t ret = this->size();
	push_back(value_type());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param the value to initialize the new entry with.  
	\return index into the array for newly allocated (available) entry.
 */
template <class Pool>
size_t
index_pool<Pool>::allocate(const value_type& v) {
	const size_t ret = this->size();
	push_back(v);
	return ret;
}

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_INDEX_POOL_TCC__

