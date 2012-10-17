/**
	\file "util/list_vector_pool.tcc"
	Implementation for container-based memory pool.  
	Basically allocates a large chunk at a time.  

	$Id: list_vector_pool.tcc,v 1.7 2006/04/27 00:17:35 fang Exp $
 */

#ifndef	__UTIL_MEMORY_LIST_VECTOR_POOL_TCC__
#define	__UTIL_MEMORY_LIST_VECTOR_POOL_TCC__

#include "util/memory/list_vector_pool.hh"

#ifndef	EXTERN_TEMPLATE_UTIL_MEMORY_LIST_VECTOR_POOL

#include <iostream>

#ifdef	EXCLUDE_DEPENDENT_TEMPLATES_UTIL_MEMORY_LIST_VECTOR_POOL
#define	EXTERN_TEMPLATE_UTIL_MEMORY_DESTRUCTION_POLICY
#endif

#include "util/memory/destruction_policy.tcc"


// turn invariant assertions on or off
#ifndef	DEBUG_LIST_VECTOR_POOL
#define	DEBUG_LIST_VECTOR_POOL	0
#endif

#ifndef	DEBUG_LIST_VECTOR_POOL_USING_WHAT
#define	DEBUG_LIST_VECTOR_POOL_USING_WHAT	1
#endif

// default off
#ifndef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE	0
#endif

// annoying debug messages
#define VERBOSE_ALLOC		1 && DEBUG_LIST_VECTOR_POOL


#if DEBUG_LIST_VECTOR_POOL
	#define INVARIANT_ASSERT(foo)	assert(foo)
#else
	#define INVARIANT_ASSERT(foo)	{ }
#endif

// problem: preprocessor definition value not being evaluated correctly?
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
	#include "util/what.tcc"
#endif

#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
	#include "util/stacktrace.hh"
	#define	LIST_VECTOR_POOL_STACKTRACE(arg)	STACKTRACE(arg)
#else
	#define	LIST_VECTOR_POOL_STACKTRACE(arg)
#endif

