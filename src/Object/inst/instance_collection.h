/**
	\file "Object/inst/instance_collection.h"
	Class declarations for scalar instances and instance collections.  
	This file was originally "Object/art_object_instance_collection.h"
		in a previous life.  
	$Id: instance_collection.h,v 1.12.6.1 2006/02/19 03:52:59 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_H__

#include <iosfwd>
#include <set>

#include "Object/type/canonical_type_fwd.h"	// for conditional
#include "Object/traits/class_traits_fwd.h"
#include "Object/inst/physical_instance_collection.h"	// for macros
#include "Object/common/multikey_index.h"
#include "util/STL/list_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"
#include "util/multikey_set.h"
#include "util/boolean_types.h"

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
using util::multikey_set;
using util::multikey_set_element_derived;
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
// friend struct collection_type_manager<Tag>;
// temporary workaround until int's type is better integrated
friend	class class_traits<Tag>::collection_type_manager_parent_type;
friend	class subinstance_manager;
public:
	typedef	class_traits<Tag>			traits_type;
private:
	typedef	Tag					category_type;
	typedef	typename traits_type::instance_collection_parent_type
							parent_type;
	typedef	INSTANCE_COLLECTION_CLASS		this_type;
// friend void subinstance_manager::unroll_port_instances(const this_type&);
public:
	typedef	typename traits_type::type_ref_type	type_ref_type;
	typedef	typename traits_type::type_ref_ptr_type	type_ref_ptr_type;
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
	typedef	never_ptr<const initial_instantiation_statement_type>
				initial_instantiation_statement_ptr_type;
	/**
		All collections track the first instantiation statement,
		for the sake of deducing the type.  
		Scalars instance collections need this too because
		of the possibility of relaxed template arguments.  
	 */
	initial_instantiation_statement_ptr_type
					initial_instantiation_statement_ptr;
protected:
	explicit
	instance_collection(const size_t d) :
		parent_type(d), collection_type_manager_parent_type(),
		initial_instantiation_statement_ptr(NULL) { }

	instance_collection(const this_type&, const footprint&);

private:
virtual	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO = 0;

public:
	instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~instance_collection();

virtual	ostream&
	what(ostream&) const = 0;

	ostream&
	type_dump(ostream&) const;

	void
	attach_initial_instantiation_statement(
		const initial_instantiation_statement_ptr_type i) {
		NEVER_NULL(i);
		if (!initial_instantiation_statement_ptr)
			initial_instantiation_statement_ptr = i;
		// else skip
	}

	/**
		CAVEAT: this statement could be conditional.  
	 */
	initial_instantiation_statement_ptr_type
	get_initial_instantiation_statement(void) const {
		return initial_instantiation_statement_ptr;
	}

	index_collection_item_ptr_type
	get_initial_instantiation_indices(void) const;

virtual	bool
	is_partially_unrolled(void) const = 0;

	// this could just return hard-coded built-in type...
	// this returns the type as given by the first instantiation statement
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	type_ref_ptr_type
	get_type_ref_subtype(void) const;

	bool
	must_match_type(const this_type&) const;

	// 2005-07-07: intended for first-time type establishment, 
	// which determines whether or not the collection is relaxed or 
	// strictly typed.  
	void
	establish_collection_type(const instance_collection_parameter_type&);

	bool
	has_relaxed_type(void) const;

	// 2005-07-07: now intended for use AFTER collection type is established
	bad_bool
	check_established_type(const instance_collection_parameter_type&) const;

	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const;

	count_ptr<nonmeta_instance_reference_base>
	make_nonmeta_instance_reference(void) const;

	member_inst_ref_ptr_type
	make_member_meta_instance_reference(const inst_ref_ptr_type&) const;

/**
	Prototype for instantiating alias indices during unroll phase.  
 */
#define	INSTANTIATE_INDICES_PROTO					\
	good_bool							\
	instantiate_indices(const const_range_list& i, 			\
		const instance_relaxed_actuals_type&, 			\
		const unroll_context&)

virtual	INSTANTIATE_INDICES_PROTO = 0;

/**
	Prototype for allocating unique state during create phase.
 */
#define	CREATE_UNIQUE_STATE_PROTO					\
	good_bool							\
	create_unique_state(const const_range_list&, footprint&)

