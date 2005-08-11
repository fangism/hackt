/**
	\file "Object/inst/enum_instance.h"
	Enumeration instance state.  
	$Id: enum_instance.h,v 1.1.2.1 2005/08/11 00:20:18 fang Exp $
 */

#ifndef	__OBJECT_INST_ENUM_INSTANCE_H__
#define	__OBJECT_INST_ENUM_INSTANCE_H__

#include "Object/traits/enum_traits.h"
#include "Object/inst/state_instance.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	An actual instantiated instance of an enum.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
class class_traits<enum_tag>::state_instance_base {
protected:
	int				state;
};	// end class state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_ENUM_INSTANCE_H__

