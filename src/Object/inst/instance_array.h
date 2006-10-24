/**
	\file "Object/inst/instance_array.h"
	Class declarations for scalar instances and instance collections.  
	This file was originally "Object/art_object_instance_collection.h"
		in a previous life.  
	$Id: instance_array.h,v 1.2 2006/10/24 07:27:11 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_ARRAY_H__
#define	__HAC_OBJECT_INST_INSTANCE_ARRAY_H__

#include "Object/inst/instance_collection.h"
#include "Object/inst/sparse_collection.h"
#include "util/memory/chunk_map_pool_fwd.h"

namespace HAC {
namespace entity {

//=============================================================================
#define	INSTANCE_ARRAY_TEMPLATE_SIGNATURE				\
template <class Tag, size_t D>

#define	INSTANCE_ARRAY_CLASS						\
instance_array<Tag,D>

/**
	Multidimensional collection of int instance aliases.  
	\param D the number of dimensions (max. 4).  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
class instance_array :
	// this is the same as instance_collection<Tag>
	public class_traits<Tag>::instance_collection_generic_type {
friend class instance_collection<Tag>;
	typedef	instance_array<Tag,D>			this_type;
public:
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::instance_collection_generic_type
							parent_type;
	typedef	typename parent_type::instance_relaxed_actuals_type
						instance_relaxed_actuals_type;
	typedef	typename parent_type::internal_alias_policy
						internal_alias_policy;
	typedef	typename traits_type::instance_alias_base_type
						instance_alias_base_type;
//	typedef	typename parent_type::instance_alias_base_ptr_type
	typedef	typename traits_type::instance_alias_base_ptr_type
						instance_alias_base_ptr_type;
	typedef	typename traits_type::alias_collection_type
							alias_collection_type;

	typedef	instance_alias_base_type		element_type;
	/**
		The simple_type meta type is specially optimized and 
		simplified for D == 1.  
	 */
	typedef typename util::multikey<D, pint_value_type>::simple_type
							key_type;
	typedef	sparse_collection<key_type, element_type>	collection_type;
	typedef	typename collection_type::value_type	value_type;
	typedef	typename parent_type::collection_type_manager_parent_type
					collection_type_manager_parent_type;
	typedef	typename parent_type::instance_placeholder_type
					instance_placeholder_type;
	typedef	typename parent_type::instance_placeholder_ptr_type
					instance_placeholder_ptr_type;
	enum { dimensions = D };
private:
	typedef	typename util::multikey<D, pint_value_type>::generator_type
							key_generator_type;
	typedef	element_type&				reference;
	typedef	typename collection_type::iterator	iterator;
	typedef	typename collection_type::const_iterator
							const_iterator;
private:
	collection_type					collection;
private:
	instance_array();

public:
	explicit
	instance_array(const instance_placeholder_ptr_type);

	~instance_array();

	ostream&
	what(ostream& o) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_element_key(ostream&, const instance_alias_base_type&) const;

	multikey_index_type
	lookup_key(const instance_alias_base_type&) const;

	size_t
	lookup_index(const instance_alias_base_type&) const;

	instance_alias_base_type&
	get_corresponding_element(const parent_type&,
		const instance_alias_base_type&);

	ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const;

	INSTANTIATE_INDICES_PROTO;

	CONNECT_PORT_ALIASES_RECURSIVE_PROTO;

	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO;

	const_index_list
	resolve_indices(const const_index_list& l) const;

	instance_alias_base_ptr_type
	lookup_instance(const multikey_index_type& l) const;

	never_ptr<element_type>
	operator [] (const key_type&) const;

	// is this used? or can it be replaced by unroll_aliases?
	bool
	lookup_instance_collection(
		typename default_list<instance_alias_base_ptr_type>::type& l, 
		const const_range_list& r) const;

	UNROLL_ALIASES_PROTO;

	instance_alias_base_type&
	load_reference(istream& i);

	CREATE_DEPENDENT_TYPES_PROTO;

	COLLECT_PORT_ALIASES_PROTO;

	CONSTRUCT_PORT_CONTEXT_PROTO;

	ASSIGN_FOOTPRINT_FRAME_PROTO;

	void
	accept(alias_visitor&) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
#if POOL_ALLOCATE_INSTANCE_COLLECTIONS
	enum {
#ifdef	HAVE_UINT64_TYPE
		pool_chunk_size = 64
#else
		pool_chunk_size = 32
#endif
	};
	CHUNK_MAP_POOL_ROBUST_STATIC_DECLARATIONS(pool_chunk_size)
#endif
};	// end class instance_array

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_ARRAY_H__

