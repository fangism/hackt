/**
	\file "util/memory/chunk_map_pool.tcc"
	Method definitions for chunk-allocated memory pool.
	$Id: chunk_map_pool.tcc,v 1.12 2007/02/21 17:00:27 fang Exp $
 */

#ifndef	__UTIL_MEMORY_CHUNK_MAP_POOL_TCC__
#define	__UTIL_MEMORY_CHUNK_MAP_POOL_TCC__

#include "util/memory/chunk_map_pool.h"

#ifndef	EXTERN_TEMPLATE_UTIL_MEMORY_CHUNK_MAP_POOL

#include <iostream>

#ifdef	EXCLUDE_DEPENDENT_TEMPLATES_UTIL_MEMORY_CHUNK_MAP_POOL
#define	EXTERN_TEMPLATE_UTIL_MEMORY_TYPELESS_MEMORY_CHUNK
#define	EXTERN_TEMPLATE_UTIL_MEMORY_DESTRUCTION_POLICY
#define	EXTERN_TEMPLATE_UTIL_WHAT
#endif

#include "util/memory/fixed_pool_chunk.tcc"
#include "util/memory/destruction_policy.tcc"
#include "util/what.tcc"

namespace util {
namespace memory {
#include "util/using_ostream.h"

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
	const_alloc_map_iterator entry(chunk_map.upper_bound(p));
	// find the chunk to which this pointer belongs
	if (entry == chunk_map.begin()) {
		// else out of bounds
		return false;
	}
	entry--;
	const chunk_set_iterator use_chunk(entry->second);
	return (p >= use_chunk->begin() && use_chunk->contains(p));
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
		chunk_map[last->begin()] = last;
		avail_set.insert(&*last);
		return last->allocate();
	} else {
		// pick any chunk with a free element
		// any selectable policy? FIFO? LIFO? LRU?
		// depends on implementation of avail_set.
		const avail_set_iterator first(avail_set.begin());
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
	alloc_map_iterator entry(chunk_map.upper_bound(p));
	// find the chunk to which this pointer belongs
	INVARIANT(entry != chunk_map.begin());	// else out of bounds
	entry--;
	const chunk_set_iterator use_chunk(entry->second);
	// INVARIANT(&*use_chunk);
	if (use_chunk->full()) {
		// deallocating an entry will qualify it for the avail_set
		// because it will not be full thereafter.
		avail_set.insert(&*use_chunk);
	}
	INVARIANT(p >= use_chunk->begin());
	INVARIANT(p < use_chunk->end());
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
	const_alloc_map_iterator iter(chunk_map.begin());
	const const_alloc_map_iterator end(chunk_map.end());
	for ( ; iter != end; ++iter) {
		iter->second->status(o << '\t');
	}
	return o;
}

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_MEMORY_CHUNK_MAP_POOL
#endif	// __UTIL_MEMORY_CHUNK_MAP_POOL_TCC__

