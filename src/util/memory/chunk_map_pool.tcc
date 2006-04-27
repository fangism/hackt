/**
	\file "util/memory/chunk_map_pool.tcc"
	Method definitions for chunk-allocated memory pool.
	$Id: chunk_map_pool.tcc,v 1.11 2006/04/27 00:17:27 fang Exp $
 */

#ifndef	__UTIL_MEMORY_CHUNK_MAP_POOL_TCC__
#define	__UTIL_MEMORY_CHUNK_MAP_POOL_TCC__

#include "util/memory/chunk_map_pool.h"

#ifndef	EXTERN_TEMPLATE_UTIL_MEMORY_CHUNK_MAP_POOL

#include <iostream>
#include <iterator>				// for std::distance
#include "util/macros.h"
#include "util/numeric/nibble_tables.h"		// may not need anymore :(
#include "util/numeric/integer_traits.h"	// may not need anymore :(
#include "util/static_assert.h"
#include "util/bitset.tcc"
#include "util/attributes.h"

#ifdef	EXCLUDE_DEPENDENT_TEMPLATES_UTIL_MEMORY_CHUNK_MAP_POOL
#define	EXTERN_TEMPLATE_UTIL_MEMORY_DESTRUCTION_POLICY
#define	EXTERN_TEMPLATE_UTIL_WHAT
#endif

#include "util/memory/destruction_policy.tcc"
#include "util/what.tcc"

#define	FORMAT_HEX_POINTER(x)	reinterpret_cast<void*>(size_t(x))

/**
	Whether or not we try to be fancy and hand optimize.  
	Define to 0 to be conservative.  
 */
#define	USE_HAND_OPTIMIZE_DISTANCE			0

