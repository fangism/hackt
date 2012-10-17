/**
	\file "Object/ref/meta_instance_reference_subtypes.hh"
	Subtype classification for meta-instance-reference base classes.
	This file was reincarnated from "Object/art_object_inst_ref_subtypes.h".
	$Id: meta_instance_reference_subtypes.hh,v 1.20 2011/03/23 00:36:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_H__
#define	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_H__

#include <vector>			// really only need fwd decl
#include "Object/ref/meta_instance_reference_base.hh"
#include "Object/ref/meta_index_list_fwd.hh"
#include "Object/traits/class_traits_fwd.hh"
#include "util/boolean_types.hh"

#include "Object/devel_switches.hh"
#if PRIVATE_MEMBER_REFERENCES
#include "Object/expr/types.hh"		// for pint_value_type
#include "util/packed_array_fwd.hh"
#endif

namespace HAC {
namespace entity {
class unroll_context;
struct global_entry_context;
class nonmeta_expr_visitor;
template <class> class simple_meta_instance_reference;
template <class> class aggregate_meta_instance_reference;
template <class> class collection_interface;
using util::good_bool;
using util::bad_bool;
#if PRIVATE_MEMBER_REFERENCES
using util::packed_array_generic;
#endif

//=============================================================================

#define	META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE	template <class Tag>
#define	META_INSTANCE_REFERENCE_CLASS		meta_instance_reference<Tag>

META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class meta_instance_reference : public meta_instance_reference_base {
	typedef	META_INSTANCE_REFERENCE_CLASS	this_type;
public:
	typedef	class_traits<Tag>		traits_type;
	typedef typename traits_type::instance_alias_info_type
						instance_alias_info_type;
	typedef never_ptr<instance_alias_info_type>
						instance_alias_info_ptr_type;
	typedef never_ptr<const instance_alias_info_type>
					const_instance_alias_info_ptr_type;
	typedef	typename traits_type::alias_collection_type
						alias_collection_type;
#if PRIVATE_MEMBER_REFERENCES
	// must use pint_value_type, not size_t, 
	// to match key_type of alias_collection_type
	typedef	index_array_reference		subindex_collection_type;
#endif
	typedef	typename traits_type::alias_connection_type
						alias_connection_type;
	/// the instance collection base type
	typedef typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	collection_interface<Tag>	collection_interface_type;
	typedef typename traits_type::instance_placeholder_type
					instance_placeholder_type;

	/// keep this typedef consistent with 
	/// simple_meta_indexed_reference_base::index_list_type
	typedef	meta_index_list_type		index_list_type;
	typedef	simple_meta_instance_reference<Tag>
						simple_reference_type;
	typedef	aggregate_meta_instance_reference<Tag>
						aggregate_reference_type;
protected:
	meta_instance_reference() : meta_instance_reference_base() { }
public:
virtual	~meta_instance_reference();

virtual	size_t
	dimensions(void) const = 0;

	// consider sub-typing?
virtual	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const = 0;

virtual bad_bool
	unroll_references_packed(const unroll_context&,
		alias_collection_type&) const = 0;

#if PRIVATE_MEMBER_REFERENCES
virtual bad_bool
	unroll_subindices_packed(const global_entry_context&,
		const unroll_context&, 
		subindex_collection_type&) const = 0;
#endif

	bool
	may_be_type_equivalent(const meta_instance_reference_base&) const;

	bool
	must_be_type_equivalent(const meta_instance_reference_base&) const;

	CONNECT_PORT_PROTO;

	// really only used for process implicit supply overrides
	bad_bool
	connect_implicit_port(physical_instance_collection&, 
		const unroll_context&) const;

virtual	void
	accept(nonmeta_expr_visitor&) const = 0;

virtual	good_bool
	lookup_globally_allocated_indices(
		const footprint&, std::vector<size_t>&) const;

protected:
	/**
		Helper function.  
	 */
	static
	bad_bool
	unroll_references_packed_helper(const unroll_context&,
		const instance_placeholder_type&,
		const count_ptr<const index_list_type>&,
		alias_collection_type&);

	static
	bad_bool
	unroll_references_packed_helper_no_lookup(const unroll_context&,
		const collection_interface_type&,
		const count_ptr<const index_list_type>&,
		alias_collection_type&);

	CREATE_INSTANCE_ATTRIBUTE_PROTO;
	CREATE_DIRECTION_DECLARATION_PROTO;

private:
	alias_connection_ptr_type
	make_aliases_connection_private(void) const;

	count_ptr<aggregate_meta_instance_reference_base>
	make_aggregate_meta_instance_reference_private(void) const;

	bad_bool
	__expand_connection_aliases(physical_instance_collection&, 
		const unroll_context&, const char*, 
		alias_collection_type&, alias_collection_type&) const;

};	// end class meta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_H__

