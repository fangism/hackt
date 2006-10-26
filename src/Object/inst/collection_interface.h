/**
	\file "Object/inst/collection_interface.h"
	Abstract class defining the interface for an instance collection.  
	$Id: collection_interface.h,v 1.1.2.1 2006/10/26 22:32:01 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_COLLECTION_INTERFACE_H__
#define	__HAC_OBJECT_INST_COLLECTION_INTERFACE_H__

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
// template <class> class instantiation_statement;

//=============================================================================
#define	COLLECTION_INTERFACE_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	COLLECTION_INTERFACE_CLASS					\
collection_interface<Tag>

/**
	Interface to collections of instance aliases.  
	Children classes will either be real collections with type information
	or proxy containers with back-references to real collections.  
 */
COLLECTION_INTERFACE_TEMPLATE_SIGNATURE
class collection_interface :
	public class_traits<Tag>::instance_collection_parent_type {
public:
	typedef	class_traits<Tag>			traits_type;
private:
	typedef	Tag					category_type;
	typedef	typename traits_type::instance_collection_parent_type
							parent_type;
	typedef	COLLECTION_INTERFACE_CLASS		this_type;
public:
	typedef	typename traits_type::type_ref_type	type_ref_type;
	typedef	typename traits_type::type_ref_ptr_type	type_ref_ptr_type;
	typedef	typename traits_type::resolved_type_ref_type
						resolved_type_ref_type;
//	typedef	typename traits_type::collection_type_manager_parent_type
//					collection_type_manager_parent_type;
	typedef	typename traits_type::instance_alias_info_type
						instance_alias_info_type;
	typedef	never_ptr<instance_alias_info_type>
						instance_alias_info_ptr_type;
	typedef	typename traits_type::alias_collection_type
						alias_collection_type;
	typedef	typename traits_type::instance_placeholder_type
					instance_placeholder_type;
	typedef	never_ptr<const instance_placeholder_type>
					instance_placeholder_ptr_type;
	typedef	typename traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef	typename traits_type::simple_meta_instance_reference_type
					simple_meta_instance_reference_type;
	typedef	typename traits_type::simple_nonmeta_instance_reference_type
					simple_nonmeta_instance_reference_type;
	typedef	typename traits_type::member_simple_meta_instance_reference_type
				member_simple_meta_instance_reference_type;
//	typedef	meta_instance_reference_base		meta_instance_reference_base_type;
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

protected:
	collection_interface() : parent_type() { }

public:

virtual	~collection_interface() { }

virtual	ostream&
	what(ostream&) const = 0;

//	ostream&
//	type_dump(ostream&) const;

virtual	ostream&
	dump_element_key(ostream&, const instance_alias_info_type&) const = 0;

virtual	multikey_index_type
	lookup_key(const instance_alias_info_type&) const = 0;

virtual	size_t
	lookup_index(const instance_alias_info_type&) const = 0;

virtual	instance_alias_info_type&
	get_corresponding_element(const this_type&,
		const instance_alias_info_type&) = 0;

//	never_ptr<const physical_instance_placeholder>
//	get_placeholder_base(void) const;

#if 0
// maybe virtual
	instance_placeholder_ptr_type
	get_placeholder(void) const {
		return this->source_placeholder;
	}
#endif

	// TODO: substitute/rename as collection_type_established()
virtual	bool
	is_partially_unrolled(void) const = 0;

#if 0
	// this could just return hard-coded built-in type...
	// this returns the type as given by the first instantiation statement
	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const;
#endif

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
#if 0
	never_ptr<const const_param_expr_list>
	get_actual_param_list(void) const;
#endif

virtual instance_alias_info_ptr_type
	lookup_instance(const multikey_index_type& i) const = 0;

virtual	bool
	lookup_instance_collection(
		typename default_list<instance_alias_info_ptr_type>::type& l, 
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
virtual	instance_alias_info_type&
	load_reference(istream& i) = 0;

#if 0
	static
	this_type*
	make_array(const instance_placeholder_ptr_type);

	static
	this_type*
	make_port_array(const instance_placeholder_ptr_type);
#endif

#if 0
	static
	persistent*
	construct_empty(const int);
#endif

#if 0
// maybe...
protected:
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
#endif

#if 0
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
#endif

};	// end class instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

// #undef	UNROLL_ALIASES_PROTO
// #undef	INSTANTIATE_INDICES_PROTO

#endif	// __HAC_OBJECT_INST_COLLECTION_INTERFACE_H__

