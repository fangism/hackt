/**
	\file "Object/ref/simple_meta_dummy_reference.hh"
	Class family for instance references in HAC.  
	This file was reincarnated from "Object/art_object_inst_ref.h".
	$Id: simple_meta_dummy_reference.hh,v 1.7 2011/03/23 00:36:15 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_META_DUMMY_REFERENCE_H__
#define	__HAC_OBJECT_REF_SIMPLE_META_DUMMY_REFERENCE_H__

#include "Object/ref/meta_instance_reference_base.hh"
#include "Object/ref/simple_meta_indexed_reference_base.hh"
#include "Object/inst/instance_collection_base.hh"
#include "Object/traits/class_traits_fwd.hh"
// #include "util/packed_array_fwd.hh"
// #include "Object/ref/inst_ref_implementation_fwd.hh"
// #include "util/STL/vector_fwd.hh"

namespace HAC {
namespace entity {
// using util::packed_array_generic;
class nonmeta_expr_visitor;
class instance_placeholder_base;
// template <class> class collection_interface;

// template <bool>	struct simple_meta_instance_reference_implementation;

//=============================================================================
#define	SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE			\
template <class Tag>

#define	SIMPLE_META_DUMMY_REFERENCE_CLASS				\
simple_meta_dummy_reference<Tag>

/**
	Dummy class for a reference to a special type.
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
class simple_meta_dummy_reference :
	public simple_meta_indexed_reference_base, 
	public meta_instance_reference_base {
	typedef	SIMPLE_META_DUMMY_REFERENCE_CLASS	this_type;
public:
	typedef	class_traits<Tag>			traits_type;
protected:
	typedef	meta_instance_reference_base		parent_type;
public:
	typedef	simple_meta_indexed_reference_base	common_base_type;
	typedef	common_base_type::indices_ptr_type	indices_ptr_type;
	typedef	common_base_type::indices_ptr_arg_type	indices_ptr_arg_type;
	typedef	typename traits_type::instance_placeholder_type
					instance_placeholder_type;
	typedef	never_ptr<const instance_placeholder_type>
					instance_placeholder_ptr_type;
	typedef	parent_type::port_connection_ptr_type
						port_connection_ptr_type;
private:
	const instance_placeholder_ptr_type	inst_collection_ref;
protected:
	simple_meta_dummy_reference();
public:
	explicit
	simple_meta_dummy_reference(const instance_placeholder_ptr_type);

	simple_meta_dummy_reference(const instance_placeholder_ptr_type, 
		indices_ptr_arg_type);

	~simple_meta_dummy_reference();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump_local(ostream&) const;

	never_ptr<const instance_placeholder_base>
	get_inst_base(void) const;

	instance_placeholder_ptr_type
	get_inst_base_subtype(void) const {
		return this->inst_collection_ref;
	}

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

#if 1
	// normally these would be from meta_dummy_reference_subtypes
	bool
	may_be_type_equivalent(const meta_instance_reference_base&) const;

	bool
	must_be_type_equivalent(const meta_instance_reference_base&) const;

	CONNECT_PORT_PROTO;
	CREATE_INSTANCE_ATTRIBUTE_PROTO;
	CREATE_DIRECTION_DECLARATION_PROTO;

	alias_connection_ptr_type
	make_aliases_connection_private(void) const;

	count_ptr<aggregate_meta_instance_reference_base>
	make_aggregate_meta_instance_reference_private(void) const;

	void
	accept(nonmeta_expr_visitor&) const;
#endif

	count_ptr<const this_type>
	__unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const this_type>&) const;

	UNROLL_RESOLVE_COPY_REFERENCE_PROTO;

	UNROLL_SCALAR_SUBSTRUCTURE_REFERENCE_PROTO;

	LOOKUP_TOP_LEVEL_REFERENCE_PROTO;
	LOOKUP_TOP_LEVEL_REFERENCES_PROTO;
#if PRIVATE_MEMBER_REFERENCES
	LOOKUP_GLOBAL_REFERENCE_INDICES_PROTO;
#endif

private:
	port_connection_ptr_type
	make_port_connection_private(
		const count_ptr<const meta_instance_reference_base>&) const;

	void
	collect_transient_info_base(persistent_object_manager& ) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

public:
	FRIEND_PERSISTENT_TRAITS
	VIRTUAL_PERSISTENT_METHODS_DECLARATIONS

};	// end class simple_meta_dummy_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_META_DUMMY_REFERENCE_H__

