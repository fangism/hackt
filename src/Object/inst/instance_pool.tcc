/**
	\file "Object/inst/instance_pool.tcc"
	Implementation of instance pool.
	$Id: instance_pool.tcc,v 1.1.2.1 2005/08/06 15:42:28 fang Exp $
 */

#ifndef	__OBJECT_INST_INSTANCE_POOL_TCC__
#define	__OBJECT_INST_INSTANCE_POOL_TCC__

#include "Object/inst/instance_pool.h"
#include "util/list_vector.tcc"

namespace ART {
namespace entity {
//=============================================================================
// class instance_pool method definitions

/**
	Note: this constructor initializes with one element pre-allocated, 
	so the first index returned by allocator is nonzero.  
 */
template <class T>
instance_pool<T>::instance_pool(const size_t s) : parent_type(s) {
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
	const size_t ret = this->size();
	push_back(T());
	return ret;
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_INSTANCE_POOL_TCC__

