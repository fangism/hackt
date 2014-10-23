/**
	\file "Object/ref/member_meta_instance_reference.hh"
	Base class family for instance references in HAC.  
	This file was reincarnated from "Object/art_object_member_inst_ref.h"
	$Id: member_meta_instance_reference.hh,v 1.23 2011/05/17 21:19:53 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_MEMBER_META_INSTANCE_REFERENCE_HH__
#define	__HAC_OBJECT_REF_SIMPLE_MEMBER_META_INSTANCE_REFERENCE_HH__

#include "Object/ref/simple_meta_instance_reference.hh"

namespace HAC {
namespace entity {
class scopespace;
class unroll_context;
using std::ostream;
using std::istream;
using util::memory::never_ptr;
using util::bad_bool;

//=============================================================================
#define	MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE			\
template <class Tag>

#define	MEMBER_INSTANCE_REFERENCE_CLASS					\
member_meta_instance_reference<Tag>

/**
	Re-usable type-specific member_meta_instance_reference class template.  
	This class is final, nothing else derives from it, 
	no need for virtual functions.  
	\param InstRef must be a type derived from simple_meta_indexed_reference_base.
	Consider moving this class definition to "art_object_inst_ref.h"?
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class member_meta_instance_reference :
	public class_traits<Tag>::simple_meta_instance_reference_type {
private:
	typedef	MEMBER_INSTANCE_REFERENCE_CLASS		this_type;
public:
	typedef	class_traits<Tag>			traits_type;
	/// the underlying type of the member instance referenced
	typedef	typename traits_type::simple_meta_instance_reference_type
							parent_type;
	typedef	typename traits_type::instance_collection_generic_type
						instance_collection_generic_type;
	typedef	typename parent_type::collection_interface_type
						collection_interface_type;
	typedef	typename traits_type::instance_placeholder_type
						instance_placeholder_type;
	typedef	never_ptr<const instance_placeholder_type>
						instance_placeholder_ptr_type;
	typedef	typename traits_type::alias_collection_type
						alias_collection_type;
	typedef	never_ptr<const instance_collection_generic_type>
						instance_collection_ptr_type;
	typedef	typename parent_type::instance_alias_info_ptr_type
						instance_alias_info_ptr_type;
	typedef	typename parent_type::const_instance_alias_info_ptr_type
					const_instance_alias_info_ptr_type;
	/// the containing type, whose member is referenced
	typedef	meta_instance_reference_base		base_inst_type;
	// should be kept consistent with
	//	instance_collection_base::inst_ref_ptr_type
	typedef	count_ptr<const base_inst_type>		base_inst_ptr_type;
	typedef	never_ptr<collection_interface_type>
						parent_member_ptr_type;
protected:
	/**
		The owning base instance reference, 
		must have dimension-0, scalar... for now
		Is type limited to simple? or can it be nested member?
	 */
	const base_inst_ptr_type			base_inst_ref;
private:
	member_meta_instance_reference();
public:
	member_meta_instance_reference(const base_inst_ptr_type& b, 
		const instance_placeholder_ptr_type m);

	~member_meta_instance_reference();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	// overrides parent's implementation.  
	bad_bool
	unroll_references_packed(const unroll_context&,
		alias_collection_type&) const;

#if PRIVATE_MEMBER_REFERENCES
	// overrides parent's implementation.  
	bad_bool
	unroll_subindices_packed(const global_entry_context&,
		const unroll_context&, 
		index_array_reference&) const;
#endif

	// overrides simple_meta...
	UNROLL_SCALAR_SUBSTRUCTURE_REFERENCE_PROTO;

	// overrides simple_meta...
	instance_alias_info_ptr_type
	unroll_generic_scalar_reference(const unroll_context&) const;

	// overrides simple_meta...
	count_ptr<const parent_type>
	__unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const parent_type>&) const;

	using parent_type::connect_port;

	const base_inst_ptr_type&
	get_base_ref(void) const { return this->base_inst_ref; }

	// overrides simple_meta...
	size_t
	lookup_globally_allocated_index(const global_entry_context&, 
		const unroll_context* = NULL) const;

	// overrides simple_meta...
	size_t
	lookup_locally_allocated_index(const unroll_context&) const;

#if 0
	good_bool
	lookup_locally_allocated_indices(const unroll_context&, 
		std::vector<size_t>&) const;
#endif

	// override meta_instance_reference<>
	good_bool
	lookup_globally_allocated_indices(
//		const global_entry_context&,
		const footprint&,
		std::vector<size_t>&) const;

	void
	accept(nonmeta_expr_visitor&) const;
protected:
	using parent_type::__unroll_generic_scalar_reference;
	using parent_type::unroll_references_packed_helper_no_lookup;

private:
	parent_member_ptr_type
	resolve_parent_member_helper(const unroll_context&) const;

public:
	FRIEND_PERSISTENT_TRAITS
	// final, non-virtual
	PERSISTENT_METHODS_DECLARATIONS
};	// end class member_meta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_MEMBER_META_INSTANCE_REFERENCE_HH__

