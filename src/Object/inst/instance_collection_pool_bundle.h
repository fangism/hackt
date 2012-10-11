/**
	\file "Object/inst/instance_collection_pool_bundle.h"
	$Id: instance_collection_pool_bundle.h,v 1.8 2010/04/07 00:12:41 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_H__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_H__

#include <iosfwd>
#include "Object/inst/collection_pool.h"
#include "Object/inst/collection_index_entry.h"
#include "util/boolean_types.h"
#include "util/size_t.h"
#include "util/persistent_fwd.h"
#include "util/memory/excl_ptr.h"

namespace HAC {
namespace entity {
using std::ostream;
using std::istream;
// forward declarations
class unroll_context;
class footprint;
class footprint_frame;
class port_alias_tracker;
template <class> class instance_placeholder;
template <class> class collection_interface;
template <class> class instance_collection;
template <class, size_t> class instance_array;
template <class Tag> class instance_array<Tag,0>;
template <class> class port_formal_array;
template <class> class port_actual_collection;
template <class> struct instance_collection_pool_bundle;
using util::persistent_object_manager;
using util::memory::never_ptr;
using util::good_bool;

//=============================================================================
/**
	Wrapper class for the sake of being able to reference a 
	pool member by class/type/tag.  
	Also re-usable for value_collections.  
 */
template <class T>
class instance_collection_pool_wrapper {
public:
	typedef	T				collection_type;
	typedef	typename collection_type::traits_type::tag_type
						tag_type;
	typedef	collection_pool<T>		pool_type;
	typedef	instance_collection_pool_bundle<tag_type>
						pool_bundle_type;
protected:
	typedef	typename pool_type::iterator		iterator;
	typedef	typename pool_type::const_iterator	const_iterator;
protected:
	pool_type				pool;
	// default ctors and dtor
	// non-copyable (guaranteed by pool_type)


	good_bool
	create_dependent_types(const footprint&);

	good_bool
	allocate_local_instance_ids(footprint&);

	void
	collect_scope_aliases(port_alias_tracker&) const;

	good_bool
	finalize_substructure_aliases(const unroll_context&);

private:
	// helper functors... "I want tr1/functional binders!!!"
	struct dependent_creator;
	struct index_allocator;
	struct scope_alias_collector;
	class footprint_frame_assigner;
	struct substructure_finalizer;

// serialization helper routines
protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_reserve_size(ostream&) const;

	void
	load_reserve_size(istream&);

	void
	write_object_base(const footprint&,
		const persistent_object_manager&, ostream&) const;

	void
	load_object_base(footprint&,
		const persistent_object_manager&, istream&);

	void
	write_connections(const pool_bundle_type&, ostream&) const;

	void
	load_connections(const pool_bundle_type&, istream&);

private:
	// helper functors... "I want tr1/functional binders!!!"
	struct collection_writer;
	struct collection_loader;
	struct connection_writer;
	struct connection_loader;

};	// end class sparse_pool_bundle

//=============================================================================
/**
	Convenient structure for bundling pools.  
	Each pool bundle contains a pool for each leaf class of
	instance collection: scalars, sparse and dense collections, etc...  
	This pool bundle is included in the footprint_base structure, 
	because footprints are now responsible for allocating
	instance collections (and value collections).  
	TODO: test definitions with enough names to make pools
		expand with pooled allocations (more than 4).  
 */
template <class Tag>
struct instance_collection_pool_bundle :
	public instance_collection_pool_wrapper<instance_array<Tag, 0> >, 
	public instance_collection_pool_wrapper<instance_array<Tag, 1> >, 
	public instance_collection_pool_wrapper<instance_array<Tag, 2> >, 
	public instance_collection_pool_wrapper<instance_array<Tag, 3> >, 
	public instance_collection_pool_wrapper<instance_array<Tag, 4> >, 
	public instance_collection_pool_wrapper<port_formal_array<Tag> >, 
	public instance_collection_pool_wrapper<port_actual_collection<Tag> > {

	/**
		This is the common base type to all collections, 
		handled by this pool manager.  
	 */
	typedef	collection_interface<Tag>	collection_interface_type;

	instance_collection_pool_bundle();
	~instance_collection_pool_bundle();

	/**
		\param S the type of the pool referenced.  S must be one 
			of the types managed by this pool.  
		\return the requested pool member.  
	 */
	template <class S>
	collection_pool<S>&
	get_collection_pool(void) {
		return instance_collection_pool_wrapper<S>::pool;
	}

	template <class S>
	const collection_pool<S>&
	get_collection_pool(void) const {
		return instance_collection_pool_wrapper<S>::pool;
	}

// translation
	never_ptr<collection_interface_type>
	lookup_collection(const unsigned char, 
		const collection_index_entry::index_type) const;

	// counterpart: collection_interface_type::write_pointer()
	collection_interface_type*
	read_pointer(istream&) const;

// allocation
	port_actual_collection<Tag>*
	allocate_port_collection(
		const never_ptr<const instance_collection<Tag> >, 
		const unroll_context&);

	instance_collection<Tag>*
	allocate_port_formal(footprint&, 
		const never_ptr<const instance_placeholder<Tag> >);

	instance_collection<Tag>*
	allocate_local_collection(footprint&, 
		const never_ptr<const instance_placeholder<Tag> >);

// unroll/creation/allocation
	good_bool
	create_dependent_types(const footprint&);

	good_bool
	allocate_local_instance_ids(footprint&);

	// iterate over port-pools (scalar and port_formal_array)
	void
	collect_scope_aliases(port_alias_tracker&) const;

	good_bool
	finalize_substructure_aliases(const unroll_context&);

// serialization routines
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_reserve_sizes(ostream&) const;

	void
	load_reserve_sizes(istream&);

	void
	write_object_base(const footprint&, 
		const persistent_object_manager&, ostream&) const;

	void
	load_object_base(footprint&, 
		const persistent_object_manager&, istream&);

};	// end struct instance_collection_pool_bundle

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_H__

