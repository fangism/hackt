/**
	\file "Object/inst/connection_policy.cc"
	$Id: connection_policy.cc,v 1.18 2011/03/23 00:36:10 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <iomanip>
#include <string>
#include "Object/inst/connection_policy.hh"
#include "Object/devel_switches.hh"
#include "common/TODO.hh"
#include "util/IO_utils.tcc"
#include "util/stacktrace.hh"

namespace HAC {
namespace entity {
using std::string;
using util::write_value;
using util::read_value;
#include "util/using_ostream.hh"

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
	"atomic",
	"RESERVED-12",
	"RESERVED-13",
	"RESERVED-14",
	"RESERVED-15",

	"loc-use",
	"loc-def",
	"sub-use",
	"sub-def",
	"port?",
	"port!",
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

#if 0
const char*
bool_connect_policy::atomic_attribute_names[] = {
	"RESERVED-0a",
	"RESERVED-1a",
	"RESERVED-2a",
	"RESERVED-3a",
	"RESERVED-4a",
	"RESERVED-5a",
	"RESERVED-6a",
	"RESERVED-7a",

	"RESERVED-8a",
	"RESERVED-9a",
	"RESERVED-10a",
	"atomic",
	"RESERVED-12a",
	"RESERVED-13a",
	"RESERVED-14a",
	"RESERVED-15a",

	"RESERVED-16a",
	"RESERVED-17a",
	"RESERVED-18a",
	"RESERVED-19a",
	"port?",
	"port!",
	"RESERVED-22a",
// this range is reserved for implicit attributes
	"port-alias",

	"loc-use",
	"RESERVED-25a",
	"loc-def",
	"RESERVED-27a",
	"sub-use",
	"RESERVED-29a",
	"sub-def",
	"RESERVED-31a",
};
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Attributes are simply copied up the instance hierarchy
	through ports.  
	\param t formal instance alias
 */
void
bool_connect_policy::initialize_actual_direction(const this_type& t) {
	STACKTRACE_VERBOSE;
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
	// for atomic attributes
	if (t.attributes & BOOL_ANY_RTE_FANOUT)
		attributes |= BOOL_SUBSTRUCT_RTE_FANOUT;
	if (t.attributes & BOOL_ANY_RTE_FANIN)
		attributes |= BOOL_SUBSTRUCT_RTE_FANIN;
#if ENABLE_STACKTRACE
	dump_raw_attributes(STACKTRACE_INDENT_PRINT("after: ")) << endl;
#endif
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
/**
	Declare that a bool is driven or used locally.
	The intent is to consider this node used to avoid being optimized away.
 */
good_bool
bool_connect_policy::declare_direction(const direction_type d) {
#if 0
	cerr <<
"Warning: direction declaration on bools are ignored (inferred from PRS only)."
		<< endl;
#else
	switch (d) {
	case CHANNEL_TYPE_SEND:
		attributes |=
			BOOL_LOCAL_PRS_FANIN_PULL_UP |
			BOOL_LOCAL_PRS_FANIN_PULL_DN;
		if (is_input_port()) {
			cerr <<
	"Error: driving a read-only (input) port (using '!') is forbidden."
				<< endl;
			return good_bool(false);
		}
		break;
	case CHANNEL_TYPE_RECEIVE:
		// output ports are always allowed to fanout locally
		attributes |=
			BOOL_LOCAL_PRS_FANOUT_PULL_UP |
			BOOL_LOCAL_PRS_FANOUT_PULL_DN;
		break;
	default:
		cerr << "Error: unhandled bool direction directive, "
			<< size_t(d) << "." << endl;
		return good_bool(false);
	}
#endif
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
	if (l.is_atomic() ^ r.is_atomic()) {
		cerr << "Error: cannot alias atomic and non-atomic bools." << endl;
		// TODO: caller needs to print diagnostic details
		return good_bool(false);
	}
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
void
bool_connect_policy::atomic_only_attribute(void) const {
	if (!is_atomic()) {
		cerr << "Error: cannot apply atomic attribute to non-atomic bool." << endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_connect_policy::nonatomic_only_attribute(void) const {
	if (is_atomic()) {
		cerr << "Error: cannot apply non-atomic attribute to atomic bool." << endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_connect_policy::nonatomic_only_prs_literal(void) const {
	if (is_atomic()) {
		cerr <<
"Error: atomic bools cannot participate in production rules." << endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_connect_policy::atomic_only_rte_literal(void) const {
	if (!is_atomic()) {
		cerr <<
"Error: only atomic bools can be defined with atomic expressions." << endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: diagnostic with node name please.
 */
void
bool_connect_policy::set_supply(const bool t) {
	static const char err_msg[] =
		"Error: a node cannot be both supply-high and supply-low.";
	nonatomic_only_attribute();
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
	nonatomic_only_attribute();
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
void
bool_connect_policy::set_atomic(const bool t) {
	// must make sure non-atomic attributes have not been set (sanity)
	if (attributes & BOOL_EXPLICIT_NONATOMIC_ATTRIBUTES_MASK) {
		cerr <<
"Error: cannot declare a bool atomic after it has other non-atomic attributes."
			<< endl;
		THROW_EXIT;
	}
	if (attributes & BOOL_CONNECTIVITY_OR_MASK) {
		cerr <<
"Error: cannot declare a bool atomic if it participates in production rules."
			<< endl;
		THROW_EXIT;
	}
	attributes |= BOOL_ATOMIC_INIT_MASK;	// or = (assign)?
	// want to retain attributes like BOOL_IS_ALIASED_TO_PORT
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only allow connection to fanin if node is not marked as read-only '?'.
 */
good_bool
bool_connect_policy::prs_fanin(const bool dir) {
	nonatomic_only_prs_literal();
	attributes |= dir ?
		BOOL_LOCAL_PRS_FANIN_PULL_UP :
		BOOL_LOCAL_PRS_FANIN_PULL_DN;
	if (is_input_port()) {
		cerr <<
"Error: driving a read-only (input) port with a production rule is forbidden."
			<< endl;
		// THROW_EXIT;
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
bool_connect_policy::rte_fanin(void) {
	atomic_only_rte_literal();
	if (is_input_port()) {
		cerr <<
"Error: a read-only (atomic) port cannot be re-defined."
			<< endl;
		// THROW_EXIT;
// TODO: diagnose undefined output atomic ports
		return good_bool(false);
	} else if (attributes & BOOL_ANY_RTE_FANIN) {
		cerr <<
"Error: atomic bool is already defined and cannot be re-defined."
			<< endl;
		return good_bool(false);
	}
	attributes |= BOOL_LOCAL_RTE_FANIN;
	return good_bool(true);
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
#if ENABLE_STACKTRACE
	dump_raw_attributes(o << " <") << '>';		// for debug
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
	const char** const anames = get_attribute_names_set();
	const char** p = anames;
while (temp && p < anames +num_display_attributes(implicit)) {
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
	const char** const anames = get_attribute_names_set();
	const char** p = anames;
while (temp && p < anames +num_display_attributes(implicit)) {
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

const char*
process_connect_policy::attribute_names[] = {
	"loc!",
	"sub!",
	"RESERVED-02",
	"port?",
	"loc?",
	"sub?",
	"RESERVED-06",
	"port!",
	"port-alias"
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
		direction_flags |= CONNECTED_TO_LOCAL_PRODUCER;
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
		direction_flags |= CONNECTED_TO_LOCAL_CONSUMER;
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
while (temp && p < attribute_names +PROCESS_NUM_ATTRIBUTES) {
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

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

