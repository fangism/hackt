/**
	\file "Object/inst/connection_policy.cc"
	$Id: connection_policy.cc,v 1.17.2.5 2011/03/22 02:43:52 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <iomanip>
#include <string>
#include "Object/inst/connection_policy.h"
#include "Object/devel_switches.h"
#include "common/TODO.h"
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
	Mostly for connectivity diagnostics debugging.
#define	DUMP_CONNECTIVITY_ATTRIBUTES			1
 */

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
	"supply_low",

	"supply_high",
	"reset_low",
	"reset_high",
	"RESERVED-11",
	"RESERVED-12",
	"RESERVED-13",
	"RESERVED-14",
	"RESERVED-15",

	"RESERVED-16",
	"RESERVED-17",
	"RESERVED-18",
	"RESERVED-19",
	"RESERVED-20",
	"RESERVED-21",
	"RESERVED-22",
// this range is reserved for implicit attributes
	"port-alias",

	"loc-FO-",
	"loc-FO+",
	"loc-FI-",
	"loc-FI+",
	"sub-FO-",
	"sub-FO+",
	"sub-FI-",
	"sub-FI+",
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Attributes are simply copied up the instance hierarchy
	through ports.  
	\param t formal instance alias
 */
void
bool_connect_policy::initialize_actual_direction(const this_type& t) {
	STACKTRACE_VERBOSE;
#if BOOL_PRS_CONNECTIVITY_CHECKING
#if ENABLE_STACKTRACE
	t.dump_raw_attributes(STACKTRACE_INDENT_PRINT("formal: ")) << endl;
	dump_raw_attributes(STACKTRACE_INDENT_PRINT("init: ")) << endl;
#endif
	attributes = (t.attributes & BOOL_ATTRIBUTES_MASK)
		| (attributes & BOOL_INIT_ATTRIBUTES_MASK);
		// to preserve local port flag
	if (t.attributes & BOOL_ANY_FANOUT_PULL_DN)
		attributes |= BOOL_SUBSTRUCT_FANOUT_PULL_DN;
	if (t.attributes & BOOL_ANY_FANOUT_PULL_UP)
		attributes |= BOOL_SUBSTRUCT_FANOUT_PULL_UP;
	if (t.attributes & BOOL_ANY_FANIN_PULL_DN)
		attributes |= BOOL_SUBSTRUCT_FANIN_PULL_DN;
	if (t.attributes & BOOL_ANY_FANIN_PULL_UP)
		attributes |= BOOL_SUBSTRUCT_FANIN_PULL_UP;
#if ENABLE_STACKTRACE
	dump_raw_attributes(STACKTRACE_INDENT_PRINT("after: ")) << endl;
#endif
#else	// BOOL_PRS_CONNECTIVITY_CHECKING
	attributes = t.attributes;
#endif	// BOOL_PRS_CONNECTIVITY_CHECKING
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Check for conflicting flags when assigning.
 */
good_bool
bool_connect_policy::set_connection_flags(const connection_flags_type f) {
	STACKTRACE_VERBOSE;
	// no possible conflicts yet
	// maybe later with connectivity constraints
	attributes |= f;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
bool_connect_policy::declare_direction(const direction_type) const {
	cerr <<
"Warning: direction declaration on bools are ignored (inferred from PRS only)."
		<< endl;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: in future, not every field will be OR'd, use masks
	to compute result.
 */
good_bool
bool_connect_policy::synchronize_flags(this_type& l, this_type& r) {
	STACKTRACE_VERBOSE;
	l.attributes |= r.attributes;
	r.attributes = l.attributes;
	// TODO: handle direction checking!
	STACKTRACE_INDENT_PRINT("flags/connectivity = 0x" << std::hex <<
		l.attributes << ", 0x" << std::hex << r.attributes << endl);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
bool_connect_policy::has_nondefault_attributes(const bool implicit) const {
	// if any attribute bits are set
	return implicit ? attributes // & BOOL_INIT_ATTRIBUTES_MASK
		: (attributes & BOOL_EXPLICIT_ATTRIBUTES_MASK);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: diagnostic with node name please.
 */
void
bool_connect_policy::set_supply(const bool t) {
	static const char err_msg[] =
		"Error: a node cannot be both supply-high and supply-low.";
	if (t) {
		attributes |= BOOL_SUPPLY_HIGH;
		if (attributes & BOOL_SUPPLY_LOW) {
			cerr << err_msg << endl;
			THROW_EXIT;
		}
	} else {
		attributes |= BOOL_SUPPLY_LOW;
		if (attributes & BOOL_SUPPLY_HIGH) {
			cerr << err_msg << endl;
			THROW_EXIT;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: diagnostic with node name please.
 */
void
bool_connect_policy::set_reset(const bool t) {
	static const char err_msg[] =
		"Error: a node cannot be both reset-high and reset-low.";
	if (t) {
		attributes |= BOOL_RESET_HIGH;
		if (attributes & BOOL_RESET_LOW) {
			cerr << err_msg << endl;
			THROW_EXIT;
		}
	} else {
		attributes |= BOOL_RESET_LOW;
		if (attributes & BOOL_RESET_HIGH) {
			cerr << err_msg << endl;
			THROW_EXIT;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For debugging only.
 */
ostream&
bool_connect_policy::dump_raw_attributes(ostream& o) const {
	return o << "0x" << std::hex << attributes;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print only names of non-default attribute values.  
	This is only for object dump debugging purposes, 
	format is not important.
	TODO: print direction flags separately?
	Print implicit attributes too for debugging.
 */
ostream&
bool_connect_policy::dump_attributes(ostream& o, const bool implicit) const {
	// maintain order of these strings w.r.t. flag enums
if (has_nondefault_attributes(implicit)) {
	o << " @[";
	dump_flat_attributes(o, implicit);
	o << " ]";
}
#if 0
	dump_raw_attributes(o << ' ');
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only applies to hflat, called by cflat_prs_printer()
 */
ostream&
bool_connect_policy::dump_flat_attributes(ostream& o,
		const bool implicit) const {
	connection_flags_type temp = attributes;	// better be unsigned!
	const char** p = attribute_names;
while (temp && p < attribute_names +num_display_attributes(implicit)) {
	// b/c upper bits are connectivity
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
bool_connect_policy::dump_split_attributes(ostream& o, const string& n, 
		const bool implicit) const {
	connection_flags_type temp = attributes;	// better be unsigned!
	const char** p = attribute_names;
while (temp && p < attribute_names +num_display_attributes(implicit)) {
	// b/c upper bits are connectivity
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

const char*
channel_connect_policy::attribute_names[] = {
	// producer flags
	"port!",
	"!!",
	"sub!",
	"chp!",
	"meta!",
	"~meta!",
	"RESERVED-06",
	"RESERVED-07",
	// consumer flags
	"port?",
	"??",
	"sub?",
	"chp?",
	"meta?",
	"~meta?",
	"RESERVED-14",
	"RESERVED-15",
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
good_bool
channel_connect_policy::declare_direction(const direction_type) const {
	cerr <<
"Warning: direction declaration on channels ignored (inferred from CHP only)."
		<< endl;
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
	Only applies to hflat, called by cflat_prs_printer()
 */
ostream&
channel_connect_policy::dump_flat_attributes(ostream& o) const {
	connection_flags_type temp = direction_flags;	// better be unsigned!
	const char** p = attribute_names;
while (temp && p < attribute_names +16) {
	// b/c upper bits are connectivity
	if (temp & 1) {
		o << ' ' << *p;
	}
	++p;
	temp >>= 1;
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_connect_policy::dump_attributes(ostream& o) const {
if (has_nondefault_attributes()) {
	o << " @[";
	dump_flat_attributes(o);
	o << " ]";
}
	return o;
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
// class process_connect_policy method definitions

#if PROCESS_CONNECTIVITY_CHECKING
const char*
process_connect_policy::attribute_names[] = {
	"port!",
	"sub!",
	"PRS!",
	"RESERVED-3",
	"port?",
	"sub?",
	"PRS?",
	"RESERVED-7"
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Check for conflicting flags.  
	Won't bother templating this method unless something else other
	than channels is directional.  
	Is this only called by CHP visits?
	Checks can be a lot more sophisticated, depending on desired semantics.
 */
good_bool
process_connect_policy::set_connection_flags(
		const connection_flags_type f) {
	STACKTRACE_VERBOSE;
	// TODO: rewrite me
	const connection_flags_type _or = f | direction_flags;
	direction_flags |= _or;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Declare that a process is sourced or sinked locally.
 */
good_bool
process_connect_policy::declare_direction(const direction_type d) {
	// should we check whether or not is already connected?
	switch (d) {
	case CHANNEL_TYPE_SEND:
		if (direction_flags & CONNECTED_TO_NONPORT_PRODUCER) {
		cerr << "Error: channel is already connected to a producer."
			<< endl;
			return good_bool(false);
		}
		if (direction_flags & CONNECTED_PORT_FORMAL_PRODUCER) {
			cerr << "Error: cannot send on a receive-only port."
				<< endl;
			return good_bool(false);
		}
		direction_flags |= CONNECTED_PRS_PRODUCER;
		break;
	case CHANNEL_TYPE_RECEIVE:
		if (direction_flags & CONNECTED_TO_NONPORT_CONSUMER) {
		cerr << "Error: channel is already connected to a consumer."
			<< endl;
			return good_bool(false);
		}
		if (direction_flags & CONNECTED_PORT_FORMAL_CONSUMER) {
			cerr << "Error: cannot receive on a send-only port."
				<< endl;
			return good_bool(false);
		}
		direction_flags |= CONNECTED_PRS_CONSUMER;
		break;
	default:
		cerr << "Error: unsupported direction_type: " << int(d) << endl;
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only applies to hflat, called by cflat_prs_printer()
 */
ostream&
process_connect_policy::dump_flat_attributes(ostream& o) const {
	connection_flags_type temp = direction_flags;	// better be unsigned!
	const char** p = attribute_names;
while (temp && p < attribute_names +8) {
	// b/c upper bits are connectivity
	if (temp & 1) {
		o << ' ' << *p;
	}
	++p;
	temp >>= 1;
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_connect_policy::dump_attributes(ostream& o) const {
//	o << " OHAI";
if (has_nondefault_attributes()) {
	o << " @[";
	dump_flat_attributes(o);
	o << " ]";
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_connect_policy::write_flags(ostream& o) const {
	write_value(o, direction_flags);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_connect_policy::read_flags(istream& i) {
	read_value(i, direction_flags);
}
#endif	// PROCESS_CONNECTIVITY_CHECKING

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

