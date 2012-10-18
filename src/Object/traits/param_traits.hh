/**
	\file "Object/traits/param_traits.hh"
	Traits and policies for general parameters.  
	This file used to be "Object/art_object_param_traits.h".
	$Id: param_traits.hh,v 1.5 2006/03/20 02:41:09 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_PARAM_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_PARAM_TRAITS_H__

#include "Object/traits/class_traits.hh"

namespace HAC {
namespace entity {
//-----------------------------------------------------------------------------
template <>
struct class_traits<parameter_value_tag> {
};	// end struct class_traits<parameter_value_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_PARAM_TRAITS_H__

