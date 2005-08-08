/**
	\file "Object/inst/instance_pool.tcc"
	Implementation of instance pool.
	$Id: instance_pool.tcc,v 1.2 2005/08/08 16:51:09 fang Exp $
 */

#ifndef	__OBJECT_INST_INSTANCE_POOL_TCC__
#define	__OBJECT_INST_INSTANCE_POOL_TCC__

#include "Object/inst/instance_pool.h"
#include "util/list_vector.tcc"
#include "util/stacktrace.h"

namespace ART {
namespace entity {
//=============================================================================
// class instance_pool method definitions

/**
	Note: this constructor initializes with one element pre-allocated, 
	so the first index returned by allocator is nonzero.  
 */
template <class T>
instance_pool<T>::instance_pool(const size_t s) : parent_type() {
	this->set_chunk_size(s);
	allocate();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
instance_pool<T>::~instance_pool() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates one element.  
	Allocation is extremely efficient because of the underlying
	list_vector implementation -- never reallocs and is pool-reserved
	per chunk.  
	\return the index of the newly allocated element.  
 */
template <class T>
size_t
instance_pool<T>::allocate(void) {
//	STACKTRACE_VERBOSE;
	STACKTRACE("instance_pool::allocate()");
	const size_t ret = this->size();
	push_back(T());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates one element with an initial value.  
	Requires that type T is copy-constructible.
 */
template <class T>
size_t
instance_pool<T>::allocate(const T& t) {
//	STACKTRACE_VERBOSE;
	STACKTRACE("instance_pool::allocate(const T&)");
	const size_t ret = this->size();
	push_back(t);
	return ret;
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_INSTANCE_POOL_TCC__

