/**
	\file "util/memory/chunk_map_pool.h"
	Class definition for chunk-allocated mapped memory pool template.  
	$Id: chunk_map_pool.h,v 1.4.4.1 2005/05/17 21:48:44 fang Exp $
 */

#ifndef	__UTIL_MEMORY_CHUNK_MAP_POOL_H__
#define	__UTIL_MEMORY_CHUNK_MAP_POOL_H__

#include <iosfwd>
#include <set>
#include <list>
#include <map>
#include "util/memory/chunk_map_pool_fwd.h"

#define	CHUNK_MAP_POOL_CHUNK_CLASS					\
chunk_map_pool_chunk<T,C>

#define	CHUNK_MAP_POOL_CLASS						\
chunk_map_pool<T,C,Threaded>

//=============================================================================
/**
	Normal definition of new and delete using chunk_map_pool as 
	the underlying allocator for a class.  
	Note: if T is a template, then will need typename keyword... arg.
 */
#define	CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(T)			\
T::pool_type T::pool;							\
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Macro for explicitly requiring orderly memory pool static 
	initialization.  
	\param T any type that is not a template-id, damn preprocessor.
 */
#define	REQUIRES_CHUNK_MAP_POOL_STATIC_INIT(T)				\
static const T::pool_ref_type						\
__pool_ref_ ## T ## __ (T::get_pool());


/**
	Initialization-ordering-safe version fo chunk_map_pool-enabled
	overloads of new and delete.  
 */
#define	CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(T)			\
REQUIRES_CHUNK_MAP_POOL_STATIC_INIT(T)					\
									\
T::pool_ref_ref_type							\
T::get_pool(void) {							\
	static pool_type*	pool = new pool_type();			\
	STATIC_RC_POOL_REF_INIT;					\
	static size_t*		count = NEW_SIZE_T;			\
	static const size_t	zero = (*count = 0);			\
	return pool_ref_ref_type(pool, count);				\
}									\
void*									\
T::operator new (size_t s) {						\
	static pool_type& pool(*get_pool());				\
	INVARIANT(sizeof(T) == s);					\
	return pool.allocate();						\
}	/* implicitly calls default ctor thereafter */			\
void*									\
T::operator new (size_t s, void*& p) {					\
	INVARIANT(sizeof(T) == s);					\
	NEVER_NULL(p);							\
	return p;							\
}	/* automatically calls copy-ctor thereafter */			\
void									\
T::operator delete (void* p) {						\
	static pool_type& pool(*get_pool());				\
	T* t = reinterpret_cast<T*>(p);					\
	NEVER_NULL(t);							\
	pool.deallocate(t);						\
}	/* implicitly calls dtor thereafter */

//=============================================================================
namespace util {
namespace memory {
using std::ostream;

/**
	Chunk size must be some power-of-2.
	Currently, we only provide specializations for 8, 16, 32.  
 */
template <size_t C>
struct chunk_size_traits;

template <>
struct chunk_size_traits<8> {
	enum { size = 8 };
	typedef	unsigned char		bit_map_type;
	// concept_check sizeof(bit_map_type)/size == 1
	typedef	size_t			print_type;
};

template <>
struct chunk_size_traits<16> {
	enum { size = 16 };
	typedef	unsigned short		bit_map_type;
	typedef	size_t			print_type;
};

template <>
struct chunk_size_traits<32> {
	enum { size = 32 };
	typedef	unsigned int		bit_map_type;
	typedef	size_t			print_type;
};

#if 0
// will be compiler/architecture -specific
template <>
struct chunk_size_traits<64> {
	typedef	unsigned long long	bit_map_type;
};
#endif

//=============================================================================
#define	TYPELESS_MEMORY_CHUNK_TEMPLATE_SIGNATURE			\
template <size_t S, size_t C>

#define	TYPELESS_MEMORY_CHUNK_CLASS					\
typeless_memory_chunk<S,C>

/**
	Micro memory manager for a fixed size block of allocation.  
	Only applicable to allocating and deallocating one element at time.  
	This class is not responsible for any construction or destruction.  
	\param S sizeof an element.
	\param C chunk size, must be a power of 2.
 */
template <size_t S, size_t C>
class typeless_memory_chunk {
	typedef	TYPELESS_MEMORY_CHUNK_CLASS	this_type;
protected:
	typedef	char				storage_type[S];
public:
	typedef	typename chunk_size_traits<C>::bit_map_type
						bit_map_type;
	enum { element_size = S };
	enum { chunk_size = C };
protected:
	/// chunk of memory, as plain old data
	storage_type				elements[C];
	/**
		The free-mask is used to determine which elements 
		are available for allocation, and which are live
		(already allocated out).
		0 means free, 1 means allocated.
	 */
	bit_map_type				free_mask;

protected:
	/**
		Default constructor, leaves elements uninitialized.
	 */
	typeless_memory_chunk() : free_mask(0) { }

public:
	/**
		Non-default copy-constructor, nothing is actually copied.  
	 */
	typeless_memory_chunk(const this_type& ) : free_mask(0) { }

