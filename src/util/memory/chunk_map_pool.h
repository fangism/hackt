/**
	\file "memory/chunk_map_pool.h"
	Class definition for chunk-allocated mapped memory pool template.  
	$Id: chunk_map_pool.h,v 1.1.2.1 2005/02/27 21:57:08 fang Exp $
 */

#ifndef	__UTIL_MEMORY_CHUNK_MAP_POOL_H__
#define	__UTIL_MEMORY_CHUNK_MAP_POOL_H__

#include <iosfwd>
#include <set>
#include <map>
#include "memory/chunk_map_pool_fwd.h"

#define	CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE				\
template <class T, size_t C>

#define	CHUNK_MAP_POOL_CHUNK_CLASS					\
chunk_map_pool_chunk<T,C>


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
};

template <>
struct chunk_size_traits<16> {
	enum { size = 16 };
	typedef	unsigned short		bit_map_type;
};

template <>
struct chunk_size_traits<32> {
	enum { size = 32 };
	typedef	unsigned int		bit_map_type;
};

#if 0
// will be compiler/architecture -specific
template <>
struct chunk_size_traits<64> {
	typedef	unsigned long long	bit_map_type;
};
#endif

//=============================================================================
/**
	Fixed size type-specific chunk.
	Defined as a struct with public members, but the allocator
	shouldn't ever leak out a reference to this type.  

	Consider making a thread-locked variation...
 */
CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE
class chunk_map_pool_chunk {
public:
	typedef	typename chunk_size_traits<C>::bit_map_type
						bit_map_type;
	typedef	T				value_type;
	enum { size = C };
private:
	value_type	elements[C];
	/**
		The free-mask is used to determine which elements 
		are available for allocation, and which are live
		(already allocated out).
	 */
	bit_map_type	free_mask;
public:
	chunk_map_pool_chunk();
	~chunk_map_pool_chunk();

	bool
	empty(void) const { return !free_mask; }

	bool
	full(void) const { return !~free_mask; }


};	// end class chunk_map_pool_chunk

//=============================================================================
// specialization
template <size_t C, bool Threaded = true>
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
	typedef	chunk_map_pool_chunk<T,C>		chunk_type;
	// may need a maplikeset_element! but not available in this branch yet!
	typedef	std::map<void*, chunk_type>		alloc_map_type;
	typedef	std::set<chunk_type*>			avail_set_type;
protected:
	/**
		The map of all allocated chunks.  
	 */
	alloc_map_type					chunk_map;
	/**
		Set of chunks with any free entries.  
	 */
	avail_set_type					avail_set;
public:
	chunk_map_pool();
	~chunk_map_pool();

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

