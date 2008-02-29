/**
	\file "sim/prsim/Channel-prsim.cc"
	$Id: Channel-prsim.cc,v 1.1.4.3 2008/02/29 04:07:21 fang Exp $
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

#if PRSIM_CHANNEL_DONT_CARES
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
#endif	// PRSIM_CHANNEL_DONT_CARES

#if PRSIM_CHANNEL_DONT_CARES
#define	DATA_VALUE(x)			x.first
#else
#define	DATA_VALUE(x)			x
#endif

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
using util::copy_if;
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
	fname = "";
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
#if PRSIM_CHANNEL_VALIDITY
		valid_signal(INVALID_NODE_INDEX), 
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
	ret.push_back(ack_signal);
#if PRSIM_CHANNEL_VALIDITY
	ret.push_back(valid_signal);
#endif
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
#if PRSIM_CHANNEL_DONT_CARES
ostream&
operator << (ostream& o, const channel::array_value_type& p) {
	if (p.second) {
		return o << 'X';
	} else {
		return o << p.first;
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't bother printing out node indices.
 */
ostream&
channel::dump(ostream& o) const {
	o << name << " : ";
	o << (get_ack_active() ? ".a" : ".e");
	o << "(init:" << (get_ack_init() ? '1' : '0') << ')';
#if PRSIM_CHANNEL_VALIDITY
	if (valid_signal) {
		o << ' ' << (get_valid_sense() ? ".v" : ".n");
	}
#endif
	// didn't store names of bundles and rails
	o << ' ' << bundles() << "x1of" << radix();
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
	if (have_value() &&
			((is_sourcing() && !is_random()) || is_expecting())) {
		o << " {";
		copy(values.begin(), values.end(), 
			ostream_iterator<array_value_type>(o, ","));
		o << '}';
		if (is_looping()) o << '*';
		o << " @" << value_index;
		o << " < " << inject_expect_file;	// source/expect
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
while (1) {
	string line;
	getline(f, line);
	if (!f) break;
	if ((line.length() > 0) && (line[0] != '#')) {
		util::string_list toks;
		util::tokenize(line, toks);
#if PRSIM_CHANNEL_DONT_CARES
		channel::array_value_type p;
		channel::value_type& i(p.first);
		p.first = 0;
		p.second = false;
		if (toks.front() == "X") {
			p.second = true;
		} else
#else
		channel::array_value_type i;
#endif
		if (string_to_num(toks.front(), i)) {
			cerr << "Error: invalid value \"" <<
				toks.front() << "\"." << endl;
			return true;
		} else
		if (i > std::numeric_limits<channel::value_type>::max() >> 1) {
			cerr << "Warning: value " << i << " is greater than "
				"max(unsigned value_type)/2, which may screw "
				"up ldiv() when translating to rails." << endl;
		}
#if PRSIM_CHANNEL_DONT_CARES
		v.push_back(p);
#else
		v.push_back(i);
#endif
	}
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common actions for configuring source operation.  
	\return true if there are errors
 */
bool
channel::set_source(const State& s, const string& file_name, const bool loop) {
	STACKTRACE_VERBOSE;
	if (__configure_source(s))	return true;
	if (loop)
		flags |= CHANNEL_VALUE_LOOP;
	value_index = 0;	// TODO: optional offset or initial position
	if (read_values_from_file(file_name, values)) return true;
	if (!values.size()) {
		cerr << "Error: no values found in file \"" << file_name
			<< "\", cannot source channel."
			<< endl;
		return true;
	}
	// no need to set inject/expect file handle's underlying stream
	inject_expect_file = file_name;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Configure source to just emit random values.  
 */
bool
channel::set_rsource(const State& s) {
	STACKTRACE_VERBOSE;
	if (__configure_source(s))	return true;
	flags |= CHANNEL_RANDOM;
	value_index = 0;
	values.resize(1);	
	// used as value holder to remember the last value
	inject_expect_file = "";
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Drive a channel from a file, sourcing values, no loop.  
	\return true on error
	TODO: check for values that overflow (bundle,rail)?
 */
bool
channel::__configure_source(const State& s) {
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

	// safety checks on signal directions
	if (!s.get_node(ack_signal).has_fanin()) {
		cerr << "Warning: channel acknowledge `" << name <<
			(get_ack_active() ? ".a" : ".e") <<
			"\' has no fanin!" << endl;
	}
	bool maybe_externally_driven = false;
#if PRSIM_CHANNEL_VALIDITY
	const State::node_type& vn(s.get_node(valid_signal));
	if (vn.has_fanin()) {
		cerr << "Warning: channel validity `" << name <<
			(get_valid_sense() ? ".v" : ".n") <<
			"\' has fanin.\n";
		maybe_externally_driven = true;
		// but make sure this is not doubly driven by another source
	}
#if 0
	if (!vn.has_fanout()) {
		// this warning might be excessive
		cerr << "Warning: channel validity `" << channel_name <<
			(c.get_valid_sense() ? ".v" : ".n") <<
			"\' has no fanout." << endl;
	}
#else
	// this is forgiveable
#endif
#endif
	data_bundle_array_type::const_iterator i(data.begin()), e(data.end());
	for ( ; i!=e; ++i) {
		const State::node_type& d(s.get_node(*i));
		if (d.has_fanin()) {
			const size_t pos = i -data.begin();	// std::distance
			const size_t j = pos / radix();
			const size_t k = pos % radix();
			cerr << "Warning: channel data rail `" << name <<
				"\' (" << j << ", " << k << ") has fanin.\n";
			maybe_externally_driven = true;
		}
	}
	if (maybe_externally_driven) {
		cerr << "Channel source may not operator properly, "
			"if driven from elsewhere." << endl;
	}
	return false;
}	// end channel::set_source

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
	if (s.get_node(ack_signal).has_fanin()) {
		cerr << "Warning: channel acknowledge `" << name <<
			(get_ack_active() ? ".a" : ".e") <<
			"\' already has fanin!\n"
			"Channel sink may not operator properly, "
			"if driven from elsewhere." << endl;
	}
#if PRSIM_CHANNEL_VALIDITY
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
#else
	// this is forgiveable
#endif
#endif
	// do we care if data rails lack fanin?
	return false;
}	// end channel::set_sink

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expecting values conflicts with sourcing values.  
	TODO: check for values that overflow (bundle,rail)?
 */
bool
channel::set_expect(const string& fn, const bool loop) {
	STACKTRACE_VERBOSE;
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
		case node_type::LOGIC_HIGH:
			++counter_state;
			break;
		case node_type::LOGIC_OTHER:
			++x_counter;
			break;
		default: break;
		}
	}
	if (counter_state > bundles()) {
		cerr << "Channel data rails are in an invalid state!" << endl;
		cerr << "In channel `" << name << "\', got " << counter_state
			<< " high rails, whereas only " << bundles() <<
			" are permitted." << endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Binding constructor functor.
 */
struct __node_setter :
	public std::unary_function<const node_index_type, env_event_type> {
	uchar				val;

	explicit
	__node_setter(const uchar v) : val(v) { }

	result_type
	operator () (argument_type ni) const {
		return result_type(ni, val);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sources will lower all data rails, 
	sinks will set acknowledges according to the reset value.
 */
void
channel::reset(vector<env_event_type>& events) {
	STACKTRACE_VERBOSE;
	typedef	State::node_type		node_type;
	if (is_sourcing()) {
		transform(data.begin(), data.end(), back_inserter(events), 
			__node_setter(node_type::LOGIC_LOW));
	}
	if (is_sinking()) {
		events.push_back(env_event_type(ack_signal, 
			(get_ack_init() ? node_type::LOGIC_HIGH
				: node_type::LOGIC_LOW)));
	}
	// else nothing else to do
	stop();	// freeze this channel until it is resumed
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Point to next value in sequence, if applicable.
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
			value_index = values.size();	// one past end
		}
	}
	// else if values.size(), values.clear() ?
#if PRSIM_CHANNEL_DONT_CARES
	if (values[value_index].second) {
		// if values is X (don't care), then choose a random value
		values[value_index].first = rand48<value_type>()();
	}
#endif
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return in r the list of data rails selected by current value.
	If there is no current value, then return nothing.  
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
			r.push_back(env_event_type(data[k], 	// node index
				(k[1] == size_t(qr.rem)) ?
				node_type::LOGIC_HIGH : node_type::LOGIC_LOW));
		}
	}
} else {
	INVARIANT(!is_random());
	// otherwise following code would wipe the random value slot!
	// no next value, just hold all data rails neutral
	transform(data.begin(), data.end(), back_inserter(r), 
		__node_setter(node_type::LOGIC_LOW));
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
		const uchar val) {
	STACKTRACE_VERBOSE;
	INVARIANT(is_sourcing());
if (have_value()) {
	vector<node_index_type> nodes;
	current_data_rails(nodes);	// use current values[value_index]
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
			case node_type::LOGIC_LOW: break;
			case node_type::LOGIC_HIGH:
				INVARIANT(!have_hi);
				have_hi = true;
				hi = k[1];
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
#if PRSIM_CHANNEL_VALIDITY
		if (validity_signal && (ni == validity_signal)) {
			return true;
		}
#endif
		// check: is it data rail of source?
		const data_rail_map_type::const_iterator
			f(__node_to_rail.find(ni));
		if (f != __node_to_rail.end()) {
			return true;
		}
	}
	if (is_sinking()) {
		if (ack_signal == ni) {
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
#if PRSIM_CHANNEL_VALIDITY
		if (validity_signal && (ni == validity_signal)) {
			copy(data.begin(), data.end(), set_inserter(ret));
		}
#endif
		// if node is data rail, then effective input is ack
		const data_rail_map_type::const_iterator
			f(__node_to_rail.find(ni));
		if (f != __node_to_rail.end()) {
			ret.insert(ack_signal);
		}
	}
	if (is_sinking()) {
	if (ack_signal == ni) {
#if PRSIM_CHANNEL_VALIDITY
		// validity is responsibility of the circuit, not the sink.
		if (validity_signal)
			ret.insert(validity_signal);
		else
#endif
		copy(data.begin(), data.end(), set_inserter(ret));
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
		const bool dir, const bool why_not, const bool verbose, 
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
#if PRSIM_CHANNEL_VALIDITY
		if (validity_signal && (ni == validity_signal)) {
			// then point back to data rails, see below
			// eventually refactor that code out
			FINISH_ME(Fang);
		}
#endif
		const data_rail_map_type::const_iterator
			f(__node_to_rail.find(ni));
		if (f != __node_to_rail.end()) {
			// then is attributed to acknowledge
			// interpret dir as data-active, 
			// which equivalently asks why acknowledge is not
			// the opposite (active).  
			// const indent __ind_nd(o, verbose ? "." : "  ");
			// INDENT_SCOPE(o);
			s.__node_why_not(o, ack_signal, 
				dir ^ get_ack_active() ^ !why_not,
				why_not, verbose, u, v);
		}
	}
	if (is_sinking() && (ni == ack_signal)) {
		// no other signal should be driven by sink
#if PRSIM_CHANNEL_VALIDITY
	if (validity_signal) {
		// TODO: not sure if the following is correct
		// it may be backwards, if I just think about it...
		// TODO: account for why_not parameter
		if (get_ack_active() ^ dir ^ get_valid_sense()) {
			s.__node_why_not(o, validity_signal, 
				true, why_not, verbose, u, v);
		} else {
			s.__node_why_not(o, validity_signal, 
				false, why_not, verbose, u, v);
		}
	} else
#endif
	{
		// TODO: factor the following code out
		// then query completion status of the data rails, 
		// assuming celem-of-or style completion of bundles
		const node_type& a(s.get_node(ack_signal));
		const uchar av = a.current_value();
		string ind_str;
		if (verbose && (bundles() > 1)) {
			ind_str += " & ";
			o << auto_indent << "-+" << endl;
		}
		const indent __ind_celem(o, ind_str);      // INDENT_SCOPE(o);
		data_rail_index_type key;
		key[0] = 0;
		for ( ; key[0] < bundles(); ++key[0]) {
			// first find out if bundle is valid
			size_t partial_valid = 0;
			key[1] = 0;
			for ( ; key[1] < radix(); ++key[1]) {
				if (s.get_node(data[key]).current_value() ==
					node_type::LOGIC_HIGH) {
					++partial_valid;
				}
			}
			// second pass, only if bundle is not ready
			string i_s;
			if (verbose && (radix() > 1)) {
				i_s += " ";
				// when to negate (nor)?
				// when data wants to be neutral, 
				// i.e. when acknowledge is active
				if ((av == node_type::LOGIC_LOW) ^ !why_not ^
						get_ack_active()) {
					i_s += "~";
				}
				i_s += "| ";
				o << auto_indent << "-+" << endl;
			}
			const indent __ind_or(o, i_s);	// INDENT_SCOPE(o);

			key[1] = 0;
			for ( ; key[1] < radix(); ++key[1]) {
				const node_index_type di = data[key];
				const node_type& d(s.get_node(di));
			switch (d.current_value()) {
			case node_type::LOGIC_LOW:
				if (get_ack_active() ^
					(av == node_type::LOGIC_HIGH)) {
					s.__node_why_not(o, di, why_not, 
						why_not, verbose, u, v);
				}
				break;
			case node_type::LOGIC_HIGH:
				if (get_ack_active() ^
					(av == node_type::LOGIC_LOW)) {
					s.__node_why_not(o, di, !why_not, 
						why_not, verbose, u, v);
				}
				break;
			default: break;	// ignore Xs
			}	// end switch
			}	// end for rails
		}	// end for bundles
	}
	}	// end if sinking
}	// end if !stopped
	return o;
}	// end channel::__node_why_not

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
		const uchar prev, const uchar next, 
		vector<env_event_type>& new_events) {
	STACKTRACE_VERBOSE;
	typedef	State::node_type	node_type;
#if ENABLE_STACKTRACE
	cout << s.get_node_canonical_name(ni) << " : " << size_t(prev) << 
		" -> " << size_t(next) << endl;
#endif
// first identify which channel node member this node is
if (ni == ack_signal) {
	// only need to take action if this is a source
	if (is_sourcing() && !stopped()) {
	switch (next) {
		// assumes that data rails are active high
	case node_type::LOGIC_LOW:
		if (get_ack_active()) {
			// \pre all data rails are neutral
			// set data rails to next data value
			set_all_data_rails(new_events);
			advance_value();
		} else {
			// reset all data rails
			transform(data.begin(), data.end(),
				back_inserter(new_events), 
				__node_setter(node_type::LOGIC_LOW));
		}
		break;
	case node_type::LOGIC_HIGH:
		if (get_ack_active()) {
			// reset all data rails
			transform(data.begin(), data.end(),
				back_inserter(new_events), 
				__node_setter(node_type::LOGIC_LOW));
		} else {
			// \pre all data rails are neutral
			// set data rails to next data value
			set_all_data_rails(new_events);
			advance_value();
		}
		break;
	default:
		// set all data to X
		transform(data.begin(), data.end(), back_inserter(new_events), 
			__node_setter(node_type::LOGIC_OTHER));
		break;
	}
	}
	// logging and expect mode don't care
#if PRSIM_CHANNEL_VALIDITY
} else if (valid_signal && (ni == valid_signal)) {
	// only need to take action if this is a sink
	if (is_sinking() && !stopped()) {
	switch (next) {
	case node_type::LOGIC_LOW:
		if (get_valid_sense()) {
			// neutral, reset ack
			new_events.push_back(env_event_type(ack_signal, 
				get_ack_active() ? node_type::LOGIC_LOW
					: node_type::LOGIC_HIGH));
		} else {
			// valid, ack
			new_events.push_back(env_event_type(ack_signal, 
				get_ack_active() ? node_type::LOGIC_HIGH
					: node_type::LOGIC_LOW));
		}
		break;
	case node_type::LOGIC_HIGH:
		if (get_valid_sense()) {
			// valid, ack
			new_events.push_back(env_event_type(ack_signal, 
				get_ack_active() ? node_type::LOGIC_HIGH
					: node_type::LOGIC_LOW));
		} else {
			// neutral, reset ack
			new_events.push_back(env_event_type(ack_signal, 
				get_ack_active() ? node_type::LOGIC_LOW
					: node_type::LOGIC_HIGH));
		}
		break;
	default:
		new_events.push_back(env_event_type(ack_signal, 
			node_type::LOGIC_OTHER));
		break;
	}
	}
#endif
} else {
	// invariant: must be data rail
	// update state counters
	switch (prev) {
	case node_type::LOGIC_HIGH:
		INVARIANT(counter_state);
		--counter_state;
		break;
	case node_type::LOGIC_OTHER:
		INVARIANT(x_counter);
		--x_counter;
		break;
	default: break;
	}
	switch (next) {
	case node_type::LOGIC_HIGH: ++counter_state; break;
	case node_type::LOGIC_OTHER: ++x_counter; break;
	default: break;
	}
	if (x_counter) {
		// if there are ANY Xs, then cannot log/expect values
		// sources/sinks should respond accordingly with X signals
		if (!stopped()) {
		if (is_sourcing()) {
#if PRSIM_CHANNEL_VALIDITY
			// for validity protocol, set valid to X
			if (valid_signal) {
				new_events.push_back(env_event_type(
					valid_signal, node_type::LOGIC_OTHER));
			}
#endif
		}
		if (is_sinking()) {
#if PRSIM_CHANNEL_VALIDITY
			// if not validity protocol, set ack to X
			if (!valid_signal) {
				new_events.push_back(env_event_type(
					ack_signal, node_type::LOGIC_OTHER));
			}
			// otherwise wait for validity to go X
#else
			new_events.push_back(env_event_type(
				ack_signal, node_type::LOGIC_OTHER));
#endif
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
		if (!stopped()) {
#if PRSIM_CHANNEL_VALIDITY
		if (is_sourcing() && valid_signal) {
			// source is responsible for resetting valid signal
			new_events.push_back(env_event_type(valid_signal, 
				get_ack_active() ? node_type::LOGIC_LOW
					: node_type::LOGIC_HIGH));
		}
#else
		// if this is sourcing, then don't care
#endif
		if (is_sinking()
#if PRSIM_CHANNEL_VALIDITY
			&& !valid_signal
#endif
		) {
			// sink should reply with ack reset
			// otherwise, valid_signal is an input
			new_events.push_back(env_event_type(ack_signal, 
				get_ack_active() ? node_type::LOGIC_LOW
					: node_type::LOGIC_HIGH));
		}
		}
	} else if (counter_state == bundles()) {
		// NOTE: stopped channels will not assert expected data nor log!
	if (!stopped()) {
		// then data rails are in valid state
		if (watched()) {
			cout << "channel\t" << name << " (.data) : " <<
				data_rails_value(s) << endl;
		}
		if (dumplog.stream && *dumplog.stream) {
			// TODO: format me, hex, dec, bin, etc...
			// should be able to just setbase()
			(*dumplog.stream) << data_rails_value(s) << endl;
			// really flush every line?
		}
		if (is_expecting()) {
		if (have_value()) {
			// don't bother waiting for validity signal
			const array_value_type& expect = current_value();
#if PRSIM_CHANNEL_DONT_CARES
			if (!expect.second) {
#endif
			const value_type got = data_rails_value(s);
			advance_value();
			if (DATA_VALUE(expect) != got) {
				throw State::channel_exception(name, 
					DATA_VALUE(expect), got);
			}
#if PRSIM_CHANNEL_DONT_CARES
			}	// else don't care
#endif
		} else {
			// exhausted values, disable expecting
			flags &= ~CHANNEL_EXPECTING;
			// might as well release memory...
			if (!values.empty()) {
				values.clear();
			}
		}
		}
		// if no value available, just ignore
#if PRSIM_CHANNEL_VALIDITY
		if (is_sourcing() && valid_signal) {
			// source is responsible for setting valid signal
			new_events.push_back(env_event_type(valid_signal, 
				get_ack_active() ? node_type::LOGIC_HIGH
					: node_type::LOGIC_LOW));
		}
#else
		// if this is sourcing, then don't care
#endif
		if (is_sinking()
#if PRSIM_CHANNEL_VALIDITY
			&& !valid_signal
#endif
		) {
			// sink should reply with ack reset
			// otherwise, valid_signal is an input
			new_events.push_back(env_event_type(ack_signal, 
				get_ack_active() ? node_type::LOGIC_HIGH
					: node_type::LOGIC_LOW));
		}
	}
	}
}
}	// end channel::process_node

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We don't know the current state of the channel because it may
	have changed while this was stopped, thus we need to refresh
	with a complete re-evaluation of signals to infer the current state.  
	Data rail activity should have been tracked even while
	channel was stopped.  
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
	// validity should be set after all data rails are valid/neutral
	const node_type& a(s.get_node(ack_signal));
	switch (a.current_value()) {
	case node_type::LOGIC_LOW:
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
#if PRSIM_CHANNEL_VALIDITY
			if (valid_signal) {
				if (x_counter) {
					events.push_back(env_event_type(
						valid_signal, 
						node_type::LOGIC_OTHER));
				} else if (!counter_state) {
					events.push_back(env_event_type(
						valid_signal, 
						get_valid_sense() ?
							node_type::LOGIC_LOW :
							node_type::LOGIC_HIGH));
				} else if (counter_state == bundles()) {
					events.push_back(env_event_type(
						valid_signal, 
						get_valid_sense() ?
							node_type::LOGIC_HIGH :
							node_type::LOGIC_LOW));
				}
				// else leave alone in intermediate state
			}
#endif
		} else {
			// reset data
			transform(data.begin(), data.end(),
				back_inserter(events),
				__node_setter(node_type::LOGIC_LOW));
		}
		break;
	case node_type::LOGIC_HIGH:
		if (get_ack_active()) {
			// reset data
			transform(data.begin(), data.end(),
				back_inserter(events),
				__node_setter(node_type::LOGIC_LOW));
		} else {
			if (counter_state && (counter_state != bundles())) {
				// ambiguous
				cerr << ambiguous_data << endl;
			}
			if ((counter_state != bundles() || x_counter)) {
				set_all_data_rails(events);
				advance_value();
			}
#if PRSIM_CHANNEL_VALIDITY
			if (valid_signal) {
				if (x_counter) {
					events.push_back(env_event_type(
						valid_signal, 
						node_type::LOGIC_OTHER));
				} else if (!counter_state) {
					events.push_back(env_event_type(
						valid_signal, 
						get_valid_sense() ?
							node_type::LOGIC_LOW :
							node_type::LOGIC_HIGH));
				} else if (counter_state == bundles()) {
					events.push_back(env_event_type(
						valid_signal, 
						get_valid_sense() ?
							node_type::LOGIC_HIGH :
							node_type::LOGIC_LOW));
				}
				// else leave alone in intermediate state
			}
#endif
		}
		break;
	default:
		transform(data.begin(), data.end(),
			back_inserter(events),
			__node_setter(node_type::LOGIC_OTHER));
	}	// end switch
}
// could also be sinking at the same time
if (is_sinking()) {
#if PRSIM_CHANNEL_VALIDITY
	if (valid_signal) {
		const node_type& v(get_node(valid_signal));
		// TODO: use xor and value inversion to simplify the following:
		switch (v.current_value()) {
		case node_type::LOGIC_LOW:
			if (get_valid_sense()) {
				// reset ack
				events.push_back(env_event_type(ack_signal, 
					get_ack_active() ?
						node_type::LOGIC_LOW :
						node_type::LOGIC_HIGH));
			} else {
				// ack
				events.push_back(env_event_type(ack_signal, 
					get_ack_active() ?
						node_type::LOGIC_HIGH :
						node_type::LOGIC_LOW));
			}
			break;
		case node_type::LOGIC_HIGH:
			if (get_valid_sense()) {
				// ack
				events.push_back(env_event_type(ack_signal, 
					get_ack_active() ?
						node_type::LOGIC_HIGH :
						node_type::LOGIC_LOW));
			} else {
				// reset ack
				events.push_back(env_event_type(ack_signal, 
					get_ack_active() ?
						node_type::LOGIC_LOW :
						node_type::LOGIC_HIGH));
			}
			break;
		default:
			events.push_back(env_event_type(
				ack_signal, node_type::LOGIC_OTHER));
		}
	} else
#endif
	if (x_counter) {
		events.push_back(env_event_type(
			ack_signal, node_type::LOGIC_OTHER));
	} else if (!counter_state) {
		// data is neutral, reset ack
		events.push_back(env_event_type(ack_signal, 
			get_ack_active() ?
				node_type::LOGIC_LOW :
				node_type::LOGIC_HIGH));
	} else if (counter_state == bundles()) {
		// data is valid, ack
		events.push_back(env_event_type(ack_signal, 
			get_ack_active() ?
				node_type::LOGIC_HIGH :
				node_type::LOGIC_LOW));
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
#if PRSIM_CHANNEL_VALIDITY
	write_value(o, valid_signal);
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
#if PRSIM_CHANNEL_VALIDITY
	read_avlue(i, valid_signal);
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
		const string& rail_name, const size_t _num_rails) {
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
				state.get_node(ni).set_in_channel();
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
// convenient macro to reduce copy-pasting...

/**
	Results in a channel reference named 'chan'.
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
		const bool ack_sense, const bool ack_init, 
		const bool have_validity, const bool validity_sense) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, base)
	const entity::module& m(state.get_module());
{
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
	state.get_node(ai).set_in_channel();		// flag in channel
	node_channels_map[ai].insert(f->second);	// reverse lookup
}
#if PRSIM_CHANNEL_VALIDITY
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
	state.get_node(vi).set_in_channel();		// flag in channel
	node_channels_map[vi].insert(f->second);	// reverse lookup
}
#endif
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Configure a registered channel to source values from a file.
 */
bool
channel_manager::source_channel(const State& s, const string& channel_name, 
		const string& file_name, const bool loop) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	if (c.set_source(s, file_name, loop)) return true;
	return check_source(c);
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
	return check_source(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if there is any connection error.
 */
bool
channel_manager::check_source(const channel& c) const {
	// warn if channel happens to be connected in wrong direction
	// TODO: check that data/validity are not driven by other sources!
#if PRSIM_CHANNEL_VALIDITY
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
#endif
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
channel_manager::expect_channel(const string& channel_name, 
		const string& file_name, const bool loop) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	return c.set_expect(file_name, loop);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::close_channel(const string& channel_name) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	c.close_stream();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_manager::close_all_channels(void) {
	STACKTRACE_VERBOSE;
	for_each(channel_pool.begin(), channel_pool.end(), 
		mem_fun_ref(&channel::close_stream));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::stop_channel(const string& channel_name) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	c.stop();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_manager::stop_all_channels(void) {
	STACKTRACE_VERBOSE;
	for_each(channel_pool.begin(), channel_pool.end(), 
		mem_fun_ref(&channel::stop));
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
bool
channel_manager::watch_channel(const string& channel_name) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	c.watch();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_manager::watch_all_channels(void) {
	STACKTRACE_VERBOSE;
	for_each(channel_pool.begin(), channel_pool.end(), 
		mem_fun_ref(&channel::watch));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::unwatch_channel(const string& channel_name) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	c.unwatch();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_manager::unwatch_all_channels(void) {
	STACKTRACE_VERBOSE;
	for_each(channel_pool.begin(), channel_pool.end(), 
		mem_fun_ref(&channel::unwatch));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if there are any assert errors.  
	\throw exception if assert value fails.
 */
void
channel_manager::process_node(const State& s, const node_index_type ni, 
		const uchar prev, const uchar next, 
		vector<env_event_type>& new_events) {
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
/// Predicate: true if node is X
struct IfNodeX : public std::unary_function<const node_index_type, bool> {
	const State&		state;

	explicit
	IfNodeX(const State& s) : state(s) { }

	result_type
	operator () (argument_type ni) const {
		return state.get_node(ni).current_value()
			== State::node_type::LOGIC_OTHER;
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
		const node_index_type ni, const bool dir, 
		const bool why_not, const bool verbose, 
		node_set_type& u, node_set_type& v) const {
	const node_channels_map_type::const_iterator
		f(node_channels_map.find(ni));
if (f != node_channels_map.end()) {
	std::set<channel_index_type>::const_iterator
		i(f->second.begin()), e(f->second.end());
	for ( ; i!=e; ++i) {
		channel_pool[*i].__node_why_not(
			s, o, ni, dir, why_not, verbose, u, v);
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
#undef	__GET_NAMED_CHANNEL
#undef	GET_NAMED_CHANNEL
#undef	GET_NAMED_CHANNEL_CONST
#undef	DATA_VALUE

}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

