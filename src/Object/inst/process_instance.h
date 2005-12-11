/**
	\file "Object/inst/process_instance.h"
	Definition of process instance base class.  
	$Id: process_instance.h,v 1.2.20.1 2005/12/11 00:45:39 fang Exp $
 */

#ifndef	__OBJECT_INST_PROCESS_INSTANCE_H__
#define	__OBJECT_INST_PROCESS_INSTANCE_H__

#include "Object/traits/proc_traits.h"
#include "Object/inst/state_instance.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	An actual instantiated instance of an process.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
	Needs to be pool allocated for efficient unique construction. 
 */
class class_traits<process_tag>::state_instance_base {
};	// end class state_instance_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_INST_PROCESS_INSTANCE_H__

