/**
	\file "util/memory/index_pool.h"
	An adapter class to turn a vector-like class into 
	an index-based allocator.  
	$Id: index_pool.h,v 1.2.6.1 2006/01/27 23:48:00 fang Exp $
 */

#ifndef	__UTIL_MEMORY_INDEX_POOL_H__
#define	__UTIL_MEMORY_INDEX_POOL_H__

// #include "util/size_t.h"

namespace util {
namespace memory {
//=============================================================================
/**
	This is an adapter class to take a vector-like array data structure
	and use it as a pool allocator that returns indices to 
	the member elements.  
	This is a special kind of allocator for which deallocation
	is not needed, only a one-time allocate interface.  
	This is NOT a general allocator, as it does not keep track of
	what's allocater or not, and never expects entries
	to be returned/deallocated.  
	Consider a different name, like index_alloc_adaptor?
 */
template <class Pool>
class index_pool : protected Pool {
public:
	/**
		The underlying array type, such as vector.  
		This interface requires that this type have a self
		reallocating implementation of push_back.  
		In this library, list_vector satisfies these requirements. 
	 */
	typedef	Pool					array_type;
	/**
		The type allocated.  
	 */
	typedef	typename array_type::value_type		value_type;
	typedef	typename array_type::iterator		iterator;
	typedef	typename array_type::const_iterator	const_iterator;
	typedef	typename array_type::size_type		size_type;
#if 0
protected:
	array_type					pool;
#endif
public:
	index_pool();
	~index_pool();

	using array_type::size;
	using array_type::operator[];
	using array_type::begin;
	using array_type::end;
	using array_type::clear;

	size_type
	allocate(void);

	size_type
	allocate(const value_type&);

};	// end class index_pool

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_INDEX_POOL_H__

