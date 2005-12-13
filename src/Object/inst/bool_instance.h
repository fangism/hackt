/**
	\file "Object/inst/bool_instance.h"
	Boolean state instance.  
	$Id: bool_instance.h,v 1.3 2005/12/13 04:15:28 fang Exp $
 */

#ifndef	__OBJECT_INST_BOOL_INSTANCE_H__
#define	__OBJECT_INST_BOOL_INSTANCE_H__

#include "Object/traits/bool_traits.h"
#include "Object/inst/state_instance.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	ACTUALLY... this is now a placeholder for unique state, 
		not the state itself.  
	TODO: update these comments...

	An actual instantiated instance of a bool, 
	what used to be called "node".
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
	This is like PrsNode from prsim -- it contains simulation state.  

	Really don't want this to be persistently allocated...
	Consider using a non-allocator pool, one whose members
		need not be deallocated until the end of the program.  
		That would save from having to reference-count.
	Is *very expensive* to keep each instance in separate
		entry per instance persistently.  
	Save this idea for later.  
	Replace reference-count pointer with index (ID#) to static pool.
		Such a scheme would have non-reclaimable memory (not leak).  
		Consider using list_vector.
		Is saving/restoring state complicated?

 */
class class_traits<bool_tag>::state_instance_base {
	// eventually add state information
};	// end class state_instance_base

//-----------------------------------------------------------------------------
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_INST_BOOL_INSTANCE_H__

