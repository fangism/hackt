/**
	\file "Object/inst/instance_scalar.hh"
	Class declarations for scalar instances and instance collections.  
	This contents of this file was split-off from 
		"Object/inst/instance_collection.h"
	$Id: instance_scalar.hh,v 1.10 2010/04/07 00:12:42 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_SCALAR_HH__
#define	__HAC_OBJECT_INST_INSTANCE_SCALAR_HH__

#include "Object/inst/instance_collection.hh"

namespace HAC {
namespace entity {
//=============================================================================
#define	INSTANCE_SCALAR_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	INSTANCE_SCALAR_CLASS						\
instance_array<Tag,0>

/**
	Scalar specialization of an instance collection.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
class instance_array<Tag,0> : public instance_collection<Tag> {
friend class instance_collection<Tag>;
	typedef	INSTANCE_SCALAR_CLASS			this_type;
public:
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::instance_collection_generic_type
							parent_type;
	typedef	typename parent_type::instance_relaxed_actuals_type
						instance_relaxed_actuals_type;
	typedef	typename parent_type::internal_alias_policy
						internal_alias_policy;
	typedef	typename parent_type::collection_interface_type
						collection_interface_type;
	typedef	typename traits_type::instance_alias_info_type
						instance_alias_info_type;
	typedef	typename traits_type::instance_alias_info_ptr_type
						instance_alias_info_ptr_type;
	typedef	typename parent_type::const_instance_alias_info_ptr_type
					const_instance_alias_info_ptr_type;
	typedef	typename traits_type::alias_collection_type
							alias_collection_type;
	typedef	instance_alias_info_type		instance_type;
	typedef	typename parent_type::collection_pool_bundle_type
					collection_pool_bundle_type;
	typedef	typename parent_type::collection_type_manager_parent_type
					collection_type_manager_parent_type;
	typedef	typename parent_type::instance_placeholder_type
					instance_placeholder_type;
	typedef	typename parent_type::instance_placeholder_ptr_type
					instance_placeholder_ptr_type;
	typedef	typename parent_type::port_actuals_type	port_actuals_type;
	enum { dimensions = 0 };
private:
	instance_type					the_instance;

public:
	instance_array();

public:
	instance_array(const footprint&, const instance_placeholder_ptr_type);

	~instance_array();

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

	bool
	is_formal(void) const;			// depends

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

	ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const;

	INSTANTIATE_INDICES_PROTO;

	CONNECT_PORT_ALIASES_RECURSIVE_PROTO;
	RECONNECT_PORT_ALIASES_RECURSIVE_PROTO;

	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO;
	DEEP_COPY_STRUCTURE_PROTO;

	instance_alias_info_ptr_type
	lookup_instance(const multikey_index_type& l) const;

	bool
	lookup_instance_collection(
		typename default_list<instance_alias_info_ptr_type>::type& l, 
		const const_range_list& r) const;

	UNROLL_ALIASES_PROTO;

	instance_alias_info_type&
	load_reference(istream& i);

	const_index_list
	resolve_indices(const const_index_list& l) const;

	instance_type&
	get_the_instance(void) { return this->the_instance; }

	CREATE_DEPENDENT_TYPES_PROTO;

	COLLECT_PORT_ALIASES_PROTO;

	FINALIZE_SUBSTRUCTURE_ALIASES_PROTO;

	GET_ALL_ALIASES_PROTO;

	void
	accept(alias_visitor&) const;

	SET_ALIAS_CONNECTION_FLAGS_PROTO;

	INSTANTIATE_ACTUALS_FROM_FORMALS_PROTO;
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

};	// end class instance_array (specialized)

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_SCALAR_HH__

