/**
	\file "Object/inst/instance_collection_pool_bundle.h"
	$Id: instance_collection_pool_bundle.h,v 1.1.2.3 2006/10/31 21:15:56 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_H__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_H__

#include <iosfwd>
#include "util/size_t.h"
#include "util/persistent_fwd.h"
#include "Object/inst/collection_pool.h"
#include "util/memory/excl_ptr.h"

namespace HAC {
namespace entity {
using std::ostream;
using std::istream;
// forward declarations
class unroll_context;
template <class> class instance_placeholder;
template <class> class collection_interface;
template <class> class instance_collection;
template <class, size_t> class instance_array;
template <class Tag> class instance_array<Tag,0>;
template <class> class port_formal_array;
template <class> class port_actual_collection;
using util::persistent_object_manager;
using util::memory::never_ptr;

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
	typedef	collection_pool<T>		pool_type;
protected:
	pool_type				pool;
	// default ctors and dtor
	// non-copyable (guaranteed by pool_type)

	// serialization helper routines

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	void
	write_connections(const persistent_object_manager&, ostream&) const;

	void
	load_connections(const persistent_object_manager&, istream&);

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

	never_ptr<collection_interface_type>
	lookup_collection(const unsigned char, const unsigned short);

// allocation
	port_actual_collection<Tag>*
	allocate_port_collection(
		const never_ptr<const instance_collection<Tag> >, 
		const unroll_context&);

	instance_collection<Tag>*
	allocate_port_formal(
		const never_ptr<const instance_placeholder<Tag> >);

	instance_collection<Tag>*
	allocate_local_collection(
		const never_ptr<const instance_placeholder<Tag> >);

	// serialization routines
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
};	// end class instance_collection_pool_bundle

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_H__

