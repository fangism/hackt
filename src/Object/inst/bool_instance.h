/**
	\file "Object/inst/bool_instance.h"
	Boolean state instance.  
	$Id: bool_instance.h,v 1.1.2.2 2005/08/17 03:15:01 fang Exp $
 */

#ifndef	__OBJECT_INST_BOOL_INSTANCE_H__
#define	__OBJECT_INST_BOOL_INSTANCE_H__

#include "Object/traits/bool_traits.h"
#include "Object/inst/state_instance.h"

namespace ART {
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

	Should be pool allocated for efficiency.  
 */
class class_traits<bool_tag>::state_instance_base {
#if !EMPTY_PLACEHOLDER_STATE_INSTANCE
#if 0
protected:
	int						state;
#endif
// persistence methods:
	STATE_INSTANCE_GET_ACTUALS_PROTO {
		return state_instance_actuals_ptr_type(NULL);
	}

	STATE_INSTANCE_SET_ACTUALS_PROTO { }

	STATE_INSTANCE_PERSISTENCE_EMPTY_DEFS
#endif
};	// end class state_instance_base

//-----------------------------------------------------------------------------
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_BOOL_INSTANCE_H__

