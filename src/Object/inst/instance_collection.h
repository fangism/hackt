/**
	\file "Object/inst/instance_collection.h"
	Class declarations for scalar instances and instance collections.  
	This file was originally "Object/art_object_instance_collection.h"
		in a previous life.  
	$Id: instance_collection.h,v 1.26.2.13 2006/11/06 20:40:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_H__

#include <iosfwd>

#include "Object/traits/class_traits_fwd.h"
#include "Object/inst/physical_instance_collection.h"	// for macros
#include "Object/common/multikey_index.h"
#include "Object/devel_switches.h"
#include "Object/inst/collection_interface.h"
#include "util/persistent_functor.h"
#include "util/STL/list_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"
#include "util/inttypes.h"

namespace HAC {
namespace entity {
template <class> class instantiation_statement;
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
template <class> class instance_collection_pool_bundle;
class collection_index_entry;
#endif

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
	public collection_interface<Tag>, 
	public class_traits<Tag>::collection_type_manager_parent_type {
friend	class class_traits<Tag>::collection_type_manager_parent_type;
friend	class subinstance_manager;
public:
	typedef	class_traits<Tag>			traits_type;
private:
	typedef	Tag					category_type;
	typedef	collection_interface<Tag>		parent_type;
	typedef	INSTANCE_COLLECTION_CLASS		this_type;
public:
	typedef	parent_type			collection_interface_type;
	typedef	typename traits_type::type_ref_type	type_ref_type;
	typedef	typename traits_type::type_ref_ptr_type	type_ref_ptr_type;
	typedef	typename traits_type::resolved_type_ref_type
						resolved_type_ref_type;
	typedef	typename traits_type::collection_type_manager_parent_type
					collection_type_manager_parent_type;
	typedef	typename traits_type::instance_alias_info_type
						instance_alias_info_type;
	typedef	never_ptr<instance_alias_info_type>
						instance_alias_info_ptr_type;
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
#if HEAP_ALLOCATE_FOOTPRINTS
	/**
		Back-reference to owning footprint.  
		Always a weak-pointer, even if footprints ref-counted. 
	 */
	never_ptr<const footprint>		footprint_ref;
#endif
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
	instance_collection(
#if HEAP_ALLOCATE_FOOTPRINTS
		const footprint&, 
#endif
		const instance_placeholder_ptr_type);

#if 0
	instance_collection(const instance_placeholder_ptr_type, 
		const instance_collection_parameter_type&);
#endif
#if !POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
public:
#endif
virtual	~instance_collection();

public:
virtual	ostream&
	what(ostream&) const = 0;

	ostream&
	type_dump(ostream&) const;

virtual	ostream&
	dump_element_key(ostream&, const instance_alias_info_type&) const = 0;

#if HEAP_ALLOCATE_FOOTPRINTS
	const footprint&
	get_footprint_owner(void) const { return *this->footprint_ref; }
#endif

	const this_type&
	get_canonical_collection(void) const;

virtual	ostream&
	dump_element_key(ostream&, const size_t) const = 0;

virtual	multikey_index_type
	lookup_key(const size_t) const = 0;

	// translate multikey index to internal index
virtual	size_t
	lookup_index(const multikey_index_type&) const = 0;

virtual	size_t
	collection_size(void) const = 0;

virtual	multikey_index_type
	lookup_key(const instance_alias_info_type&) const = 0;

virtual	size_t
	lookup_index(const instance_alias_info_type&) const = 0;

virtual	instance_alias_info_type&
	get_corresponding_element(const collection_interface_type&,
		const instance_alias_info_type&) = 0;

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
virtual	INSTANTIATE_INDICES_PROTO = 0;

virtual	CONNECT_PORT_ALIASES_RECURSIVE_PROTO = 0;

protected:
virtual	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO = 0;

public:

	never_ptr<const const_param_expr_list>
	get_actual_param_list(void) const;

virtual instance_alias_info_ptr_type
	lookup_instance(const multikey_index_type& i) const = 0;

virtual	bool
	lookup_instance_collection(
		typename default_list<instance_alias_info_ptr_type>::type& l, 
		const const_range_list& r) const = 0;

virtual	const_index_list
	resolve_indices(const const_index_list& l) const = 0;

virtual	UNROLL_ALIASES_PROTO = 0;

virtual	COLLECT_PORT_ALIASES_PROTO = 0;

virtual	CONSTRUCT_PORT_CONTEXT_PROTO = 0;

virtual	ASSIGN_FOOTPRINT_FRAME_PROTO = 0;

virtual	void
	accept(alias_visitor&) const = 0;

public:
virtual	instance_alias_info_type&
	load_reference(istream& i) = 0;

#if !POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
	static
	this_type*
	make_array(const instance_placeholder_ptr_type);

	static
	this_type*
	make_port_formal_array(const instance_placeholder_ptr_type);
#endif

// probably won't need this after pool-allocation
	static
	persistent*
	construct_empty(const int);

public:
	// NOTE: these really belong to instance_alias_info...
	// not that all alias elements are equal, 
	// we can factor out common functionality
	/**
		Functor to collect transient info in the aliases.  
	 */
	typedef	util::persistent_collector_ref	element_collector;

	/**
		Functor to write alias elements.  
	 */
	class element_writer : public util::persistent_writer_base {
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
		const footprint&		fp;
#endif
	public:
		element_writer(
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
			const footprint& f, 
#endif
			const persistent_object_manager& m,
			ostream& o) : util::persistent_writer_base(m, o)
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
				, fp(f)
#endif
				{ }

		void
		operator () (const instance_alias_info_type&) const;
	};	// end class element_writer

	/**
		Now, this also re-links element to parent container.  
	 */
	class element_loader : public util::persistent_loader_base {
		const never_ptr<const parent_type>	back_ref;
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
		const footprint&		fp;
#endif
	public:
		element_loader(
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
			const footprint& f, 
#endif
			const persistent_object_manager& m,
			istream& i, const never_ptr<const parent_type> b) : 
			persistent_loader_base(m, i), back_ref(b)
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
			, fp(f)
#endif
			{ }

		void
		operator () (instance_alias_info_type&);	// const?
	};	// end class connection_loader

	class connection_writer {
		ostream& os;
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
		const instance_collection_pool_bundle<Tag>& pom;
#else
		const persistent_object_manager& pom;
#endif
	public:
		connection_writer(
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
			const instance_collection_pool_bundle<Tag>& m, 
#else
			const persistent_object_manager& m,
#endif
			ostream& o) : os(o), pom(m) { }

		void
		operator () (const instance_alias_info_type&) const;
	};	// end class connection_writer

	class connection_loader {
		istream& is;
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
		const instance_collection_pool_bundle<Tag>& pom;
#else
		const persistent_object_manager& pom;
#endif
	public:
		connection_loader(
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
			const instance_collection_pool_bundle<Tag>& m, 
#else
			const persistent_object_manager& m,
#endif
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

#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
virtual	void
	write_pointer(ostream&, 
		const instance_collection_pool_bundle<Tag>&) const = 0;

#if HEAP_ALLOCATE_FOOTPRINTS
	void
	write_external_pointer(const persistent_object_manager&,
		ostream&) const;

	static
	never_ptr<const this_type>
	read_external_pointer(const persistent_object_manager&, istream&);
#endif
#endif

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(
#if HEAP_ALLOCATE_FOOTPRINTS
		const footprint&, 
#endif
		const persistent_object_manager&, istream&);

};	// end class instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

// #undef	UNROLL_ALIASES_PROTO
// #undef	INSTANTIATE_INDICES_PROTO

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_H__

