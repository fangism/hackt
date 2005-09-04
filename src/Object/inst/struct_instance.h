/**
	\file "Object/inst/struct_instance.h"
	Definition of struct state.  
	$Id: struct_instance.h,v 1.2 2005/09/04 21:14:53 fang Exp $
 */

#ifndef	__OBJECT_INST_STRUCT_INSTANCE_H__
#define	__OBJECT_INST_STRUCT_INSTANCE_H__

#include "Object/traits/struct_traits.h"
#include "Object/inst/state_instance.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Placeholder for...
	An actual instantiated instance of a struct.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
class class_traits<datastruct_tag>::state_instance_base {
};	// end class state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_STRUCT_INSTANCE_H__

