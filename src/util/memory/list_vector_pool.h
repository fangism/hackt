/**
	\file "list_vector_pool.h"
	Simple template container-based memory pool.  
	Basically allocates a large chunk at a time.  

	$Id: list_vector_pool.h,v 1.4 2005/01/12 03:20:02 fang Exp $
 */

#ifndef	__LIST_VECTOR_POOL_H__
#define	__LIST_VECTOR_POOL_H__

#include "memory/list_vector_pool_fwd.h"

#include <pthread.h>

#include <assert.h>
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

#ifndef	DEBUG_USING_WHAT
#define	DEBUG_USING_WHAT	1
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
#if DEBUG_USING_WHAT
	#include "what.tcc"
#endif

namespace util {
//=============================================================================
/**
	Memory-management utility namespace.
	Should implement the std::allocator interface.  
 */
namespace memory {
typedef	pthread_mutex_t				mutex_type;
using std::queue;
using std::list;
using std::vector;
using std::ostream;
using std::endl;
#if DEBUG_USING_WHAT
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
	Only good for allocating one element at a time, 
	so is not general purpose.  
	For a more general pool allocator use the __gnu_cxx::__pool_alloc, 
	which is likely to be the default allocator.  
 */
template <class T, bool Threaded>
class list_vector_pool {
#if DEBUG_USING_WHAT
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
public:
	/**
		Reserves one chunk up-front.  
		\param C is number of elements to allocate at a time.
			Should be related to page_size/sizeof(T).  
	 */
	explicit
	list_vector_pool(const size_type C = 16) : 
			chunk_size(C), pool(), free_list() {
		assert(chunk_size);
		// worry about alignment, placement and pages sizes later
		pool.push_back(chunk_type());
		pool.back().reserve(chunk_size);
#if VERBOSE_ALLOC
		// doesn't like what<T>::name, program terminates normally!?
		cerr << "Reserved " << 
#if DEBUG_USING_WHAT
			what<T>::name << 
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

	// default destructor suffices
	~list_vector_pool() {
#if VERBOSE_ALLOC
		status(cerr << "~list_vector_pool<" <<
#if DEBUG_USING_WHAT
			what<T>::name <<
#endif
			">()" << endl);
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
#if DEBUG_USING_WHAT
				what<T>::name << 
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
#if DEBUG_USING_WHAT
					what<T>::name <<
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
#if DEBUG_USING_WHAT
				what<T>::name <<
#endif
				" from pool @ " << ret << endl;
#endif
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
#if THREADED_ALLOC
		Lock got_the_mutex;
#endif
		assert(p);
#if VERBOSE_ALLOC
		cerr << "Returned " <<
#if DEBUG_USING_WHAT
			what<T>::name <<
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
		assert(p);
#if VERBOSE_ALLOC
		cerr << "Constructing " <<
#if DEBUG_USING_WHAT
			what<T>::name <<
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
		assert(p);
#if VERBOSE_ALLOC
		cerr << "Punting destruction for " << p;
#endif
	}

	// other miscellaneous methods
	size_type max_size() const;

	// rebinding typedef

	// feedback IO
	ostream&
	status(ostream& o) const {
		o << pool.size() << " chunks of " << chunk_size << "*" <<
			sizeof(T) << " " <<
#if DEBUG_USING_WHAT
			what<T>::name <<
#endif
			" have been allocated." << endl;
		o << "Free-list has " << free_list.size() << 
			" entries available." << endl;
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

