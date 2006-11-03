/**
	\file "Object/def/footprint_base.h"
	Data structure for each complete type's footprint template.  
	$Id: footprint_base.h,v 1.1.2.1 2006/11/03 07:07:28 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_FOOTPRINT_BASE_H__
#define	__HAC_OBJECT_DEF_FOOTPRINT_BASE_H__

#include <iosfwd>
#include "Object/devel_switches.h"
#include "Object/inst/instance_pool.h"
#include "Object/inst/state_instance.h"

#include "util/persistent_fwd.h"
#include "util/memory/excl_ptr.h"

namespace HAC {
namespace entity {
class state_manager;
class footprint_frame;
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
template <class> class instance_collection_pool_bundle;
template <class> class value_collection_pool_bundle;
#endif

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
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
	typedef	instance_collection_pool_bundle<Tag>
					collection_pool_bundle_type;
#endif
private:
	typedef	typename instance_pool_type::const_iterator	const_iterator;
protected:
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
	const excl_ptr<collection_pool_bundle_type>	collection_pool_bundle;
#endif
	const excl_ptr<instance_pool_type>		_instance_pool;

	footprint_base();
	~footprint_base();

	good_bool
	__allocate_global_state(state_manager&) const;

	good_bool
	__expand_unique_subinstances(const footprint_frame&,
		state_manager&, const size_t) const;

	good_bool
	__expand_production_rules(const footprint_frame&, 
		state_manager&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class footprint_base

//=============================================================================
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
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
	~value_footprint_base();

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class value_footprint_base
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_FOOTPRINT_BASE_H__

