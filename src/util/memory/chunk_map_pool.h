/**
	\file "util/memory/chunk_map_pool.h"
	Class definition for chunk-allocated mapped memory pool template.  
	$Id: chunk_map_pool.h,v 1.11 2006/04/27 00:17:27 fang Exp $
 */

#ifndef	__UTIL_MEMORY_CHUNK_MAP_POOL_H__
#define	__UTIL_MEMORY_CHUNK_MAP_POOL_H__

#include <iosfwd>
#include <set>
#include <list>
#include <map>
#include "util/memory/chunk_map_pool_fwd.h"
#include "util/numeric/inttype_traits.h"
#include "util/bitset.h"
#include "util/attributes.h"
#include "util/cppcat.h"

#define	CHUNK_MAP_POOL_CHUNK_CLASS					\
chunk_map_pool_chunk<T,C>

#define	CHUNK_MAP_POOL_CLASS						\
chunk_map_pool<T,C,Threaded>

//=============================================================================
/**
	General template macro for defining the new operator 
	for a (default) chunk_map_pool-allocated class.  
	\param TemplSpec optional template specification.
	\param _typename the typename keyword if needed.
	\param T the name of the pool-allocated class. 
	NOTE: T may be a simple template-id without commas, due to
		proprocessor limitations.  
 */
#define	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_NEW(TemplSpec, _typename, T)	\
_typename T::pool_type T::pool;						\
TemplSpec								\
void*									\
T::operator new (size_t s) {						\
	INVARIANT(sizeof(T) == s);					\
	return pool.allocate();						\
}									\

/**
	Template macro for defining the placement new operator
	for a (default) chunk_map_pool-allocated class.  
 */
#define	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_PLACEMENT_NEW(TemplSpec, T)	\
TemplSpec								\
void*									\
T::operator new (size_t s, void* p) {					\
	INVARIANT(sizeof(T) == s);					\
	NEVER_NULL(p); return p;					\
}									\

/**
	Template macro for defining the delete operator
	for a (default) chunk_map_pool-allocated class.  
 */
#define	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_DELETE(TemplSpec, T)		\
TemplSpec								\
void									\
T::operator delete (void* p) {						\
	T* t = reinterpret_cast<T*>(p);					\
	NEVER_NULL(t);							\
	pool.deallocate(t);						\
}

/**
	Template macro for defining class member methods for 
	chunk_map_pool-allocating that class.  
 */
#define	__CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(TemplSpec, _typename, T) \
	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_NEW(TemplSpec, _typename, T)	\
	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_PLACEMENT_NEW(TemplSpec, T)	\
	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_DELETE(TemplSpec, T)

/**
	Use this macro to conveniently define all the necessary
	class member functions for chunk_map_pool-allocating.  
	Normal definition of new and delete using chunk_map_pool as 
	the underlying allocator for a class.  
 */
#define	CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(T)			\
	__CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION( , , T)

/**
	Template version of the class member definition macro. 
 */
#define	TEMPLATE_CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(TemplSpec, T)	\
	__CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(TemplSpec, typename, T)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Macro for explicitly requiring orderly memory pool static 
	initialization.  
	\param T any type that is not a template-id, damn preprocessor.
	Workaround: use preprocessor definition as substitute.  
 */
#define	__REQUIRES_CHUNK_MAP_POOL_STATIC_INIT(TemplSpec, _typename, T)	\
static const _typename T::pool_ref_type					\
UNIQUIFY(__pool_ref__) (T::get_pool());

/**
	This is the class-static initializer.  
 */
#define	__SELF_CHUNK_MAP_POOL_STATIC_INIT(TemplSpec, _typename, T)	\
TemplSpec								\
const _typename T::pool_ref_type					\
T::__robust_init__ (T::get_pool());

/**
	Declares and initializes an external anchor for properly
	initializing a dependent pool.  
	Use this macro for non-template types.  
 */
#define	REQUIRES_CHUNK_MAP_POOL_STATIC_INIT(T)				\
	__REQUIRES_CHUNK_MAP_POOL_STATIC_INIT( , , T)			

/**
	Declares and initializes an external anchor for properly
	initializing a dependent pool.  
	Use this macro for template types.  
 */
#define	TEMPLATE_REQUIRES_CHUNK_MAP_POOL_STATIC_INIT(TemplSpec, T)	\
	__REQUIRES_CHUNK_MAP_POOL_STATIC_INIT(TemplSpec, typename, T)

/**
	Template macro for defining class static method get_pool().
 */
#define	__CHUNK_MAP_POOL_ROBUST_STATIC_GET_POOL(TemplSpec, _typename, T) \
TemplSpec								\
_typename T::pool_ref_ref_type						\
T::get_pool(void) {							\
	static pool_type*	pool = new pool_type();			\
	STATIC_RC_POOL_REF_INIT;					\
	static size_t*		count = NEW_SIZE_T;			\
	static const size_t zero __ATTRIBUTE_UNUSED__ = (*count = 0);	\
	return pool_ref_ref_type(pool, count);				\
}									\

/**
	Template macro for defining class operator new, 
	for chunk_map_pool allocation.  
 */
