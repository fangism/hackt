/**
	\file "Object/ref/simple_meta_instance_reference.h"
	Class family for instance references in HAC.  
	This file was reincarnated from "Object/art_object_inst_ref.h".
	$Id: simple_meta_instance_reference.h,v 1.14.4.7 2006/10/17 04:47:03 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_H__
#define	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_H__

#include "Object/ref/meta_instance_reference_base.h"
#include "Object/ref/simple_meta_indexed_reference_base.h"
#include "Object/inst/instance_collection_base.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/devel_switches.h"
#include "util/packed_array_fwd.h"
#include "Object/ref/inst_ref_implementation_fwd.h"
#include "util/STL/vector_fwd.h"

namespace HAC {
namespace entity {
using util::packed_array_generic;
#if USE_INSTANCE_PLACEHOLDERS
class instance_placeholder_base;
#endif

template <bool>	struct simple_meta_instance_reference_implementation;

//=============================================================================
#define	SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE		\
template <class Tag>

#define	SIMPLE_META_INSTANCE_REFERENCE_CLASS				\
simple_meta_instance_reference<Tag>

/**
	Class template for physical instance references.
	Needs to be virtual so that member_meta_instance_reference may safely
	derive from this class.  
	\param Collection the instance collection type.
	\param Parent the type from which this is derived, 
		probably simple_meta_indexed_reference_base or descendant.  
	TODO: derive using multiple inheritance from now on:
		always derive from simple_meta_indexed_reference_base and the
		type interface designated by class_traits.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class simple_meta_instance_reference :
	public simple_meta_indexed_reference_base, 
	public class_traits<Tag>::meta_instance_reference_parent_type {
#if 1
	template <bool>
	friend struct simple_meta_instance_reference_implementation;
#endif
	typedef	SIMPLE_META_INSTANCE_REFERENCE_CLASS	this_type;
public:
	typedef	class_traits<Tag>		traits_type;
protected:
	typedef	typename traits_type::meta_instance_reference_parent_type
						parent_type;
	typedef	simple_meta_instance_reference_implementation
			<traits_type::has_substructure>
					substructure_implementation_policy;
public:
	typedef	simple_meta_indexed_reference_base	common_base_type;
	typedef	common_base_type::indices_ptr_type	indices_ptr_type;
	typedef	common_base_type::indices_ptr_arg_type	indices_ptr_arg_type;
	/// the instance collection base type
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
#if USE_INSTANCE_PLACEHOLDERS
	typedef	typename traits_type::instance_placeholder_type
					instance_placeholder_type;
	typedef	never_ptr<const instance_placeholder_type>
					instance_placeholder_ptr_type;
#endif
	/// the type of alias element contained by instance collections
	typedef	typename traits_type::instance_alias_base_type
						instance_alias_base_type;
	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	typedef	never_ptr<const instance_alias_base_type>
					const_instance_alias_base_ptr_type;
	/// the type of connections formed by the alias type
	typedef	typename traits_type::alias_connection_type
						alias_connection_type;
	/// type used to unroll collections of instance aliases
	typedef	typename traits_type::alias_collection_type
						alias_collection_type;
	/// pointer type for instance collections
	typedef	never_ptr<const instance_collection_generic_type>
						instance_collection_ptr_type;
	typedef	typename parent_type::port_connection_ptr_type
						port_connection_ptr_type;
private:
#if USE_INSTANCE_PLACEHOLDERS
	const instance_placeholder_ptr_type	inst_collection_ref;
#else
	const instance_collection_ptr_type	inst_collection_ref;
#endif
protected:
	simple_meta_instance_reference();
public:
#if USE_INSTANCE_PLACEHOLDERS
	explicit
	simple_meta_instance_reference(const instance_placeholder_ptr_type);
#else
	explicit
	simple_meta_instance_reference(const instance_collection_ptr_type);
#endif

virtual	~simple_meta_instance_reference();

	ostream&
	what(ostream&) const;

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const;

#if USE_INSTANCE_PLACEHOLDERS
	never_ptr<const instance_placeholder_base>
#else
	never_ptr<const instance_collection_base>
#endif
	get_inst_base(void) const;

#if USE_INSTANCE_PLACEHOLDERS
	instance_placeholder_ptr_type
	get_inst_base_subtype(void) const {
		return this->inst_collection_ref;
	}
#endif

	ostream&
	dump_type_size(ostream&) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const;

	size_t
	dimensions(void) const;

	good_bool
	attach_indices(indices_ptr_arg_type);

private:
	using parent_type::unroll_references_packed_helper;
protected:
	using parent_type::unroll_references_packed_helper_no_lookup;

public:
	// overridden by member_meta_instance_reference
virtual	bad_bool
	unroll_references_packed(const unroll_context&, alias_collection_type&) const;

virtual	UNROLL_SCALAR_SUBSTRUCTURE_REFERENCE_PROTO;

virtual	instance_alias_base_ptr_type
	unroll_generic_scalar_reference(const unroll_context&) const;

virtual	LOOKUP_FOOTPRINT_FRAME_PROTO;

virtual	size_t
	lookup_globally_allocated_index(const state_manager&
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
		, footprint&
#endif
		) const;

	good_bool
	lookup_globally_allocated_indices(const state_manager&, 
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
		footprint&, 
#endif
		std::default_vector<size_t>::type&) const;

private:
	port_connection_ptr_type
	make_port_connection_private(
		const count_ptr<const meta_instance_reference_base>&) const;

protected:
	// helper function, also used by member
	static
	instance_alias_base_ptr_type
	__unroll_generic_scalar_reference(
#if USE_INSTANCE_PLACEHOLDERS
		const instance_placeholder_type&, 
#else
		const instance_collection_generic_type&, 
#endif
#if REF_COUNT_ARRAY_INDICES
		const count_ptr<const index_list_type>&,
#else
		const never_ptr<const index_list_type>,
#endif
		const unroll_context&
#if !USE_INSTANCE_PLACEHOLDERS
			, const bool
#endif
			);

#if USE_INSTANCE_PLACEHOLDERS
	static
	instance_alias_base_ptr_type
	__unroll_generic_scalar_reference_no_lookup(
		const instance_collection_generic_type&, 
#if REF_COUNT_ARRAY_INDICES
		const count_ptr<const index_list_type>&,
#else
		const never_ptr<const index_list_type>,
#endif
		const unroll_context&);
#endif	// USE_INSTANCE_PLACEHOLDERS

	static
	good_bool
	__unroll_generic_scalar_references(
#if USE_INSTANCE_PLACEHOLDERS
		const instance_placeholder_type&, 
#else
		const instance_collection_generic_type&, 
#endif
#if REF_COUNT_ARRAY_INDICES
		const count_ptr<const index_list_type>&,
#else
		const never_ptr<const index_list_type>,
#endif
		const unroll_context&, 
#if !USE_INSTANCE_PLACEHOLDERS
		const bool, 
#endif
		alias_collection_type&);

#if USE_INSTANCE_PLACEHOLDERS
	static
	good_bool
	__unroll_generic_scalar_references_no_lookup(
		const instance_collection_generic_type&, 
#if REF_COUNT_ARRAY_INDICES
		const count_ptr<const index_list_type>&,
#else
		const never_ptr<const index_list_type>,
#endif
		const unroll_context&, 
		alias_collection_type&);
#endif	// USE_INSTANCE_PLACEHOLDERS

	void
	collect_transient_info_base(persistent_object_manager& ) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

public:
	FRIEND_PERSISTENT_TRAITS
	VIRTUAL_PERSISTENT_METHODS_DECLARATIONS

};	// end class simple_meta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_H__

