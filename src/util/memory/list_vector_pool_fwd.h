/**
	\file "list_vector_pool_fwd.h"
	Forward declaration for container-based memory pool.  

	$Id: list_vector_pool_fwd.h,v 1.2 2005/01/15 19:13:44 fang Exp $
 */

#ifndef	__LIST_VECTOR_POOL_FWD_H__
#define	__LIST_VECTOR_POOL_FWD_H__

/**
	These are the enw and delete operators required when
	allocating with a list_vector_pool.  
	This declaration should appear only inside a class!
	The class should contain a typedef named `this_type'
	referring to its own type.
	The class should also contain a default constructor.  
	This macro may be invoked in a class's public section.  
	The first new operator will be defined to call allocate().
	The second new operator is just a placement constructor, no-op
		-- this should really be private and inline when defined, 
		-- it needs to be accessible to std::_Construct.
	The delete operator will be defined to call deallocate().  
	The pool_type and pool declarations will always appear in the 
	private section, so remember that subsequent declarations will
	also be private unless visibility is restated.  

	TODO: make a version of pool declaration that is 
		static-initialization order-safe, using a pool ptr.  
		This current definition is NOT guaranteed to work on 
		types that are allocated during static initialization.  
 */
#define	LIST_VECTOR_POOL_STATIC_DECLARATIONS				\
	static void*	operator new (size_t);				\
	static void	operator delete (void*);			\
	static void*	operator new (size_t, void*&);			\
private:								\
	typedef	list_vector_pool<this_type>		pool_type;	\
	static pool_type				pool;

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