#define	__CHUNK_MAP_POOL_ROBUST_OPERATOR_NEW(TemplSpec, T)		\
TemplSpec								\
void*									\
T::operator new (size_t s) {						\
	static pool_type& pool(*get_pool());				\
	INVARIANT(sizeof(T) == s);					\
	return pool.allocate();						\
}	/* implicitly calls default ctor thereafter */			\

/**
	Template macro for defining class operator placement new, 
	for chunk_map_pool allocation.  
 */
#define	__CHUNK_MAP_POOL_ROBUST_OPERATOR_PLACEMENT_NEW(TemplSpec, T)	\
TemplSpec								\
void*									\
T::operator new (size_t s, void* p) {					\
	INVARIANT(sizeof(T) == s);					\
	NEVER_NULL(p);							\
	return p;							\
}	/* automatically calls copy-ctor thereafter */			\

/**
	Template macro for defining class operator delete, 
	for chunk_map_pool allocation.  
 */
#define	__CHUNK_MAP_POOL_ROBUST_OPERATOR_DELETE(TemplSpec, T)		\
TemplSpec								\
void									\
T::operator delete (void* p) {						\
	static pool_type& pool(*get_pool());				\
	T* t = reinterpret_cast<T*>(p);					\
	NEVER_NULL(t);							\
	pool.deallocate(t);						\
}	/* implicitly calls dtor thereafter */

/**
	Bundled template macro for declaring all necessary member functions
	for (robust) chunk_map_pool allocation.  
 */
#define	__CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(TemplSpec, _typename, T) \
	__CHUNK_MAP_POOL_ROBUST_STATIC_GET_POOL(TemplSpec, _typename, T) \
	__CHUNK_MAP_POOL_ROBUST_OPERATOR_NEW(TemplSpec, T)		\
	__CHUNK_MAP_POOL_ROBUST_OPERATOR_PLACEMENT_NEW(TemplSpec, T)	\
	__CHUNK_MAP_POOL_ROBUST_OPERATOR_DELETE(TemplSpec, T)

/**
	Initialization-ordering-safe version fo chunk_map_pool-enabled
	overloads of new and delete.  
 */
#define	CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(T)			\
	__SELF_CHUNK_MAP_POOL_STATIC_INIT( , , T)			\
	__CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION( , , T)

/**
	Template version of same definitions.  
 */
#define	TEMPLATE_CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(TemplSpec, T)	\
	__SELF_CHUNK_MAP_POOL_STATIC_INIT(TemplSpec, typename, T)	\
	__CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(TemplSpec, typename, T)


//=============================================================================
namespace util {
namespace memory {
using std::ostream;

#if 0
// OBSOLETE
/**
	Chunk size must be some power-of-2.
	Currently, we only provide specializations for 8, 16, 32.  
 */
template <size_t C>
struct chunk_size_traits;

template <>
struct chunk_size_traits<8> {
//	enum { size = 8 };
//	typedef	uint8			bit_map_type;
	// concept_check sizeof(bit_map_type)/size == 1
	typedef	size_t			print_type;
};

template <>
struct chunk_size_traits<16> {
//	enum { size = 16 };
//	typedef	uint16			bit_map_type;
	typedef	size_t			print_type;
};

template <>
struct chunk_size_traits<32> {
//	enum { size = 32 };
//	typedef	uint32			bit_map_type;
	typedef	size_t			print_type;
};

// will be compiler/architecture -specific
template <>
struct chunk_size_traits<64> {
//	enum { size = 64 };
//	typedef	uint64			bit_map_type;
#if	defined(HAVE_UINT64_TYPE)
	typedef	uint64			print_type;
#elif	defined(HAVE_INT64_TYPE)
	// signed type will also do
	typedef	int64			print_type;
#else
#error	"Missing 64b type, need some help."
#endif
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
	/**
		Bit mask type signedness matter if we ever shift right.  
	 */
#if 0
	typedef	typename chunk_size_traits<C>::bit_map_type
#else
	typedef	typename numeric::uint_of_size<C>::type
#endif
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
		The interface should follow that of std::bitset();
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
	empty(void) const
#if 0
		{ return !this->free_mask; }
#else
		{ return !any_bits<bit_map_type>()(this->free_mask); }
#endif

	/// free means no element is available for allocation
	bool
	full(void) const
#if 0
		{ return !bit_map_type(this->free_mask +1); }
//		{ return this->free_mask == bit_map_type(-1); }
#else
		{ return all_bits<bit_map_type>()(this->free_mask); }
#endif

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
	__allocate(void) __ATTRIBUTE_MALLOC__;

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
	allocate(void) __ATTRIBUTE_MALLOC__ ;

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
	/**
		Rebind allocator to another type.  
	 */
	template <class S>
	struct rebind {	typedef	chunk_map_pool<S,C>	other; };
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
	allocate(void) __ATTRIBUTE_MALLOC__ ;

#if 0
	// unimplemented
	pointer
	allocate(size_type n,
		chunk_map_pool<void,C,Threaded>::const_pointer hint = 0)
		__ATTRIBUTE_MALLOC__ ;
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

