/**
	\file "Object/inst/value_array.h"
	$Id: value_array.h,v 1.2.2.1 2006/10/31 05:23:52 fang Exp $
	This fail spawned from:
	Id: value_collection.h,v 1.19.2.1 2006/10/22 08:03:28 fang Exp
 */

#ifndef	__HAC_OBJECT_INST_VALUE_ARRAY_H__
#define	__HAC_OBJECT_INST_VALUE_ARRAY_H__

#include "Object/inst/value_collection.h"

#include "util/multikey_map.h"
#if !POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
#include "util/new_functor_fwd.h"
#include "util/memory/chunk_map_pool_fwd.h"
#endif

namespace HAC {
namespace entity {
// template <class> class param_instantiation_statement;
using util::default_multikey_map;

//=============================================================================
#define VALUE_ARRAY_TEMPLATE_SIGNATURE					\
template <class Tag, size_t D>

#define	VALUE_ARRAY_CLASS						\
value_array<Tag,D>

/**
	Dimension-specific array of parameters.
	NOTE: no need to separate key from value in map because
	these value collections are not heavily replicated.
 */
VALUE_ARRAY_TEMPLATE_SIGNATURE
class value_array : public value_collection<Tag> {
private:
	typedef VALUE_ARRAY_CLASS			this_type;
	typedef	value_collection<Tag>			parent_type;
friend class value_collection<Tag>;
public:
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::value_type	value_type;
	typedef	typename traits_type::instance_type	element_type;

	// later change this to multikey_set or not?
	/// Type for actual values, including validity and status.
private:
	typedef	default_multikey_map<D, pint_value_type, element_type>
							__helper_map_type;
	typedef	typename __helper_map_type::type	map_type;
public:
	typedef	util::multikey_map<D, pint_value_type, element_type, map_type>
							collection_type;
	typedef	typename collection_type::key_type	key_type;
	typedef	typename traits_type::const_collection_type
							const_collection_type;
	typedef	typename traits_type::value_reference_collection_type
					value_reference_collection_type;
	typedef	typename parent_type::value_placeholder_type
							value_placeholder_type;
	typedef typename parent_type::value_placeholder_ptr_type
						value_placeholder_ptr_type;
private:
	/// the collection of boolean instances
	collection_type					collection;
	// value cache is not persistent
	const_collection_type				cached_values;
	// tracking validity and density of the value cache?
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
public:
#endif
	value_array();

public:
	explicit
	value_array(const value_placeholder_ptr_type);

	~value_array();

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_values(ostream& o) const;

	// update this to accept const_range_list instead
	good_bool
	instantiate_indices(const const_range_list&);

	const_index_list
	resolve_indices(const const_index_list& l) const;

	LOOKUP_VALUE_INDEXED_PROTO;

	UNROLL_LVALUE_REFERENCES_PROTO;

	/// helper functor for dumping values
	struct key_value_dumper {
		ostream& os;
		key_value_dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const typename collection_type::value_type&);
	};      // end struct key_value_dumper

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
#if POOL_ALLOCATE_VALUE_COLLECTIONS
	enum {
#ifdef	HAVE_UINT64_TYPE
		pool_chunk_size = 64
#else
		pool_chunk_size = 32
#endif
	};
	CHUNK_MAP_POOL_ROBUST_STATIC_DECLARATIONS(pool_chunk_size)
#endif
};	// end class value_array

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_VALUE_ARRAY_H__

