/**
	\file "Object/inst/connection_policy.cc"
	$Id: connection_policy.cc,v 1.5 2008/10/05 23:00:10 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "Object/inst/connection_policy.h"
#include "Object/devel_switches.h"
#include "util/IO_utils.tcc"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
using util::write_value;
using util::read_value;
#include "util/using_ostream.h"

//=============================================================================
// bool_connect_policy method definitions
// TODO: handle directions

/**
	Attributes are simply copied up the instance hierarchy
	through ports.  
 */
void
bool_connect_policy::initialize_actual_direction(const this_type& t) {
	attributes = t.attributes;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Check for conflicting flags when assigning.
 */
good_bool
bool_connect_policy::set_connection_flags(const connection_flags_type f) {
	// no possible conflicts yet
	attributes = f;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_connect_policy::write_flags(ostream& o) const {
	write_value(o, attributes);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_connect_policy::read_flags(istream& i) {
	read_value(i, attributes);
}

//=============================================================================
// channel_connect_policy method definitions
/**
	Check for conflicting flags.  
	Won't bother templating this method unless something else other
	than channels is directional.  
	Is this only called by CHP visits?
	Checks can be a lot more sophisticated, depending on desired semantics.
 */
good_bool
channel_connect_policy::set_connection_flags(
		const connection_flags_type f) {
	if (f & CONNECTED_CHP_PRODUCER) {
		if (direction_flags & CONNECTED_TO_NONCHP_PRODUCER) {
			cerr << "Error: cannot connect to producer by both "
				"aliasing and CHP!" << endl;
			return good_bool(false);
		}
	} 
	// mutually exclusive, by caller
	else if (f & CONNECTED_CHP_CONSUMER) {
		if (direction_flags & CONNECTED_TO_NONCHP_CONSUMER) {
			cerr << "Error: cannot connect to consumer by both "
				"aliasing and CHP!" << endl;
			return good_bool(false);
		}
	}
	const connection_flags_type _or = f | direction_flags;
	if (!check_meta_nonmeta_usage(_or, "channel").good) {
		// already have error message
		return good_bool(false);
	}
	// already connected in CHP, connecting again OK
	direction_flags |= _or;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks to make sure a producer/consumer isn't referenced both
	by meta (constant) and nonmeta means.  
	\param _or the bitwise or of two flag sets to check.  
	\param n meta-type name.  
 */
good_bool
channel_connect_policy::check_meta_nonmeta_usage(
		const connection_flags_type _or, 
		const char* n) {
	bool good = true;
	if ((_or & CONNECTED_PRODUCER_IS_META) &&
			(_or & CONNECTED_PRODUCER_IS_NONMETA)) {
		cerr << "Error: cannot mix meta- and nonmeta-referenced " <<
			n << " in producer alias." << endl;
		good = false;
	}
	if ((_or & CONNECTED_CONSUMER_IS_META) &&
			(_or & CONNECTED_CONSUMER_IS_NONMETA)) {
		cerr << "Error: cannot mix meta- and nonmeta-referenced " <<
			n << " in consumer alias." << endl;
		good = false;
	}
	return good_bool(good);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write out flags.  
 */
void
channel_connect_policy::write_flags(ostream& o) const {
	write_value(o, direction_flags);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write out flags.  
 */
void
channel_connect_policy::read_flags(istream& i) {
	read_value(i, direction_flags);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

