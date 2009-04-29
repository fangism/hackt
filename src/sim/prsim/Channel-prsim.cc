/**
	\file "sim/prsim/Channel-prsim.cc"
	$Id: Channel-prsim.cc,v 1.21 2009/04/29 05:33:40 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <iterator>			// for ostream_iterator
#include "sim/prsim/Channel-prsim.h"
#include "sim/prsim/State-prsim.h"
#include "parser/instref.h"
#include "util/iterator_more.h"		// for set_inserter
#include "util/copy_if.h"
#include "util/memory/count_ptr.tcc"
#include "util/string.tcc"
#include "util/tokenize.h"
#include "util/IO_utils.tcc"
#include "util/packed_array.tcc"
#include "util/indent.h"
#include "util/numeric/div.h"
#include "util/numeric/random.h"	// for rand48 family
#include "util/stacktrace.h"
#include "common/TODO.h"

namespace util {
using std::istream;
using std::ostream;
using HAC::SIM::PRSIM::channel;

template <>
struct value_writer<channel::array_value_type> {
	ostream&		os;
	explicit
	value_writer(ostream& o) : os(o) { }

	void
	operator () (const channel::array_value_type& p) {
		write_value(os, p.first);
		write_value(os, p.second);
	}
};

template <>
struct value_reader<channel::array_value_type> {
	istream&		is;
	explicit
	value_reader(istream& i) : is(i) { }

	void
	operator () (channel::array_value_type& p) {
		read_value(is, p.first);
		read_value(is, p.second);
	}
};

}	// end namespace util

#define	DATA_VALUE(x)			x.first

namespace HAC {
namespace SIM {
namespace PRSIM {
using parser::parse_node_to_index;
using std::set;
using std::ios_base;
using std::ifstream;
using std::ofstream;
using std::ostringstream;
using std::for_each;
using std::pair;
using std::make_pair;
using std::mem_fun_ref;
using std::ostream_iterator;
using std::back_inserter;
USING_COPY_IF
using util::set_inserter;
#include "util/using_ostream.h"
using util::read_value;
using util::write_value;
using util::indent;
using util::auto_indent;
using util::strings::string_to_num;
using util::numeric::div;
using util::numeric::div_type;
using util::numeric::rand48;

//=============================================================================
#if PRSIM_CHANNEL_TIMING
env_event_type::env_event_type(const node_index_type ni, 
		const value_enum v, const channel& c) :
		node_index(ni), value(v) {
	switch (c.timing_mode) {
	case CHANNEL_TIMING_GLOBAL:
		use_global = true; break;
	case CHANNEL_TIMING_AFTER:
		use_global = false;
		delay = c.after; break;
	case CHANNEL_TIMING_RANDOM:
		use_global = false;
		if (c.after < 0) {	// negative signals unbounded
		delay = State::exponential_random_delay(); break;
		} else {
		delay = State::uniform_random_delay() *c.after; break;
		}
		break;
	default: DIE;
	}
}
#endif

//=============================================================================
// class channel_exception method definitions

error_policy_enum
channel_exception::inspect(const State& s, ostream& o) const {
	o << "ERROR: value assertion failed on channel `" <<
		name << "\'." << endl;
	o << "\texpected: " << expect << ", got: " << got << endl;
	return s.get_channel_expect_fail_policy();
}

//=============================================================================
// class channel_file_handle method definitions

channel_file_handle::channel_file_handle() :
		fname(), 
		stream(NULL) {
}

channel_file_handle::~channel_file_handle() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Open a file stream in write (non-append) mode.  
 */
