/**
	\file "Object/def/footprint_base.h"
	Data structure for each complete type's footprint template.  
	$Id: footprint_base.h,v 1.5.14.1 2009/12/17 02:07:36 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_FOOTPRINT_BASE_H__
#define	__HAC_OBJECT_DEF_FOOTPRINT_BASE_H__

#include <iosfwd>
#include "Object/inst/instance_pool.h"
#include "Object/inst/state_instance.h"
#include "Object/devel_switches.h"

#include "util/persistent_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/uninitialized.h"

namespace HAC {
namespace entity {
class state_manager;
class footprint_frame;
template <class> class instance_collection_pool_bundle;
template <class> class value_collection_pool_bundle;

using std::istream;
using std::ostream;
using util::memory::excl_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	Meta-type specific base class.  
	This is a meta-tagged wrapper around the instance pool
	that contains a collection of *unique* instances.  
	This covers all public port instances of the collections
	in the footprint, but not private instances.  
	This also wraps around the instance_collection pools used to
	allocate all instance collections.  
	NOTE: this uses the private-implementation "pimpl" idiom, 
		that hides the implementation, by using a pointer
		to the underlying implementations.  
	TODO: consider pool allocating pool_bundle and instance_pools.  
 */
template <class Tag>
class footprint_base {
	typedef	footprint_base<Tag>			this_type;
protected:
	typedef	typename state_instance<Tag>::pool_type	instance_pool_type;
	typedef	instance_collection_pool_bundle<Tag>
					collection_pool_bundle_type;
private:
	typedef	typename instance_pool_type::const_iterator	const_iterator;
protected:
	const excl_ptr<collection_pool_bundle_type>	collection_pool_bundle;
	/**
		This is where final unique instances are allocated.
	 */
	const excl_ptr<instance_pool_type>		_instance_pool;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
public:	// out of sheer laziness for now...
	// use state_instance and instance_pool instead of global_entry_pool!
	// see "Object/inst/state_instance.h"
	/**
		Allocation time: the number of private instances of this
		meta-type that are not locally reachable, but are deeper
		in the hierarchy.  (computed by accumulation)
		This is how we create a properly sized and spaced 
		memory map.
		Only for processes' footprints can these values be non-zero, 
		because other structs only have public ports.  
		This can be post-computed after the last private map
		entry has been added.  
	 */
	size_t						_private_entries;
	/**
		Map entry (remains sorted) is amended each time a local
		structure is allocated.  key,value increase monotonically
		with each new entry, hence invariantly ordered.
	 */
	typedef	pair<size_t, size_t>			private_map_entry_type;
	/**
		Do a binary search on this sorted map to find the
		index of the local instance to descend into.
		key: lower bound of index (subtract this offset)
		value: index local instance to which it belongs.
		The intervals between entries of this map should correspond
		to the _private_entries sizes of the referenced local 
		instance' footprint.  
	 */
	vector<private_map_entry_type>			private_entry_map;
protected:
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

	footprint_base();

	// inline, uninitialized!
	explicit
	footprint_base(const util::uninitialized_tag_type&)
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		: _private_entries(0)
#endif
		{ }

	~footprint_base();

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	good_bool
	__allocate_global_state(state_manager&) const;
#endif

	// for process_tag ONLY!
	good_bool
	__expand_unique_subinstances(const footprint_frame&,
		state_manager&, const size_t) const;

	// for process_tag ONLY!
	good_bool
	__expand_production_rules(const footprint_frame&, 
		state_manager&) const;

	// for channel_tag ONLY!
	good_bool
	__set_channel_footprints(state_manager&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_reserve_sizes(ostream&) const;

	void
	load_reserve_sizes(istream&);

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class footprint_base

//=============================================================================
/**
	This wrapper class is in charge of allocation pools for
	value collections.  
	NOTE: also uses "pimpl" idiom.  
 */
template <class Tag>
class value_footprint_base {
	typedef	value_footprint_base<Tag>	this_type;
protected:
	typedef	value_collection_pool_bundle<Tag>
						collection_pool_bundle_type;
	const excl_ptr<collection_pool_bundle_type>
						collection_pool_bundle;
	value_footprint_base();

	// inline, uninitialized
	explicit
	value_footprint_base(const util::uninitialized_tag_type&) { }

	~value_footprint_base();

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class value_footprint_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_FOOTPRINT_BASE_H__

