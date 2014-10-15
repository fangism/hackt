/**
	\file "Object/inst/port_actual_collection.hh"
	$Id: port_actual_collection.hh,v 1.8 2010/04/07 00:12:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PORT_ACTUAL_COLLECTION_H__
#define	__HAC_OBJECT_INST_PORT_ACTUAL_COLLECTION_H__

#include <valarray>
#include "Object/inst/collection_interface.hh"

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
/**
	We conservatively use a reference-count pointer for now, 
	though we are considering a never_ptr.  
	reference-counting should not result in cycles, as cyclic
	type references (specifically footprint) are forbidden and rejected.  
 */
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
	typedef	typename parent_type::collection_pool_bundle_type
					collection_pool_bundle_type;
public:
	// valarray iterator
	typedef	element_type*			iterator;
	typedef	const element_type*		const_iterator;
private:
	// super_instance? for now use instance_collection_base::super_instance
	formal_collection_ptr_type		formal_collection;
	array_type				value_array;
public:
	port_actual_collection();

	// partial copy constructor (shallow) for use with deep_copy
	// wants to be private-explicit,
	// but made public so vector/pool allocator works
	// explicit
	port_actual_collection(const this_type&);
public:
	port_actual_collection(const formal_collection_ptr_type, 
		const unroll_context&);

	~port_actual_collection();

	const formal_collection_type&
	get_canonical_collection(void) const;

	const formal_collection_ptr_type&
	get_formal_collection(void) const {
		return this->formal_collection;
	}

	size_t
	collection_size(void) const { return this->value_array.size(); }

	ostream&
	what(ostream&) const;

	ostream&
	type_dump(ostream&) const;

	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const;

	instance_placeholder_ptr_type
	get_placeholder(void) const;

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

	instance_alias_info_type&
	only_element(void) {
		INVARIANT(this->value_array.size() == 1);
		return this->value_array[0];
	}

	bool
	is_partially_unrolled(void) const;	// true

	bool
	is_formal(void) const;			// false

	ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const;

	INSTANTIATE_INDICES_PROTO;		// never call

	CONNECT_PORT_ALIASES_RECURSIVE_PROTO;
	RECONNECT_PORT_ALIASES_RECURSIVE_PROTO;
	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO;
//	DEEP_COPY_STRUCTURE_PROTO;

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

	void
	accept(alias_visitor&) const;

	instance_alias_info_type&
	load_reference(istream&);


	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_connections(const collection_pool_bundle_type&, ostream&) const;

	void
	load_connections(const collection_pool_bundle_type&, istream&);

	void
	write_pointer(ostream&, const collection_pool_bundle_type&) const;

	void
	write_object(const footprint&, 
		const persistent_object_manager&, ostream&) const;

	void
	load_object(footprint&, 
		const persistent_object_manager&, istream&);

public:
	iterator
	begin(void);

	const_iterator
	begin(void) const;

	iterator
	end(void);

	const_iterator
	end(void) const;

};	// end class port_actual_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PORT_ACTUAL_COLLECTION_H__

