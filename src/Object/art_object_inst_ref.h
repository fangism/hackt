/**
	\file "Object/art_object_inst_ref.h"
	Class family for instance references in ART.  
	TODO: rename file to simple_meta_instance_reference
	$Id: art_object_inst_ref.h,v 1.22 2005/06/19 01:58:40 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_REF_H__
#define	__OBJECT_ART_OBJECT_INST_REF_H__

#include "Object/art_object_inst_ref_base.h"
#include "Object/art_object_instance_base.h"
#include "Object/art_object_classification_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/packed_array_fwd.h"

namespace ART {
namespace entity {
class unroll_context;
using std::ostream;
using std::istream;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::packed_array_generic;

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
		probably simple_meta_instance_reference_base or descendant.  
	TODO: this could be properly renamed simple_meta_instance_reference.
	TODO: derive using multiple inheritance from now on:
		always derive from simple_meta_instance_reference_base and the
		type interface designated by class_traits.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class simple_meta_instance_reference :
	public simple_meta_instance_reference_base, 
	public class_traits<Tag>::meta_instance_reference_parent_type {
	typedef	SIMPLE_META_INSTANCE_REFERENCE_CLASS	this_type;
protected:
	typedef	typename class_traits<Tag>::meta_instance_reference_parent_type
						parent_type;
public:
	typedef	simple_meta_instance_reference_base	common_base_type;
	/// the instance collection base type
	typedef	typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	/// the type of alias element contained by instance collections
	typedef	typename class_traits<Tag>::instance_alias_base_type
						instance_alias_base_type;
	/// the type of connections formed by the alias type
	typedef	typename class_traits<Tag>::alias_connection_type
						alias_connection_type;
	/// type used to unroll collections of instance aliases
	typedef	typename class_traits<Tag>::alias_collection_type
						alias_collection_type;
	/// pointer type for instance collections
	typedef	never_ptr<const instance_collection_generic_type>
						instance_collection_ptr_type;
private:
	const instance_collection_ptr_type	inst_collection_ref;
protected:
	simple_meta_instance_reference();
public:
	explicit
	simple_meta_instance_reference(const instance_collection_ptr_type);

virtual	~simple_meta_instance_reference();

	ostream&
	what(ostream&) const;

	using parent_type::dump;

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	// overridden by member_meta_instance_reference
virtual	bad_bool
	unroll_references(unroll_context&, alias_collection_type&) const;

private:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;

protected:
	void
	collect_transient_info_base(persistent_object_manager& ) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

public:
	FRIEND_PERSISTENT_TRAITS
	VIRTUAL_PERSISTENT_METHODS_DECLARATIONS

};	// end class meta_instance_reference

//=============================================================================
// classes pint_meta_instance_reference and pbool_meta_instance_reference
//	are in "art_object_expr_param_ref.*"

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_REF_H__

