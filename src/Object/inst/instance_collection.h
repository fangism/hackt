/**
	\file "Object/inst/instance_collection.h"
	Class declarations for scalar instances and instance collections.  
	This file was originally "Object/art_object_instance_collection.h"
		in a previous life.  
	$Id: instance_collection.h,v 1.25.2.4 2006/10/22 21:25:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_H__

#include <iosfwd>

#include "Object/traits/class_traits_fwd.h"
#include "Object/inst/physical_instance_collection.h"	// for macros
#include "Object/common/multikey_index.h"
#include "Object/devel_switches.h"
#include "util/STL/list_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"
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
using std::string;
using util::memory::count_ptr;
using util::memory::never_ptr;
using util::good_bool;
using util::bad_bool;
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
template <class, size_t>
class instance_array;

// forward declaration of partial specialization
template <class Tag>
class instance_array<Tag,0>;

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
	// TODO: consider pushing down to instance_array_class
	// to avoid replication between formals and actuals.
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
		TODO: consider pushing to instance_array to avoid
			replicating between formals and actual collections.  
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

	// TODO: substitute/rename as collection_type_established()
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
#if COLLECTION_SEPARATE_KEY_FROM_VALUE
	// not that all alias elements are equal, 
	// we can factor out common functionality
	/**
		Functor to collect transient info in the aliases.  
	 */
	class element_collector {
		persistent_object_manager& pom;
	public:
		element_collector(persistent_object_manager& m) : pom(m) { }

		void
		operator () (const instance_alias_info_type&) const;
	};	// end class element_collector

	/**
		Functor to write alias elements.  
	 */
	class element_writer {
		ostream& os;
		const persistent_object_manager& pom;
	public:
		element_writer(const persistent_object_manager& m,
			ostream& o) : os(o), pom(m) { }

		void
		operator () (const instance_alias_info_type&) const;
	};	// end class element_writer

	class element_loader {
		istream& is;
		const persistent_object_manager& pom;
	public:
		element_loader(const persistent_object_manager& m,
			istream& i) : is(i), pom(m) { }

		void
		operator () (instance_alias_info_type&);	// const?
	};	// end class connection_loader

	class connection_writer {
		ostream& os;
		const persistent_object_manager& pom;
	public:
		connection_writer(const persistent_object_manager& m,
			ostream& o) : os(o), pom(m) { }

		void
		operator () (const instance_alias_info_type&) const;
	};	// end class connection_writer

	class connection_loader {
		istream& is;
		const persistent_object_manager& pom;
	public:
		connection_loader(const persistent_object_manager& m,
			istream& i) : is(i), pom(m) { }

		void
		operator () (instance_alias_info_type&);	// const?
	};	// end class connection_loader

	struct key_dumper {
		ostream& os;
		const dump_flags& df;

		key_dumper(ostream& o, const dump_flags& _df) :
			os(o), df(_df) { }

		ostream&
		operator () (const instance_alias_info_type&);
	};	// end struct key_dumper

#define	COLLECTION_HELPER_TEMPLATE_SIGNATURE				\
	INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
#define	COLLECTION_HELPER_CLASS						\
	INSTANCE_COLLECTION_CLASS
#define	COLLECTION_HELPER_ARG_TYPE					\
	instance_alias_info_type
#else
#define	COLLECTION_HELPER_TEMPLATE_SIGNATURE				\
	INSTANCE_ARRAY_TEMPLATE_SIGNATURE
#define	COLLECTION_HELPER_CLASS						\
	INSTANCE_ARRAY_CLASS
#define	COLLECTION_HELPER_ARG_TYPE					\
	element_type
#endif

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

// #undef	UNROLL_ALIASES_PROTO
// #undef	INSTANTIATE_INDICES_PROTO

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_H__