virtual	CREATE_UNIQUE_STATE_PROTO = 0;

virtual	good_bool
	allocate_state(footprint&) = 0;

virtual	good_bool
	merge_created_state(physical_instance_collection&, footprint&) = 0;

virtual	void
	inherit_created_state(const physical_instance_collection&, 
		const footprint&) = 0;

virtual	good_bool
	synchronize_actuals(physical_instance_collection&) = 0;

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

virtual	CFLAT_ALIASES_PROTO = 0;

#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
virtual	HACK_REMAP_INDICES_PROTO = 0;
#endif

public:
virtual	instance_alias_base_type&
	load_reference(istream& i) const = 0;

	static
	this_type*
	make_array(const scopespace& o, const string& n, const size_t d);

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
	// template explicitly required by g++-4.0
	typedef	typename traits_type::template instance_alias<D>::type
							element_type;
	/**
		This is the data structure used to implement the collection.  
	 */
	typedef	multikey_set<D, element_type>		collection_type;
	typedef	typename element_type::key_type		key_type;
	typedef	typename collection_type::value_type	value_type;
	typedef	typename parent_type::collection_type_manager_parent_type
					collection_type_manager_parent_type;
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

	instance_array(const this_type&, const footprint&);

	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO;

public:
	instance_array(const scopespace& o, const string& n);
	~instance_array();

	ostream&
	what(ostream& o) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const;

	INSTANTIATE_INDICES_PROTO;

	CREATE_UNIQUE_STATE_PROTO;

	good_bool
	allocate_state(footprint&);

	good_bool
	merge_created_state(physical_instance_collection&, footprint&);

	void
	inherit_created_state(const physical_instance_collection&, 
		const footprint&);

	good_bool
	synchronize_actuals(physical_instance_collection&);

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

	UNROLL_PORT_ONLY_PROTO;

	instance_alias_base_type&
	load_reference(istream& i) const;

	CREATE_DEPENDENT_TYPES_PROTO;

	COLLECT_PORT_ALIASES_PROTO;

	CONSTRUCT_PORT_CONTEXT_PROTO;

	ASSIGN_FOOTPRINT_FRAME_PROTO;

	CFLAT_ALIASES_PROTO;

#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
	HACK_REMAP_INDICES_PROTO;
#endif

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
	// template explicitly required by g++-4.0
	typedef	typename traits_type::template instance_alias<0>::type
							instance_type;
	typedef	typename parent_type::collection_type_manager_parent_type
					collection_type_manager_parent_type;
private:
	instance_type					the_instance;

private:
	instance_array();

	instance_array(const this_type&, const footprint&);

	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO;

public:
	instance_array(const scopespace& o, const string& n);

	~instance_array();

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const;

	INSTANTIATE_INDICES_PROTO;

	CREATE_UNIQUE_STATE_PROTO;

	good_bool
	allocate_state(footprint&);

	good_bool
	merge_created_state(physical_instance_collection&, footprint&);

	void
	inherit_created_state(const physical_instance_collection&, 
		const footprint&);

	good_bool
	synchronize_actuals(physical_instance_collection&);

	instance_alias_base_ptr_type
	lookup_instance(const multikey_index_type& l) const;

	bool
	lookup_instance_collection(
		typename default_list<instance_alias_base_ptr_type>::type& l, 
		const const_range_list& r) const;

	UNROLL_ALIASES_PROTO;

	UNROLL_PORT_ONLY_PROTO;

	instance_alias_base_type&
	load_reference(istream& i) const;

	const_index_list
	resolve_indices(const const_index_list& l) const;

	typename instance_type::parent_type&
	get_the_instance(void) { return the_instance; }

	CREATE_DEPENDENT_TYPES_PROTO;

	COLLECT_PORT_ALIASES_PROTO;

	CONSTRUCT_PORT_CONTEXT_PROTO;

	ASSIGN_FOOTPRINT_FRAME_PROTO;

	CFLAT_ALIASES_PROTO;

#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
	HACK_REMAP_INDICES_PROTO;
#endif

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
#if 0
	// soon...
	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
#endif
};	// end class instance_array (specialized)

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_H__

