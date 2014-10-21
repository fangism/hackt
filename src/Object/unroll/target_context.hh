/**
	\file "Object/unroll/target_context.hh"
	A conditional typedef on forward declarations.
 */
#ifndef	__HAC_OBJECT_UNROLL_TARGET_CONTEXT_HH__
#define	__HAC_OBJECT_UNROLL_TARGET_CONTEXT_HH__

#include "Object/devel_switches.hh"

namespace HAC {
namespace entity {


#if CACHE_SUBSTRUCTURES_IN_FOOTPRINT
class footprint;
typedef	footprint			target_context;
#else
class unroll_context;
typedef	const unroll_context		target_context;
#endif

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_TARGET_CONTEXT_HH__
