/**
	\file "list_vector_pool_fwd.h"
	Forward declaration for container-based memory pool.  

	$Id: list_vector_pool_fwd.h,v 1.1 2005/01/11 01:56:05 fang Exp $
 */

#ifndef	__LIST_VECTOR_POOL_FWD_H__
#define	__LIST_VECTOR_POOL_FWD_H__

namespace util {
//=============================================================================
/**
	Memory-management utility namespace.
	Should implement the std::allocator interface.  
 */
namespace memory {

//=============================================================================
// forward declaration
template <class T, bool Threaded = true>
class list_vector_pool;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
template <class T>
inline
bool
operator == (const list_vector_pool<T>&, const list_vector_pool<T>&) {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
inline
bool
operator != (const list_vector_pool<T>&, const list_vector_pool<T>&) {
	return false;
}
#endif

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	//	__LIST_VECTOR_POOL_FWD_H__

