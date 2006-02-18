/**
	\file "Object/ref/meta_instance_reference_subtypes.h"
	Subtype classification for meta-instance-reference base classes.
	This file was reincarnated from "Object/art_object_inst_ref_subtypes.h".
	$Id: meta_instance_reference_subtypes.h,v 1.4.18.2 2006/02/18 08:29:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_H__
#define	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_H__

#include "Object/ref/meta_instance_reference_base.h"
#include "Object/traits/class_traits_fwd.h"
// might as well include it here
// #include "Object/ref/nonmeta_instance_reference_subtypes.h"

namespace HAC {
namespace entity {
//=============================================================================

#define	META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE	template <class Tag>
#define	META_INSTANCE_REFERENCE_CLASS		meta_instance_reference<Tag>

META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class meta_instance_reference : public meta_instance_reference_base {
	typedef	META_INSTANCE_REFERENCE_CLASS	this_type;
protected:
	meta_instance_reference() : meta_instance_reference_base() { }
public:
virtual	~meta_instance_reference() { }

virtual	size_t
	dimensions(void) const = 0;

	// consider sub-typing?
virtual	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const = 0;

	bool
	may_be_type_equivalent(const meta_instance_reference_base&) const;

	bool
	must_be_type_equivalent(const meta_instance_reference_base&) const;

};	// end class meta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_H__

