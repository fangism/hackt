/**
	\file "Object/inst/sparse_collection.h"
	$Id: sparse_collection.h,v 1.1.2.1 2006/10/19 23:04:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_SPARSE_COLLECTION_H__
#define	__HAC_OBJECT_INST_SPARSE_COLLECTION_H__

#include "util/size_t.h"
// #include "util/list_vector.h"
#include <iosfwd>
#include <list>
#include <vector>
#include <map>

namespace HAC {
namespace entity {

#define	SPARSE_COLLECTION_TEMPLATE_SIGNATURE				\
template <typename Key, typename Value>

#define	SPARSE_COLLECTION_CLASS						\
sparse_collection<Key, Value>

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

	\param Key key type for associativity.
	\param Value the value type contained.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
class sparse_collection {
	typedef	SPARSE_COLLECTION_CLASS		this_type;
	/**
		The initial chunk size to pre-allocate.  
	 */
	enum { INITIAL_RESERVE = 4 };
public:
	typedef	Key				key_type;
	typedef	Value				value_type;
	typedef	size_t				size_type;
	typedef	size_type			index_type;
protected:
	/**
		Translates key-type to internal index/ID.  
	 */
	typedef	std::map<key_type, size_type>	key_index_map_type;
	/**
		We store these iterators into the map in an array for 
		efficient constant-time access.  
	 */
	typedef	typename key_index_map_type::iterator
						key_index_map_iterator;
	typedef	typename key_index_map_type::const_iterator
						key_index_map_const_iterator;
	/**
		This structure provides direct fast access into the 
		key-to-index map.
		The index of this vector is simply the ID number
		of an element in the container, and can be thought of
		as an array offset.  
	 */
	typedef	std::vector<key_index_map_const_iterator>
						key_index_array_type;

	/**
		We utilize the reserve and capacity functions
		of vector to determine the occupancy of the chunk.  
	 */
	typedef	std::vector<value_type>		value_chunk_type;
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
private:
	key_index_map_type			key_index_map;
	key_index_array_type			key_index_array;
	value_pool_type				value_pool;
	index_value_map_type			index_value_map;
	address_chunk_map_type			address_chunk_map;
private:
	/**
		Since this container deals with raw pointers
		and internal memory allocation, copy-constructing
		correctly would be non-trivial (but possible).  
		For now, we just forbid it.  
	 */
	explicit
	sparse_collection(const this_type&);
public:
	sparse_collection();
	~sparse_collection();

	size_type
	size(void) const { return this->key_index_array.size(); }

// accessors: precondition -- element must already exist
	const value_type&
	operator [] (const size_type) const;

	const value_type&
	operator [] (const key_type&) const;

#if 0
	value_type&
	operator [] (const size_type);

	value_type&
	operator [] (const key_type&);
#endif

private:
	const value_type*
	__find(const size_type) const;

	const key_type&
	__lookup_key(const size_type) const;

	size_type
	__lookup_index(const key_type&) const;

	size_type
	__lookup_index(const value_type&) const;

public:
	const value_type*
	find(const size_type) const;

	const value_type*
	find(const key_type&) const;

	const key_type&
	lookup_key(const value_type&) const;

	const key_type&
	lookup_key(const size_type) const;

	size_type
	lookup_index(const value_type&) const;

	size_type
	lookup_index(const key_type&) const;

// mutators
	// return true if actually inserted
	bool
	insert(const key_type&, const value_type&);

// diagnostic
	std::ostream&
	dump(std::ostream&) const;

// serialization

};	// end class sparse_collection

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_SPARSE_COLLECTION_H__

