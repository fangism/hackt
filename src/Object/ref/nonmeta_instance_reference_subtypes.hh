/**
	\file "Object/ref/nonmeta_instance_reference_subtypes.hh"
	Class family for instance references in HAC.  
	This file used to be "Object/art_object_nonmeta_inst_ref_subtypes.h"
		in a previous life.  
	$Id: nonmeta_instance_reference_subtypes.hh,v 1.6 2006/03/20 02:41:07 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_NONMETA_INSTANCE_REFERENCE_SUBTYPES_H__
#define	__HAC_OBJECT_REF_NONMETA_INSTANCE_REFERENCE_SUBTYPES_H__

#include "Object/ref/nonmeta_instance_reference_base.hh"
#include "Object/ref/data_nonmeta_instance_reference.hh"
#include "Object/traits/class_traits_fwd.hh"

namespace HAC {
namespace entity {
//=============================================================================
// classes pint_nonmeta_instance_reference and pbool_nonmeta_instance_reference
//	are in "art_object_expr_param_ref.*"

//=============================================================================
#define	NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE	template <class Tag>
#define	NONMETA_INSTANCE_REFERENCE_CLASS	nonmeta_instance_reference<Tag>
/**
	Meta-class specific base classes for nonmeta references.  
	From these classes, simply-indexed, and member references will sprout.  
	This concept is modeled after the meta_instance_reference hierarchy.  
	The base class specified by the traits_type must be either
	nonmeta_instance_reference_base or some derivative thereof.  
 */
NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class nonmeta_instance_reference :
	public class_traits<Tag>::nonmeta_instance_reference_base_type {
public:
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::nonmeta_instance_reference_base_type
							parent_type;
protected:
	nonmeta_instance_reference() : parent_type() { }

virtual	~nonmeta_instance_reference() { }

};	// end class nonmeta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_NONMETA_INSTANCE_REFERENCE_SUBTYPES_H__

