/**
	\file "Object/common/dump_flags.cc"
	Global static initializer for default.  
	$Id: dump_flags.cc,v 1.2 2005/09/14 15:30:27 fang Exp $
 */

#include "Object/common/dump_flags.h"

namespace ART {
namespace entity {
//=============================================================================

const dump_flags
dump_flags::default_value = dump_flags();

//=============================================================================
}	// end namespace entity
}	// end namespace ART

