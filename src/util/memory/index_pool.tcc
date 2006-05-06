/**
	\file "util/memory/index_pool.tcc"
	$Id: index_pool.tcc,v 1.4 2006/05/06 22:08:41 fang Exp $
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
typename index_pool<Pool>::size_type
index_pool<Pool>::allocate(void) {
	const size_type ret = this->size();
	this->push_back(value_type());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param the value to initialize the new entry with.  
	\return index into the array for newly allocated (available) entry.
 */
template <class Pool>
typename index_pool<Pool>::size_type
index_pool<Pool>::allocate(const value_type& v) {
	const size_type ret = this->size();
	this->push_back(v);
	return ret;
}

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_INDEX_POOL_TCC__

