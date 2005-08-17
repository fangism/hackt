/**
	\file "Object/inst/int_instance.h"
	Integer state information.  
	$Id: int_instance.h,v 1.1.2.2 2005/08/17 03:15:02 fang Exp $
 */

#ifndef	__OBJECT_INST_INT_INSTANCE_H__
#define	__OBJECT_INST_INT_INSTANCE_H__

#include "Object/traits/int_traits.h"
#include "Object/inst/state_instance.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Placeholder for...
	State information for an integer.  
 */
class class_traits<int_tag>::state_instance_base {
#if !EMPTY_PLACEHOLDER_STATE_INSTANCE
#if 0
protected:
	int						state;
#endif
public:
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

#endif	// __OBJECT_INST_INT_INSTANCE_H__

