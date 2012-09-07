/**
	\file "util/memory/typeless_memory_chunk.tcc"
	Method definitions for chunk-allocated memory pool.
	$Id: typeless_memory_chunk.tcc,v 1.3 2007/08/29 04:45:53 fang Exp $
 */

#ifndef	__UTIL_MEMORY_TYPELESS_MEMORY_CHUNK_TCC__
#define	__UTIL_MEMORY_TYPELESS_MEMORY_CHUNK_TCC__

#include "util/memory/typeless_memory_chunk.h"

/// for suppressing contents of this file
#ifndef	EXTERN_TEMPLATE_UTIL_MEMORY_TYPELESS_MEMORY_CHUNK

// #include <iostream>
#include <iterator>				// for std::distance
#include "util/macros.h"
#include "util/static_assert.h"
#include "util/bitset.tcc"
#include "util/numeric/integer_traits.h"	// may not need anymore :(
#if !TYPELESS_MEMORY_CHUNK_USE_BITSET
#include "util/numeric/clz.h"			// may not need anymore :(
#endif

/**
	Whether or not we try to be fancy and hand optimize.  
	Define to 0 to be conservative.  
 */
#define	USE_HAND_OPTIMIZE_DISTANCE			0

namespace util {
namespace memory {
// #include "util/using_ostream.h"
#if !TYPELESS_MEMORY_CHUNK_USE_BITSET
using numeric::msb;
#endif
using numeric::divide_by_constant;
using numeric::is_power_of_2;

//=============================================================================
// class typeless_memory_chunk method definitions

TYPELESS_MEMORY_CHUNK_TEMPLATE_SIGNATURE
bool
TYPELESS_MEMORY_CHUNK_CLASS::contains(const void* p) const {
#if USE_HAND_OPTIMIZE_DISTANCE
	// some compilers will complain about suspicious reinterpret_cast
	const size_t
		diff = reinterpret_cast<size_t>(p)
			-reinterpret_cast<size_t>(&elements[0]);
	register const size_t offset =
		divide_by_constant<element_size, size_t>(diff);
#else
	// the more proper way, but relying on compiler to optimize
	register const typename
		std::iterator_traits<storage_type*>::difference_type
		offset = std::distance(&elements[0],
			reinterpret_cast<const storage_type*>(p));
#endif
	if (offset >= chunk_size)
		return false;
	const bit_map_type dealloc_mask(bit_map_type(1) << offset);
	return any_bits<bit_map_type>()(free_mask & dealloc_mask);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the address of an available element for allocation.  
	An alternative to the invariant assertions could be exception throwing.
 */
TYPELESS_MEMORY_CHUNK_TEMPLATE_SIGNATURE
void*
TYPELESS_MEMORY_CHUNK_CLASS::__allocate(void) {
#if	!TYPELESS_MEMORY_CHUNK_USE_BITSET
	UTIL_STATIC_ASSERT_DEPENDENT(is_power_of_2<chunk_size>::value);
#endif
	INVARIANT(!this->full());
	const bit_map_type after_alloc = set_any_bit<bit_map_type>()(free_mask);
	const bit_map_type alloc_bit = after_alloc ^ free_mask;
	const bool v __ATTRIBUTE_UNUSED__ = any_bits<bit_map_type>()(alloc_bit);
	INVARIANT(v);
	register const size_t alloc_position =
#if	TYPELESS_MEMORY_CHUNK_USE_BITSET
		alloc_bit.find_first();		// or LSB
#else
		msb<bit_map_type>()(alloc_bit);
#endif
	INVARIANT(alloc_position < chunk_size);
#if 0
	cerr << "free_mask = " << FORMAT_HEX_POINTER(free_mask);
	cerr << ", alloc_bit = " << FORMAT_HEX_POINTER(alloc_bit) << endl;
#endif
	free_mask = after_alloc;
	return &elements[alloc_position];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the address of an available element for allocation.  
	An alternative to the invariant assertions could be exception throwing.
 */
TYPELESS_MEMORY_CHUNK_TEMPLATE_SIGNATURE
void
TYPELESS_MEMORY_CHUNK_CLASS::__deallocate(void* p) {
	// For shame! pointer-arithmetic!
	// worry about portability later...
#if USE_HAND_OPTIMIZE_DISTANCE
	const size_t
		diff = reinterpret_cast<size_t>(p)
			-reinterpret_cast<size_t>(&elements[0]);
	register const size_t offset =
		divide_by_constant<element_size, size_t>(diff);
#else
	// the more proper way, but relying on compiler to optimize
	register const typename
		std::iterator_traits<storage_type*>::difference_type
		offset = std::distance(&elements[0],
			reinterpret_cast<storage_type*>(p));
	INVARIANT(offset >= 0);
#endif
#if 0
	// for debugging
	if (offset >= chunk_size) {
		cerr << "p = " << p <<
			", diff = " << diff <<
			", offset = " << offset <<
			", chunk_size = " << chunk_size << endl;
	}
#endif
	INVARIANT(offset < chunk_size);	// else doesn't belong to this chunk!
	const bit_map_type dealloc_mask(bit_map_type(1) << size_t(offset));
	// was actually allocated and not already freed
#if 0
	cerr << "start = " << begin();
	cerr << ", p = " << p;
	cerr << ", free_mask = " << FORMAT_HEX_POINTER(free_mask);
	cerr << ", dealloc_mask = " << FORMAT_HEX_POINTER(dealloc_mask) << endl;
	if (!(free_mask & dealloc_mask)) {
		cerr << "PANIC!" << endl;
	}
#endif
	// .operator() needed for g++-3.3 :S
	INVARIANT(any_bits<bit_map_type>().operator()(free_mask & dealloc_mask));
#if	TYPELESS_MEMORY_CHUNK_USE_BITSET
	free_mask = free_mask.to_ulong() - dealloc_mask.to_ulong();
#else
	free_mask -= dealloc_mask;
#endif
}

//=============================================================================
}	// end namespace memory
}	// end namespace util

#undef	USE_HAND_OPTIMIZE_DISTANCE

#endif	// EXTERN_TEMPLATE_UTIL_MEMORY_TYPELESS_MEMORY_CHUNK
#endif	// __UTIL_MEMORY_TYPELESS_MEMORY_CHUNK_TCC__

