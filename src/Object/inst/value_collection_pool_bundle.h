/**
	\file "Object/inst/value_collection_pool_bundle.h"
	$Id: value_collection_pool_bundle.h,v 1.1.2.2 2006/10/31 21:16:02 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_VALUE_COLLECTION_POOL_BUNDLE_H__
#define	__HAC_OBJECT_INST_VALUE_COLLECTION_POOL_BUNDLE_H__

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
template <class> class value_collection;
template <class, size_t> class value_array;
template <class Tag> class value_array<Tag,0>;
using util::persistent_object_manager;
using util::memory::never_ptr;

//=============================================================================
/**
	Wrapper class for the sake of being able to reference a 
	pool member by class/type/tag.  
	Also re-usable for value_collections.  
 */
template <class T>
class value_collection_pool_wrapper {
public:
	typedef	T				collection_type;
	typedef	collection_pool<T>		pool_type;
protected:
	pool_type				pool;
	// default ctors and dtor
	// non-copyable (guaranteed by pool_type)

	// serialization helper routines

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

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
struct value_collection_pool_bundle :
	public value_collection_pool_wrapper<value_array<Tag, 0> >, 
	public value_collection_pool_wrapper<value_array<Tag, 1> >, 
	public value_collection_pool_wrapper<value_array<Tag, 2> >, 
	public value_collection_pool_wrapper<value_array<Tag, 3> >, 
	public value_collection_pool_wrapper<value_array<Tag, 4> >
//	public value_collection_pool_wrapper<value_formal_array<Tag> >
{
	typedef	value_collection<Tag>		collection_base_type;

	value_collection_pool_bundle();
	~value_collection_pool_bundle();

	/**
		\param S the type of the pool referenced.  S must be one 
			of the types managed by this pool.  
		\return the requested pool member.  
	 */
	template <class S>
	collection_pool<S>&
	get_collection_pool(void) {
		return value_collection_pool_wrapper<S>::pool;
	}

	never_ptr<collection_base_type>
	lookup_collection(const unsigned char, const unsigned short);

	// serialization routines
	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
};	// end class value_collection_pool_bundle

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_VALUE_COLLECTION_POOL_BUNDLE_H__

