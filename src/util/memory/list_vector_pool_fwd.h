/**
	\file "list_vector_pool_fwd.h"
	Forward declaration for container-based memory pool.  

	$Id: list_vector_pool_fwd.h,v 1.2.10.1 2005/01/22 06:38:28 fang Exp $
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

/**
	Similar macro to the non-robust version defined above.  
	This version is needed when allocation from such list_vector_pools
	is done during static initialization across modules.  
	Use of this macro requires inclusion of the "memory/pointer_classes.h"
	header.  
 */
#define	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS			\
	static void*	operator new (size_t);				\
	static void	operator delete (void*);			\
	static void*	operator new (size_t, void*&);			\
private:								\
	typedef	list_vector_pool<this_type>		pool_type;	\
private:								\
	static								\
	pool_type&							\
	get_pool(void);

// public:
//	typedef	count_ptr<pool_type>			pool_ref_type;
// private:							
//	static pool_type*				pool;		
//	static size_t*					pool_ref_count;	

//	static pool_ref_type				pool_ref;

/**
	Convenient macro for explicitly requiring that a memory pool
	be ready during static initialization of a particular module.  
	This is not required now...
 */
#define REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(T)			\
static T::pool_type&							\
__pool_ref_ ## T ## __ (T::acquire_pool_reference());

#if 0
static const T::pool_ref_type
__pool_ref_ ## T ## __ (T::acquire_pool_reference());
#endif


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

