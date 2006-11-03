/**
	\file "Object/inst/collection_pool.h"
	This bears much resemblance to util::list_vector!
	However, no free-list is required.  
	$Id: collection_pool.h,v 1.1.2.2 2006/11/03 05:22:20 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_COLLECTION_POOL_H__
#define	__HAC_OBJECT_INST_COLLECTION_POOL_H__

#include "util/size_t.h"
#include "util/macros.h"
#include <iosfwd>
#include <list>
#include <vector>
#include <map>
#include "util/nested_iterator.h"

namespace HAC {
namespace entity {
struct collection_index_entry;

//=============================================================================
#define	COLLECTION_POOL_TEMPLATE_SIGNATURE				\
template <typename Value>

#define	COLLECTION_POOL_CLASS						\
collection_pool<Value>

//=============================================================================
/**
	This special sparse container has the following properties:
	No memory managed by this container is ever reallocated or moved.
	The elements contained therein may be addressed directly by 
	internal index/ID, or by key, both of which are log-time accesses.  
	To avoid expensive replication of keys, we maintain an array
	of map-iterators, which are essentially pointers; this array is
	reallocatable and remains contiguous, indexed by ID number.  
	Furthermore, the relation between index and physical address
	may not even be monotonic.  
	The iterators remain valid through the lifetime of the container 
	because they are never removed from the map once created.  
	Thus we can translate betwen index and key.  
	We also maintain a map that translates absolute addresses
	to indices.  This is valid because no reallocation can occur, 
	thus addresses of the values remain valid through the lifetime
	of this container.  

	Note: interface indexing starts at 1 because we use 0 to signal 
		non-existence.  (Internally, however, indices are 0-based.)

	This container bears some attributes of util::list_vector
	and util::memory::list_vector_pool, but neither one entirely
	meets the needs of this application.  
	e.g. this needs two address modes, and does not need a free-list, 
	as elements are not explicitly deallocated.  

	This would be a good candidate addition to the library if it 
	weren't so specialized.  

	This container is difficult to iterate through.  

	\param Key key type for associativity.
	\param Value the value type contained.  
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
class collection_pool {
	typedef	COLLECTION_POOL_CLASS		this_type;
	/**
		The initial chunk size to pre-allocate.  
	 */
	enum { INITIAL_RESERVE = 2 };
// template <class T, class V>
// friend class _collection_pool_index_ordered_iterator;
public:
	typedef	Value				value_type;
	typedef	size_t				size_type;
	typedef	size_type			index_type;
protected:
	/**
		We utilize the reserve and capacity functions
		of vector to determine the occupancy of the chunk.  
	 */
	typedef	std::vector<value_type>		value_chunk_type;
	typedef	typename value_chunk_type::iterator	value_chunk_iterator;
	typedef	typename value_chunk_type::const_iterator
						value_chunk_const_iterator;
	/**
		We use a list because its values cannot be reallocated.  
		As each chunk is filled, append new chunks to the list.  
		We never actually have to traverse this list sequentially, 
		i.e. we never incur linear-time operations.  
	 */
	typedef	std::list<value_chunk_type>	value_pool_type;
	/**
		The handle type we use for chunks in the list.
	 */
	typedef	typename value_pool_type::iterator
						value_pool_iterator;
	typedef	typename value_pool_type::const_iterator
						value_pool_const_iterator;

	/**
		The key of this map is the lowest index corresponding
		to the element chunk pointed to by the value-iterator.  
		As elements are added, the index key increases
		monotonically.  
		Using the index sought as a lower bound quickly
		identifies the chunk to which it belongs. 
		The remaining difference can be used as a relative offset
		into the chunk array.  
	 */
	typedef	std::map<size_type, value_pool_iterator>
						index_value_map_type;
	/**
		Since the index_value_map never deletes entries during
		this container's lifetime, the iterators remain valid. 
	 */
	typedef	typename index_value_map_type::iterator
						index_value_map_iterator;
	typedef	typename index_value_map_type::const_iterator
						index_value_map_const_iterator;
	/**
		\post The key of this map should point to the beginning
		address of the chunk pointed to by the iterator value.  
		Again, the memory chunks may not be allocated 
		in monotonic addresses.  
		This can be used to quickly translate an address into
		an index into the key_index_array.  
	 */
	typedef	std::map<const value_type*, index_value_map_const_iterator>
						address_chunk_map_type;
	typedef	typename address_chunk_map_type::const_iterator
					address_chunk_map_const_iterator;
public:
	typedef	util::nested_iterator<
			value_pool_iterator, value_chunk_iterator>
						iterator;
	typedef	util::nested_iterator<
			value_pool_const_iterator, value_chunk_const_iterator>
						const_iterator;
private:
	value_pool_type				value_pool;
	index_value_map_type			index_value_map;
	address_chunk_map_type			address_chunk_map;
protected:
	/**
		One default-constructed value, to avoid repetitive
		default construction of empty temporaries.  
	 */
	static const value_type			default_value;
private:
	/**
		Since this container deals with raw pointers
		and internal memory allocation, copy-constructing
		correctly would be non-trivial (but possible).  
		For now, we just forbid it.  
	 */
	explicit
	collection_pool(const this_type&);
public:
	collection_pool();
	~collection_pool();

	size_type
	size(void) const {
		const index_value_map_const_iterator
			b(--this->index_value_map.end());
		return b->first + b->second->size();
	}

	bool
	empty(void) const {
		return !this->index_value_map.begin()->second->size();
	}

// accessors: precondition -- element must already exist

	value_type&
	operator [] (const size_type) const;

#if 0
	value_type&
	operator [] (const size_type);
#endif

private:
	index_value_map_const_iterator
	__find_index_value_map_iterator(const size_type) const;

	value_type*
	__find(const size_type) const;

#if 0
	value_type*
	__find(const size_type);
#endif

	size_type
	__lookup_index(const value_type&) const;

	value_chunk_type*
	__new_chunk(const size_type);

public:
	value_type*
	find(const size_type) const;

#if 0
	value_type*
	find(const size_type);
#endif

	size_type
	lookup_index(const value_type&) const;

// mutators

// allocator
	value_type*
	allocate(void);

	void
	allocate(const size_type);

	value_type*
	push_back(const value_type&);

// iterators
	iterator
	begin(void) {
		return iterator(this->value_pool.begin(), 
			this->value_pool.front().begin());
	}

	const_iterator
	begin(void) const {
		return const_iterator(this->value_pool.begin(), 
			this->value_pool.front().begin());
	}

	iterator
	end(void) {
		return iterator(--this->value_pool.end(), 
			this->value_pool.back().end());
	}

	const_iterator
	end(void) const {
		return const_iterator(--this->value_pool.end(), 
			this->value_pool.back().end());
	}

// diagnostic
	std::ostream&
	dump(std::ostream&) const;

// serialization
};	// end class collection_pool

COLLECTION_POOL_TEMPLATE_SIGNATURE
collection_index_entry
lookup_collection_pool_index_entry(
	const collection_pool<Value>&, const Value&);


//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_COLLECTION_POOL_H__

