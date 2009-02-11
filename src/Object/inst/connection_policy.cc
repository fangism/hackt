/**
	\file "Object/inst/connection_policy.cc"
	$Id: connection_policy.cc,v 1.10 2009/02/11 02:35:10 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <string>
#include "Object/inst/connection_policy.h"
#include "Object/devel_switches.h"
#include "util/IO_utils.tcc"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
using std::string;
using util::write_value;
using util::read_value;
#include "util/using_ostream.h"

//=============================================================================
// bool_connect_policy method definitions
// TODO: handle directions

/**
	These strings should be ordered according their corresponding
	bit fields (enum) in the attribute flags.  
 */
const char*
bool_connect_policy::attribute_names[] = {
	"iscomb",
	"!autokeeper",
	"isrvc1",
	"isrvc2",
	"isrvc3",
	"may_interfere",
	"may_weak_interfere",
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Attributes are simply copied up the instance hierarchy
	through ports.  
 */
void
bool_connect_policy::initialize_actual_direction(const this_type& t) {
	STACKTRACE_VERBOSE;
	attributes = t.attributes;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Check for conflicting flags when assigning.
 */
good_bool
bool_connect_policy::set_connection_flags(const connection_flags_type f) {
	STACKTRACE_VERBOSE;
	// no possible conflicts yet
	attributes = f;
	return good_bool(true);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
bool_connect_policy::synchronize_flags(this_type& l, this_type& r) {
	STACKTRACE_VERBOSE;
	l.attributes |= r.attributes;
	r.attributes = l.attributes;
	// TODO: handle direction checking!
	STACKTRACE_INDENT_PRINT("flags = " << l.attributes << ", "
		<< r.attributes << endl);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print only names of non-default attribute values.  
	This is only for object dump debugging purposes, 
	format is not important.
	TODO: print direction flags separately?
 */
ostream&
bool_connect_policy::dump_attributes(ostream& o) const {
	// maintain order of these strings w.r.t. flag enums
if (has_nondefault_attributes()) {
	o << " @[";
	dump_flat_attributes(o);
	o << " ]";
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only applies to hflat, called by cflat_prs_printer()
 */
ostream&
bool_connect_policy::dump_flat_attributes(ostream& o) const {
	connection_flags_type temp = attributes;	// better be unsigned!
	const char** p = attribute_names;
while (temp) {
	if (temp & 1) {
		o << ' ' << *p;
	}
	++p;
	temp >>= 1;
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only applies to hflat, called by cflat_prs_printer()
 */
ostream&
bool_connect_policy::dump_split_attributes(ostream& o, const string& n) const {
	connection_flags_type temp = attributes;	// better be unsigned!
	const char** p = attribute_names;
while (temp) {
	if (temp & 1) {
		o << "@ " << n << ' ' << *p << endl;
	}
	++p;
	temp >>= 1;
}
	return o;
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
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
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

