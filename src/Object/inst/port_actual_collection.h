/**
	\file "Object/inst/port_actual_collection.h"
	$Id: port_actual_collection.h,v 1.1.2.1 2006/10/28 03:03:10 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PORT_ACTUAL_COLLECTION_H__
#define	__HAC_OBJECT_INST_PORT_ACTUAL_COLLECTION_H__

#include <valarray>
#include "Object/inst/collection_interface.h"

namespace HAC {
namespace entity {
template <class>
class instance_alias_info;

template <class>
class instance_collection;

//=============================================================================

#define	PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE	template <class Tag>
#define	PORT_ACTUAL_COLLECTION_CLASS		port_actual_collection<Tag>

/**
	This is a lightweight subinstance collection that contains
	a plain array of instance_aliases.  
	The interpretation of the array is determined by the formal collection
	to which this corresponds -- it may be scalar or multidimensional.  
	(It could even be sparse, but the language precludes this.)
	The formal collection is referenced with a pointer.  
	This effectively avoids replicating structural information.  

	This structure is allocation-critical!  Allocations are not done 
	directly by the heap -- instead, they are pool-allocated and mapped
	by the footprint in-charge, be it top-level or definition-local.  
 */
template <class Tag>
class port_actual_collection : public collection_interface<Tag> {
	typedef	port_actual_collection		this_type;
public:
	typedef	class_traits<Tag>		traits_type;
	typedef	collection_interface<Tag>	parent_type;
	typedef	parent_type			collection_interface_type;
	typedef	instance_collection<Tag>	formal_collection_type;
	typedef	never_ptr<const formal_collection_type>
						formal_collection_ptr_type;
	typedef	instance_alias_info<Tag>	instance_alias_info_type;
	typedef	instance_alias_info_type	element_type;
	typedef	std::valarray<instance_alias_info_type>
						array_type;
//	typedef	typename array_type::size_type	size_type;
	typedef	size_t				size_type;
//	typedef	typename array_type::key_type	key_type;
//	typedef	typename key_type::generator_type	key_generator_type;
	typedef	typename parent_type::instance_alias_info_ptr_type
						instance_alias_info_ptr_type;
	typedef	typename parent_type::internal_alias_policy
						internal_alias_policy;
	typedef	typename traits_type::collection_type_manager_parent_type
					collection_type_manager_parent_type;
	typedef	typename parent_type::instance_placeholder_ptr_type
						instance_placeholder_ptr_type;
	typedef	typename traits_type::alias_collection_type
						alias_collection_type;
	typedef	typename parent_type::instance_relaxed_actuals_type
						instance_relaxed_actuals_type;
	typedef	typename traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
private:
#if 0
	typedef	typename array_type::iterator	iterator;
	typedef	typename array_type::const_iterator	const_iterator;
#else
	// valarray iterator
	typedef	element_type*			iterator;
	typedef	const element_type*		const_iterator;
#endif
private:
	// super_instance? for now use instance_collection_base::super_instance
	formal_collection_ptr_type		formal_collection;
	array_type				value_array;
private:
	port_actual_collection();
public:
	explicit
	port_actual_collection(const formal_collection_ptr_type);

	~port_actual_collection();

	ostream&
	what(ostream&) const;

	ostream&
	type_dump(ostream&) const;

	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const;

	never_ptr<const physical_instance_placeholder>
	get_placeholder_base(void) const;

	ostream&
	dump_element_key(ostream&, const instance_alias_info_type&) const;

	ostream&
	dump_element_key(ostream&, const size_t) const;

	multikey_index_type
	lookup_key(const size_t) const;

	multikey_index_type
	lookup_key(const instance_alias_info_type&) const;

	size_t
	lookup_index(const instance_alias_info_type&) const;

	instance_alias_info_type&
	get_corresponding_element(const collection_interface_type&, 
		const instance_alias_info_type&);

	bool
	is_partially_unrolled(void) const;	// true

	ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const;

	INSTANTIATE_INDICES_PROTO;		// never call

	CONNECT_PORT_ALIASES_RECURSIVE_PROTO;
	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO;

	instance_alias_info_ptr_type
	lookup_instance(const multikey_index_type&) const;

	bool
	lookup_instance_collection(
		typename default_list<instance_alias_info_ptr_type>::type&, 
		const const_range_list&) const;

	const_index_list
	resolve_indices(const const_index_list&) const;

	UNROLL_ALIASES_PROTO;
	CREATE_DEPENDENT_TYPES_PROTO;
	COLLECT_PORT_ALIASES_PROTO;
	CONSTRUCT_PORT_CONTEXT_PROTO;
	ASSIGN_FOOTPRINT_FRAME_PROTO;

	void
	accept(alias_visitor&) const;

	instance_alias_info_type&
	load_reference(istream&);

	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC

private:
	iterator
	begin(void);

	const_iterator
	begin(void) const;

	iterator
	end(void);

	const_iterator
	end(void) const;

#if 0
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
#endif
};	// end class port_actual_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PORT_ACTUAL_COLLECTION_H__

