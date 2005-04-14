/**
	\file "memory/chunk_map_pool.tcc"
	Method definitions for chunk-allocated memory pool.
	$Id: chunk_map_pool.tcc,v 1.4 2005/04/14 19:46:36 fang Exp $
 */

#ifndef	__UTIL_MEMORY_CHUNK_MAP_POOL_TCC__
#define	__UTIL_MEMORY_CHUNK_MAP_POOL_TCC__

#include <iostream>
#include "macros.h"
#include "memory/chunk_map_pool.h"
#include "numeric/nibble_tables.h"
#include "numeric/integer_traits.h"
#include "memory/destruction_policy.tcc"
#include "what.tcc"

#define	FORMAT_HEX_POINTER(x)	reinterpret_cast<void*>(size_t(x))

namespace util {
namespace memory {
#include "using_ostream.h"
using numeric::MSB_position;
using numeric::divide_by_constant;

//=============================================================================
// class typeless_memory_chunk method definitions

/**
	Returns the address of an available element for allocation.  
	An alternative to the invariant assertions could be exception throwing.
 */
TYPELESS_MEMORY_CHUNK_TEMPLATE_SIGNATURE
void*
TYPELESS_MEMORY_CHUNK_CLASS::__allocate(void) {
	INVARIANT(!this->full());
	register const bit_map_type after_alloc = free_mask | (free_mask +1);
	register const bit_map_type alloc_bit = after_alloc ^ free_mask;
	INVARIANT(alloc_bit);	// throw bad_alloc()
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
	const size_t
		diff = reinterpret_cast<size_t>(p)
			-reinterpret_cast<size_t>(&elements[0]);
	register const size_t offset =
		divide_by_constant<element_size, size_t>(diff);
#if 0
	cerr << "diff = " << diff <<
		", offset = " << offset <<
		", chunk_size = " << chunk_size << endl;
#endif
	INVARIANT(offset < chunk_size);	// else doesn't belong to this chunk!
	register const bit_map_type dealloc_mask = bit_map_type(1) << offset;
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
	o << FORMAT_HEX_POINTER(this->free_mask) << endl;
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
		cerr << "WARNING: chunk map destroyed while chunks were live!"
			<< endl;
	}
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
	INVARIANT(!chunk_map.empty());
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

#endif	// __UTIL_MEMORY_CHUNK_MAP_POOL_TCC__

