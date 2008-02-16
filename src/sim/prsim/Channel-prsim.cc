/**
	\file "sim/prsim/Channel-prsim.cc"
	$Id: Channel-prsim.cc,v 1.1.2.2 2008/02/16 02:29:50 fang Exp $
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
#include "util/IO_utils.tcc"

namespace HAC {
namespace SIM {
namespace PRSIM {
using parser::parse_node_to_index;
using std::ios_base;
using std::ifstream;
using std::ofstream;
using std::ostringstream;
using std::for_each;
using std::pair;
using std::make_pair;
using std::mem_fun_ref;
using std::ostream_iterator;
#include "util/using_ostream.h"
using util::read_value;
using util::write_value;

//=============================================================================
// class channel_file_handle method definitions

channel_file_handle::channel_file_handle() :
		fname(), 
		stream(NULL),
		flags(0) {
}

channel_file_handle::~channel_file_handle() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_file_handle::save_checkpoint(ostream& o) const {
	write_value(o, flags);
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
	read_value(i, flags);
	bool v;
	read_value(i, v);
	if (v) {
		read_value(i, fname);
		if (flags & CHANNEL_FILE_READ) {
			stream = count_ptr<std::ifstream>(
				new std::ifstream(fname.c_str()));
			if (!stream || !*stream) {
				cerr << "Error re-opening file `" <<
					fname << "\' for reading." << endl;
				return true;
			}
		} else if (flags & CHANNEL_FILE_WRITE) {
			stream = count_ptr<std::ofstream>(
				new std::ofstream(fname.c_str(), 
					ios_base::app));
			if (!stream || !*stream) {
				cerr << "Error re-opening file `" <<
					fname << "\' for writing." << endl;
				return true;
			}
		}
	}
	return false;
}

//=============================================================================
// class channel method definitions

channel::channel() :
		ack_signal(INVALID_NODE_INDEX), 
#if PRSIM_CHANNEL_VALIDITY
		valid_signal(INVALID_NODE_INDEX), 
#endif
		flags(CHANNEL_DEFAULT_FLAGS), 
		counter_state(0), 
		data(), 
		inject_expect(), 
		dumplog(), 
		values() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel::~channel() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note all subnodes of this channel.
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
			ostream_iterator<entity::pint_value_type>(o, ","));
		o << '}';
		if (is_looping()) o << '*';
		o << " < " << inject_expect.fname;
	}
	if (dumplog.stream) {
		o << " > " << dumplog.fname;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel::dump_state(ostream& o) const {
	o << "count: " << counter_state;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel::save_checkpoint(ostream& o) const {
	write_value(o, ack_signal);
#if PRSIM_CHANNEL_VALIDITY
	write_value(o, valid_signal);
#endif
	write_value(o, flags);
	write_value(o, counter_state);
	write_value(o, bundles());	// size_t
	write_value(o, radix());	// size_t
	size_t j = 0;
	for ( ; j<bundles(); ++j) {
	size_t k = 0;
	for ( ; k<radix(); ++k) {
		write_value(o, data[j][k]);
	}
	}
	if (inject_expect.save_checkpoint(o)) return true;
	if (dumplog.save_checkpoint(o)) return true;
	util::write_sequence(o, values);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel::load_checkpoint(istream& i) {
	read_value(i, ack_signal);
#if PRSIM_CHANNEL_VALIDITY
	read_avlue(i, valid_signal);
#endif
	read_value(i, flags);
	read_value(i, counter_state);
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
	if (inject_expect.load_checkpoint(i)) return true;
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
/**
	\pre channel has already been created by new_channel()
	\return true on error.
 */
bool
channel_manager::set_channel_ack_valid(State& state, const string& base, 
		const bool ack_sense, const bool ack_init, 
		const bool have_validity, const bool validity_sense) {
	const channel_set_type::const_iterator f(channel_index_set.find(base));
//	INVARIANT(f != channel_index_set.end());
	if (f == channel_index_set.end()) {
		cerr << "Error, channel `" << base << "\' not yet registered."
			<< endl;
		return true;
	}
	const entity::module& m(state.get_module());
	channel& c(channel_pool[f->second]);
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
	vector<string> name_map(channel_index_set.size());
{
	channel_set_type::const_iterator
		i(channel_index_set.begin()), e(channel_index_set.end());
	for ( ; i!=e; ++i) {
		name_map[i->second] = i->first;
	}
}
	write_value(o, name_map.size());
	channel_pool_type::const_iterator
		i(channel_pool.begin()), e(channel_pool.end());
	vector<string>::const_iterator s(name_map.begin());
	for ( ; i!=e; ++i, ++s) {
		write_value(o, *s);
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
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

