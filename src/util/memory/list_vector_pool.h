/**
	\file "list_vector_pool.h"
	Simple template container-based memory pool.  
	Basically allocates a large chunk at a time.  

	$Id: list_vector_pool.h,v 1.7.4.1.2.1 2005/01/24 19:46:11 fang Exp $
 */

#ifndef	__LIST_VECTOR_POOL_H__
#define	__LIST_VECTOR_POOL_H__

#include "memory/list_vector_pool_fwd.h"
#include "macros.h"

#include <pthread.h>

#include <cassert>
#include <queue>
#include <list>
#include <vector>

#include <iostream>

// because FreeBSD's <pthreads.h> defines initializer as NULL
// and I haven't figured out a workaround yet.
#if defined(__FreeBSD__)
// #warn	"This allocator will not be thread-safe in FreeBSD... yet."
#define	THREADED_ALLOC		0
#else
#define	THREADED_ALLOC		1
#endif

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
#define	VERBOSE_LOCK		0 && VERBOSE_ALLOC


#if DEBUG_LIST_VECTOR_POOL
	#define INVARIANT_ASSERT(foo)	assert(foo)
#else
	#define INVARIANT_ASSERT(foo)	{ }
#endif

// problem: preprocessor definition value not being evaluated correctly?
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
	#include "what.tcc"
#endif

#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
	#include "stacktrace.h"
	#define	LIST_VECTOR_POOL_STACKTRACE(arg)	STACKTRACE(arg)
#else
	#define	LIST_VECTOR_POOL_STACKTRACE(arg)
#endif

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
 */
#define	LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(T,C)			\
T::pool_type T::pool(C);						\
void* T::operator new (size_t s)					\
	{ return pool.allocate(); }					\
inline void* T::operator new (size_t s, void*& p)			\
	{ NEVER_NULL(p); return p; }					\
void T::operator delete (void* p)					\
	{ T* t = reinterpret_cast<T*>(p); NEVER_NULL(t); pool.deallocate(t); }

/**
	Convenient macro for explicitly requiring that a memory pool
	be ready during static initialization of a particular module.
	This is not required now... kept in comments for historical reference.
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
T::pool_ref_type							\
T::get_pool(void) {							\
	static pool_type*	pool = new pool_type(C);		\
	static size_t*		count = new size_t(0);			\
	static const pool_ref_type ret(pool, count);			\
	return ret;							\
}									\
									\
void*									\
T::operator new (size_t s) {						\
	static pool_type& pool(*get_pool());				\
	LIST_VECTOR_POOL_STACKTRACE("operator new");			\
	return pool.allocate();						\
}									\
									\
inline									\
void*									\
T::operator new (size_t s, void*& p) {					\
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
typedef	pthread_mutex_t				mutex_type;
using std::queue;
using std::list;
using std::vector;
#include "using_ostream.h"
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
using util::what;
#endif

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
template <class T, bool Threaded>
class list_vector_pool {
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
private:
	typedef	typename util::what<T>		what_type;
#endif
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
#if THREADED_ALLOC
	static mutex_type			the_mutex;
	friend struct Lock;			// grant access to the_mutex
	struct Lock {
		Lock() {
#if VERBOSE_LOCK
			cerr << "getting lock at " << &the_mutex << "... ";
#endif
			if (Threaded)
				assert(!pthread_mutex_lock(&the_mutex));
#if VERBOSE_LOCK
			cerr << "got it." << endl;
#endif
		}

		~Lock() {
#if VERBOSE_LOCK
			cerr << "releasing lock... ";
#endif
			if (Threaded)
				assert(!pthread_mutex_unlock(&the_mutex));
#if VERBOSE_LOCK
			cerr << "released." << endl;
#endif
		}
	};	// end struct Lock
	// may need __attribute__ ((__unused__))
#endif	// THREADED_ALLOC

private:
	// probably no need to change...
	const size_t				chunk_size;
	impl_type				pool;
	free_list_type				free_list;
	/**
		Records the peak number of elements allocated.
		Useful for detecting memory leaks.  
	 */
	size_t					peak;
