/**
	\file "util/memory/fixed_pool_chunk.tcc"
	Method definitions for chunk-allocated memory pool.
	$Id: fixed_pool_chunk.tcc,v 1.1 2007/02/21 17:00:28 fang Exp $
 */

#ifndef	__UTIL_MEMORY_FIXED_POOL_CHUNK_TCC__
#define	__UTIL_MEMORY_FIXED_POOL_CHUNK_TCC__

#include "util/memory/fixed_pool_chunk.hh"
#include "util/memory/typeless_memory_chunk.tcc"
#include <iostream>
#include "util/what.tcc"

#define	FORMAT_HEX_POINTER(x)	reinterpret_cast<void*>(size_t(x))

namespace util {
namespace memory {
#include "util/using_ostream.hh"

//=============================================================================
// class fixed_pool_chunk method definitions

FIXED_POOL_CHUNK_TEMPLATE_SIGNATURE
FIXED_POOL_CHUNK_CLASS::fixed_pool_chunk() : parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FIXED_POOL_CHUNK_TEMPLATE_SIGNATURE
FIXED_POOL_CHUNK_CLASS::~fixed_pool_chunk() {
	if (!this->empty()) {
		cerr << "WARNING: chunk freed while element still live!"
			<< endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FIXED_POOL_CHUNK_TEMPLATE_SIGNATURE
bool
FIXED_POOL_CHUNK_CLASS::contains(pointer p) const {
	return parent_type::contains(reinterpret_cast<void*>(p));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the address of an available element for allocation.  
 */
FIXED_POOL_CHUNK_TEMPLATE_SIGNATURE
typename FIXED_POOL_CHUNK_CLASS::pointer
FIXED_POOL_CHUNK_CLASS::allocate(void) {
	return reinterpret_cast<pointer>(parent_type::__allocate());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the address of an available element for allocation.  
 */
FIXED_POOL_CHUNK_TEMPLATE_SIGNATURE
void
FIXED_POOL_CHUNK_CLASS::deallocate(pointer p) {
	parent_type::__deallocate(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reports the status of the local allocator.  
 */
FIXED_POOL_CHUNK_TEMPLATE_SIGNATURE
ostream&
FIXED_POOL_CHUNK_CLASS::status(ostream& o) const {
	o << "map_chunk<" << what<T>::name() << "> @" << this <<
		" has the free_mask: (hex) ";
//	const std::ios_base::fmtflags f = o.flags();
//	o.flags(f | std::ios_base::hex);
	// interpreting for getting hexadecimal formatting
#if 0
	o << FORMAT_HEX_POINTER(this->free_mask) << endl;
#else
#if	TYPELESS_MEMORY_CHUNK_USE_BITSET
	return print_bits_hex<typename bit_map_type::word_type>()(
		o, this->free_mask.to_ulong()) << endl;
#else
	return print_bits_hex<bit_map_type>()(o, this->free_mask) << endl;
#endif	// TYPELESS_MEMORY_CHUNK_USE_BITSET
#endif
//	o.flags(f);
	return o;
}

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_FIXED_POOL_CHUNK_TCC__

