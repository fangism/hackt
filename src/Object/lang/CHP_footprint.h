/**
	\file "Object/lang/CHP_footprint.h"
	Class definitions for unrolled CHP action instances.  
	$Id: CHP_footprint.h,v 1.3.30.1 2007/08/31 22:59:25 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_FOOTPRINT_H__
#define	__HAC_OBJECT_LANG_CHP_FOOTPRINT_H__

#include <iosfwd>
// #include <vector>
// #include "Object/expr/types.h"

namespace HAC {
namespace entity {
#if 0
namespace PRS {
	class cflat_visitor;
}
#endif
namespace CHP {
// typedef	size_t			channel_index_type;

//=============================================================================
/**
	The CHP footprint consists of a map of locally allocated events, 
	based on the set of CHP actions in the process definition.
	Event successor/predecessor relationships are referenced
	using local indices because events cannot directly cross processes.  
	References, however cannot be resolved at compile-time.  
	During global allocation, each process's events can be bulk allocated
	by essentially copying the event footprint.
 */
class local_event_footprint {
	// parent process/channel footprint? need back-link?
public:
};	// end class footprint

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_FOOTPRINT_H__

