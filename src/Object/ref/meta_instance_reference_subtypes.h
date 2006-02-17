/**
	\file "Object/ref/meta_instance_reference_subtypes.h"
	Subtype classification for meta-instance-reference base classes.
	This file was reincarnated from "Object/art_object_inst_ref_subtypes.h".
	$Id: meta_instance_reference_subtypes.h,v 1.4.18.1 2006/02/17 05:07:44 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_H__
#define	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_H__

#include "Object/devel_switches.h"
#include "Object/ref/meta_instance_reference_base.h"
#include "Object/traits/class_traits_fwd.h"
// might as well include it here
#include "Object/ref/nonmeta_instance_reference_subtypes.h"

namespace HAC {
namespace entity {
//=============================================================================

#define	META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE	template <class Tag>
#define	META_INSTANCE_REFERENCE_CLASS		meta_instance_reference<Tag>

META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class meta_instance_reference :
#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
	public meta_instance_reference_base
#else
	virtual public meta_instance_reference_base, 
	virtual public class_traits<Tag>::nonmeta_instance_reference_base_type
#endif
{
#if !DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
	typedef	typename class_traits<Tag>::nonmeta_instance_reference_base_type
						nonmeta_parent_type;
#endif
	typedef	META_INSTANCE_REFERENCE_CLASS	this_type;
protected:
#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
	meta_instance_reference() : meta_instance_reference_base() { }
#else
	meta_instance_reference() : meta_instance_reference_base(), 
		nonmeta_parent_type() { }
#endif
public:
virtual	~meta_instance_reference() { }

#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
virtual	size_t
	dimensions(void) const = 0;

	// consider sub-typing?
virtual	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const = 0;

	bool
	may_be_type_equivalent(const meta_instance_reference_base&) const;

	bool
	must_be_type_equivalent(const meta_instance_reference_base&) const;
#endif
};	// end class meta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_H__

