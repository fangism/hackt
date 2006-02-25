/**
	\file "util/memory/list_vector_pool.h"
	Simple template container-based memory pool.  
	Basically allocates a large chunk at a time.  

	$Id: list_vector_pool.h,v 1.15 2006/02/25 04:55:04 fang Exp $
 */

#ifndef	__UTIL_MEMORY_LIST_VECTOR_POOL_H__
#define	__UTIL_MEMORY_LIST_VECTOR_POOL_H__

#include "util/memory/list_vector_pool_fwd.h"
#include "util/memory/thread_lock.h"
#include "util/memory/destruction_policy.h"
#include "util/macros.h"
#include "util/attributes.h"

#include <queue>
#include <list>
#include <vector>
#include <iosfwd>


#define	LIST_VECTOR_POOL_TEMPLATE_SIGNATURE				\
template <class T, bool Threaded>


//=============================================================================
// interface macros

/**
	\param T the class type to custom-allocate.
	\param C the chunk size to allocate for this class.  

        Non-member static allocation pool for a particular class.
        Note static linkage.  
        This static initialization must appear before any uses in methods.  

	Placement construct, needed by vector allocation.  
	Just No-op.  Does nothing.  

	Pool deallocation for this class.  
	A reinterpret_cast is needed because this particular allocator is 
	works with type-specific arguments, but the standard allocator 
	interface works with generic void-pointers.  

	NOTE:
	http://www.informit.com/guides/content.asp?g=cplusplus&seqNum=40&rl=1

	Implementation

	The implementation of new is straightforward: it calls a custom 
	allocation function, say, allocate_from_pool(), and returns the 
	resulting pointer. You can use any other memory allocation function 
	that suits your needs, such as malloc() or GlobalAlloc(), as long as 
	that function meets the memory alignment requirement of your 
	objects. Here's a typical implementation of new:

	void* A::operator new (size_t size) {
		void *p=allocate_from_pool(size);
		return p;
	} // A's default ctor implicitly called here

	Note that when this function exits, the class's default constructor 
	executes automatically and constructs an object of type A. 
	The matching version of delete looks as follows:

	void A::operator delete (void *p) {
		release(p); // return memory to pool
	} // A's dtor implicitly called at this point

	C++ guarantees that an object's destructor is automatically called 
	just before delete executes. Therefore, you shouldn't invoke A's 
	destructor explicitly (doing so would cause undefined behavior as 
	the destructor will actually run twice).

	--- end quote ---

	SUMMARY:
	Did you catch the part about implicit ctor/dtor calls?
	This means we CANNOT use vector<T> as our underlying memory chunk.  
	We cannot suppress the call to the destructor!
	The only hack we can do to make this safe is upon destruction of
	the pool, walk the entire free-list and default-construct
	each location pointer in the free-list.  This clears the contents
	of what is about to be destroyed.  Then when the vector destructor 
	is called, it should be safe to destroy each element.
 */
#define	LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(T,C)			\
T::pool_type T::pool(C);						\
void*									\
T::operator new (size_t s) {						\
	INVARIANT(sizeof(T) == s);					\
	return pool.allocate();						\
}									\
void*									\
T::operator new (size_t s, void*& p) {					\
	INVARIANT(sizeof(T) == s);					\
	NEVER_NULL(p); return p;					\
}									\
void									\
T::operator delete (void* p) {						\
	T* t = reinterpret_cast<T*>(p);					\
	NEVER_NULL(t);							\
	pool.deallocate(t);						\
}

/**
	Convenient macro for explicitly requiring that a memory pool
	be ready during static initialization of a particular module.
	This is not required now... kept in comments for historical reference.
	\param T cannot be a template-id, use a convenient simple typedef.
 */
#define REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(T)			\
static const T::pool_ref_type						\
__pool_ref_ ## T ## __ (T::get_pool());

/**
	These definitions are intended for using a reference-counted
	memory pool, as required by static global initialization ordering.  
	The static reference count object for the global pool will be 
	set by the call to acquire_pool_reference(), which is the only
	public interface to the allocator.  

	\param the name of the type.
	\param C the chunk size.  

	We cannot use a static count_ptr because we cannot guarantee
	that it will be initialized once by acquire_pool_reference()
	across all modules -- the home module may come along later and
	clobber it to NULL, because of object initialization.  
	Thus we must resort to plain-old-data (POD) 
	pool and pool_ref_count, which are guaranteed to be 
	NULL upon initialization.  

	Old obsolete notes on abandoning a reference-count scheme.  
	Fortunately, count_ptr gives us a means of faking reference 
	counts for such situations.  We return corecively constructed
	reference-count pointers with explicit count* arguments.  
	This didn't seem to destroy the last reference to the pool...
 */
#define	LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(T,C)			\
									\
REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(T)				\
									\
T::pool_ref_ref_type							\
T::get_pool(void) {							\
	static pool_type*	pool = new pool_type(C);		\
	STATIC_RC_POOL_REF_INIT;					\
	static size_t*		count = NEW_SIZE_T;			\
	static const size_t zero __ATTRIBUTE_UNUSED__ = (*count = 0);	\
	return pool_ref_ref_type(pool, count);				\
}									\
									\
void*									\
T::operator new (size_t s) {						\
	static pool_type& pool(*get_pool());				\
	LIST_VECTOR_POOL_STACKTRACE("operator new");			\
	INVARIANT(sizeof(T) == s);					\
	return pool.allocate();						\
}									\
									\
void*									\
T::operator new (size_t s, void*& p) {					\
	INVARIANT(sizeof(T) == s);					\
	NEVER_NULL(p); return p;					\
}									\
									\
