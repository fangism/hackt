/**
	\file "Object/inst/enum_instance.h"
	Enumeration instance state.  
	$Id: enum_instance.h,v 1.3 2005/12/13 04:15:28 fang Exp $
 */

#ifndef	__OBJECT_INST_ENUM_INSTANCE_H__
#define	__OBJECT_INST_ENUM_INSTANCE_H__

#include "Object/traits/enum_traits.h"
#include "Object/inst/state_instance.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Placedholder for...
	an actual instantiated instance of an enum.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
class class_traits<enum_tag>::state_instance_base {
};	// end class state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_INST_ENUM_INSTANCE_H__