namespace util {
namespace memory {
using std::_Construct;
using std::_Destroy;
#include "util/using_ostream.hh"
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
using util::what;
#endif

//-----------------------------------------------------------------------------
/**
	Pool constructor and initializer.
	Reserves one chunk up-front.  
	\param C is number of elements to allocate at a time.
		Should be related to page_size/sizeof(T).  
 */
LIST_VECTOR_POOL_TEMPLATE_SIGNATURE
list_vector_pool<T,Threaded>::list_vector_pool(const size_type C) : 
		chunk_size(C), pool(), free_list(), peak(0), the_mutex() {
	LIST_VECTOR_POOL_STACKTRACE(
		"list_vector_pool()");
	static const mutex_type default_mutex_init = PTHREAD_MUTEX_INITIALIZER;
	the_mutex = default_mutex_init;
	assert(chunk_size);
	// worry about alignment, placement and pages sizes later
	pool.push_back(chunk_type());
	pool.back().reserve(chunk_size);
#if VERBOSE_ALLOC
	cerr << "Reserved " << 
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
		what<T>::name() << 
#endif
		" chunk of size " << chunk_size << "*" << sizeof(T) <<
		" starting at " << &pool.back().front() << endl;
#endif
	INVARIANT_ASSERT(pool.back().capacity() == chunk_size);
	INVARIANT_ASSERT(!pool.back().size());
	INVARIANT_ASSERT(free_list.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor with diagnostics.
	There is a non-fatal memory leak if peak != free_list's size, 
	because all elements that belong to this allocator's region
	should be returned to this free list before the entire
	allocator is released!  Else subsequent memory allocations
	may overwrite the same physical space in memory, 
	corrupting memory reference by the old non-freed pointers!
 */
LIST_VECTOR_POOL_TEMPLATE_SIGNATURE
list_vector_pool<T,Threaded>::~list_vector_pool() {
	LIST_VECTOR_POOL_STACKTRACE("~list_vector_pool()");
#if VERBOSE_ALLOC
	status(cerr << "~list_vector_pool<" <<
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
		what<T>::name() <<
#endif
		">() at " << this << endl);
#if VERBOSE_ALLOC && 0
	// for debugging deallocation path only
	typename impl_type::iterator i = pool.begin();
	const typename impl_type::iterator e = pool.end();
	cerr << "Clearing chunk-list: " << endl;
	for ( ; i!=e; i++) {
		i->clear();
		cerr << "\tcleared chunk." << endl;
	}
	pool.clear();
	cerr << "... cleared chunk-list." << endl;
	cerr << "Clearing free-list... ";
	while(!free_list.empty())
		free_list.pop();
	cerr << "...cleared." << endl;
#endif	// VERBOSE_ALLOC && 0
#endif	// VERBOSE_ALLOC
	INVARIANT(free_list.size() <= peak);
	const size_t leak = peak -free_list.size();
	if (leak) {
		cerr << "\t*** YOU MAY HAVE A MEMORY LEAK! ***" << endl;
		cerr << '\t' << leak << ' ' << what<T>::name() <<
			" are unaccounted for." << endl;
	}
	while (!free_list.empty()) {
		// to prevent double-destruction!
		// if a delete operator already implicitly called dtors!
		const pointer destroy_me = free_list.front();
		lazy_construct(destroy_me,
			list_vector_pool_destruction_policy<T>());
		free_list.pop();	// is queue::pop_front()
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// some handy local macros for verbose debugging

#if VERBOSE_ALLOC
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
	#define	ALLOCATED_FREE_LIST_MESSAGE				\
		cerr << "Allocated " << what<T>::name() << 		\
			" from free-list @ " << ret << endl
	#define	ALLOCATED_FROM_POOL_MESSAGE				\
		cerr << "Allocated " << what<T>::name() <<		\
			" from pool @ " << ret << endl;
	#define NEW_CHUNK_MESSAGE					\
		cerr << "New chunk of " << chunk_size << " " <<		\
			what<T>::name() << " allocated." << endl;
	#define	RETURN_TO_FREE_LIST_MESSAGE				\
		cerr << "Returned " << what<T>::name() <<		\
			" @ " << p << " to free-list." << endl;
#else	// USING_WHAT
	#define	ALLOCATED_FREE_LIST_MESSAGE				\
		cerr << "Allocated from free-list @ " << ret << endl
	#define	ALLOCATED_FROM_POOL_MESSAGE				\
		cerr << "Allocated from pool @ " << ret << endl;
	#define	NEW_CHUNK_MESSAGE					\
		cerr << "New chunk of " << chunk_size << " allocated." << endl
	#define	RETURN_TO_FREE_LIST_MESSAGE				\
		cerr << "Returned @ " << p << " to free-list." << endl;
#endif	// USING_WHAT
#else	// VERBOSE_ALLOC
	#define	ALLOCATED_FREE_LIST_MESSAGE
	#define	ALLOCATED_FROM_POOL_MESSAGE
	#define	NEW_CHUNK_MESSAGE
	#define	RETURN_TO_FREE_LIST_MESSAGE
#endif	// VERBOSE_ALLOC

/**
	Allocate one element only, without construction.
	Here is where lazy deletion takes place:
	Any pointer that was returned to the free-list
	by deallocate() (below) was not destroyed;
	there is no need to destroy until it is re-allocated.  
	It MUST be destroyed upon reallocation to guarantee
	that old structures are not leaked!  
 */
LIST_VECTOR_POOL_TEMPLATE_SIGNATURE
typename list_vector_pool<T,Threaded>::pointer
list_vector_pool<T,Threaded>::allocate(void) {
	LIST_VECTOR_POOL_STACKTRACE("list_vector_pool::allocate()");
	// volatile? do not optimize away?
	lock_type got_the_mutex(&the_mutex);
	// see if an address is available from the free-list
	if (!free_list.empty()) {
		pointer ret = free_list.front();
		INVARIANT_ASSERT(ret);
#if 0
		lazy_destroy(ret, list_vector_pool_destruction_policy<T>());
#endif
		free_list.pop();
		ALLOCATED_FREE_LIST_MESSAGE;
		return ret;
	} else {
		// if not, use the chunk.
		INVARIANT_ASSERT(!pool.empty());
		chunk_type* chunk = &pool.back();
		INVARIANT_ASSERT(chunk->capacity());
		if (chunk->size() == chunk->capacity()) {
			// rare case: allocate chunk
			NEW_CHUNK_MESSAGE;
			pool.push_back(chunk_type());
			chunk = &pool.back();
			chunk->reserve(chunk_size);
			INVARIANT_ASSERT(!chunk->size());
		}
		INVARIANT_ASSERT(chunk->capacity() == chunk_size);
		chunk->push_back(T());
		// T must be copy-constructible
		pointer ret = &chunk->back();
		INVARIANT_ASSERT(ret);
		ALLOCATED_FROM_POOL_MESSAGE;
		peak++;
		return ret;
	}
	// lock will expire at end-of-scope
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	allocate n elements without construction, unimplemented
 */
LIST_VECTOR_POOL_TEMPLATE_SIGNATURE
typename list_vector_pool<T,Threaded>::pointer
list_vector_pool<T,Threaded>::allocate(size_type n, 
	list_vector_pool<void>::const_pointer hint) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Deallocates, returning memory to the freelist.  
	Destruction action depends on the policy for the allocated type.  
	TODO: optional safety checks for multiple deletions?
		if so, use set<pointer> for the free_list
 */
LIST_VECTOR_POOL_TEMPLATE_SIGNATURE
void
list_vector_pool<T,Threaded>::deallocate(pointer p) {
	LIST_VECTOR_POOL_STACKTRACE("list_vector_pool::deallocate()");
	lock_type got_the_mutex(&the_mutex);
	assert(p);
	RETURN_TO_FREE_LIST_MESSAGE;
#if 0
	eager_destroy(p, list_vector_pool_destruction_policy<T>());
#endif
	free_list.push(p);
	// lock will expire at end-of-scope
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Bulk deallocation, unimplemented.  
 */
LIST_VECTOR_POOL_TEMPLATE_SIGNATURE
void
list_vector_pool<T,Threaded>::deallocate(pointer p, size_type n) {
}
#endif

#undef	ALLOCATED_FREE_LIST_MESSAGE
#undef	ALLOCATED_FROM_POOL_MESSAGE
#undef	NEW_CHUNK_MESSAGE
#undef	RETURN_TO_FREE_LIST_MESSAGE

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initialize *p by val.  
	The new operator is given a placed address, 
	which doesn't actually reallocate.  
	The value is copy-constructed in-place.  
 */
LIST_VECTOR_POOL_TEMPLATE_SIGNATURE
void
list_vector_pool<T,Threaded>::construct(pointer p, const T& val) {
	LIST_VECTOR_POOL_STACKTRACE("list_vector_pool::construct()");
	assert(p);
#if VERBOSE_ALLOC
	cerr << "Constructing " <<
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
		what<T>::name() <<
#endif
		" @ " << p;
#endif	// VERBOSE_ALLOC
//	lazy_destroy(p, list_vector_pool_destruction_policy<T>());
	_Construct(p, val);	// new(p) T(val);
#if VERBOSE_ALLOC
	cerr << " ... constructed." << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Destroys *p without deallocating.
	QUESTION: should this ever be destroyed, 
	or should the vector take care of it?
	ANSWER: lazy destruction, destroy when it is
	reallocated at a later point and clobbered...
	See allocate(), when free-list pointer is recycled.
 */
LIST_VECTOR_POOL_TEMPLATE_SIGNATURE
void
list_vector_pool<T,Threaded>::destroy(pointer p) {
	LIST_VECTOR_POOL_STACKTRACE("list_vector_pool::destroy()");
	assert(p);
#if VERBOSE_ALLOC
	cerr << "Punting destruction for " << p;
#endif
#if 0
	// DO NOT DESTROY only, the vector will bulk destroy
	_Destroy(p);
	_Construct(p);		// must reconstruct to prevent double_deletion
#elif 0
	eager_destroy(p, list_vector_pool_destruction_policy<T>());
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
LIST_VECTOR_POOL_TEMPLATE_SIGNATURE
typename list_vector_pool<T,Threaded>::size_type
list_vector_pool<T,Threaded>::max_size() const;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Feedback IO, indented one-tab by default.
 */
LIST_VECTOR_POOL_TEMPLATE_SIGNATURE
ostream&
list_vector_pool<T,Threaded>::status(ostream& o) const {
	LIST_VECTOR_POOL_STACKTRACE("list_vector_pool::status()");
	o << '\t' << pool.size() << " chunks of " << chunk_size <<
		"*" << sizeof(T) << " " <<
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
		what<T>::name() <<
#endif
		" allocated." << endl;
	o << '\t' << "Peak usage: " << peak << " elements, " <<
		"free-list has " << free_list.size() << 
		" remaining." << endl;
	return o;
}

//-----------------------------------------------------------------------------
#if 0
#if THREADED_ALLOC
// static initializer of STL mutex element
LIST_VECTOR_POOL_TEMPLATE_SIGNATURE
mutex_type
list_vector_pool<T, Threaded>::the_mutex = PTHREAD_MUTEX_INITIALIZER;

#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

//=============================================================================
}	// end namespace memory
}	// end namespace util

#undef	INVARIANT_ASSERT

#endif	// EXTERN_TEMPLATE_UTIL_MEMORY_LIST_VECTOR_POOL
#endif	//	__UTIL_MEMORY_LIST_VECTOR_POOL_TCC__

