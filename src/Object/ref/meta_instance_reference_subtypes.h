/**
	\file "Object/ref/meta_instance_reference_subtypes.h"
	Subtype classification for meta-instance-reference base classes.
	This file was reincarnated from "Object/art_object_inst_ref_subtypes.h".
	$Id: meta_instance_reference_subtypes.h,v 1.2.32.1 2005/12/11 00:45:45 fang Exp $
 */

#ifndef	__OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_H__
#define	__OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_H__

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
	virtual public meta_instance_reference_base, 
	virtual public class_traits<Tag>::nonmeta_instance_reference_base_type {
	typedef	typename class_traits<Tag>::nonmeta_instance_reference_base_type
						nonmeta_parent_type;
	typedef	META_INSTANCE_REFERENCE_CLASS	this_type;
protected:
	meta_instance_reference() : meta_instance_reference_base(), 
		nonmeta_parent_type() { }
public:
virtual	~meta_instance_reference() { }
};	// end class meta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_H__

