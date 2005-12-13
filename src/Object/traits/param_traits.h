/**
	\file "Object/traits/param_traits.h"
	Traits and policies for general parameters.  
	This file used to be "Object/art_object_param_traits.h".
	$Id: param_traits.h,v 1.3 2005/12/13 04:15:38 fang Exp $
 */

#ifndef	__OBJECT_TRAITS_PARAM_TRAITS_H__
#define	__OBJECT_TRAITS_PARAM_TRAITS_H__

#include "Object/traits/class_traits.h"

namespace HAC {
namespace entity {
//-----------------------------------------------------------------------------
template <>
struct class_traits<parameter_value_tag> {
	typedef	param_instance_reference_base	
					nonmeta_instance_reference_base_type;
};	// end struct class_traits<parameter_value_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_TRAITS_PARAM_TRAITS_H__

