/**
	\file "Object/inst/instance_collection.h"
	Class declarations for scalar instances and instance collections.  
	This file was originally "Object/art_object_instance_collection.h"
		in a previous life.  
	$Id: instance_collection.h,v 1.25.2.2 2006/10/21 20:08:17 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_H__

#include <iosfwd>
#include <set>

#include "Object/type/canonical_type_fwd.h"	// for conditional
#include "Object/traits/class_traits_fwd.h"
#include "Object/inst/physical_instance_collection.h"	// for macros
#include "Object/common/multikey_index.h"
#include "Object/devel_switches.h"
#if COLLECTION_SEPARATE_KEY_FROM_VALUE
#include "Object/inst/sparse_collection.h"
#endif
#include "util/STL/list_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"
#if !COLLECTION_SEPARATE_KEY_FROM_VALUE
#include "util/multikey_set.h"
#endif
#include "util/boolean_types.h"
#include "util/memory/chunk_map_pool_fwd.h"
#include "util/inttypes.h"

/**
	Define to 1 if you want instance_arrays and scalars pool-allocated.  
	Causes regression in one strange test case, needs debugging.  
 */
#define	POOL_ALLOCATE_INSTANCE_COLLECTIONS		1

namespace HAC {
namespace entity {
using std::list;
using std::default_list;
using std::istream;
using std::ostream;
using std::set;
using std::string;
using util::memory::count_ptr;
using util::memory::never_ptr;
using util::good_bool;
using util::bad_bool;
#if !COLLECTION_SEPARATE_KEY_FROM_VALUE
using util::default_multikey_set;
using util::multikey_set_element_derived;
#endif
using util::persistent;
using util::persistent_object_manager;

class scopespace;
class footprint;
class physical_instance_collection;
class meta_instance_reference_base;
class nonmeta_instance_reference_base;
class const_index_list;
class const_range_list;
class const_param_expr_list;
class unroll_context;
class subinstance_manager;
template <bool> class internal_aliases_policy;
template <class> class instantiation_statement;

//=============================================================================
#define	INSTANCE_COLLECTION_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	INSTANCE_COLLECTION_CLASS					\
instance_collection<Tag>

/**
	Interface to collection of instance aliases.  
	This abstract base class is dimension-generic.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
class instance_collection :
	public class_traits<Tag>::instance_collection_parent_type, 
	public class_traits<Tag>::collection_type_manager_parent_type {
friend	class class_traits<Tag>::collection_type_manager_parent_type;
friend	class subinstance_manager;
public:
	typedef	class_traits<Tag>			traits_type;
private:
	typedef	Tag					category_type;
	typedef	typename traits_type::instance_collection_parent_type
							parent_type;
	typedef	INSTANCE_COLLECTION_CLASS		this_type;
public:
	typedef	typename traits_type::type_ref_type	type_ref_type;
	typedef	typename traits_type::type_ref_ptr_type	type_ref_ptr_type;
	typedef	typename traits_type::resolved_type_ref_type
						resolved_type_ref_type;
	typedef	typename traits_type::collection_type_manager_parent_type
					collection_type_manager_parent_type;
	typedef	typename traits_type::instance_alias_info_type
						instance_alias_info_type;
	typedef	typename traits_type::instance_alias_base_type
						instance_alias_base_type;
	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	typedef	typename traits_type::alias_collection_type
						alias_collection_type;
	typedef	typename traits_type::instance_placeholder_type
					instance_placeholder_type;
	typedef	typename traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef	typename traits_type::simple_meta_instance_reference_type
					simple_meta_instance_reference_type;
	typedef	typename traits_type::simple_nonmeta_instance_reference_type
					simple_nonmeta_instance_reference_type;
	typedef	typename traits_type::member_simple_meta_instance_reference_type
				member_simple_meta_instance_reference_type;
//	typedef	meta_instance_reference_base		meta_instance_reference_base_type;
// public:
protected:
	typedef	typename parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	typename parent_type::member_inst_ref_ptr_type
						member_inst_ref_ptr_type;
	typedef	typename parent_type::instance_relaxed_actuals_type
						instance_relaxed_actuals_type;
	// type parameter, if applicable is inherited from
	// collection_type_manager_parent_type
	typedef	internal_aliases_policy<traits_type::can_internally_alias>
						internal_alias_policy;

public:
	typedef	typename traits_type::instantiation_statement_type
					initial_instantiation_statement_type;
	typedef	never_ptr<const instance_placeholder_type>
				instance_placeholder_ptr_type;
protected:
	/**
		This is a back-reference to the placeholder that resides
		in the scopespace, that contains basic collection information,
		prior to unrolling.
	 */
	instance_placeholder_ptr_type	source_placeholder;
protected:
	instance_collection();

	/// requires a back-reference to the source collection placeholder
	explicit
	instance_collection(const instance_placeholder_ptr_type);
public:

virtual	~instance_collection();

virtual	ostream&
	what(ostream&) const = 0;