	/**
		No safety checks in this destructor.  
	 */
	~typeless_memory_chunk() { }

	/// empty means every element is available for allocation
	bool
	empty(void) const { return !this->free_mask; }

	/// free means no element is available for allocation
	bool
	full(void) const { return !bit_map_type(this->free_mask +1); }

	const void*
	start_address(void) const {
		return this->elements;
	}

	const void*
	past_end_address(void) const {
		return &this->elements[C];
	}

	bool
	contains(void*) const;

protected:		// really only intended for internal use
	void*
	__allocate(void);

	void
	__deallocate(void*);

};	// end class typeless_memory_chunk

//=============================================================================
/**
	Fixed size type-specific chunk.
	Defined as a struct with public members, but the allocator
	shouldn't ever leak out a reference to this type.  
	Even this alone may be useful for local allocation.  
	Consider making a thread-locked variation...
	currently, this is thread-blind.
 */
CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE
class chunk_map_pool_chunk : public typeless_memory_chunk<sizeof(T), C> {
	typedef	CHUNK_MAP_POOL_CHUNK_CLASS	this_type;
protected:
	typedef	typeless_memory_chunk<sizeof(T), C>	parent_type;
public:
	typedef	typename parent_type::bit_map_type	bit_map_type;
	typedef	T				value_type;
	typedef	T*				pointer;
public:
	chunk_map_pool_chunk();

	~chunk_map_pool_chunk();

	bool
	contains(pointer) const;

	/// wrap around parent's
	pointer
	allocate(void);

	void
	deallocate(pointer);

	ostream&
	status(ostream&) const;

};	// end class chunk_map_pool_chunk

//=============================================================================
// specialization
template <size_t C, bool Threaded>
class chunk_map_pool<void, C, Threaded> {
public:
	typedef	size_t		size_type;
	typedef	ptrdiff_t	difference_type;
	typedef	void*		pointer;
	typedef	const void*	const_pointer;
	typedef	void		value_type;
// no rebind yet...
};

//=============================================================================
/**
	Chunky allocator.  
	This allocator keeps tracks of memory in a map of chunks.  
	Allocation is performed one chunk at a time.  
	Each chunk maintains a free_mask.  
	When a chunk becomes "empty" its meory is eagerly reclaimed.  

	Tradeoff:
	The larger the chunk the fewer calls to the underlying
	allocator methods (allocate, deallocate), but the coarse-grain
	the memory fragmentation.  
 */
CHUNK_MAP_POOL_TEMPLATE_SIGNATURE
class chunk_map_pool {
	typedef	CHUNK_MAP_POOL_CLASS			this_type;
public:
	typedef	T					value_type;
	typedef	size_t					size_type;
	typedef	ptrdiff_t				difference_type;
	typedef	T*					pointer;
	typedef	const T*				const_pointer;
	typedef	T&					reference;
	typedef	const T&				const_reference;
protected:
	// or use typeless_memory_chunk?
	typedef	chunk_map_pool_chunk<T,C>		chunk_type;
	// possibly be able to pass in underlying allocator to chunk_set
	typedef	std::list<chunk_type>			chunk_set_type;
	typedef	typename chunk_set_type::iterator	chunk_set_iterator;
	typedef	std::map<const void*, chunk_set_iterator>
							alloc_map_type;
	typedef	typename alloc_map_type::iterator	alloc_map_iterator;
	typedef	typename alloc_map_type::const_iterator
						const_alloc_map_iterator;
	typedef	std::set<chunk_type*>			avail_set_type;
	typedef	typename avail_set_type::iterator	avail_set_iterator;
protected:
	/**
		The home of the memory chunks.  
	 */
	chunk_set_type					chunk_set;
	/**
		The map to all allocated chunks, indexed by address of value.  
	 */
	alloc_map_type					chunk_map;
	/**
		Set of chunks with any free entries.  
	 */
	avail_set_type					avail_set;
public:
	chunk_map_pool();

	~chunk_map_pool();

	bool
	contains(pointer p) const;

	pointer
	allocate(void);

#if 0
	// unimplemented
	pointer
	allocate(size_type n,
		chunk_map_pool<void,C,Threaded>::const_pointer hint = 0);
#endif

	void
	deallocate(pointer);

#if 0
	// unimplemented
	void
	deallocate(pointer p, size_type n);
#endif

	void
	construct(pointer, const_reference);

	void
	destroy(pointer);

	size_type
	max_size(void) const;

	ostream&
	status(ostream& o) const;

};	// end class chunk_map_pool

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_CHUNK_MAP_POOL_H__

