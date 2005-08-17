/**
	\file "Object/inst/enum_instance.h"
	Enumeration instance state.  
	$Id: enum_instance.h,v 1.1.2.2 2005/08/17 03:15:02 fang Exp $
 */

#ifndef	__OBJECT_INST_ENUM_INSTANCE_H__
#define	__OBJECT_INST_ENUM_INSTANCE_H__

#include "Object/traits/enum_traits.h"
#include "Object/inst/state_instance.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Placedholder for...
	an actual instantiated instance of an enum.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
class class_traits<enum_tag>::state_instance_base {
#if !EMPTY_PLACEHOLDER_STATE_INSTANCE
#if 0
protected:
	int				state;
#endif
	STATE_INSTANCE_GET_ACTUALS_PROTO {
		return state_instance_actuals_ptr_type(NULL);
	}

	STATE_INSTANCE_SET_ACTUALS_PROTO { }

	STATE_INSTANCE_PERSISTENCE_EMPTY_DEFS
#endif
};	// end class state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_ENUM_INSTANCE_H__

