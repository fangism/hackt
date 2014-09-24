/**
	\file "Object/inst/collection_interface.hh"
	Abstract class defining the interface for an instance collection.  
	$Id: collection_interface.hh,v 1.7 2010/04/07 00:12:39 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_COLLECTION_INTERFACE_H__
#define	__HAC_OBJECT_INST_COLLECTION_INTERFACE_H__

#include <iosfwd>

#include "Object/traits/class_traits_fwd.hh"
#include "Object/inst/physical_instance_collection.hh"	// for macros
#include "Object/common/multikey_index.hh"
#include "util/STL/list_fwd.hh"
#include "util/memory/excl_ptr.hh"
#include "util/memory/count_ptr.hh"
#include "util/boolean_types.hh"
#include "util/inttypes.h"
#include "util/persistent_fwd.hh"

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
template <bool> struct internal_aliases_policy;
template <class> class instance_collection;
template <class> class collection_pool;
template <class> struct instance_collection_pool_bundle;

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
	typedef	instance_collection_pool_bundle<Tag>
					collection_pool_bundle_type;
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

virtual	~collection_interface() { }

public:
virtual	ostream&
	what(ostream&) const = 0;

virtual	const instance_collection<Tag>&
	get_canonical_collection(void) const = 0;

virtual	ostream&
	dump_element_key(ostream&, const instance_alias_info_type&) const = 0;

virtual	multikey_index_type
	lookup_key(const instance_alias_info_type&) const = 0;

virtual	size_t
	lookup_index(const instance_alias_info_type&) const = 0;

virtual	instance_alias_info_type&
	get_corresponding_element(const this_type&,
		const instance_alias_info_type&) = 0;

	// TODO: substitute/rename as collection_type_established()
virtual	bool
	is_partially_unrolled(void) const = 0;

virtual	bool
	is_formal(void) const = 0;

/**
	Prototype for instantiating alias indices during unroll phase.  
	NOTE: context shouldn't be necessary at the collection, 
	only needed to resolved placeholders!
 */
#define	INSTANTIATE_INDICES_PROTO					\
	good_bool							\
	instantiate_indices(const const_range_list&, const unroll_context&)

virtual	INSTANTIATE_INDICES_PROTO = 0;

virtual	CONNECT_PORT_ALIASES_RECURSIVE_PROTO = 0;
virtual	RECONNECT_PORT_ALIASES_RECURSIVE_PROTO = 0;

protected:
virtual	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO = 0;

public:

virtual instance_alias_info_ptr_type
	lookup_instance(const multikey_index_type& i) const = 0;

virtual	bool
	lookup_instance_collection(
		typename default_list<instance_alias_info_ptr_type>::type&, 
		const const_range_list&) const = 0;

virtual	const_index_list
	resolve_indices(const const_index_list&) const = 0;

#define	UNROLL_ALIASES_PROTO						\
	bad_bool							\
	unroll_aliases(const multikey_index_type&, 			\
		const multikey_index_type&, 				\
		alias_collection_type&) const

virtual	UNROLL_ALIASES_PROTO = 0;

virtual	COLLECT_PORT_ALIASES_PROTO = 0;

#define	FINALIZE_SUBSTRUCTURE_ALIASES_PROTO				\
	void								\
	finalize_substructure_aliases(const unroll_context&)

// virtual	FINALIZE_SUBSTRUCTURE_ALIASES_PROTO = 0;

virtual	void
	accept(alias_visitor&) const = 0;

public:
virtual	instance_alias_info_type&
	load_reference(istream&) = 0;

virtual	void
	write_pointer(ostream&, const collection_pool_bundle_type&) const = 0;

	// this variation includes the type_tag_enum
	void
	write_local_pointer(const footprint&, ostream&) const;

virtual	void
	collect_transient_info_base(persistent_object_manager&) const = 0;

virtual	void
	write_object(const footprint&, 
		const persistent_object_manager&, ostream&) const = 0;

virtual	void
	load_object(footprint&, 
		const persistent_object_manager&, istream&) = 0;

};	// end class collection_interface

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

// #undef	UNROLL_ALIASES_PROTO
// #undef	INSTANTIATE_INDICES_PROTO

#endif	// __HAC_OBJECT_INST_COLLECTION_INTERFACE_H__

