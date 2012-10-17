/**
	\file "Object/def/footprint_base.hh"
	Data structure for each complete type's footprint template.  
	$Id: footprint_base.hh,v 1.7 2010/04/07 00:12:35 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_FOOTPRINT_BASE_H__
#define	__HAC_OBJECT_DEF_FOOTPRINT_BASE_H__

#include <iosfwd>
// #include "Object/inst/instance_pool.hh"
// #include "Object/inst/state_instance.hh"
#include "util/boolean_types.hh"
#include "util/persistent_fwd.hh"
#include "util/memory/excl_ptr.hh"
#include "util/uninitialized.hh"

namespace HAC {
namespace entity {
class state_manager;
class footprint_frame;
template <class> class state_instance;
template <class> class instance_pool;
template <class> struct instance_collection_pool_bundle;
template <class> struct value_collection_pool_bundle;
class port_alias_tracker;
using std::istream;
using std::ostream;
using util::good_bool;
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
#if 0
	typedef	typename state_instance<Tag>::pool_type	instance_pool_type;
#else
	typedef	instance_pool<state_instance<Tag> >	instance_pool_type;
#endif
	typedef	instance_collection_pool_bundle<Tag>
					collection_pool_bundle_type;
protected:
	const excl_ptr<collection_pool_bundle_type>	collection_pool_bundle;
	/**
		This is where final unique instances are allocated.
	 */
	const excl_ptr<instance_pool_type>		_instance_pool;

	footprint_base();

	// inline, uninitialized!
	explicit
	footprint_base(const util::uninitialized_tag_type&) { }

	~footprint_base();

	// for process_tag ONLY!
	good_bool
	__expand_unique_subinstances(
		void
		// const port_alias_tracker&, const footprint_frame&
		);

	void
	__partition_local_instance_pool(const port_alias_tracker&);

	// for process_tag ONLY!
	good_bool
	__expand_production_rules(const footprint_frame&, 
		state_manager&) const;

	// for channel_tag ONLY!
	void
	__append_private_map_entry(const this_type&, const size_t);

	good_bool
	__set_channel_footprints(void);

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

