/**
	\file "Object/inst/port_formal_array.h"
	Wrapper class around packed_array_generic.  
	$Id: port_formal_array.h,v 1.2.2.4 2006/10/31 00:28:27 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PORT_FORMAL_ARRAY_H__
#define	__HAC_OBJECT_INST_PORT_FORMAL_ARRAY_H__

#include "Object/inst/instance_collection.h"
#include "util/packed_array.h"
#if !POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
#include "util/memory/chunk_map_pool_fwd.h"
#endif

namespace HAC {
namespace entity {
template <class>
class instance_alias_info;

//=============================================================================

#define	PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE	template <class Tag>
#define	PORT_FORMAL_ARRAY_CLASS			port_formal_array<Tag>

/**
	Dense container, for very efficient storage and retrieval, using
	multidimensional indices.  
	Based on util::packed_array_generic, whose internal implementation
	is a 1D vector.  

	This is intended for use as top-level instances in footprints, 
	but nothing prevents it from being used as general purposes
	dense collections.  
	This is meant to be used to unroll collections of port-formal
	instances local to a definition's footprint.  

	Subinstances should (recommended) be unrolled using the
	indirect: port_actuals_array, which back-links to a port_formal_array, 
	to avoid replicating data.  

	NOTE: this is not really necessary for scalar instances, just arrays.  
 */
template <class Tag>
class port_formal_array : public instance_collection<Tag> {
	typedef	port_formal_array		this_type;
public:
	typedef	class_traits<Tag>		traits_type;
	typedef	instance_collection<Tag>	parent_type;
	typedef	instance_alias_info<Tag>	instance_alias_info_type;
	typedef	instance_alias_info_type	element_type;
	typedef	util::packed_array_generic<
			pint_value_type, instance_alias_info_type>
						array_type;
	typedef	typename array_type::size_type	size_type;
	typedef	typename array_type::key_type	key_type;
	typedef	typename key_type::generator_type	key_generator_type;
	typedef	typename parent_type::instance_alias_info_ptr_type
						instance_alias_info_ptr_type;
	typedef	typename parent_type::internal_alias_policy
						internal_alias_policy;
	typedef	typename parent_type::collection_interface_type
						collection_interface_type;
	typedef	typename parent_type::collection_type_manager_parent_type
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
	typedef	typename array_type::iterator	iterator;
	typedef	typename array_type::const_iterator	const_iterator;
private:
	array_type				value_array;
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
public:
#else
private:
#endif
	port_formal_array();
public:
	explicit
	port_formal_array(const instance_placeholder_ptr_type);

#if 0
	port_formal_array(const instance_placeholder_ptr_type, const key_type&,
		const instance_collection_parameter_type&, 
		const count_ptr<const const_param_expr_list>&, 
		const unroll_context&);
#endif

	~port_formal_array();

	ostream&
	what(ostream&) const;

	ostream&
	dump_element_key(ostream&, const instance_alias_info_type&) const;

	multikey_index_type
	lookup_key(const instance_alias_info_type&) const;

#if USE_COLLECTION_INTERFACES
	ostream&
	dump_element_key(ostream&, const size_t) const;

	multikey_index_type
	lookup_key(const size_t) const;

	size_t
	lookup_index(const multikey_index_type&) const;

	size_t
	collection_size(void) const;
#endif

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

private:
	iterator
	begin(void);

	const_iterator
	begin(void) const;

	iterator
	end(void);

	const_iterator
	end(void) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_connections(const persistent_object_manager&, ostream&) const;

	void
	load_connections(const persistent_object_manager&, istream&);

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

};	// end class port_formal_array

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PORT_FORMAL_ARRAY_H__

