/**
	\file "Object/inst/bool_instance.hh"
	Boolean state instance.  
	$Id: bool_instance.hh,v 1.4 2006/01/22 18:19:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_BOOL_INSTANCE_H__
#define	__HAC_OBJECT_INST_BOOL_INSTANCE_H__

#include "Object/traits/bool_traits.hh"
#include "Object/inst/state_instance.hh"

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
struct class_traits<bool_tag>::state_instance_base {
	// eventually add state information
};	// end struct state_instance_base

//-----------------------------------------------------------------------------
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_BOOL_INSTANCE_H__

