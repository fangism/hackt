/**
	\file "Object/inst/instance_array.hh"
	Class declarations for scalar instances and instance collections.  
	This file was originally "Object/art_object_instance_collection.h"
		in a previous life.  
	$Id: instance_array.hh,v 1.10 2010/04/07 00:12:40 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_ARRAY_HH__
#define	__HAC_OBJECT_INST_INSTANCE_ARRAY_HH__

#include "Object/inst/instance_collection.hh"
#include "Object/inst/sparse_collection.hh"

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
class instance_array : public instance_collection<Tag> {
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
	typedef	typename traits_type::instance_alias_info_type
						instance_alias_info_type;
	typedef	typename traits_type::instance_alias_info_ptr_type
						instance_alias_info_ptr_type;
	typedef	typename parent_type::const_instance_alias_info_ptr_type
					const_instance_alias_info_ptr_type;
	typedef	typename traits_type::alias_collection_type
							alias_collection_type;
	typedef	typename parent_type::collection_interface_type
						collection_interface_type;
	typedef	instance_alias_info_type		element_type;
	typedef instance_collection_pool_bundle<Tag>
					collection_pool_bundle_type;
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
	typedef	typename parent_type::port_actuals_type	port_actuals_type;
	typedef	typename util::multikey<D, pint_value_type>::generator_type
							key_generator_type;
	typedef	element_type&				reference;
	typedef	typename collection_type::iterator	iterator;
	typedef	typename collection_type::const_iterator
							const_iterator;
private:
	collection_type					collection;
public:
	instance_array();

	instance_array(const this_type&);

public:
	instance_array(const footprint&, const instance_placeholder_ptr_type);

	~instance_array();

	ostream&
	what(ostream& o) const;

	bool
	is_partially_unrolled(void) const;

	bool
	is_formal(void) const;		// false

	ostream&
	dump_element_key(ostream&, const instance_alias_info_type&) const;

	ostream&
	dump_element_key(ostream&, const size_t) const;

	multikey_index_type
	lookup_key(const size_t) const;

	size_t
	lookup_index(const multikey_index_type&) const;

	size_t
	collection_size(void) const;

	multikey_index_type
	lookup_key(const instance_alias_info_type&) const;

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

	const_index_list
	resolve_indices(const const_index_list& l) const;

	instance_alias_info_ptr_type
	lookup_instance(const multikey_index_type& l) const;

	never_ptr<element_type>
	operator [] (const key_type&) const;

	// is this used? or can it be replaced by unroll_aliases?
	bool
	lookup_instance_collection(
		typename default_list<instance_alias_info_ptr_type>::type& l, 
		const const_range_list& r) const;

	UNROLL_ALIASES_PROTO;

	instance_alias_info_type&
	load_reference(istream& i);

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
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_pointer(ostream&, 
		const collection_pool_bundle_type&) const;

	void
	write_object(const footprint&, 
		const persistent_object_manager&, ostream&) const;

	void
	load_object(footprint&, 
		const persistent_object_manager&, istream&);

	void
	write_connections(const collection_pool_bundle_type&, 
		ostream&) const;

	void
	load_connections(const collection_pool_bundle_type&, 
		istream&);

};	// end class instance_array

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_ARRAY_HH__

