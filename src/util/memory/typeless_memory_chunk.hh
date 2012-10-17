/**
	\file "util/memory/typeless_memory_chunk.h"
	A finite-size type-less pool, can be used to construct 
	other allocators.  
	$Id: typeless_memory_chunk.hh,v 1.1 2007/02/21 17:00:29 fang Exp $
 */

#ifndef	__UTIL_MEMORY_TYPELESS_MEMORY_CHUNK_H__
#define	__UTIL_MEMORY_TYPELESS_MEMORY_CHUNK_H__

// #include <iosfwd>
#include "util/numeric/inttype_traits.hh"
// #include "util/macros.h"
#include "util/bitset.hh"
#include "util/attributes.h"

/**
	Define to 1 to use std::bitset or util::bitset.
	This removes the power-of-2 constraint.  
	Old-value: 0
	Status: tested
	Goal: ?
 */
#define	TYPELESS_MEMORY_CHUNK_USE_BITSET		1

//=============================================================================
namespace util {
namespace memory {
using std::ostream;

//=============================================================================
#define	TYPELESS_MEMORY_CHUNK_TEMPLATE_SIGNATURE			\
template <size_t S, size_t C>

#define	TYPELESS_MEMORY_CHUNK_CLASS					\
typeless_memory_chunk<S,C>

/**
	Micro memory manager for a fixed size block of allocation.  
	Only applicable to allocating and deallocating one element at time.  
	This class is not responsible for any construction or destruction.  
	See also class fixed_pool_chunk.
	\param S sizeof an element.
	\param C chunk size, must be a power of 2 (unless we use bitset).
 */
template <size_t S, size_t C>
class typeless_memory_chunk {
	typedef	TYPELESS_MEMORY_CHUNK_CLASS	this_type;
protected:
	typedef	char				storage_type[S];
public:
	/**
		Bit mask type signedness matter if we ever shift right.  
		TODO: use std::bitset or util::bitset wrapper.
	 */
#if TYPELESS_MEMORY_CHUNK_USE_BITSET
	typedef	util::bitset<C>
#else
	typedef	typename numeric::uint_of_size<C>::type
#endif
						bit_map_type;
	enum { element_size = S };
	enum { chunk_size = C };
	typedef	const void*			const_iterator;
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
	empty(void) const {
		return !any_bits<bit_map_type>()(this->free_mask);
	}

	/// free means no element is available for allocation
	bool
	full(void) const {
		return all_bits<bit_map_type>()(this->free_mask);
	}

	const_iterator
	begin(void) const {
		return this->elements;
	}

	const_iterator
	end(void) const {
		return &this->elements[C];
	}

	bool
	contains(const void*) const;

protected:		// really only intended for internal use
	void*
	__allocate(void) __ATTRIBUTE_MALLOC__;

	void
	__deallocate(void*);

};	// end class typeless_memory_chunk

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_TYPELESS_MEMORY_CHUNK_H__

