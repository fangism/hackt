/**
	\file "Object/inst/instance_pool.hh"
	Template class wrapper around list_vector.
	$Id: instance_pool.hh,v 1.15 2011/05/17 21:19:52 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_POOL_H__
#define	__HAC_OBJECT_INST_INSTANCE_POOL_H__

#include <iosfwd>
#include <vector>
#include "Object/inst/instance_pool_fwd.hh"
#include "util/boolean_types.hh"
#include "util/persistent_fwd.hh"

namespace HAC {
namespace entity {
class footprint;
using std::istream;
using std::ostream;
using std::vector;
using util::good_bool;
using util::persistent_object_manager;
template <class> struct instance_collection_pool_bundle;


/**
	Map entry (remains sorted) is amended each time a local
	structure is allocated.  key,value increase monotonically
	with each new entry, hence invariantly ordered.
	Is sparse entry, so can skip indices for substructures
	with no private entries.  
	first: is 1-indexed to process pool
	second: 0-indexed offset
 */
typedef	std::pair<size_t, size_t>		pool_private_map_entry_type;
typedef	vector<pool_private_map_entry_type>	pool_private_entry_map_type;

//=============================================================================
/**
	Wrapped interface to list_vector being used as an indexable pool, 
	intended for use of pseudo-allocating instances.  
	Consider adding this as an index_vector_pool to the util library.  
	TODO: consider using collection_pool.
	This structure is closely related to footprint member
		port_alias_tracker footprint::scope_aliases.
 */
template <class T>
class instance_pool : private vector<T> {
	typedef	vector<T>				parent_type;
	typedef	instance_pool<T>		this_type;
	typedef	typename T::tag_type		tag_type;
public:
	typedef	typename T::traits_type			traits_type;
	typedef	typename parent_type::const_iterator	const_iterator;
	typedef	typename parent_type::iterator		iterator;
	typedef	typename parent_type::size_type		size_type;
	typedef	typename parent_type::value_type	value_type;
	typedef	instance_collection_pool_bundle<tag_type>
				collection_pool_bundle_type;
private:
	/**
		Default chunk size when not specified.  
	 */
	enum	{ default_chunk_size = 32 };
public:	// out of sheer laziness for now...
	// see "Object/inst/state_instance.h"
	/**
		Allocation time, after partitioning, this index points
		to one-past the last port-aliased instance in this pool.
		This is also used to determine the size of the footprint
		frame for instances of this footprint.  
	 */
	size_t						_port_entries;
	/**
		Allocation time: the number of private instances of this
		meta-type that are not locally reachable, but are deeper
		in the hierarchy.  (computed by accumulation)
		This is how we create a properly sized and spaced 
		memory map.
		Only for processes' footprints can these values be non-zero, 
		because other structs only have public ports.  
		This can be post-computed after the last private map
		entry has been added, by accumulating over process
		subinstances.  
		This should equal the number of non-local private entries
		from private_entry_map.back() plus the number of
		local non-port entries in this instance_pool.
	 */
	typedef	pool_private_map_entry_type	private_map_entry_type;
	typedef	pool_private_entry_map_type	private_entry_map_type;
	/**
		Do a binary search on this sorted map to find the
		index of the local instance to descend into.
		key: index local instance to which it belongs (1-indexed).
		value: lower bound of index (subtract this offset)
		The intervals between entries of this map should correspond
		to the _private_entries sizes of the referenced local 
		instance' footprint.  
		key and value should be monotonically increasing.
	 */
	private_entry_map_type				private_entry_map;

public:
	// custom default constructor
	instance_pool();

private:
	// copy-construction policy
	instance_pool(const this_type&);
public:
	~instance_pool();

	using parent_type::begin;
	using parent_type::end;
	using parent_type::operator[];

	const_iterator
	local_private_begin(void) const;

	using parent_type::resize;
	size_t
	allocate(const value_type& t) {
		this->push_back(t);
		return this->size() -1;
	}

	// there is no deallocate

	/**
		\return number of _port_entries
		Use this value to size footprint_frames.
	 */
	size_t
	port_entries(void) const {
		return this->_port_entries;
	}

	/**
		\return the number of instances that are local 
		but not aliased to public ports.  
	 */
	size_t
	local_private_entries(void) const {
		return this->size() -this->_port_entries;
	}

	/**
		\return number of local instances, including ports.
	 */
	size_t
	local_entries(void) const {
		return this->size();
	}

	/**
		The number of local entries, port and non-port.
	 */
	size_t
	non_local_private_entries(void) const {
		return this->private_entry_map.back().second;
	}

	size_t
	total_private_entries(void) const {
		return this->non_local_private_entries()
			+this->local_private_entries();
	}

	/**
		\return the total number of unique instances, 
			counting public, port, and non-local private
			subinstances accumulated.
	 */
	size_t
	total_entries(void) const {
		return this->local_entries()
			+this->non_local_private_entries();
	}

	const private_map_entry_type&
	locate_private_entry(const size_t) const;

	const private_map_entry_type&
	locate_cumulative_entry(const size_t) const;

	ostream&
	dump(ostream&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const collection_pool_bundle_type&, 
		ostream&) const;

	void
	load_object_base(const collection_pool_bundle_type&, 
		istream&);
};	// end class instance_pool

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_POOL_H__

