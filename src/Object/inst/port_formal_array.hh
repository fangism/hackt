/**
	\file "Object/inst/port_formal_array.hh"
	Wrapper class around packed_array_generic.  
	$Id: port_formal_array.hh,v 1.11 2010/04/07 00:12:44 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PORT_FORMAL_ARRAY_HH__
#define	__HAC_OBJECT_INST_PORT_FORMAL_ARRAY_HH__

#include "Object/inst/instance_collection.hh"
#include "util/packed_array.hh"

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
	typedef	typename parent_type::const_instance_alias_info_ptr_type
					const_instance_alias_info_ptr_type;
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
	typedef	typename parent_type::collection_pool_bundle_type
					collection_pool_bundle_type;
	typedef	typename parent_type::port_actuals_type	port_actuals_type;
private:
	typedef	typename array_type::iterator	iterator;
	typedef	typename array_type::const_iterator	const_iterator;
private:
	array_type				value_array;
public:
	port_formal_array();
public:
	port_formal_array(const footprint&, 
		const instance_placeholder_ptr_type);

	~port_formal_array();

	ostream&
	what(ostream&) const;

	ostream&
	dump_element_key(ostream&, const instance_alias_info_type&) const;

	multikey_index_type
	lookup_key(const instance_alias_info_type&) const;

	ostream&
	dump_element_key(ostream&, const size_t) const;

	multikey_index_type
	lookup_key(const size_t) const;

	size_t
	lookup_index(const multikey_index_type&) const;

	size_t
	collection_size(void) const;

	size_t
	lookup_index(const instance_alias_info_type&) const;

	instance_alias_info_type&
	get_corresponding_element(const collection_interface_type&, 
		const instance_alias_info_type&);

	bool
	is_partially_unrolled(void) const;	// true

	bool
	is_formal(void) const;			// true

	ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const;

	INSTANTIATE_INDICES_PROTO;		// never call

	CONNECT_PORT_ALIASES_RECURSIVE_PROTO;
	RECONNECT_PORT_ALIASES_RECURSIVE_PROTO;
	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO;
	DEEP_COPY_STRUCTURE_PROTO;

	instance_alias_info_ptr_type
	lookup_instance(const multikey_index_type&) const;

	bool
	lookup_instance_collection(
		typename default_list<instance_alias_info_ptr_type>::type&, 
		const const_range_list&) const;

	const_index_list
	resolve_indices(const const_index_list&) const;

	GET_ALL_ALIASES_PROTO;
	UNROLL_ALIASES_PROTO;
	CREATE_DEPENDENT_TYPES_PROTO;
	COLLECT_PORT_ALIASES_PROTO;
	FINALIZE_SUBSTRUCTURE_ALIASES_PROTO;

	void
	accept(alias_visitor&) const;

	SET_ALIAS_CONNECTION_FLAGS_PROTO;

	instance_alias_info_type&
	load_reference(istream&);

	INSTANTIATE_ACTUALS_FROM_FORMALS_PROTO;

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

	void
	write_pointer(ostream&, const collection_pool_bundle_type&) const;

	void
	write_object(const footprint&, 
		const persistent_object_manager&, ostream&) const;

	void
	load_object(footprint&, 
		const persistent_object_manager&, istream&);

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_connections(const collection_pool_bundle_type&, ostream&) const;

	void
	load_connections(const collection_pool_bundle_type&, istream&);

};	// end class port_formal_array

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PORT_FORMAL_ARRAY_HH__

