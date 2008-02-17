/**
	\file "sim/prsim/Channel-prsim.cc"
	$Id: Channel-prsim.cc,v 1.1.2.3 2008/02/17 02:20:39 fang Exp $
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <iterator>			// for ostream_iterator
#include "sim/prsim/Channel-prsim.h"
#include "sim/prsim/State-prsim.h"
#include "parser/instref.h"
#include "util/memory/count_ptr.tcc"
#include "util/string.tcc"
#include "util/tokenize.h"
#include "util/IO_utils.tcc"

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
using entity::int_value_type;
#include "util/using_ostream.h"
using util::read_value;
using util::write_value;
using util::strings::string_to_num;

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
	size_t j = 0;
	for ( ; j<bundles(); ++j) {
	size_t k = 0;
	for ( ; k<radix(); ++k) {
		ret.push_back(data[j][k]);
	}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel::dump(ostream& o) const {
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
	o << ' ' << (is_sourcing() ? "source" : is_sinking() ? "sink" : "off")
		<< (is_expecting() ? ",expect" : "");
	if (is_sourcing() || is_expecting()) {
		o << " {";
		copy(values.begin(), values.end(), 
			ostream_iterator<int_value_type>(o, ","));
		o << '}';
		if (is_looping()) o << '*';
		o << " @" << value_index;
		o << " < " << inject_expect_file;	// source/expect
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
	o << "count: " << counter_state;
	o << ", unkonwns: " << x_counter;
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
read_values_from_file(const string& fn, vector<int_value_type>& v) {
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
		int_value_type i;
		if (string_to_num(toks.front(), i)) {
			cerr << "Error: invalid value \"" <<
				toks.front() << "\"." << endl;
			return true;
		}
	}
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Drive a channel from a file, sourcing values, no loop.  
	\return true on error
	TODO: check for values that overflow (bundle,rail)?
 */
