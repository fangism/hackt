/**
	\file "Object/inst/int_instance.h"
	Integer state information.  
	$Id: int_instance.h,v 1.1.2.1 2005/08/11 00:20:20 fang Exp $
 */

#ifndef	__OBJECT_INST_INT_INSTANCE_H__
#define	__OBJECT_INST_INT_INSTANCE_H__

#include "Object/traits/int_traits.h"
#include "Object/inst/state_instance.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	State information for an integer.  
 */
class class_traits<int_tag>::state_instance_base {
protected:
	int						state;
};	// end class state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_INT_INSTANCE_H__

