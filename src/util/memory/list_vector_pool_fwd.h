/**
	\file "list_vector_pool_fwd.h"
	Forward declaration for container-based memory pool.  

	$Id: list_vector_pool_fwd.h,v 1.5 2005/03/06 04:36:49 fang Exp $
 */

#ifndef	__LIST_VECTOR_POOL_FWD_H__
#define	__LIST_VECTOR_POOL_FWD_H__

#include "STL/construct_fwd.h"
// #include "memory/count_ptr.h"

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
#define	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS			\
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
	The call to get_pool is the replacement for the static
	pool -- the actual pool is a function-local static object, 
	guaranteed to be initialized exactly once upon first
	entry into the function.  

	Tried and abandoned: using reference-counted pointers to the 
	static pool, and using a macro to "require" and initialize the
	pool from another module.  
	This was abandoned because the last reference was not properly
	destroying the allocator.  But now I suspect that it may
	be due to a different nature of function-local statics.  

	QUIRK: such an allocator is sometimes destroyed upon program 
	termination, and I don't know why -- perhaps it is being conservative
	w.r.t the possibility of external references to it?
	In the case where the allocator is not destroyed, 
	one will not be able to get the leak diagnostics from the pool.  
 */
#define	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS			\
	static void*	operator new (size_t);				\
	static void	operator delete (void*);			\
private:								\
	static void*	operator new (size_t, void*&);			\
	typedef	list_vector_pool<this_type>		pool_type;	\
	typedef	raw_count_ptr<pool_type>	pool_ref_ref_type;	\
public:									\
	typedef	count_ptr<const pool_type>		pool_ref_type;	\
									\
	static								\
	pool_ref_ref_type						\
	get_pool(void);

/**
	Friends needed only if default constructor is private.
	NOTE: the formal parameters identifiers for the _Construct 
	friend declarations (__p, __value) are kept to work around a 
	major lookup bug in gcc-3.3.
	Otherwise, it would be better off without it.  
	Fortunately, there's no harm in keeping them; only the definition's
	formal parameters matter -- it's just an eyesore to see them here.  

	See "util/test/friend_function_formal_bug.cc" for example.  
 */
#define	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS				\
	friend class list_vector_pool<this_type>;			\
	friend void _Construct<this_type>(this_type* __p);		\
	friend void _Construct<this_type, this_type>(			\
		this_type* __p, const this_type& __value);

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