bool
channel::set_source(const State& s, const string& file_name, const bool loop) {
	if (is_sourcing()) {
		cout <<
"Warning: reconnecting channel from old source to new source."
		<< endl;
	} else if (is_expecting()) {
		cout <<
"Warning: no longer asserting expected channel values; connecting to source."
		<< endl;
	}
	// is OK to log
	// warn if data/validity is already driven with fanin
	// warn if ack has no fanin
	flags &= ~(CHANNEL_EXPECTING | CHANNEL_VALUE_LOOP);
	flags |= CHANNEL_SOURCING;
	if (loop)
		flags |= CHANNEL_VALUE_LOOP;
	value_index = 0;	// TODO: optional offset or initial position
	if (read_values_from_file(file_name, values)) return true;
	if (!values.size()) {
		cerr << "Error: no values found in file, cannot source channel."
			<< endl;
		return true;
	}
	// no need to set inject/expect file handle's underlying stream
	inject_expect_file = file_name;

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
	size_t j = 0;
	for ( ; j<bundles(); ++j) {
	size_t k = 0;
	for ( ; k<radix(); ++k) {
		const State::node_type& d(s.get_node(data[j][k]));
		if (d.has_fanin()) {
			cerr << "Warning: channel data rail `" << name <<
				"\' (" << j << ", " << k << ") has fanin.\n";
			maybe_externally_driven = true;
		}
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
		cerr << "I will automatically drive this signal for you "
			"according to the state of the data rails... "
			"because I am so kind." << endl;
		// but make sure this is not doubly driven by another source
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
	initialize_data_counter(s);
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
 */
void
channel::initialize_data_counter(const State& s) {
	counter_state = 0;
	x_counter = 0;
	size_t j = 0;
	for ( ; j<bundles(); ++j) {
	size_t k = 0;
	for ( ; k<radix(); ++k) {
		typedef	State::node_type		node_type;
		switch (s.get_node(data[j][k]).current_value()) {
		case node_type::LOGIC_HIGH:
			++counter_state;
			break;
		case node_type::LOGIC_OTHER:
			++x_counter;
			break;
		default: break;
		}
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
	Technically counter_state and x_counter should be reconstructible
	from the current state of the nodes.  
 */
bool
channel::save_checkpoint(ostream& o) const {
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
	size_t j = 0;
	for ( ; j<bundles(); ++j) {
	size_t k = 0;
	for ( ; k<radix(); ++k) {
		write_value(o, data[j][k]);
	}
	}
	write_value(o, value_index);
//	write_value(o, inject_expect_file);
	if (dumplog.save_checkpoint(o)) return true;
	util::write_sequence(o, values);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel::load_checkpoint(istream& i) {
	// don't restore the name here, let caller set it
	read_value(i, ack_signal);
#if PRSIM_CHANNEL_VALIDITY
	read_avlue(i, valid_signal);
#endif
	read_value(i, flags);
	read_value(i, counter_state);
	read_value(i, x_counter);
	size_t _bundles, _radix;
	read_value(i, _bundles);
	read_value(i, _radix);
	data.resize(_bundles);
	size_t j = 0;
	for ( ; j<_bundles; ++j) {
		rails_array_type& r(data[j]);
		r.resize(_radix);
		size_t k = 0;
		for ( ; k<_radix; ++k) {
			read_value(i, r[k]);
		}
	}
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
	c.data.resize(num_bundles);
	// this would be easier with a packed_array...
	for_each(c.data.begin(), c.data.end(),
		bind2nd(mem_fun_ref(&channel::rails_array_type::resize),
			num_rails));
	const entity::module& m(state.get_module());
	// lookup and assign node-indices
	size_t j = 0;
	for ( ; j<num_bundles; ++j) {
		ostringstream bundle_segment;
		if (bundle_name.length()) {
			bundle_segment << "." << bundle_name;
			if (_num_bundles) {
				bundle_segment << "[" << j << "]";
			}
		}
		size_t k = 0;
		for ( ; k<num_rails; ++k) {
			ostringstream n;
			n << base << bundle_segment.str() << "." << rail_name;
			if (_num_rails) {
				n << "[" << k << "]";
			}
			const node_index_type ni =
				parse_node_to_index(n.str(), m);
			if (ni) {
				c.data[j][k] = ni;
				// flag node for consistency
				state.get_node(ni).set_in_channel();
				c.__node_to_rail[ni] = make_pair(j, k);
				// lookup from node to channels
				node_channels_map[ni].insert(key);
			} else {
				cerr << "Error: no such node `" << n.str() <<
					"\' in channel." << endl;
				return true;
			}
		}
	}
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
#define	GET_NAMED_CHANNEL(chan, name)					\
	const channel_set_type::const_iterator				\
		f(channel_index_set.find(name));			\
	if (f == channel_index_set.end()) {				\
		cerr << "Error, channel `" << name <<			\
			"\' not yet registered." << endl;		\
		return true;						\
	}								\
	channel& chan(channel_pool[f->second]);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre channel has already been created by new_channel()
	\return true on error.
 */
bool
channel_manager::set_channel_ack_valid(State& state, const string& base, 
		const bool ack_sense, const bool ack_init, 
		const bool have_validity, const bool validity_sense) {
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
	c.ack_signal = ai;
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
	c.valid_signal = vi;
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
	GET_NAMED_CHANNEL(c, channel_name)
	if (c.set_source(s, file_name, loop)) return true;
	// warn if channel happens to be connected in wrong direction
	// TODO: check that data/validity are not driven by other sources!
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
	GET_NAMED_CHANNEL(c, channel_name)
	if (c.set_sink(s)) return true; // does many checks

	// check if signal is registered with other sinking channels?
	node_channels_map_type::const_iterator
		m(node_channels_map.find(f->second));
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
	GET_NAMED_CHANNEL(c, channel_name)
	return c.set_log(file_name);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::expect_channel(const string& channel_name, 
		const string& file_name, const bool loop) {
	GET_NAMED_CHANNEL(c, channel_name)
	return c.set_expect(file_name, loop);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::close_channel(const string& channel_name) {
	GET_NAMED_CHANNEL(c, channel_name)
	c.close_stream();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_manager::close_all_channels(void) {
	for_each(channel_pool.begin(), channel_pool.end(), 
		mem_fun_ref(&channel::close_stream));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::stop_channel(const string& channel_name) {
	GET_NAMED_CHANNEL(c, channel_name)
	c.stop();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_manager::stop_all_channels(void) {
	for_each(channel_pool.begin(), channel_pool.end(), 
		mem_fun_ref(&channel::stop));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::resume_channel(const string& channel_name) {
	GET_NAMED_CHANNEL(c, channel_name)
	c.stop();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_manager::resume_all_channels(void) {
	for_each(channel_pool.begin(), channel_pool.end(), 
		mem_fun_ref(&channel::resume));
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
	o << i->first << " : ";		// channel name
	const channel& c(channel_pool[i->second]);
	c.dump(o) << endl;
	if (state) {
		c.dump_state(o << '\t') << endl;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true upon error
 */
bool
channel_manager::save_checkpoint(ostream& o) const {
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
#undef	GET_NAMED_CHANNEL
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

