/**
	\file "Object/inst/connection_policy.cc"
	$Id: connection_policy.cc,v 1.1.2.3 2006/11/20 04:57:39 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "Object/inst/connection_policy.h"
#include "util/IO_utils.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
/**
	Won't bother templating this method unless something else other
	than channels is directional.  
	Is this only called by CHP visits?
	Checks can be a lot more sophisticated, depending on desired semantics.
 */
good_bool
directional_connect_policy<true>::set_connection_flags(const unsigned char f) {
	if (f & CONNECTED_CHP_PRODUCER) {
		if (direction_flags & CONNECTED_TO_PRODUCER) {
			cerr << "Error: cannot connect to producer by both "
				"aliasing and CHP!" << endl;
			return good_bool(false);
		}
	} 
	// mutually exclusive, by caller
	else if (f & CONNECTED_CHP_CONSUMER) {
		if (direction_flags & CONNECTED_TO_CONSUMER) {
			cerr << "Error: cannot connect to consumer by both "
				"aliasing and CHP!" << endl;
			return good_bool(false);
		}
	}
	// already connected in CHP, connecting again OK
	direction_flags |= f;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

