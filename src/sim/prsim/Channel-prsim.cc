/**
	\file "sim/prsim/Channel-prsim.cc"
	$Id: Channel-prsim.cc,v 1.1.2.1 2008/02/15 02:22:26 fang Exp $
 */

#include <iostream>
#include <functional>
#include <algorithm>
#include "sim/prsim/Channel-prsim.h"
#include "parser/instref.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using parser::parse_node_to_index;
using std::for_each;
using std::pair;
using std::make_pair;
using std::mem_fun_ref;
#include "util/using_ostream.h"

//=============================================================================
// class channel method definitions

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
	\return true on error.
 */
bool
channel_manager::new_channel(const State& state, const string& base, 
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
		bind2nd(mem_fun_ref(
			&channel::rails_array_type::resize), 
			num_rails));
	// lookup and assign node-indices
#if 0
	// FINISH_ME
	size_t j = 0;
	for ( ; j<num_bundles; ++j) {
		string bundle_segment;
		if (bundle_name.length()) {
			bundle_segment += ".";
			bundle_segment += bundle_name;
		}
		size_t k = 0;
		for ( ; k<num_rails; ++k) {
			
		}
	}
	return false;
#endif
} else {
	return true;
}
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