public:
	/**
		Reserves one chunk up-front.  
		\param C is number of elements to allocate at a time.
			Should be related to page_size/sizeof(T).  
	 */
	explicit
	list_vector_pool(const size_type C = 16) : 
			chunk_size(C), pool(), free_list(), peak(0) {
		LIST_VECTOR_POOL_STACKTRACE(
			"list_vector_pool::list_vector_pool()");
		assert(chunk_size);
		// worry about alignment, placement and pages sizes later
		pool.push_back(chunk_type());
		pool.back().reserve(chunk_size);
#if VERBOSE_ALLOC
		cerr << "Reserved " << 
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
			what<T>::name() << 
#endif
			" chunk of size " << chunk_size << "*" <<
			sizeof(T) << " starting at " <<
			&pool.back().front() << endl;
#endif
		INVARIANT_ASSERT(pool.back().capacity() == chunk_size);
		INVARIANT_ASSERT(!pool.back().size());
		INVARIANT_ASSERT(free_list.empty());
	}

	// no copy-constructor

	/**
		Default destructor with diagnostics.
		There is a non-fatal memory leak if peak != free_list's size, 
		because all elements that belong to this allocator's region
		should be returned to this free list before the entire
		allocator is released!  Else subsequent memory allocations
		may overwrite the same physical space in memory, 
		corrupting memory reference by the old non-freed pointers!
	 */
	~list_vector_pool() {
		LIST_VECTOR_POOL_STACKTRACE(
			"list_vector_pool::~list_vector_pool()");
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
#endif
#endif
		INVARIANT(free_list.size() <= peak);
		const size_t leak = peak -free_list.size();
		if (leak) {
			cerr << "\t*** YOU MAY HAVE A MEMORY LEAK! ***" << endl;
			cerr << '\t' << leak << ' ' << what<T>::name() <<
				" are unaccounted for." << endl;
		}
	}

	/**
		Allocate one element only, without construction.
		Here is where lazy deletion takes place:
		Any pointer that was returned to the free-list
		by deallocate() (below) was not destroyed;
		there is no need to destroy until it is re-allocated.  
		It MUST be destroyed upon reallocation to guarantee
		that old structures are not leaked!  
	 */
	pointer
	allocate(void) {
		LIST_VECTOR_POOL_STACKTRACE("list_vector_pool::allocate()");
#if THREADED_ALLOC
		// volatile? do not optimize away?
		Lock got_the_mutex;
#endif
		// see if an address is available from the free-list
		if (!free_list.empty()) {
			pointer ret = free_list.front();
			INVARIANT_ASSERT(ret);
			ret->~T();		// Lazy deletion!
			free_list.pop();
#if VERBOSE_ALLOC
			cerr << "Allocated " <<
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
				what<T>::name() << 
#endif
				" from free-list @ " << ret << endl;
#endif
			return ret;
		} else {
			// if not, use the chunk.
			INVARIANT_ASSERT(!pool.empty());
			chunk_type* chunk = &pool.back();
			INVARIANT_ASSERT(chunk->capacity());
			if (chunk->size() == chunk->capacity()) {
				// rare case: allocate chunk
#if VERBOSE_ALLOC
				cerr << "New chunk of " << chunk_size << " " <<
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
					what<T>::name() <<
#endif
					" allocated." << endl;
#endif
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
#if VERBOSE_ALLOC
			cerr << "Allocated " <<
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
				what<T>::name() <<
#endif
				" from pool @ " << ret << endl;
#endif
			peak++;
			return ret;
		}
		// lock will expire at end-of-scope
	}

	/// allocate n elements without construction, unimplemented
	pointer
	allocate(size_type n, list_vector_pool<void>::const_pointer hint = 0);

	/// deallocates without destruction
	// any safety checks for multiple deletions?
	// if so, use set<pointer> for the free_list
	void
	deallocate(pointer p) {
		LIST_VECTOR_POOL_STACKTRACE("list_vector_pool::deallocate()");
#if THREADED_ALLOC
		Lock got_the_mutex;
#endif
		assert(p);
#if VERBOSE_ALLOC
		cerr << "Returned " <<
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
			what<T>::name() <<
#endif
			" @ " << p << " to free-list." << endl;
#endif
		free_list.push(p);
		// lock will expire at end-of-scope
	}

	/// deallocates without destruction, unimplemented
	void
	deallocate(pointer p, size_type n);

	/**
		Initialize *p by val.  
		The new operator is given a placed address, 
		which doesn't actually reallocate.  
		The value is copy-constructed in-place.  
	 */
	void
	construct(pointer p, const T& val) {
		LIST_VECTOR_POOL_STACKTRACE("list_vector_pool::construct()");
		assert(p);
#if VERBOSE_ALLOC
		cerr << "Constructing " <<
#if DEBUG_LIST_VECTOR_POOL_USING_WHAT
			what<T>::name() <<
#endif
			" @ " << p;
		new(p) T(val);
		cerr << " ... constructed." << endl;
#else
		new(p) T(val);
#endif
	}

	/**
		Destroys *p without deallocating.
		QUESTION: should this ever be destroyed, 
		or should the vector take care of it?
		ANSWER: lazy destruction, destroy when it is
		reallocated at a later point and clobbered...
		See allocate(), when free-list pointer is recycled.
	 */
	void
	destroy(pointer p) {
		LIST_VECTOR_POOL_STACKTRACE("list_vector_pool::destroy()");
		assert(p);
#if VERBOSE_ALLOC
		cerr << "Punting destruction for " << p;
#endif
	}

	// other miscellaneous methods
	size_type
	max_size() const;

	// rebinding typedef

	/// feedback IO, indented one-tab by default
	ostream&
	status(ostream& o) const {
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

};	// end class list_vector_pool

//-----------------------------------------------------------------------------
#if THREADED_ALLOC
// static initializer of STL mutex element
template<class T, bool Threaded>
mutex_type
list_vector_pool<T, Threaded>::the_mutex = PTHREAD_MUTEX_INITIALIZER;

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

#endif	//	__LIST_VECTOR_POOL_H__

