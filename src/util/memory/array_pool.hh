/**
	\file "util/memory/array_pool.hh"
	A pool that returns indices/keys into an array-indexed
	or associative container.
	This was born out of sim/prsim/Event-prsim.hh's array_pool class.
 */

#ifndef	__UTIL_MEMORY_ARRAY_POOL_HH__
#define	__UTIL_MEMORY_ARRAY_POOL_HH__

#include <iosfwd>
#include "util/likely.h"
#include "util/attributes.h"
#include "util/macros.h"
#include "util/utypes.h"
#include "util/memory/index_pool.hh"
#include "util/memory/free_list.hh"

/**
	Verbosely trace each item added and removed from pool free list.
 */
#define	DEBUG_ARRAY_POOL_ALLOC				0

namespace util {
namespace memory {
using std::istream;
using std::ostream;
using util::memory::index_pool;
using util::memory::free_list_acquire;
using util::memory::free_list_release;

//=============================================================================
/**
	Allocator that interfaces via indices instead of pointer.  
	\param T resource array type, like vector<...>
	\param F free-list container type, any container of integers. 
 */
template <class T, class F>
class array_pool {
public:
	typedef	T				resource_pool_type;
	typedef	typename resource_pool_type::value_type
						value_type;
	typedef	index_pool<resource_pool_type>	index_allocator_type;
	/**
		TODO: use a more compact discrete_interval_set
	 */
	typedef	F				free_list_type;
	typedef	typename free_list_type::value_type
						index_type;
protected:
	index_allocator_type			_index_pool;
	free_list_type				free_indices;
public:
	array_pool();
	~array_pool();

	const value_type&
	operator [] (const index_type i) const {
		INVARIANT(i);
		return _index_pool[i];
	}

	value_type&
	operator [] (const index_type i) {
		INVARIANT(i);
		return _index_pool[i];
	}

	bool
	check_valid_empty(void) const;

	index_type
	pool_size(void) const {
		return _index_pool.size();
	}

#if DEBUG_ARRAY_POOL_ALLOC
	index_type
	allocate(const value_type& e);
#else
	index_type
	allocate(const value_type& e) {
		if (UNLIKELY(free_indices.empty())) {	// UNLIKELY
			const index_type ret = _index_pool.size();
			_index_pool.allocate(e);	// will realloc
			INVARIANT(ret);
			return ret;
		} else {			// LIKELY
			const index_type ret =
				free_list_acquire(free_indices);
			_index_pool[ret] = e;
			INVARIANT(ret);
			return ret;
		}
	}
#endif

#if DEBUG_ARRAY_POOL_ALLOC
	void
	deallocate(const index_type i);
#else
	void
	deallocate(const index_type i) {
		INVARIANT(i);
		free_list_release(free_indices, i);
	}
#endif

	void
	clear(void);

};	// end class array_pool

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_ARRAY_POOL_HH__

