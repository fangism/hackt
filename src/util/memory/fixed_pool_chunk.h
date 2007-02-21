/**
	\file "util/memory/fixed_pool_chunk.h"
	Class definition for chunk-allocated mapped memory pool template.  
	$Id: fixed_pool_chunk.h,v 1.1 2007/02/21 17:00:28 fang Exp $
 */

#ifndef	__UTIL_MEMORY_FIXED_POOL_CHUNK_H__
#define	__UTIL_MEMORY_FIXED_POOL_CHUNK_H__

#include <iosfwd>
#include "util/memory/typeless_memory_chunk.h"

//=============================================================================
namespace util {
namespace memory {
using std::ostream;

#define FIXED_POOL_CHUNK_TEMPLATE_SIGNATURE				\
template <class T, size_t C>

#define	FIXED_POOL_CHUNK_CLASS					\
fixed_pool_chunk<T,C>

#define	CHUNK_MAP_POOL_CLASS						\
chunk_map_pool<T,C,Threaded>

FIXED_POOL_CHUNK_TEMPLATE_SIGNATURE
class fixed_pool_chunk;

//=============================================================================
/**
	Fixed size type-specific chunk.
	Defined as a struct with public members, but the allocator
	shouldn't ever leak out a reference to this type.  
	Even this alone may be useful for local allocation.  
	Consider making a thread-locked variation...
	currently, this is thread-blind.
 */
FIXED_POOL_CHUNK_TEMPLATE_SIGNATURE
class fixed_pool_chunk : protected typeless_memory_chunk<sizeof(T), C> {
	typedef	FIXED_POOL_CHUNK_CLASS	this_type;
protected:
	typedef	typeless_memory_chunk<sizeof(T), C>	parent_type;
public:
	typedef	typename parent_type::bit_map_type	bit_map_type;
	typedef	T				value_type;
	typedef	T*				pointer;
	// TODO: rebind template typedef
public:
	fixed_pool_chunk();

	~fixed_pool_chunk();

	bool
	contains(pointer) const;

	using parent_type::full;
	using parent_type::empty;
	using parent_type::begin;
	using parent_type::end;

	/// wrap around parent's
	pointer
	allocate(void) __ATTRIBUTE_MALLOC__ ;

	void
	deallocate(pointer);

	ostream&
	status(ostream&) const;

};	// end class fixed_pool_chunk

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_FIXED_POOL_CHUNK_H__

