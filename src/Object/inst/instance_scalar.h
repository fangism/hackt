/**
	\file "Object/inst/instance_scalar.h"
	Class declarations for scalar instances and instance collections.  
	This contents of this file was split-off from 
		"Object/inst/instance_collection.h"
	$Id: instance_scalar.h,v 1.1.2.2 2006/10/23 06:51:17 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_SCALAR_H__
#define	__HAC_OBJECT_INST_INSTANCE_SCALAR_H__

#include "Object/inst/instance_collection.h"
// #include "Object/type/canonical_type_fwd.h"	// for conditional
#include "util/memory/chunk_map_pool_fwd.h"

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
class instance_array<Tag,0> :
		public class_traits<Tag>::instance_collection_generic_type {
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
	typedef	typename traits_type::instance_alias_base_type
						instance_alias_base_type;
	typedef	typename traits_type::instance_alias_base_ptr_type
						instance_alias_base_ptr_type;
	typedef	typename traits_type::alias_collection_type
							alias_collection_type;
#if COLLECTION_SEPARATE_KEY_FROM_VALUE
	typedef	instance_alias_base_type		instance_type;
#else
	// template explicitly required by g++-4.0
	typedef	typename traits_type::template instance_alias<0>::type
							instance_type;
#endif
	typedef	typename parent_type::collection_type_manager_parent_type
					collection_type_manager_parent_type;
#if DENSE_FORMAL_COLLECTIONS
	typedef	typename parent_type::instance_collection_parameter_type
					instance_collection_parameter_type;
#endif
	typedef	typename parent_type::instance_placeholder_type
					instance_placeholder_type;
	typedef	typename parent_type::instance_placeholder_ptr_type
					instance_placeholder_ptr_type;
	enum { dimensions = 0 };
private:
	instance_type					the_instance;

private:
	instance_array();

public:
	explicit
	instance_array(const instance_placeholder_ptr_type);

#if DENSE_FORMAL_COLLECTIONS
	instance_array(const instance_placeholder_ptr_type, 
		const instance_collection_parameter_type&, 
		const count_ptr<const const_param_expr_list>&, 
		const unroll_context&);
#endif

	~instance_array();

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

#if COLLECTION_SEPARATE_KEY_FROM_VALUE
	ostream&
	dump_element_key(ostream&, const instance_alias_base_type&) const;

	multikey_index_type
	lookup_key(const instance_alias_base_type&) const;

	size_t
	lookup_index(const instance_alias_base_type&) const;

	instance_alias_base_type&
	get_corresponding_element(const parent_type&,
		const instance_alias_base_type&);
#endif

	ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const;

	INSTANTIATE_INDICES_PROTO;

	CONNECT_PORT_ALIASES_RECURSIVE_PROTO;

	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO;

	instance_alias_base_ptr_type
	lookup_instance(const multikey_index_type& l) const;

	bool
	lookup_instance_collection(
		typename default_list<instance_alias_base_ptr_type>::type& l, 
		const const_range_list& r) const;

	UNROLL_ALIASES_PROTO;

	instance_alias_base_type&
	load_reference(istream& i);

	const_index_list
	resolve_indices(const const_index_list& l) const;

#if COLLECTION_SEPARATE_KEY_FROM_VALUE
	instance_type&
#else
	typename instance_type::parent_type&
#endif
	get_the_instance(void) { return this->the_instance; }

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
};	// end class instance_array (specialized)

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_SCALAR_H__