	ostream&
	type_dump(ostream&) const;

#if COLLECTION_SEPARATE_KEY_FROM_VALUE
virtual	ostream&
	dump_element_key(ostream&, const instance_alias_info_type&) const = 0;

virtual	size_t
	lookup_index(const instance_alias_info_type&) const = 0;

virtual	instance_alias_info_type&
	get_corresponding_element(const this_type&,
		const instance_alias_info_type&) = 0;
#endif

	never_ptr<const physical_instance_placeholder>
	get_placeholder_base(void) const;

	instance_placeholder_ptr_type
	get_placeholder(void) const {
		return this->source_placeholder;
	}

virtual	bool
	is_partially_unrolled(void) const = 0;

	// this could just return hard-coded built-in type...
	// this returns the type as given by the first instantiation statement
	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const;

	using collection_type_manager_parent_type::get_resolved_canonical_type;

	bool
	must_be_collectibly_type_equivalent(const this_type&) const;

	// 2005-07-07: intended for first-time type establishment, 
	// which determines whether or not the collection is relaxed or 
	// strictly typed.  
	good_bool
	establish_collection_type(const instance_collection_parameter_type&);

	bool
	has_relaxed_type(void) const;

	// 2005-07-07: now intended for use AFTER collection type is established
	bad_bool
	check_established_type(const instance_collection_parameter_type&) const;

/**
	Prototype for instantiating alias indices during unroll phase.  
	NOTE: context shouldn't be necessary at the collection, 
	only needed to resolved placeholders!
 */
#define	INSTANTIATE_INDICES_PROTO					\
	good_bool							\
	instantiate_indices(const const_range_list& i, 			\
		const instance_relaxed_actuals_type&, 			\
		const unroll_context&)

virtual	INSTANTIATE_INDICES_PROTO = 0;

virtual	CONNECT_PORT_ALIASES_RECURSIVE_PROTO = 0;

protected:
virtual	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO = 0;

public:

	never_ptr<const const_param_expr_list>
	get_actual_param_list(void) const;

virtual instance_alias_base_ptr_type
	lookup_instance(const multikey_index_type& i) const = 0;

virtual	bool
	lookup_instance_collection(
		typename default_list<instance_alias_base_ptr_type>::type& l, 
		const const_range_list& r) const = 0;

virtual	const_index_list
	resolve_indices(const const_index_list& l) const = 0;

#define	UNROLL_ALIASES_PROTO						\
	bad_bool							\
	unroll_aliases(const multikey_index_type&, 			\
		const multikey_index_type&, 				\
		alias_collection_type&) const

virtual	UNROLL_ALIASES_PROTO = 0;

virtual	COLLECT_PORT_ALIASES_PROTO = 0;

virtual	CONSTRUCT_PORT_CONTEXT_PROTO = 0;

virtual	ASSIGN_FOOTPRINT_FRAME_PROTO = 0;

virtual	void
	accept(alias_visitor&) const = 0;

public:
virtual	instance_alias_base_type&
	load_reference(istream& i) = 0;

	static
	this_type*
	make_array(const instance_placeholder_ptr_type);

	static
	persistent*
	construct_empty(const int);

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class instance_collection

//-----------------------------------------------------------------------------
#define	INSTANCE_ARRAY_TEMPLATE_SIGNATURE				\
template <class Tag, size_t D>

#define	INSTANCE_SCALAR_TEMPLATE_SIGNATURE				\
template <class Tag>


#define	INSTANCE_ARRAY_CLASS						\
instance_array<Tag,D>

#define	INSTANCE_SCALAR_CLASS						\
instance_array<Tag,0>

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

#if COLLECTION_SEPARATE_KEY_FROM_VALUE
	typedef	instance_alias_base_type		element_type;
	/**
		The simple_type meta type is specially optimized and 
		simplified for D == 1.  
	 */
	typedef typename util::multikey<D, pint_value_type>::simple_type
							key_type;
	typedef	sparse_collection<key_type, element_type>	collection_type;
#else
	// template explicitly required by g++-4.0
	typedef	typename traits_type::template instance_alias<D>::type
							element_type;
	/**
		This is the data structure used to implement the collection.  
	 */
	typedef	typename default_multikey_set<D, element_type>::type
							collection_type;
	typedef	typename element_type::key_type		key_type;
#endif
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

#if COLLECTION_SEPARATE_KEY_FROM_VALUE
	ostream&
	dump_element_key(ostream&, const instance_alias_base_type&) const;

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

private:
	class element_collector;
	class element_writer;
	class element_loader;
	class connection_writer;
	class connection_loader;
	struct key_dumper;

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

//-----------------------------------------------------------------------------
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

	~instance_array();

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

#if COLLECTION_SEPARATE_KEY_FROM_VALUE
	ostream&
	dump_element_key(ostream&, const instance_alias_base_type&) const;

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

#undef	UNROLL_ALIASES_PROTO
#undef	INSTANTIATE_INDICES_PROTO

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_H__

