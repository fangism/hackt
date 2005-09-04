/**
	\file "Object/inst/int_instance.h"
	Integer state information.  
	$Id: int_instance.h,v 1.2 2005/09/04 21:14:50 fang Exp $
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
};	// end class state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_INT_INSTANCE_H__

