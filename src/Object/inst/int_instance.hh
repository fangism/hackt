/**
	\file "Object/inst/int_instance.hh"
	Integer state information.  
	$Id: int_instance.hh,v 1.4 2006/01/22 18:20:06 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INT_INSTANCE_H__
#define	__HAC_OBJECT_INST_INT_INSTANCE_H__

#include "Object/traits/int_traits.hh"
#include "Object/inst/state_instance.hh"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Placeholder for...
	State information for an integer.  
 */
struct class_traits<int_tag>::state_instance_base {
};	// end struct state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INT_INSTANCE_H__

