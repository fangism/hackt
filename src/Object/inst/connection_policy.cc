/**
	\file "Object/inst/connection_policy.cc"
	$Id: connection_policy.cc,v 1.1.2.1 2006/11/16 20:28:41 fang Exp $
 */

#include <iostream>
#include "Object/inst/connection_policy.h"
#include "util/IO_utils.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
/**
	Write out flags.  
 */
void
directional_connect_policy<true>::write_flags(ostream& o) const {
	util::write_value(o, direction_flags);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write out flags.  
 */
void
directional_connect_policy<true>::read_flags(istream& i) {
	util::read_value(i, direction_flags);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

