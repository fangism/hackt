/**
	\file "Object/inst/instance_pool.h"
	Template class wrapper around list_vector.
	$Id: instance_pool.h,v 1.12.88.1 2010/01/09 03:30:04 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_POOL_H__
#define	__HAC_OBJECT_INST_INSTANCE_POOL_H__

#include <iosfwd>
#include "Object/inst/instance_pool_fwd.h"
#include "util/list_vector.h"
#include "util/boolean_types.h"
#include "util/persistent_fwd.h"
#include "util/memory/index_pool.h"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {
class footprint;
using std::istream;
using std::ostream;
using std::vector;
using util::good_bool;
using util::persistent_object_manager;
using util::memory::index_pool;
template <class> class instance_collection_pool_bundle;

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
class instance_pool : private index_pool<util::list_vector<T> > {
	typedef	index_pool<util::list_vector<T> >	parent_type;
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
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
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
#if 0
	size_t						_private_entries;
#else
	// can be deduced, use total_private_entries()
#endif
	/**
		Map entry (remains sorted) is amended each time a local
		structure is allocated.  key,value increase monotonically
		with each new entry, hence invariantly ordered.
		Is sparse entry, so can skip indices for substructures
		with no private entries.  
	 */
	typedef	std::pair<size_t, size_t>		private_map_entry_type;
	typedef	vector<private_map_entry_type>		private_entry_map_type;
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
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION
public:
	// custom default constructor
	instance_pool();

	explicit
	instance_pool(const size_type);

private:
	// copy-construction policy
	instance_pool(const this_type&);
public:
	~instance_pool();

	using parent_type::size;
	using parent_type::begin;
	using parent_type::end;
	using parent_type::operator[];
	using parent_type::allocate;

	// there is no deallocate

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	/**
		\return the number of instances that are local 
		but not aliased to public ports.  
	 */
	size_t
	local_private_entries(void) const {
		return this->size() -this->_port_entries;
	}

	/**
		The 
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
#endif

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

