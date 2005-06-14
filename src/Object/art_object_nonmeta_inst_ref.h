/**
	\file "Object/art_object_nonmeta_inst_ref.h"
	Class family for instance references in ART.  
	$Id: art_object_nonmeta_inst_ref.h,v 1.1.4.4 2005/06/14 05:38:36 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_NONMETA_INST_REF_H__
#define	__OBJECT_ART_OBJECT_NONMETA_INST_REF_H__

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
#define	SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE		\
template <class Tag>

#define	SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS				\
simple_nonmeta_instance_reference<Tag>

/**
	Class template for physical instance references.
	Needs to be virtual so that member_nonmeta_instance_reference may safely
	derive from this class.  
	\param Collection the instance collection type.
	\param Parent the type from which this is derived, 
		probably simple_nonmeta_instance_reference or descendant.  
	NOTE: nothing is virtual, this is a final class until
		otherwise changed.  
 */
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class simple_nonmeta_instance_reference :
	public simple_nonmeta_instance_reference_base, 
	public class_traits<Tag>::nonmeta_instance_reference_parent_type {
	typedef	SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS	this_type;
protected:
	typedef	simple_nonmeta_instance_reference_base
						common_base_type;
	typedef	typename class_traits<Tag>::nonmeta_instance_reference_parent_type
						parent_type;
public:
	/// the instance collection base type
	typedef	typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	/// pointer type for instance collections
	typedef	never_ptr<const instance_collection_generic_type>
						instance_collection_ptr_type;
private:
	const instance_collection_ptr_type	inst_collection_ref;
protected:
	simple_nonmeta_instance_reference();
public:
	explicit
	simple_nonmeta_instance_reference(const instance_collection_ptr_type);

	~simple_nonmeta_instance_reference();

	ostream&
	what(ostream&) const;

	using parent_type::dump;

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	instance_collection_ptr_type
	get_inst_base_subtype(void) const;

#if 0
// only needed if included statically, such as in member_instance_reference
// however, we don't support members references of 
// non-meta instance references... yet.
protected:
	void
	collect_transient_info_base(persistent_object_manager& ) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
#endif

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

};	// end class nonmeta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_NONMETA_INST_REF_H__