bool
channel_file_handle::open_write(const string& fn) {
	stream = count_ptr<ofstream>(new ofstream(fn.c_str()));
	if (!stream || !*stream) {
		cerr << "Error opening file \"" << fn << "\" for writing."
			<< endl;
		return true;
	}
	fname = fn;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_file_handle::close(void) {
	fname.clear();
	if (stream) {
		stream = count_ptr<ofstream>(NULL);
		// automatically close and deallocates
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_file_handle::save_checkpoint(ostream& o) const {
	const bool v = (stream && *stream);
	write_value(o, v);
	if (v) {
		write_value(o, fname);
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-opening an output stream should APPEND!
 */
bool
channel_file_handle::load_checkpoint(istream& i) {
	bool v;
	read_value(i, v);
	if (v) {
		read_value(i, fname);
		stream = count_ptr<ofstream>(
			new std::ofstream(fname.c_str(), ios_base::app));
		if (!stream || !*stream) {
			cerr << "Error re-opening file `" <<
				fname << "\' for writing (append)." << endl;
			return true;
		}
	}
	return false;
}

//=============================================================================
// class channel method definitions

channel::channel() :
		name(), 
		ack_signal(INVALID_NODE_INDEX), 
		valid_signal(INVALID_NODE_INDEX), 
#if PRSIM_CHANNEL_TIMING
		timing_mode(CHANNEL_TIMING_DEFAULT),
		after(State::rule_type::default_unspecified_delay),
#endif
		flags(CHANNEL_DEFAULT_FLAGS), 
		counter_state(0), 	// invalid
		x_counter(0),		// invalid
		data(), 
		inject_expect_file(), 
		dumplog(), 
		values(), 
		value_index(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel::~channel() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note all subnodes of this channel, data, ack, valid.
 */
void
channel::get_all_nodes(vector<node_index_type>& ret) const {
if (ack_signal) {
	ret.push_back(ack_signal);
}
if (valid_signal) {
	ret.push_back(valid_signal);
}
	copy(data.begin(), data.end(), back_inserter(ret));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Search for a match.  
	\return true if there is an alias conflict with any data rails, 
	in which case, channel cannot function properly.  
 */
bool
channel::alias_data_rails(const node_index_type ni) const {
	return std::find(data.begin(), data.end(), ni) != data.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
operator << (ostream& o, const channel::array_value_type& p) {
	if (p.second) {
		return o << 'X';
	} else {
		return o << p.first;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't bother printing out node indices.
 */
ostream&
channel::dump(ostream& o) const {
	o << name << " :";
	if (ack_signal) {
	o << (get_ack_active() ? " .a" : " .e");
	o << "(init:" << (get_ack_init() ? '1' : '0') << ')';
	}
	if (valid_signal) {
		o << ' ' << (get_valid_sense() ? ".v" : ".n");
	}
	// didn't store names of bundles and rails
	o << ' ' << bundles() << 'x';
#if PRSIM_CHANNEL_RAILS_INVERTED
	if (get_data_sense()) { o << '~'; }
#endif
	o << "1of" << radix();
	// print internal node IDs? names?
	bool something = false;
	o << ' ';
	if (is_sourcing()) {
		o << "source";
		if (is_random()) {
			o << "-random";
		}
		something = true;
	}
	if (is_sinking()) {
		if (something) o << ',';
		o << "sink";
		something = true;
	}
	if (is_expecting()) {
		if (something) o << ',';
		o << "expect";
		something = true;
	}
	if (watched()) {
		if (something) o << ',';
		o << "watch";
		something = true;
	}
	if (!something) {
		o << "off";
	}
	if (stopped()) {
		o << ",stopped";
	}
	if (ignored()) {
		o << ",ignored";
	}
#if PRSIM_CHANNEL_TIMING
if (is_sinking() || is_sourcing()) {
	dump_timing(o << ", ");
}	// otherwise, is irrelevant
#endif
	if (have_value() &&
			((is_sourcing() && !is_random()) || is_expecting())) {
		o << " {";
		copy(values.begin(), values.end(), 
			ostream_iterator<array_value_type>(o, ","));
		o << '}';
		if (is_looping()) o << '*';
		o << " @" << value_index;
		if (inject_expect_file.length()) {
			o << " < " << inject_expect_file;	// source/expect
		}
		// values may come from command arguments
	} else if (is_random()) {
#if 0
		o << " {" << values.front() << '}';
#endif
	}
	if (dumplog.stream) {
		o << " > " << dumplog.fname;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print additional stateful information.  
 */
ostream&
channel::dump_state(ostream& o) const {
	o << "count: " << size_t(counter_state);
	o << ", unknowns: " << size_t(x_counter);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true on error.
 */
static
bool
read_values_from_list(const string_list& s,
		vector<channel::array_value_type>& v) {
	STACKTRACE_VERBOSE;
	v.reserve(s.size());
	string_list::const_iterator j(s.begin()), e(s.end());
	for ( ; j!=e; ++j) {
		const string& tok(*j);
		channel::array_value_type p;
		channel::value_type& i(p.first);
		p.first = 0;
		p.second = false;
		if (tok == "X" || tok == "x") {
			p.second = true;
		} else
		if (string_to_num(tok, i)) {
			cerr << "Error: invalid value \"" << tok <<
				"\"." << endl;
			return true;
		} else
		if (i > std::numeric_limits<channel::value_type>::max() >> 1) {
			cerr << "Warning: value " << i << " is greater than "
				"max(unsigned value_type)/2, which may screw "
				"up ldiv() when translating to rails." << endl;
		}
		v.push_back(p);
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads values from a file line-by-line.
	TODO: make this function re-usable to others
	TODO: lookup file in search paths (from interpreter?)
	TODO: actual lexing?
	TODO: accept values in various formats.
 */
static
bool
read_values_from_file(const string& fn, vector<channel::array_value_type>& v) {
	STACKTRACE_VERBOSE;
	v.clear();
	ifstream f(fn.c_str());
	if (!f) {
		cerr << "Error opening file \"" << fn << "\" for reading."
			<< endl;
		return true;
	}
	// honor '#' comments
	string_list s;
while (1) {
	string line;
	getline(f, line);
	if (!f) break;
	if ((line.length() > 0) && (line[0] != '#')) {
		util::string_list toks;
		util::tokenize(line, toks);
		s.push_back(toks.front());
	}
}
	return read_values_from_list(s, v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common actions for configuring source operation.  
	\return true if there are errors
 */
bool
channel::set_source_file(const State& s, const string& file_name, 
		const bool loop) {
	STACKTRACE_VERBOSE;
	if (__configure_source(s, loop))	return true;
	value_index = 0;	// TODO: optional offset or initial position
	if (read_values_from_file(file_name, values)) return true;
	if (!values.size()) {
		cerr << "Warning: no values found in file \"" << file_name
			<< "\", channel will remain neutral."
			<< endl;
		// return false;	// downgraded from error
	}
	// no need to set inject/expect file handle's underlying stream
	inject_expect_file = file_name;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common actions for configuring source operation.  
	\pre v list of string-values must be non-empty.
	\return true if there are errors
 */
bool
channel::set_source_args(const State& s, const string_list& v, 
		const bool loop) {
	STACKTRACE_VERBOSE;
	if (__configure_source(s, loop))	return true;
	value_index = 0;	// TODO: optional offset or initial position
	read_values_from_list(v, values);
	if (!values.size()) {
		cerr << "Warning: no values given, channel will remain neutral."
			<< endl;
		return false;	// downgraded from error
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Configure source to just emit random values.  
 */
bool
channel::set_rsource(const State& s) {
	STACKTRACE_VERBOSE;
	if (__configure_source(s, false))	return true;
	flags |= CHANNEL_RANDOM;
	value_index = 0;
	values.resize(1);	
	// used as value holder to remember the last value
	inject_expect_file.clear();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Drive a channel from a file, sourcing values, no loop.  
	\return true on error
	TODO: check for values that overflow (bundle,rail)?
 */
bool
channel::__configure_source(const State& s, const bool loop) {
	STACKTRACE_VERBOSE;
	if (is_sourcing()) {
		cout <<
"Warning: reconfiguring channel from old source to new source."
		<< endl;
	} else if (is_expecting()) {
		cout <<
"Warning: no longer asserting expected channel values; connecting to source."
		<< endl;
	}
	// is OK to log
	// warn if data/validity is already driven with fanin
	// warn if ack has no fanin
	flags &= ~(CHANNEL_EXPECTING | CHANNEL_VALUE_LOOP | CHANNEL_RANDOM);
	flags |= CHANNEL_SOURCING;
	if (loop)
		flags |= CHANNEL_VALUE_LOOP;

	// safety checks on signal directions
if (ack_signal) {
	if (!s.get_node(ack_signal).has_fanin()) {
		cerr << "Warning: channel acknowledge `" << name <<
			(get_ack_active() ? ".a" : ".e") <<
			"\' has no fanin!" << endl;
	}
} else {
	cerr << "Error: acknowledgeless channels cannot be sourced!" << endl;
	return true;
}
	bool maybe_externally_driven = false;
if (valid_signal) {
	const State::node_type& vn(s.get_node(valid_signal));
	if (vn.has_fanin()) {
		cerr << "Error: channel validity `" << name <<
			(get_valid_sense() ? ".v" : ".n") <<
			"\' has fanin.\n";
		cerr <<
"If the validity is built from a completion tree, use an `e\' or `a\' channel."
			<< endl;
		maybe_externally_driven = true;
		// but make sure this is not doubly driven by another source
	}
	if (!vn.has_fanout()) {
		// this warning might be excessive
		cerr << "Warning: channel validity `" << name <<
			(get_valid_sense() ? ".v" : ".n") <<
			"\' has no fanout, but is being sourced." << endl;
	}
}
	const data_bundle_array_type::const_iterator
		b(data.begin()), e(data.end());
	data_bundle_array_type::const_iterator i(b);
	for ( ; i!=e; ++i) {
		const State::node_type& d(s.get_node(*i));
		if (d.has_fanin()) {
			const size_t pos = i -b;	// std::distance
			// TODO: use lldiv
			const size_t j = pos / radix();
			const size_t k = pos % radix();
			cerr << "Warning: channel data rail `" << name <<
				"\' (" << j << ", " << k << ") has fanin.\n";
			maybe_externally_driven = true;
		}
	}
	if (maybe_externally_driven) {
		cerr << "Channel source may not operate properly, "
			"if driven from elsewhere." << endl;
	}
	return false;
}	// end channel::set_source_file

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sink a channel.  
	Sinking is orthogonal to logging/expecting mode.  
	\return true on error
 */
bool
channel::set_sink(const State& s) {
	STACKTRACE_VERBOSE;
	// sinking should not conflict with any other mode
	// is OK to log
	// warn if data/validity is already driven with fanin
	// warn if ack has no fanin
	flags |= CHANNEL_SINKING;

	// additional signal checks
	// warn if channel happens to be connected in wrong direction
if (ack_signal) {
	if (s.get_node(ack_signal).has_fanin()) {
		cerr << "Warning: channel acknowledge `" << name <<
			(get_ack_active() ? ".a" : ".e") <<
			"\' already has fanin!\n"
			"Channel sink may not operate properly, "
			"if driven from elsewhere." << endl;
	}
} else {
	cerr << "Error: acknowledgeless channel cannot consume tokens!" << endl;
	return true;
}
if (valid_signal) {
	const State::node_type& vn(s.get_node(valid_signal));
	if (!vn.has_fanin()) {
		cerr << "Warning: channel validity `" << name <<
			(get_valid_sense() ? ".v" : ".n") <<
			"\' has no fanin." << endl;
#if 0
		cerr << "I will automatically drive this signal for you "
			"according to the state of the data rails... "
			"because I am so kind." << endl;
		// but make sure this is not doubly driven by another source
#endif
	}
#if 0
	if (vn.has_fanout()) {
		// this warning might be excessive
		cerr << "Warning: channel validity `" << channel_name <<
			(c.get_valid_sense() ? ".v" : ".n") <<
			"\' has fanout." << endl;
	}
#endif
}
	// TODO: do we care if data rails lack fanin?
	return false;
}	// end channel::set_sink

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel::__configure_expect(const bool loop) {
	if (is_sourcing()) {
		cout <<
"Warning: no longer sourcing channel values; configuring to expect."
		<< endl;
	}
	// is OK to log and expect
	flags &= ~(CHANNEL_SOURCING | CHANNEL_VALUE_LOOP);
	flags |= CHANNEL_EXPECTING;
	if (loop) {
		flags |= CHANNEL_VALUE_LOOP;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expecting values conflicts with sourcing values.  
	TODO: check for values that overflow (bundle,rail)?
 */
bool
channel::set_expect_file(const string& fn, const bool loop) {
	STACKTRACE_VERBOSE;
	__configure_expect(loop);
	value_index = 0;
	values.clear();
	if (read_values_from_file(fn, values)) return true;
	if (values.size()) {
		inject_expect_file = fn;	// save name
	} else {
		cerr <<
	"Warning: no values found in expect file, ignoring expect."
			<< endl;
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expecting values conflicts with sourcing values.  
	TODO: check for values that overflow (bundle,rail)?
 */
bool
channel::set_expect_args(const string_list& v, const bool loop) {
	STACKTRACE_VERBOSE;
	__configure_expect(loop);
	value_index = 0;
	values.clear();
	read_values_from_list(v, values);
	if (!values.size()) {
		cerr <<
	"Warning: no values found in expect file, ignoring expect."
			<< endl;
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel::close_stream(void) {
	dumplog.close();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Always open in non-append mode?
 */
bool
channel::set_log(const string& fn) {
	if (dumplog.open_write(fn)) return true;	// already have msg
	(*dumplog.stream) << "# log for channel: " << name << endl;
	// delay flushing for performance?
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Immediately upon registering a channel, initialize the state 
	of the data counter according the current values of all
	data rail nodes.  
	One LOGIC_HIGH counts, X's are treated as 0s.  
	invariant: if exclusiveness is violated, this will complain!
	invariant: no data rails alias
 */
void
channel::initialize_data_counter(const State& s) {
	STACKTRACE_VERBOSE;
	typedef	State::node_type		node_type;
	counter_state = 0;
	x_counter = 0;
	data_bundle_array_type::const_iterator i(data.begin()), e(data.end());
	for ( ; i!=e; ++i) {
		const node_type& n(s.get_node(*i));
		switch (n.current_value()) {
		case LOGIC_HIGH:
			if (!get_data_sense()) ++counter_state;
			break;
		case LOGIC_LOW:
			if (get_data_sense()) ++counter_state;
			break;
		case LOGIC_OTHER:
			++x_counter;
			break;
		default: break;
		}
	}
	if (counter_state > bundles()) {
		cerr << "Channel data rails are in an invalid state!" << endl;
		cerr << "In channel `" << name << "\', got " << counter_state
			<< " active rails, whereas only " << bundles() <<
			" are permitted." << endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre all nodes are set to X, so counters should be thus initialized.
	Close output logging stream.  
	Restarts all values sequences to first index, which may not necessarily
		do what user intends if channels were sourced mid-simulation!
 */
void
channel::initialize(void) {
	counter_state = 0;
	// independent of data-rail sense
	x_counter = bundles() * radix();
	// retain values in sequence, but reset index
	value_index = 0;
	// retain inject_expect_file
	// retain data-rails, ack, validity
	close_stream();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Is this even necessary (if channels are going to be destroyed)?
	\pre simulator's nodes have wiped any references to channels.  
	Might as well just destroy in-place and re-initialize?
 */
void
channel::clobber(void) {
	initialize();
	name.clear();
	values.clear();
	inject_expect_file.clear();
	flags = CHANNEL_DEFAULT_FLAGS;
	data.~data_bundle_array_type();		// placement dtor
	new (&data) data_bundle_array_type;	// placement ctor
	__node_to_rail.clear();
	ack_signal = INVALID_NODE_INDEX;
	valid_signal = INVALID_NODE_INDEX;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_TIMING
/**
	\return true if there is a syntax error.  
	TODO: use a map to parsers.  
 */
bool
channel::set_timing(const string& m, const string_list& a) {
	static const string __random("random");
	static const string __global("global");
	static const string __after("after");
	if (m == __random) {
		timing_mode = CHANNEL_TIMING_RANDOM;
		if (a.size()) {
			// bounded, uniform
			return string_to_num(a.front(), after);
			// ignore trailing values
		} else {
			after = -1;
			// unbounded, exponential var.
		}
	} else if (m == __global) {
		timing_mode = CHANNEL_TIMING_GLOBAL;
		// no need to set after
	} else if (m == __after) {
		timing_mode = CHANNEL_TIMING_AFTER;
		if (a.size()) {
			return string_to_num(a.front(), after);
		}
	} else {
		cerr << "Error: invalid mode: " << m << endl;
		return true;
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel::dump_timing(ostream& o) const {
	o << "timing: ";
	switch (timing_mode) {
	case CHANNEL_TIMING_GLOBAL: o << "global"; break;
	case CHANNEL_TIMING_AFTER: o << "after=" << after; break;
	case CHANNEL_TIMING_RANDOM: o << "random"; 
		if (after < 0.0) o << "(exp.)";
		else	o << "(<" << after << ')';
		break;
	default: o << "unknown"; DIE;
	}
	return o;
	// NOTE: no endl
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel::help_timing(ostream& o) {
o << "available channel timing modes:" << endl;
o << "\tglobal : use the global policy set by \'timing\'" << endl;
o << "\trandom [max] : if max given, uniform bounded, else exponential variate" << endl;
o << "\tafter [del] : if del given, set fixed delay, else use prev. value" << endl;
	return o;
}
#endif	// PRSIM_CHANNEL_TIMING

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Binding constructor functor.
 */
#if PRSIM_CHANNEL_TIMING
#define	__node_setter(x)	__node_setter__(x, *this)
struct __node_setter__ :
#else
struct __node_setter :
#endif
	public std::unary_function<const node_index_type, env_event_type> {
	value_enum				val;
#if PRSIM_CHANNEL_TIMING
	const channel&				chan;
#endif

#if PRSIM_CHANNEL_TIMING
	__node_setter__(const value_enum v, const channel& c) : 
		val(v), chan(c) { }
#else
	explicit
	__node_setter(const value_enum v) : val(v) { }
#endif

	result_type
	operator () (argument_type ni) const {
#if PRSIM_CHANNEL_TIMING
		// set immediately, like a set command?
		// or use delay mode?
		return result_type(ni, val, chan);
#else
		return result_type(ni, val);
#endif
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_TIMING
#define	ENV_EVENT(x, y)	env_event_type(x, y, *this)
#else
#define	ENV_EVENT(x, y)	env_event_type(x, y)
#endif
/**
	Sources will lower all data rails, 
	Sources with validity signals will react to the data-rails being set.
	sinks will set acknowledges according to the reset value.
 */
void
channel::reset(vector<env_event_type>& events) {
	STACKTRACE_VERBOSE;
	typedef	State::node_type		node_type;
	if (is_sourcing()) {
		reset_all_data_rails(events);
		// once nodes all become neutral, the validity should be reset
	}
	if (is_sinking()) {
		INVARIANT(ack_signal);	// ack-less cannot configure source/sink
		events.push_back(ENV_EVENT(ack_signal, 
			(get_ack_init() ? LOGIC_HIGH
				: LOGIC_LOW)));
		// use global timing policy
	}
	// else nothing else to do
	stop();	// freeze this channel until it is resumed
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Point to next value in sequence, if applicable.
	For finite value sequences, when run-out of values, 
	clear the array and any string associated with its values (file).  
 */
void
channel::advance_value(void) {
if (is_random()) {
	INVARIANT(!value_index);
	INVARIANT(values.size() == 1);
	DATA_VALUE(values.front()) = rand48<value_type>()();
	// long (lrand48): [0..2^31 -1]
} else {
	++value_index;		// overflow?
	if (value_index >= values.size()) {
		if (is_looping()) {
			value_index = 0;
		} else {
			values.clear();
			value_index = 0;
			INVARIANT(!have_value());
			inject_expect_file.clear();
			// value_index = values.size();	// one past end
		}
	}
	// else if values.size(), values.clear() ?
	if (have_value() && values[value_index].second) {
		// if values is X (don't care), then choose a random value
		values[value_index].first = rand48<value_type>()();
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return in r the list of data rails selected by current value.
	If there is no current value, then return nothing.  
	Active sense of data rails is irrelevant here. 
 */
void
channel::current_data_rails(vector<node_index_type>& r) const {
	INVARIANT(r.empty());
if (have_value()) {
	const int_value_type rdx = radix();
	div_type<int_value_type>::return_type qr;
	qr.quot = DATA_VALUE(current_value());
	qr.rem = 0;	// unused
	data_rail_index_type k;
	k[0] = 0;
	while (r.size() < bundles()) {
		qr = div(qr.quot, rdx);
		INVARIANT(qr.rem >= 0);	// because division is signed
		k[1] = qr.rem;
		r.push_back(data[k]);
		++k[0];
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
void
channel::reset_all_data_rails(vector<env_event_type>& events) {
	transform(data.begin(), data.end(), back_inserter(events),
		__node_setter(get_data_sense() ?  LOGIC_HIGH : LOGIC_LOW));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
void
channel::X_all_data_rails(vector<env_event_type>& events) {
	transform(data.begin(), data.end(), back_inserter(events),
		__node_setter(LOGIC_OTHER));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Similar to current_data_rails, but emits events for setting
	all data rails high or low, according to the current value.  
	If there is no current value, reset all data rails.  
	This is useful for coming out of an uninitialized state (resume).
	When run out of data values, clear the sourcing flag, 
	and clear the value array.  
 */
void
channel::set_all_data_rails(vector<env_event_type>& r) {
	STACKTRACE_VERBOSE;
	typedef	State::node_type		node_type;
if (have_value()) {
	const int_value_type rdx = radix();
	// NOTE: div is *signed*
	div_type<int_value_type>::return_type qr;
	qr.quot = DATA_VALUE(current_value());
	qr.rem = 0;	// unused
	data_rail_index_type k;
	k[0] = 0;
	for ( ; k[0] < bundles(); ++k[0]) {
		qr = div(qr.quot, rdx);
		k[1] = 0;
		for ( ; k[1] < size_t(rdx); ++k[1]) {
			const node_index_type ni = data[k];	// node index
			const value_enum v = 
				(k[1] == size_t(qr.rem)) ^ get_data_sense() ?
				LOGIC_HIGH : LOGIC_LOW;
			r.push_back(ENV_EVENT(ni, v));
		}
	}
} else {
	INVARIANT(!is_random());
	// otherwise following code would wipe the random value slot!
	// no next value, just hold all data rails neutral
	reset_all_data_rails(r);
	flags &= ~CHANNEL_SOURCING;
	if (!values.empty()) {
		values.clear();
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// don't bother using this, error prone, not worth added efficiency
/**
	When sourcing, set data values.
	This does NOT check the stopped state, caller is responsible.
 */
void
channel::set_current_data_rails(vector<env_event_type>& events, 
		const value_enum val) {
	STACKTRACE_VERBOSE;
	INVARIANT(is_sourcing());
if (have_value()) {
	vector<node_index_type> nodes;
	current_data_rails(nodes);	// use current values[value_index]
	// TODO: mind the data-active sense!
	transform(nodes.begin(), nodes.end(), back_inserter(events), 
		__node_setter(val));
} else {
	// out of values, might as well turn of sourcing
	flags &= ~CHANNEL_SOURCING;
	if (!values.empty()) {
		values.clear();
	}
}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: Almost correct: need exactly one rail high per bundle, 
	but that's more effort to check. 
	This is independent of data-sense.  
 */
bool
channel::data_is_valid(void) const {
	return !x_counter && (counter_state == bundles());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre data rails must be in a valid (active) state, i.e.
		exactly one rail per bundle high, no unknowns.
		If this ever fails, we didn't do enough tracking/counting.
	\return the numerical value as represented by data rails.
 */
channel::value_type
channel::data_rails_value(const State& s) const {
	STACKTRACE_VERBOSE;
	typedef	State::node_type	node_type;
	value_type ret = 0;
	data_rail_index_type k;
	k[0] = bundles();
	const size_t rdx = radix();	// sign mismatch?
	while (k[0]) {
		--k[0];
		k[1] = 0;
		ret *= rdx;
		bool have_hi = false;
		size_t hi = 0;
		// check all rails for paranoia
		for ( ; k[1] < rdx; ++k[1]) {
			const node_type& n(s.get_node(data[k]));
			switch (n.current_value()) {
			case LOGIC_LOW:
			if (get_data_sense()) {
				INVARIANT(!have_hi);
				have_hi = true;
				hi = k[1];
			}
				break;
			case LOGIC_HIGH:
			if (!get_data_sense()) {
				INVARIANT(!have_hi);
				have_hi = true;
				hi = k[1];
			}
				break;
			default: DIE;
			}
		}
		INVARIANT(have_hi);
		ret += hi;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if this channel may drive the node.
	Node may be driven if channel is sourced and node is a 
	data rail, or channel is sinking and node is an acknowledge.
	Sources may also drive validity signals.  
	Nodes are reported regardless of the stopped state of the channel.
 */
bool
channel::may_drive_node(const node_index_type ni) const {
	if (is_sourcing()) {
		if (valid_signal && (ni == valid_signal)) {
			return true;
		}
		// check: is it data rail of source?
		const data_rail_map_type::const_iterator
			f(__node_to_rail.find(ni));
		if (f != __node_to_rail.end()) {
			return true;
		}
	}
	if (is_sinking()) {
		if (ack_signal && (ack_signal == ni)) {
			return true;
		}
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Is node in channel's sensitivity list?
	\return true if this channel may respond to changes in node.
 */
bool
channel::reads_node(const node_index_type ni) const {
	const data_rail_map_type::const_iterator
		f(__node_to_rail.find(ni));
	const bool is_data_rail = (f != __node_to_rail.end());
	if (is_sourcing()) {
		if (ack_signal && (ni == ack_signal)) {
			return true;
		}
		if (valid_signal && is_data_rail) {
			// source's validity must respond to its own data rails
			return true;
		}
	}
	if (is_sinking()) {
		if (valid_signal) {
			// ack only responds to validity, not data rails
			if (ni == valid_signal)
				return true;
		} else {
			// yes, ack responds to data rails
			if (is_data_rail)
				return true;
		}
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Return all nodes that may effectively drive node ni
	that are in an unkonwn (X) state.  
 */
void
channel::__get_fanins(const node_index_type ni, 
		std::set<node_index_type>& ret) const {
	if (is_sourcing()) {
		if (valid_signal && (ni == valid_signal)) {
			// source-validity responds to data rails
			copy(data.begin(), data.end(), set_inserter(ret));
		}
		// if node is data rail, then effective input is ack
		if (ack_signal) {
		const data_rail_map_type::const_iterator
			f(__node_to_rail.find(ni));
		if (f != __node_to_rail.end()) {
			ret.insert(ack_signal);
		}
		}
	}
	if (is_sinking()) {
	if (ack_signal && (ack_signal == ni)) {
		// validity is responsibility of the circuit, not the sink.
		// the acknowledge only follows the validity signal directly
		if (valid_signal)
			ret.insert(valid_signal);
		else	copy(data.begin(), data.end(), set_inserter(ret));
	}
	}
}	// end channel::get_fanins

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Report why node in question is not driven in direction dir. 
	Is mutually recursive with State::__node_why_not().
	If channel is stopped, do not report.  
	TODO: test eMx1ofN channels
 */
ostream&
channel::__node_why_not(const State& s, ostream& o, const node_index_type ni, 
		const size_t limit, const bool dir, 
		const bool why_not, const bool verbose, 
		node_set_type& u, node_set_type& v) const {
	typedef	State::node_type		node_type;
	const indent __ind_outer(o, verbose ? " " : "");
if (stopped()) {
	// && (is_sourcing() || is_sinking())
	// TODO: this should really only be printed in cases below
	// where it is actually acting as a source or sink.
	// watched and logged channels won't care...
	o << auto_indent << "(channel " << name << " is stopped.)" << endl;
	// FIXME: later
} else {
	// const node_type& n(s.get_node(ni));
	if (is_sourcing()) {
		// only data or validity can be driven by source
		if (valid_signal && (ni == valid_signal)) {
			__node_why_not_data_rails(s, o, 
				valid_signal, get_valid_sense(), 
				limit, dir, why_not, verbose, u, v);
		} else if (ack_signal) {
		const data_rail_map_type::const_iterator
			f(__node_to_rail.find(ni));
		if (f != __node_to_rail.end()) {
			// then is attributed to acknowledge
			// interpret dir as data-active, 
			// which equivalently asks why acknowledge is not
			// the opposite (active).  
			// const indent __ind_nd(o, verbose ? "." : "  ");
			// INDENT_SCOPE(o);
			s.__node_why_not(o, ack_signal, limit, 
				dir ^ get_ack_active() ^ !why_not,
				why_not, verbose, u, v);
		}
		}
	}
	if (is_sinking() && ack_signal && (ni == ack_signal)) {
		// no other signal should be driven by sink
	if (valid_signal) {
		// TODO: not sure if the following is correct
		// it may be backwards, if I just think about it...
		// TODO: account for why_not parameter
		if (get_ack_active() ^ dir ^ get_valid_sense()) {
			s.__node_why_not(o, valid_signal, limit, 
				true, why_not, verbose, u, v);
		} else {
			s.__node_why_not(o, valid_signal, limit, 
				false, why_not, verbose, u, v);
		}
	} else {
		__node_why_not_data_rails(s, o, ack_signal, get_ack_active(), 
			limit, dir, why_not, verbose, u, v);
	}
	}	// end if sinking
}	// end if !stopped
	return o;
}	// end channel::__node_why_not

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Asks why the data rails are/not in their present state.
	This analysis will depend on the sense of the data signals.  
	\param ni is the node in question, e.g. validity signal, 
		or acknowledge signal.  
	\param is the active sense of the ni signal.  
 */
ostream&
channel::__node_why_not_data_rails(const State& s, ostream& o, 
		const node_index_type ni, const bool active, 
		const size_t limit, const bool dir, 
		const bool why_not, const bool verbose, 
		node_set_type& u, node_set_type& v) const {
	// query completion status of the data rails, 
	// assuming celem-of-or style completion of bundles
	typedef	State::node_type		node_type;
	const size_t _bundles = bundles();
	const size_t _radix = radix();
	const node_type& a(s.get_node(ni));
	const value_enum av = a.current_value();
	string ind_str;
	if (verbose && (_bundles > 1)) {
		ind_str += " & ";
		o << auto_indent << "-+" << endl;
	}
	const indent __ind_celem(o, ind_str);      // INDENT_SCOPE(o);
	data_rail_index_type key;
	key[0] = 0;
	for ( ; key[0] < _bundles; ++key[0]) {
		// first find out if bundle is valid
		size_t partial_valid = 0;
		key[1] = 0;
		for ( ; key[1] < _radix; ++key[1]) {
			if (s.get_node(data[key]).current_value() ==
				get_data_sense() ? LOGIC_LOW : LOGIC_HIGH) {
				++partial_valid;
			}
		}
		// second pass, only if bundle is not ready
		string i_s;
		if (verbose && (_radix > 1)) {
			i_s += " ";
			// when to negate (nor)?
			// when data wants to be neutral, 
			// i.e. when acknowledge is active
			if ((av == LOGIC_LOW) ^ !why_not ^ active
					^ get_data_sense()) {
				i_s += "~";	// nor
			}
			i_s += "| ";
			o << auto_indent << "-+" << endl;
		}
		const indent __ind_or(o, i_s);	// INDENT_SCOPE(o);

		key[1] = 0;
		for ( ; key[1] < _radix; ++key[1]) {
			const node_index_type di = data[key];
			const node_type& d(s.get_node(di));
		switch (d.current_value()) {
		// TODO: simplify me (logic)
		case LOGIC_LOW:
		if (get_data_sense()) {		// active-low
			if (active ^ (av == LOGIC_LOW)) {
				s.__node_why_not(o, di, 
					limit, why_not, 
					why_not, verbose, u, v);
			}
		} else {
			if (active ^ (av == LOGIC_HIGH)) {
				s.__node_why_not(o, di, 
					limit, why_not, 
					why_not, verbose, u, v);
			}
		}
			break;
		case LOGIC_HIGH:
		if (get_data_sense()) {		// active-low
			if (active ^ (av == LOGIC_HIGH)) {
				s.__node_why_not(o, di, 
					limit, !why_not, 
					why_not, verbose, u, v);
			}
		} else {
			if (active ^ (av == LOGIC_LOW)) {
				s.__node_why_not(o, di, 
					limit, !why_not, 
					why_not, verbose, u, v);
			}
		}
			break;
		default: break;	// ignore Xs
		}	// end switch
		}	// end for rails
	}	// end for bundles
	return o;
}	// end __node_why_not_data_rails

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Report why node in question is not driven in direction dir. 
	Is mutually recursive with State::__node_why_not().
	If channel is stopped, do not report.  
	TODO: test eMx1ofN channels
 */
ostream&
channel::__node_why_X(const State& s, ostream& o, const node_index_type ni, 
		const size_t limit, const bool verbose, 
		node_set_type& u, node_set_type& v) const {
	typedef	State::node_type		node_type;
	const indent __ind_outer(o, verbose ? " " : "");
if (stopped()) {
	// TODO: this should really only be printed in cases below
	// where it is actually acting as a source or sink.
	// watched and logged channels won't care...
	o << auto_indent << "(channel " << name << " is stopped.)" << endl;
	// FIXME: later
} else {
	// const node_type& n(s.get_node(ni));
	if (is_sourcing()) {
		// only data or validity can be driven by source
		if (valid_signal && (ni == valid_signal)) {
			// then point back to data rails, see below
			// eventually refactor that code out
			__node_why_X_data_rails(s, o, get_valid_sense(), 
				limit, verbose, u, v);
		} else if (ack_signal) {
		const data_rail_map_type::const_iterator
			f(__node_to_rail.find(ni));
		if (f != __node_to_rail.end()) {
			// then is attributed to acknowledge
			// interpret dir as data-active, 
			// which equivalently asks why acknowledge is not
			// the opposite (active).  
			const indent __ind_nd(o, verbose ? "." : "  ");
			// INDENT_SCOPE(o);
			s.__node_why_X(o, ack_signal, limit, verbose, u, v);
		}
		}
	}
	if (is_sinking() && ack_signal && (ni == ack_signal)) {
		// no other signal should be driven by sink
	if (valid_signal) {
		s.__node_why_X(o, valid_signal, limit, verbose, u, v);
	} else {	// depends on data rails directly
		__node_why_X_data_rails(s, o, get_ack_active(), 
			limit, verbose, u, v);
	}
	}	// end if sinking
}	// end if !stopped
	return o;
}	// end channel::__node_why_X

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic query on why data rails are causing X.  
 */
ostream&
channel::__node_why_X_data_rails(const State& s, ostream& o, 
		const bool active, 
		const size_t limit, const bool verbose, 
		node_set_type& u, node_set_type& v) const {
	typedef	State::node_type		node_type;
	const size_t _bundles = bundles();
	const size_t _radix = radix();
	string ind_str(verbose ? "" : "  ");
	if (verbose && (_bundles > 1)) {
		ind_str += " & ";
		o << auto_indent << "-+" << endl;
	}
	const indent __ind_celem(o, ind_str);      // INDENT_SCOPE(o);
	data_rail_index_type key;
	key[0] = 0;
	for ( ; key[0] < _bundles; ++key[0]) {
		// first find out if bundle is X
		key[1] = 0;
		string i_s;
		if (verbose && (_radix > 1)) {
			i_s += " ";
			// when to negate (nor)?
			// when data wants to be neutral, 
			// i.e. when acknowledge is active
			if (!active ^ get_data_sense()) {
				i_s += "~";
			}
			i_s += "| ";
			o << auto_indent << "-+" << endl;
		}
		const indent __ind_or(o, i_s);	// INDENT_SCOPE(o);
		for ( ; key[1] < _radix; ++key[1]) {
			const node_index_type d(data[key]);
			if (s.get_node(d).current_value() == LOGIC_OTHER) {
				s.__node_why_X(o, d, limit, verbose, u, v);
			}
		}	// end for rails
	}	// end for bundles
	return o;
}	// end __node_why_X_data_rails

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Event on node ni may trigger environment events, logging, 
	checking, etc... process them here.
	This assumes that in any given channel, the acknowledge
	is not aliased to any of the data rails.  
	Only sets node if not in stopped state.  
	The events returned in new_events may be vacuous, and should
		filtered out by the caller.
	\param new_events is where new events from the environment
		should be staged.  
	\throw exception if there is an expected value mismatch.
 */
void
channel::process_node(const State& s, const node_index_type ni, 
		const value_enum prev, const value_enum next, 
		vector<env_event_type>& new_events) throw(channel_exception) {
	STACKTRACE_VERBOSE;
	typedef	State::node_type	node_type;
#if ENABLE_STACKTRACE
	cout << s.get_node_canonical_name(ni) << " : " << size_t(prev) << 
		" -> " << size_t(next) << endl;
#endif
// first identify which channel node member this node is
if (ack_signal && (ni == ack_signal)) {
	STACKTRACE_INDENT_PRINT("got ack update" << endl);
	// only need to take action if this is a source
	if (is_sourcing() && !stopped()) {
	STACKTRACE_INDENT_PRINT("source responding..." << endl);
	switch (next) {
		// assumes that data rails are active high
	case LOGIC_LOW:
		if (get_ack_active()) {
			// \pre all data rails are neutral
			// set data rails to next data value
			set_all_data_rails(new_events);
			advance_value();
		} else {
			reset_all_data_rails(new_events);
		}
		break;
	case LOGIC_HIGH:
		if (get_ack_active()) {
			reset_all_data_rails(new_events);
		} else {
			// \pre all data rails are neutral
			// set data rails to next data value
			set_all_data_rails(new_events);
			advance_value();
		}
		break;
	default:
		// set all data to X
		// do not advance
		X_all_data_rails(new_events);
		break;
	}
	}
	// logging and expect mode don't care
} else if (valid_signal && (ni == valid_signal)) {
	STACKTRACE_INDENT_PRINT("got validity update" << endl);
	switch (next) {
	// print, watch, log, check data NOW
	case LOGIC_LOW:
		if (!get_valid_sense()) { process_data(s); } break;
	case LOGIC_HIGH:
		if (get_valid_sense()) { process_data(s); } break;
	default: break;
	}
	// only need to take action if this is a sink
	if (is_sinking() && !stopped()) {
		INVARIANT(ack_signal);
	switch (next) {
	case LOGIC_LOW:
		if (get_valid_sense()) {
			// neutral, reset ack
			new_events.push_back(ENV_EVENT(ack_signal, 
				get_ack_active() ? LOGIC_LOW
					: LOGIC_HIGH));
		} else {
			// valid, ack
			new_events.push_back(ENV_EVENT(ack_signal, 
				get_ack_active() ? LOGIC_HIGH
					: LOGIC_LOW));
		}
		break;
	case LOGIC_HIGH:
		if (get_valid_sense()) {
			// valid, ack
			new_events.push_back(ENV_EVENT(ack_signal, 
				get_ack_active() ? LOGIC_HIGH
					: LOGIC_LOW));
		} else {
			// neutral, reset ack
			new_events.push_back(ENV_EVENT(ack_signal, 
				get_ack_active() ? LOGIC_LOW
					: LOGIC_HIGH));
		}
		break;
	default:
		new_events.push_back(ENV_EVENT(ack_signal, 
			LOGIC_OTHER));
		break;
	}
	}
} else {
	STACKTRACE_INDENT_PRINT("got data-rail update" << endl);
	// invariant: must be data rail
	// update state counters
	switch (prev) {
	case LOGIC_LOW:
	if (get_data_sense()) {
		INVARIANT(counter_state);
		--counter_state;
	}
		break;
	case LOGIC_HIGH:
	if (!get_data_sense()) {
		INVARIANT(counter_state);
		--counter_state;
	}
		break;
	case LOGIC_OTHER:
		INVARIANT(x_counter);
		--x_counter;
		break;
	default: break;
	}
	switch (next) {
	case LOGIC_LOW: if (get_data_sense()) ++counter_state; break;
	case LOGIC_HIGH: if (!get_data_sense()) ++counter_state; break;
	case LOGIC_OTHER: ++x_counter; break;
	default: break;
	}
	if (x_counter) {
		// if there are ANY Xs, then cannot log/expect values
		// sources/sinks should respond accordingly with X signals
		if (is_sourcing()) {
			// for validity protocol, set valid to X
			// validity signal reacts even when channel stopped
			if (valid_signal) {
				new_events.push_back(ENV_EVENT(
					valid_signal, LOGIC_OTHER));
			}
		}
		if (!stopped()) {
		if (is_sinking() && (next == LOGIC_OTHER)
				&& (x_counter == 1)) {
			INVARIANT(ack_signal);
			// if counter was JUST incremented to 1
			// otherwise multiple X's are vacuous
			// if not validity protocol, set ack to X
			if (!valid_signal) {
			new_events.push_back(ENV_EVENT(
				ack_signal, LOGIC_OTHER));
			}
			// otherwise wait for validity to go X
		}
		}	// end if !stopped
	// need to take action for EACH of the following that hold:
	// 1) this is sink AND not a valid-request protocol
	//	(otherwise, depends on valid signal)
	// 2) this is a source on valid-request protocol, 
	//	and thus need to set valid signal automatically
	// 3) this is being logged
	// 4) this is being expected
	} else if (!counter_state) {
		// then data rails are in neutral state
		if (is_sourcing() && valid_signal) {
			// source is responsible for resetting valid signal
			// should react to data rails 
			// EVEN WHEN CHANNEL IS STOPPED
			new_events.push_back(ENV_EVENT(valid_signal, 
				get_valid_sense() ? LOGIC_LOW
					: LOGIC_HIGH));
		}
		if (!stopped()) {
		if (is_sinking() && !valid_signal) {
			INVARIANT(ack_signal);
			// sink should reply with ack reset
			// otherwise, valid_signal is an input
			new_events.push_back(ENV_EVENT(ack_signal, 
				get_ack_active() ? LOGIC_LOW
					: LOGIC_HIGH));
		}
		}
	} else if (counter_state == bundles()) {
		// NOTE: stopped channels will not assert expected data nor log!
		if (is_sourcing() && valid_signal) {
			// source is responsible for setting valid signal
			// validity signal always reacts to data rails
			// even when channel is stopped
			new_events.push_back(ENV_EVENT(valid_signal, 
				get_valid_sense() ? LOGIC_HIGH
					: LOGIC_LOW));
		}
	if (!valid_signal) {
		// then data rails are in valid state
		process_data(s);
	}
	if (!stopped()) {
		// otherwise, data is logged/checked on validity signal
		// if no value available, just ignore
		if (is_sinking() && !valid_signal) {
			INVARIANT(ack_signal);
			// sink should reply with ack reset
			// otherwise, valid_signal is an input
			new_events.push_back(ENV_EVENT(ack_signal, 
				get_ack_active() ? LOGIC_HIGH
					: LOGIC_LOW));
		}
	}
	}
}
}	// end channel::process_node

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	React to data value.
	Print it if it is watched, log it if it is logged.  
	Check it if it is expected.  
	TODO: what to do if data is in bad state (validity protocol only).
 */
void
channel::process_data(const State& s) throw (channel_exception) {
	const bool v = data_is_valid();
	if (watched()) {
		cout << "channel\t" << name << " (.data) : ";
		if (v) {
			cout << data_rails_value(s) << endl;
		} else {	// data is in invalid state
			cout << 'X' << endl;
		}
	}
	if (!ignored() && dumplog.stream && *dumplog.stream) {
		// TODO: format me, hex, dec, bin, etc...
		// should be able to just setbase()
		if (v) {
		(*dumplog.stream) << data_rails_value(s) << endl;
		} else {
		(*dumplog.stream) << 'X' << endl;
		}
		// really flush every line?
	}
	if (is_expecting() && !ignored()) {
	if (have_value()) {
		const array_value_type& expect = current_value();
		if (!expect.second) {
		const error_policy_enum e(s.get_channel_expect_fail_policy());
		if (v) {
		const value_type got = data_rails_value(s);
		advance_value();
		if (DATA_VALUE(expect) != got) {
			const channel_exception
				ex(name, DATA_VALUE(expect), got);
			if (e == ERROR_WARN) {
				ex.inspect(s, cout);
			} else if (e > ERROR_WARN) {
				throw ex;
			}
		} else if (s.confirm_asserts()) {
			cout << "channel " << name << " has value " <<
				DATA_VALUE(expect) << ", as expected." << endl;
		}
		} else {	// cannot expect invalid value
			const channel_exception
				ex(name, DATA_VALUE(expect), 0xDEADBEEF);
			if (e == ERROR_WARN) {
				ex.inspect(s, cout);
			} else if (e > ERROR_WARN) {
				throw ex;
			}
		}
		} else {	// else don't care
			// don't print confirmed values for don't cares
			if (v) {
				// on valid data, advance
				advance_value();
			}
		}
	} else {
		// exhausted values, disable expecting
		flags &= ~CHANNEL_EXPECTING;
		// might as well release memory...
		if (!values.empty()) {
			values.clear();
		}
	}
	}
}	// end method process_data

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We don't know the current state of the channel because it may
	have changed while this was stopped, thus we need to refresh
	with a complete re-evaluation of signals to infer the current state.  
	Data rail activity should have been tracked even while
	channel was stopped.  
	TODO: refactor code in this function...
 */
void
channel::resume(const State& s, vector<env_event_type>& events) {
	STACKTRACE_VERBOSE;
	typedef	State::node_type		node_type;
	flags &= ~CHANNEL_STOPPED;
	static const char ambiguous_data[] = 
"Warning: the current state of data rails is neither valid nor neutral, "
"so I\'m assuming that current sequence value has NOT already been used; "
"we are using the current value.";
if (is_sourcing()) {
	INVARIANT(ack_signal);
	// validity should be set after all data rails are valid/neutral
	const node_type& a(s.get_node(ack_signal));
	switch (a.current_value()) {
	case LOGIC_LOW:
		if (get_ack_active()) {
			// should send new data, if available
			// Q: if counter_state is *between* 0 and #bundles, 
			// should we assume that the current value was
			// already used or not?
			if (counter_state && (counter_state != bundles())) {
				// ambiguous
				cerr << ambiguous_data << endl;
			}
			if ((counter_state != bundles() || x_counter)) {
				set_all_data_rails(events);
				advance_value();
			}
			if (valid_signal) {
				if (x_counter) {
					events.push_back(ENV_EVENT(
						valid_signal, 
						LOGIC_OTHER));
				} else if (!counter_state) {
					events.push_back(ENV_EVENT(
						valid_signal, 
						get_valid_sense() ?
							LOGIC_LOW :
							LOGIC_HIGH));
				} else if (counter_state == bundles()) {
					events.push_back(ENV_EVENT(
						valid_signal, 
						get_valid_sense() ?
							LOGIC_HIGH :
							LOGIC_LOW));
				}
				// else leave alone in intermediate state
			}
		} else {
			reset_all_data_rails(events);
		}
		break;
	case LOGIC_HIGH:
		if (get_ack_active()) {
			reset_all_data_rails(events);
		} else {
			if (counter_state && (counter_state != bundles())) {
				// ambiguous
				cerr << ambiguous_data << endl;
			}
			if ((counter_state != bundles() || x_counter)) {
				set_all_data_rails(events);
				advance_value();
			}
			if (valid_signal) {
				if (x_counter) {
					events.push_back(ENV_EVENT(
						valid_signal, 
						LOGIC_OTHER));
				} else if (!counter_state) {
					events.push_back(ENV_EVENT(
						valid_signal, 
						get_valid_sense() ?
							LOGIC_LOW :
							LOGIC_HIGH));
				} else if (counter_state == bundles()) {
					events.push_back(ENV_EVENT(
						valid_signal, 
						get_valid_sense() ?
							LOGIC_HIGH :
							LOGIC_LOW));
				}
				// else leave alone in intermediate state
			}
		}
		break;
	default:
		X_all_data_rails(events);
	}	// end switch
}
// could also be sinking at the same time
if (is_sinking()) {
	INVARIANT(ack_signal);
	if (valid_signal) {
		// only react to the valid signal
		const node_type& v(s.get_node(valid_signal));
		// TODO: use xor and value inversion to simplify the following:
		switch (v.current_value()) {
		case LOGIC_LOW:
			if (get_valid_sense()) {
				// reset ack
				events.push_back(ENV_EVENT(ack_signal, 
					get_ack_active() ?
						LOGIC_LOW :
						LOGIC_HIGH));
			} else {
				// ack
				events.push_back(ENV_EVENT(ack_signal, 
					get_ack_active() ?
						LOGIC_HIGH :
						LOGIC_LOW));
			}
			break;
		case LOGIC_HIGH:
			if (get_valid_sense()) {
				// ack
				events.push_back(ENV_EVENT(ack_signal, 
					get_ack_active() ?
						LOGIC_HIGH :
						LOGIC_LOW));
			} else {
				// reset ack
				events.push_back(ENV_EVENT(ack_signal, 
					get_ack_active() ?
						LOGIC_LOW :
						LOGIC_HIGH));
			}
			break;
		default:
			events.push_back(ENV_EVENT(
				ack_signal, LOGIC_OTHER));
		}
	} else if (x_counter) {
		events.push_back(ENV_EVENT(
			ack_signal, LOGIC_OTHER));
	} else if (!counter_state) {
		// data is neutral, reset ack
		events.push_back(ENV_EVENT(ack_signal, 
			get_ack_active() ?
				LOGIC_LOW :
				LOGIC_HIGH));
	} else if (counter_state == bundles()) {
		// data is valid, ack
		events.push_back(ENV_EVENT(ack_signal, 
			get_ack_active() ?
				LOGIC_HIGH :
				LOGIC_LOW));
	}
	// else in some intermediate state, leave acknowledge alone
}
}	// end channel::resume

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Technically counter_state and x_counter should be reconstructible
	from the current state of the nodes.  
 */
bool
channel::save_checkpoint(ostream& o) const {
	STACKTRACE_VERBOSE;
	// don't write the name, let caller save it
	write_value(o, ack_signal);
	write_value(o, valid_signal);
#if PRSIM_CHANNEL_TIMING
	write_value(o, timing_mode);
	write_value(o, after);
#endif
	write_value(o, flags);
	write_value(o, counter_state);
	write_value(o, x_counter);
	write_value(o, bundles());	// size_t
	write_value(o, radix());	// size_t
	util::write_range(o, data.begin(), data.end());
	write_value(o, value_index);
//	write_value(o, inject_expect_file);
	if (dumplog.save_checkpoint(o)) return true;
	util::write_sequence(o, values);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel::load_checkpoint(istream& i) {
	STACKTRACE_VERBOSE;
	// don't restore the name here, let caller set it
	read_value(i, ack_signal);
	read_value(i, valid_signal);
#if PRSIM_CHANNEL_TIMING
	read_value(i, timing_mode);
	read_value(i, after);
#endif
	read_value(i, flags);
	read_value(i, counter_state);
	read_value(i, x_counter);
	data_rail_index_type k;
	read_value(i, k[0]);	// bundles
	read_value(i, k[1]);	// radix
	data.resize(k);
	util::read_range(i, data.begin(), data.end());
	read_value(i, value_index);
//	read_value(i, inject_expect_file);
	// no need to re-open file, already have its values
	if (dumplog.load_checkpoint(i)) return true;
	util::read_sequence_resize(i, values);
	return false;
}

//=============================================================================
// class channel method definitions

channel_manager::channel_manager() : 
		channel_pool(), 
		channel_index_set(), 
		node_channels_map() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_manager::~channel_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a channel, but leaves the acknowledgement and validity
	rails uninitialized.  
	Should probably call set_channel_ack_valid after this.
	\return true on error.
 */
bool
channel_manager::new_channel(State& state, const string& base, 
		const string& bundle_name, const size_t _num_bundles, 
		const string& rail_name, const size_t _num_rails, 
		const bool active_low) {
	STACKTRACE_VERBOSE;
	// 0 indicates that bundle/rail is scalar, not array
	// in any case, size should be at least 1
	const size_t num_bundles = _num_bundles ? _num_bundles : 1;
	const size_t num_rails = _num_rails ? _num_rails : 1;

	const size_t key = channel_pool.size();
	const pair<channel_set_type::iterator, bool>
		i(channel_index_set.insert(make_pair(base, key)));
if (i.second) {
	channel_pool.resize(key +1);	// default construct
	channel& c(channel_pool.back());
	c.name = base;
#if PRSIM_CHANNEL_RAILS_INVERTED
	c.set_data_sense(active_low);
#endif
	// allocate data rail references:
	channel::data_rail_index_type dk;
	dk[0] = num_bundles;
	dk[1] = num_rails;
	c.data.resize(dk);
	const entity::module& m(state.get_module());
	// lookup and assign node-indices
	dk[0] = 0;
	size_t& j = dk[0];
	for ( ; j<num_bundles; ++j) {
		ostringstream bundle_segment;
		if (bundle_name.length()) {
			bundle_segment << "." << bundle_name;
			if (_num_bundles) {
				bundle_segment << "[" << j << "]";
			}
		}
		dk[1] = 0;
		size_t& k = dk[1];
		for ( ; k<num_rails; ++k) {
			ostringstream n;
			n << base << bundle_segment.str() << "." << rail_name;
			if (_num_rails) {
				n << "[" << k << "]";
			}
			const node_index_type ni =
				parse_node_to_index(n.str(), m);
			if (ni) {
				c.data[dk] = ni;
				// flag node for consistency
				state.__get_node(ni).set_in_channel();
				c.__node_to_rail[ni] = dk;
				// lookup from node to channels
				node_channels_map[ni].insert(key);
			} else {
				cerr << "Error: no such node `" << n.str() <<
					"\' in channel." << endl;
				return true;
			}
		}
	}
	// initialize data-rail state counters from current values
	c.initialize_data_counter(state);
	return false;
} else {
	return true;
}
}	// end new_channel

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel*
channel_manager::lookup(const string& name) {
	const channel_set_type::const_iterator
		f(channel_index_set.find(name));
	if (f == channel_index_set.end()) {
		cerr << "Error, channel `" << name <<
			"\' not yet registered." << endl;
		return NULL;
	}
	return &channel_pool[f->second];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const channel*
channel_manager::lookup(const string& name) const {
	const channel_set_type::const_iterator
		f(channel_index_set.find(name));
	if (f == channel_index_set.end()) {
		cerr << "Error, channel `" << name <<
			"\' not yet registered." << endl;
		return NULL;
	}
	return &channel_pool[f->second];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// convenient macro to reduce copy-pasting...

/**
	Results in a channel reference named 'chan'.
	TODO: use lookup() instead.
	\return true on error
 */
#define	__GET_NAMED_CHANNEL(name)					\
	const channel_set_type::const_iterator				\
		f(channel_index_set.find(name));			\
	if (f == channel_index_set.end()) {				\
		cerr << "Error, channel `" << name <<			\
			"\' not yet registered." << endl;		\
		return true;						\
	}								\

#define	GET_NAMED_CHANNEL(chan, name)					\
	__GET_NAMED_CHANNEL(name)					\
	channel& chan(channel_pool[f->second]);

#define	GET_NAMED_CHANNEL_CONST(chan, name)				\
	__GET_NAMED_CHANNEL(name)					\
	const channel& chan(channel_pool[f->second]);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre channel has already been created by new_channel()
	\return true on error.
 */
bool
channel_manager::set_channel_ack_valid(State& state, const string& base, 
		const bool have_ack, const bool ack_sense, const bool ack_init, 
		const bool have_validity, const bool validity_sense) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, base)
	const entity::module& m(state.get_module());
if (have_ack) {
	c.set_ack_active(ack_sense);
	c.set_ack_init(ack_init);
	const string ack_name(base + (ack_sense ? ".a" : ".e"));
	const node_index_type ai = parse_node_to_index(ack_name, m);
	if (!ai) {
		cerr << "Error: no such node `" << ack_name <<
			"\' in channel." << endl;
		return true;
	}
	if (c.set_ack_signal(ai)) {
		cerr << "Channel acknowledge is not allowed to alias "
			"any of its data rails!" << endl;
		return true;
	}
	state.__get_node(ai).set_in_channel();		// flag in channel
	node_channels_map[ai].insert(f->second);	// reverse lookup
}
if (have_validity) {
	c.set_valid_sense(validity_sense);
	const string v_name(base + (validity_sense ? ".v" : ".n"));
	const node_index_type vi = parse_node_to_index(v_name, m);
	if (!vi) {
		cerr << "Error: no such node `" << v_name <<
			"\' in channel." << endl;
		return true;
	}
	if (c.set_valid_signal(vi)) {
		cerr << "Channel acknowledge is not allowed to alias "
			"any of its data rails!" << endl;
		return true;
	}
	if (c.ack_signal == c.valid_signal) {
		cerr << "Channel acknowledge and validity are not allowed to "
			"alias each other!" << endl;
		return true;
	}
	state.__get_node(vi).set_in_channel();		// flag in channel
	node_channels_map[vi].insert(f->second);	// reverse lookup
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Configure a registered channel to source values from a file.
 */
bool
channel_manager::source_channel_file(const State& s, 
		const string& channel_name, 
		const string& file_name, const bool loop) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	if (c.set_source_file(s, file_name, loop)) return true;
	return check_source(c, channel_name);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Configure a registered channel to source values from a list.
 */
bool
channel_manager::source_channel_args(const State& s, 
		const string& channel_name, 
		const string_list& values, const bool loop) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	if (c.set_source_args(s, values, loop)) return true;
	return check_source(c, channel_name);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Configure a registered channel to source values randomly, 
	and infinitely.
 */
bool
channel_manager::rsource_channel(const State& s, const string& channel_name) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	if (c.set_rsource(s)) return true;
	return check_source(c, channel_name);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if there is any connection error.
 */
bool
channel_manager::check_source(const channel& c, const string& chan_name) const {
	// warn if channel happens to be connected in wrong direction
	// TODO: check that data/validity are not driven by other sources!
if (c.valid_signal) {
	STACKTRACE_VERBOSE;
	__GET_NAMED_CHANNEL(chan_name)
	node_channels_map_type::const_iterator
		m(node_channels_map.find(c.valid_signal));
	INVARIANT(m != node_channels_map.end());
	set<channel_index_type>::const_iterator
		ti(m->second.begin()), te(m->second.end());
	for ( ; ti!=te; ++ti) {
	if (*ti != f->second) {
		const channel& ch(channel_pool[*ti]);
		if (ch.is_sourcing() && (ch.valid_signal == c.valid_signal)) {
			cerr << "Warning: channel validity is already "
				"being driven by source on channel `" <<
				ch.name << "\'." << endl;
			// return true;
		}
	}
	}
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Configure a registered channel to sink values, 
	with an optional file of expected values.
	\param loop is only applicable if file_name refers to a file of values.
 */
bool
channel_manager::sink_channel(const State& s, const string& channel_name) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	if (c.set_sink(s)) return true; // does many checks
	// already asserts(ack_signal)

	// check if signal is registered with other sinking channels?
	node_channels_map_type::const_iterator
		m(node_channels_map.find(c.ack_signal));
	INVARIANT(m != node_channels_map.end());

	set<channel_index_type>::const_iterator
		ti(m->second.begin()), te(m->second.end());
	for ( ; ti!=te; ++ti) {
	if (*ti != f->second) {
		const channel& ch(channel_pool[*ti]);
		if (ch.is_sinking() && (ch.ack_signal == c.ack_signal)) {
			cerr << "Warning: channel acknowledge is already "
				"being driven by sink on channel `" <<
				ch.name << "\'." << endl;
		}
	}
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::log_channel(const string& channel_name, 
		const string& file_name) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	return c.set_log(file_name);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::expect_channel_file(const string& channel_name, 
		const string& file_name, const bool loop) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	return c.set_expect_file(file_name, loop);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::expect_channel_args(const string& channel_name, 
		const string_list& values, const bool loop) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	return c.set_expect_args(values, loop);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::apply_one(const string& channel_name,
		void (channel::*f)(void)) {
	STACKTRACE_VERBOSE;
	channel* const c = lookup(channel_name);	// has error message
	if (c) {
		(c->*f)();
		return false;
	} else	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_manager::apply_all(void (channel::*f)(void)) {
	STACKTRACE_VERBOSE;
	for_each(channel_pool.begin(), channel_pool.end(), mem_fun_ref(f));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::resume_channel(const State& s, const string& channel_name, 
		vector<env_event_type>& events) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	c.resume(s, events);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_manager::resume_all_channels(const State& s,
		vector<env_event_type>& events) {
	STACKTRACE_VERBOSE;
	channel_pool_type::iterator
		i(channel_pool.begin()), e(channel_pool.end());
	for ( ; i!=e; ++i) {
		i->resume(s, events);	// could bind2nd_argval...
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::reset_channel(const string& channel_name, 
		vector<env_event_type>& events) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	c.reset(events);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_manager::reset_all_channels(vector<env_event_type>& events) {
	STACKTRACE_VERBOSE;
	// damn it, give me boost::lambda!
	channel_pool_type::iterator
		i(channel_pool.begin()), e(channel_pool.end());
	for ( ; i!=e; ++i) {
		i->reset(events);	// could bind2nd_argval...
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initialize all channels.
	Retains some modes.
 */
void
channel_manager::initialize(void) {
	STACKTRACE_VERBOSE;
	for_each(channel_pool.begin(), channel_pool.end(), 
		mem_fun_ref(&channel::initialize));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wipes all modes and settings.  
	Unregisters all channels.  
 */
void
channel_manager::clobber_all(void) {
	STACKTRACE_VERBOSE;
#if 0
	for_each(channel_pool.begin(), channel_pool.end(), 
		mem_fun_ref(&channel::clobber));
#else
	// basically just destroy in-place and re-construct in-place
	node_channels_map.clear();
	channel_index_set.clear();
	channel_pool.clear();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Handles an event on a node, according to sensitivity list.
	\return true if there are any assert errors.  
	\throw exception if assert value fails.
 */
void
channel_manager::process_node(const State& s, const node_index_type ni, 
		const value_enum prev, const value_enum next, 
		vector<env_event_type>& new_events) throw(channel_exception) {
	STACKTRACE_VERBOSE;
	// find all channels that this node participates in:
	const node_channels_map_type::const_iterator
		f(node_channels_map.find(ni));
if (f != node_channels_map.end()) {
	std::set<channel_index_type>::const_iterator
		i(f->second.begin()), e(f->second.end());
	for ( ; i!=e; ++i) {
		channel_pool[*i].process_node(s, ni, prev, next, new_events);
	}
}
	// else ignore, nothing to do, only cost one map lookup
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if node is potentially driven by any registered channel. 
 */
bool
channel_manager::node_has_fanin(const node_index_type ni) const {
	const node_channels_map_type::const_iterator
		f(node_channels_map.find(ni));
if (f != node_channels_map.end()) {
	std::set<channel_index_type>::const_iterator
		i(f->second.begin()), e(f->second.end());
	for ( ; i!=e; ++i) {
		if (channel_pool[*i].may_drive_node(ni))
			return true;
	}
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if any channel is potentially driven node.
 */
bool
channel_manager::node_has_fanout(const node_index_type ni) const {
	const node_channels_map_type::const_iterator
		f(node_channels_map.find(ni));
if (f != node_channels_map.end()) {
	std::set<channel_index_type>::const_iterator
		i(f->second.begin()), e(f->second.end());
	for ( ; i!=e; ++i) {
		if (channel_pool[*i].reads_node(ni))
			return true;
	}
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_manager::dump_node_fanin(ostream& o, const node_index_type ni) const {
	const node_channels_map_type::const_iterator
		f(node_channels_map.find(ni));
if (f != node_channels_map.end()) {
	std::set<channel_index_type>::const_iterator
		i(f->second.begin()), e(f->second.end());
	for ( ; i!=e; ++i) {
		const channel& c(channel_pool[*i]);
		if (c.may_drive_node(ni))
			o << ' ' << c.name;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_manager::dump_node_fanout(ostream& o, const node_index_type ni) const {
	const node_channels_map_type::const_iterator
		f(node_channels_map.find(ni));
if (f != node_channels_map.end()) {
	std::set<channel_index_type>::const_iterator
		i(f->second.begin()), e(f->second.end());
	for ( ; i!=e; ++i) {
		const channel& c(channel_pool[*i]);
		if (c.reads_node(ni))
			o << ' ' << c.name;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// Predicate: true if node is X
struct IfNodeX : public std::unary_function<const node_index_type, bool> {
	const State&		state;

	explicit
	IfNodeX(const State& s) : state(s) { }

	result_type
	operator () (argument_type ni) const {
		return state.get_node(ni).current_value() == LOGIC_OTHER;
	}
};	// end struct IfNodeX

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Return a list of nodes that this channel is dependent upon
	with value X.  
 */
void
channel_manager::__get_X_fanins(const State& s, const node_index_type ni, 
		node_set_type& ret) const {
	node_set_type fanins;
	const node_channels_map_type::const_iterator
		f(node_channels_map.find(ni));
if (f != node_channels_map.end()) {
	std::set<channel_index_type>::const_iterator
		i(f->second.begin()), e(f->second.end());
	for ( ; i!=e; ++i) {
		channel_pool[*i].__get_fanins(ni, fanins);
	}
	// filter out only those that are X
	copy_if(fanins.begin(), fanins.end(), set_inserter(ret), IfNodeX(s));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Asks why a node (if driven by channel, not-stopped) has not been
	driven up or down.
	\pre caller has already done visited/cycle check on sets u, v, 
		and that this node/path has not been visited before.  
	\pre there is no pending event on this node 'ni', else caller
		should have stopped there.  
 */
ostream&
channel_manager::__node_why_not(const State& s, ostream& o, 
		const node_index_type ni, const size_t limit, const bool dir, 
		const bool why_not, const bool verbose, 
		node_set_type& u, node_set_type& v) const {
	const node_channels_map_type::const_iterator
		f(node_channels_map.find(ni));
if (f != node_channels_map.end()) {
	std::set<channel_index_type>::const_iterator
		i(f->second.begin()), e(f->second.end());
	for ( ; i!=e; ++i) {
		channel_pool[*i].__node_why_not(
			s, o, ni, limit, dir, why_not, verbose, u, v);
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_manager::__node_why_X(const State& s, ostream& o, 
		const node_index_type ni, const size_t limit, 
		const bool verbose, node_set_type& u, node_set_type& v) const {
	const node_channels_map_type::const_iterator
		f(node_channels_map.find(ni));
if (f != node_channels_map.end()) {
	std::set<channel_index_type>::const_iterator
		i(f->second.begin()), e(f->second.end());
	for ( ; i!=e; ++i) {
		channel_pool[*i].__node_why_X(s, o, ni, limit, verbose, u, v);
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just show all managed channels.  
 */
ostream&
channel_manager::__dump(ostream& o, const bool state) const {
	o << "channels:" << endl;
	channel_set_type::const_iterator
		i(channel_index_set.begin()), e(channel_index_set.end());
for ( ; i!=e; ++i) {
	const channel& c(channel_pool[i->second]);
	c.dump(o) << endl;		// contains channel name
	if (state) {
		c.dump_state(o << '\t') << endl;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just show all managed channels.  
 */
bool
channel_manager::__dump_channel(ostream& o, const string& channel_name, 
		const bool state) const {
	GET_NAMED_CHANNEL_CONST(c, channel_name)
	c.dump(o) << endl;		// contains channel name
	if (state) {
		c.dump_state(o << '\t') << endl;
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true upon error
 */
bool
channel_manager::save_checkpoint(ostream& o) const {
	STACKTRACE_VERBOSE;
	write_value(o, channel_pool.size());
	channel_pool_type::const_iterator
		i(channel_pool.begin()), e(channel_pool.end());
	for ( ; i!=e; ++i) {
		// INVARIANT(*s == i->name);
		write_value(o, i->name);
		if (i->save_checkpoint(o)) {
			return true;
		}
		// node_channels_map is redundant, just reconstruct it
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true upon error
 */
bool
channel_manager::load_checkpoint(istream& i) {
	STACKTRACE_VERBOSE;
	channel_pool.clear();
	channel_index_set.clear();
	node_channels_map.clear();
	size_t s, j=0;
	read_value(i, s);
for ( ; j<s; ++j) {
	string c;
	read_value(i, c);	// name of channel
	size_t key = channel_pool.size();
	channel_pool.resize(key +1);
	channel_index_set[c] = key;
	channel& chan(channel_pool.back());
	chan.load_checkpoint(i);
	chan.name = c;
	// reconstruct node_channel_map reverse map
	vector<node_index_type> subnodes;
	chan.get_all_nodes(subnodes);
	vector<node_index_type>::const_iterator
		si(subnodes.begin()), se(subnodes.end());
	for ( ; si!=se; ++si) {
		node_channels_map[*si].insert(key);
	}
}
	return false;
}

//=============================================================================
#undef	ENV_EVENT
#undef	__GET_NAMED_CHANNEL
#undef	GET_NAMED_CHANNEL
#undef	GET_NAMED_CHANNEL_CONST
#undef	DATA_VALUE

}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

