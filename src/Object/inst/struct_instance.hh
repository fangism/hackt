/**
	\file "Object/inst/struct_instance.hh"
	Definition of struct state.  
	$Id: struct_instance.hh,v 1.4 2006/01/22 18:20:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_STRUCT_INSTANCE_H__
#define	__HAC_OBJECT_INST_STRUCT_INSTANCE_H__

#include "Object/traits/struct_traits.hh"
#include "Object/inst/state_instance.hh"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Placeholder for...
	An actual instantiated instance of a struct.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
struct class_traits<datastruct_tag>::state_instance_base {
};	// end struct state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_STRUCT_INSTANCE_H__

