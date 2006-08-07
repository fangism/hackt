/**
	\file "Object/ref/member_meta_instance_reference.h"
	Base class family for instance references in HAC.  
	This file was reincarnated from "Object/art_object_member_inst_ref.h"
	$Id: member_meta_instance_reference.h,v 1.11 2006/08/07 22:39:42 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_MEMBER_META_INSTANCE_REFERENCE_H__
#define	__HAC_OBJECT_REF_SIMPLE_MEMBER_META_INSTANCE_REFERENCE_H__

#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/traits/class_traits_fwd.h"

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
	typedef	typename traits_type::alias_collection_type
						alias_collection_type;
	typedef	never_ptr<const instance_collection_generic_type>
						instance_collection_ptr_type;
	typedef	typename parent_type::instance_alias_base_ptr_type
						instance_alias_base_ptr_type;
	/// the containing type, whose member is referenced
	typedef	meta_instance_reference_base		base_inst_type;
	// should be kept consistent with
	//	instance_collection_base::inst_ref_ptr_type
	typedef	count_ptr<const base_inst_type>		base_inst_ptr_type;
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
		const instance_collection_ptr_type m);

	~member_meta_instance_reference();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	// overrides parent's implementation.  
	bad_bool
	unroll_references(const unroll_context&, alias_collection_type&) const;

	// overrides simple_meta...
	UNROLL_SCALAR_SUBSTRUCTURE_REFERENCE_PROTO;

	// overrides simple_meta...
	instance_alias_base_ptr_type
	unroll_generic_scalar_reference(const unroll_context&) const;

	using parent_type::connect_port;

	// overrides simple_meta...
	LOOKUP_FOOTPRINT_FRAME_PROTO;

	// overrides simple_meta...
	size_t
	lookup_globally_allocated_index(const state_manager&) const;

protected:
	using parent_type::__unroll_generic_scalar_reference;
	using parent_type::unroll_references_helper_no_lookup;

private:
	count_ptr<instance_collection_generic_type>
	resolve_parent_member_helper(const unroll_context&) const;

public:
	FRIEND_PERSISTENT_TRAITS
	// final, non-virtual
	PERSISTENT_METHODS_DECLARATIONS
};	// end class member_meta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_MEMBER_META_INSTANCE_REFERENCE_H__

