/**
	\file "Object/lang/SPEC_fwd.h"
	Useful forward declarations for SPEC headers.  
	$Id: SPEC_fwd.h,v 1.2 2006/02/04 06:43:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_FWD_H__
#define	__HAC_OBJECT_LANG_SPEC_FWD_H__

#include "Object/inst/instance_pool_fwd.h"
#include "util/memory/pointer_classes_fwd.h"

namespace HAC {
namespace entity {
class unroll_context;
struct bool_tag;
template <class> class state_instance;
namespace PRS {
	class literal;
	class rule_dump_context;
}	// end namespace PRS
namespace SPEC {
class footprint;
class footprint_directive;

//=============================================================================
typedef	util::memory::count_ptr<PRS::literal>		literal_ptr_type;
typedef	state_instance<bool_tag>			bool_instance_type;
typedef	instance_pool<bool_instance_type>		node_pool_type;

class directive;
class directives_set;

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_SPEC_FWD_H__