void									\
T::operator delete (void* p) {						\
	static pool_type& pool(*get_pool());				\
	LIST_VECTOR_POOL_STACKTRACE("operator delete");			\
	T* t = reinterpret_cast<T*>(p); NEVER_NULL(t);			\
	pool.deallocate(t);						\
}


//=============================================================================

namespace util {
//=============================================================================
/**
	Memory-management utility namespace.
 */
namespace memory {
using std::queue;
using std::list;
using std::vector;
using std::ostream;

//=============================================================================
/**
	Default to eager / early destruction.  
	Is safe to use lazy destruction for non-recursive 
	destructors, i.e. object that do not contain pointers
	to other likewise pooled objects.  
	To override the default eager destruction, 
	define a specialization of this (in the util::memory namespace)
	in the module where the pool is instantiated.  
	A macro is provided below.  
	\param T the class type to be pooled.  
 */
template <class T>
struct list_vector_pool_policy {
	typedef	eager_destruction_tag	destruction_policy;
};      // end struct list_vector_pool_policy

//-----------------------------------------------------------------------------
/**
	This macro must appear in the util::memory namespace.  
 */
#define	LIST_VECTOR_POOL_LAZY_DESTRUCTION(T)				\
template <>								\
struct list_vector_pool_policy<T> {					\
	typedef	lazy_destruction_tag	destruction_policy;		\
};	// end struct list_vector_pool_policy

/**
	Helper method for distinguishing destruction policies.
 */
template <class T>
inline
typename list_vector_pool_policy<T>::destruction_policy
list_vector_pool_destruction_policy(void) {
	return typename list_vector_pool_policy<T>::destruction_policy();
}

//=============================================================================
// specialization
template <>
class list_vector_pool<void> {
public:
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef void*		pointer;
	typedef const void*	const_pointer;
	typedef void		value_type;
// no rebind yet...
};

//-----------------------------------------------------------------------------
#if 0
// Partial specialization for when allocated type is just pointer, 
// to prevent pointer recursion, not needed unless free_list_type is fancy
template <class T, bool Threaded>
class list_vector_pool<T*, Threaded> {
public:
	typedef	T*				value_type;
	typedef	size_t				size_type;
	typedef	ptrdiff_t			difference_type;
	typedef	value_type*			pointer;
	typedef	const value_type*		const_pointer;
	typedef	value_type&			reference;
	typedef	const value_type&		const_reference;
private:
	typedef	vector<value_type>		chunk_type;
	typedef	list<chunk_type>		impl_type;
	// plain queue to stop pointer recursion
	typedef	queue<pointer>			free_list_type;
public:

};	// end class list_vector_pool specialization
#endif

//-----------------------------------------------------------------------------
/**
	Simple, efficient memory pool.  
	No alignment or placement.  
	Should implement the std::allocator interface.  
	Only good for allocating one element at a time, 
	so is not general-purpose, not intended for arrays.  
	For a more general pool allocator use the __gnu_cxx::__pool_alloc, 
	which is likely to be the default allocator.  

	For stronger diagnostics.
	IDEA: use a bit-vector map to track which addresses are available.
	IDEA: use a discrete_interval_set to track address ranges.
 */
LIST_VECTOR_POOL_TEMPLATE_SIGNATURE
class list_vector_pool {
public:
	typedef	T				value_type;
	typedef	size_t				size_type;
	typedef	ptrdiff_t			difference_type;
	typedef	T*				pointer;
	typedef	const T*			const_pointer;
	typedef	T&				reference;
	typedef	const T&			const_reference;
private:
	// could propagate thread-sensitivity parameter to the respective
	// STL data-structures' allocators...
	typedef	vector<value_type>		chunk_type;
	typedef	list<chunk_type>		impl_type;
	typedef	pool_thread_lock<Threaded>	lock_type;
#if 1
	// could also try priority_queue... using less<pointer>
	// could try set<pointer> for uniqueness checking
		// set<> maintains begin() and end() with constant-time
	typedef	queue<pointer>			free_list_type;
#else
	// ballsy: use itself as allocator! (template recursion problem?)
	typedef	queue<pointer, deque<pointer, list_vector_pool<pointer> > >
						free_list_type;
#endif

	// probably no need to change...
	const size_t				chunk_size;
	impl_type				pool;
	free_list_type				free_list;
	/**
		Records the peak number of elements allocated.
		Useful for detecting memory leaks.  
	 */
	size_t					peak;

	// mutex shouldn't be static!  should be PER pool
	mutex_type				the_mutex;

public:
	explicit
	list_vector_pool(const size_type C = 16);

	// no copy-constructor

	~list_vector_pool();

	pointer
	allocate(void) __ATTRIBUTE_MALLOC__ ;

	/// allocate n elements without construction, unimplemented
	pointer
	allocate(size_type n, list_vector_pool<void>::const_pointer hint = 0)
		__ATTRIBUTE_MALLOC__ ;

	void
	deallocate(pointer p);

	/// deallocates, unimplemented
	void
	deallocate(pointer p, size_type n);

	void
	construct(pointer p, const T& val);

	void
	destroy(pointer p);

	// other miscellaneous methods
	size_type
	max_size(void) const;

	// rebinding typedef

	/// feedback IO, indented one-tab by default
	ostream&
	status(ostream& o) const;

};	// end class list_vector_pool

//-----------------------------------------------------------------------------
template <class T>
bool
operator == (const list_vector_pool<T>&, const list_vector_pool<T>&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
bool
operator != (const list_vector_pool<T>&, const list_vector_pool<T>&);

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_LIST_VECTOR_POOL_H__

