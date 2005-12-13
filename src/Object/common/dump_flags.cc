/**
	\file "Object/common/dump_flags.cc"
	Global static initializer for default.  
	$Id: dump_flags.cc,v 1.4 2005/12/13 04:15:17 fang Exp $
 */

#include "Object/common/dump_flags.h"

namespace HAC {
namespace entity {
//=============================================================================
// class dump_flags method and static member definitions

const dump_flags
dump_flags::default_value = dump_flags();

const dump_flags
dump_flags::no_owner = dump_flags(false);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dump_flags::dump_flags(const bool _show_owner) : show_owner(_show_owner) { }

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