namespace util {
namespace memory {
#include "util/using_ostream.h"
using numeric::MSB_position;
using numeric::divide_by_constant;
using numeric::is_power_of_2;

//=============================================================================
// class typeless_memory_chunk method definitions

TYPELESS_MEMORY_CHUNK_TEMPLATE_SIGNATURE
bool
TYPELESS_MEMORY_CHUNK_CLASS::contains(void* p) const {
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
	const bit_map_type dealloc_mask = bit_map_type(1) << offset;
	return (free_mask & dealloc_mask);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the address of an available element for allocation.  
	An alternative to the invariant assertions could be exception throwing.
 */
TYPELESS_MEMORY_CHUNK_TEMPLATE_SIGNATURE
void*
TYPELESS_MEMORY_CHUNK_CLASS::__allocate(void) {
	UTIL_STATIC_ASSERT_DEPENDENT(is_power_of_2<chunk_size>::value);
	INVARIANT(!this->full());
#if 0
	const bit_map_type after_alloc = free_mask | (free_mask +1);
#else
	const bit_map_type after_alloc = set_any_bit<bit_map_type>()(free_mask);
#endif
	const bit_map_type alloc_bit = after_alloc ^ free_mask;
#if 0
	INVARIANT(alloc_bit);	// throw bad_alloc()
#else
	const bool v __ATTRIBUTE_UNUSED__ = any_bits<bit_map_type>()(alloc_bit);
	INVARIANT(v);
#endif
	register const size_t alloc_position =
		MSB_position<bit_map_type>()(alloc_bit);
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
	const bit_map_type dealloc_mask = bit_map_type(1) << offset;
	// was actually allocated and not already freed
#if 0
	cerr << "start = " << start_address();
	cerr << ", p = " << p;
	cerr << ", free_mask = " << FORMAT_HEX_POINTER(free_mask);
	cerr << ", dealloc_mask = " << FORMAT_HEX_POINTER(dealloc_mask) << endl;
	if (!(free_mask & dealloc_mask)) {
		cerr << "PANIC!" << endl;
	}
#endif
	INVARIANT(free_mask & dealloc_mask);
	free_mask -= dealloc_mask;
}

//=============================================================================
// class chunk_map_pool_chunk method definitions

CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE
CHUNK_MAP_POOL_CHUNK_CLASS::chunk_map_pool_chunk() : parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE
CHUNK_MAP_POOL_CHUNK_CLASS::~chunk_map_pool_chunk() {
	if (!this->empty()) {
		cerr << "WARNING: chunk freed while element still live!"
			<< endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE
bool
CHUNK_MAP_POOL_CHUNK_CLASS::contains(pointer p) const {
	return parent_type::contains(reinterpret_cast<void*>(p));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the address of an available element for allocation.  
 */
CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE
typename CHUNK_MAP_POOL_CHUNK_CLASS::pointer
CHUNK_MAP_POOL_CHUNK_CLASS::allocate(void) {
	return reinterpret_cast<pointer>(parent_type::__allocate());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the address of an available element for allocation.  
 */
CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE
void
CHUNK_MAP_POOL_CHUNK_CLASS::deallocate(pointer p) {
	parent_type::__deallocate(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reports the status of the local allocator.  
 */
CHUNK_MAP_POOL_CHUNK_TEMPLATE_SIGNATURE
ostream&
CHUNK_MAP_POOL_CHUNK_CLASS::status(ostream& o) const {
	o << "map_chunk<" << what<T>::name() << "> @" << this <<
		" has the free_mask: (hex) ";
//	const std::ios_base::fmtflags f = o.flags();
//	o.flags(f | std::ios_base::hex);
	// interpreting for getting hexadecimal formatting
#if 0
	o << FORMAT_HEX_POINTER(this->free_mask) << endl;
#else
	return print_bits_hex<bit_map_type>()(o, this->free_mask) << endl;
#endif
//	o.flags(f);
	return o;
}

//=============================================================================
// class chunk_map_pool method definitions

CHUNK_MAP_POOL_TEMPLATE_SIGNATURE
CHUNK_MAP_POOL_CLASS::chunk_map_pool() :
		chunk_set(), chunk_map(), avail_set() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_TEMPLATE_SIGNATURE
CHUNK_MAP_POOL_CLASS::~chunk_map_pool() {
	if (!this->chunk_map.empty()) {
		cerr << "WARNING: chunk_map_pool<" << what<T>::name() <<
			"> destroyed while chunks were live!" << endl;
		status(cerr);
#if 0
		cerr << "Hanging." << endl; while(1) { }
#endif
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param p the pointer address in question
	\return whether or not the pointer is managed by this allocator.  
		If it WAS managed by this allocator, by was already freed, 
		then this returns false.  
 */
CHUNK_MAP_POOL_TEMPLATE_SIGNATURE
bool
CHUNK_MAP_POOL_CLASS::contains(pointer p) const {
	if (chunk_map.empty())
		return false;
	const_alloc_map_iterator entry = chunk_map.upper_bound(p);
	// find the chunk to which this pointer belongs
	if (entry == chunk_map.begin()) {
		// else out of bounds
		return false;
	}
	entry--;
	const chunk_set_iterator use_chunk = entry->second;
	return (p >= use_chunk->start_address() && use_chunk->contains(p));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_TEMPLATE_SIGNATURE
typename CHUNK_MAP_POOL_CLASS::pointer
CHUNK_MAP_POOL_CLASS::allocate(void) {
	if (this->avail_set.empty()) {
		// need to create a chunk
		// instead of push_back, which calls constructor, 
		// then copy-constructor, then destructor of a chunk, 
		// just call resize() which default-constructs new elements.
#if 0
		chunk_set.resize(chunk_set.size() +1);
#else
		chunk_set.push_back(chunk_type());
#endif
		const chunk_set_iterator last = --chunk_set.end();
		chunk_map[last->start_address()] = last;
		avail_set.insert(&*last);
		return last->allocate();
	} else {
		// pick any chunk with a free element
		// any selectable policy? FIFO? LIFO? LRU?
		// depends on implementation of avail_set.
		const avail_set_iterator first = avail_set.begin();
		chunk_type* const use_chunk = *first;
		// probably the lowest address chunk
		const pointer ret = use_chunk->allocate();
		if (use_chunk->full()) {
			avail_set.erase(first);
		}
		// chunk_map doesn't change
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_TEMPLATE_SIGNATURE
void
CHUNK_MAP_POOL_CLASS::deallocate(pointer p) {
	NEVER_NULL(p);
#if 0
	INVARIANT(!chunk_map.empty());
#else
	// for debugging
	if (chunk_map.empty()) {
		// This may happen on exit(1) without proper clean-up
		// during stack unwinding.  Suggest throwing an exception
		// to clean up the stack if intending to exit.  
		cerr << "WARNING: trying to deallocate " << what<T>::name() <<
			" at " << p << " after chunk map is already empty.  "
			"Ignoring." << endl;
		// { char c; std::cin >> c; }
		// cerr << "Pausing." << endl; while(1) { }
		return;
	}
#endif
	// see "list_vector.h" for similar code.
	alloc_map_iterator entry = chunk_map.upper_bound(p);
	// find the chunk to which this pointer belongs
	INVARIANT(entry != chunk_map.begin());	// else out of bounds
	entry--;
	const chunk_set_iterator use_chunk = entry->second;
	// INVARIANT(&*use_chunk);
	if (use_chunk->full()) {
		// deallocating an entry will qualify it for the avail_set
		// because it will not be full thereafter.
		avail_set.insert(&*use_chunk);
	}
	INVARIANT(p >= use_chunk->start_address());
	INVARIANT(p < use_chunk->past_end_address());
#if 0
	this->status(cerr);
	cerr << "about to delete " << p << endl;
#endif
	use_chunk->deallocate(p);
	if (use_chunk->empty()) {
		// whole chunk is free, eagerly release the chunk's memory
		// or whatever chunk_set_type's allocator does...
		avail_set.erase(&*use_chunk);
		chunk_set.erase(use_chunk);
		chunk_map.erase(entry);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_TEMPLATE_SIGNATURE
ostream&
CHUNK_MAP_POOL_CLASS::status(ostream& o) const {
	o << "chunk_map_pool<" << what<T>::name() << "> @" << this <<
		" has the following chunks:" << endl;
	const_alloc_map_iterator iter = chunk_map.begin();
	const const_alloc_map_iterator end = chunk_map.end();
	for ( ; iter != end; iter++) {
		iter->second->status(o << '\t');
	}
	return o;
}

//=============================================================================
}	// end namespace memory
}	// end namespace util

#undef	USE_HAND_OPTIMIZE_DISTANCE

#endif	// EXTERN_TEMPLATE_UTIL_MEMORY_CHUNK_MAP_POOL
#endif	// __UTIL_MEMORY_CHUNK_MAP_POOL_TCC__

