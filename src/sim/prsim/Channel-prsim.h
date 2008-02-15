/**
	\file "sim/prsim/Channel-prsim.h"
	Structure for managing envinronment-managed channels.  
	Primitive channel support, inspired by reference-prsim.
	TODO: piggy-back this on top of channel types from language.
	Define a channel type map to make automatic!
	auto-channel (based on consumer/producer connectivity), 
	top-level only!
	$Id: Channel-prsim.h,v 1.1.2.1 2008/02/15 02:22:28 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_CHANNEL_H__
#define	__HAC_SIM_PRSIM_CHANNEL_H__

#include <iosfwd>
#include <string>
#include <map>
#include <set>
#include <vector>
#include "sim/common.h"
#include "Object/expr/types.h"
#include "util/utypes.h"
#include "util/macros.h"
#include "util/memory/count_ptr.h"

/**
	Define to 1 to add support for channel validity, for example
	with 'ev' protocols.  
 */
#define	PRSIM_CHANNEL_VALIDITY			0

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::vector;
using std::string;
using std::ostream;
using std::fstream;
using util::memory::count_ptr;
class State;
class channel;
class channel_manager;

typedef	node_index_type				channel_index_type;

//=============================================================================
/**
	TODO: format flags!
 */
struct channel_file_handle {
	string					fname;
	count_ptr<fstream>			stream;
	enum __flags {
		/// if true, re-open file at EOF
		CHANNEL_FILE_LOOP = 		0x01,
		CHANNEL_FILE_READ = 		0x02,
		CHANNEL_FILE_WRITE = 		0x04
	};
#if 0
	// position in file
	// no longer need if entire file is read into array at once
	size_t					linenum;
#endif
	// flags
	uchar					flags;

	channel_file_handle();
	~channel_file_handle();

};	// end struct channel_file_handle

//=============================================================================
/**
	Crude channel type structure, for grouping signals.
	e.g. eMx1ofN
	TODO: eventually add a configurable delay for environment 
		response time.  
 */
class channel {
	friend class channel_manager;
#if 0
	/**
		Data encoding: Mx1ofN, M is _size, N is radix.
		Hetergenous channels not yet supported.  
	 */
	uchar					_size;
	/**
		The base of the data rails.  
	 */
	uchar					radix;
#endif

	enum channel_flags {
		// the value of channel enable on reset
		CHANNEL_ACK_RESET_VALUE =	0x01,
		// true for active high (a), false for active low (e)
		CHANNEL_ACK_ACTIVE_SENSE =	0x02
#if PRSIM_CHANNEL_VALIDITY
		// true for active high (a), false for active low (e)
		, CHANNEL_VALID_ACTIVE_SENSE =	0x04
#endif
	};

	ushort					flags;

	/// node index for acknowledge/enable
	node_index_type				ack_signal;
#if PRSIM_CHANNEL_VALIDITY
	/// node index for validity/neutrality (some types)
	node_index_type				valid_signal;
#endif

	// is valarray copy-safe? who cares for now...
	typedef	vector<node_index_type>		rails_array_type;
	/**
		Size of this array is the number of bundles (rail sets). 
		Size of each element is the radix of the data rails.
		TODO: use packed_array<T, 2>!!
	 */
	vector<rails_array_type>		data;
	/**
		counter for the number of high rails among the
		data rail bundles.
		When this counter hits 0 or M (#bundles), 
		an action may be triggered.  
		invariant: only one rail within each bundle may be high
		at a time.
		invariant: [return-to-zero] counter must monotonically
		increase/decrease between 0 and M, the nmuber of bundles.  
	 */
	size_t					counter_state;
	/**
		Actions associated between channels and files.
		Q: can some of these be overloaded?  
		e.g. don't need to inject and dump/expect.
		Also once loaded into values cache, no need to keep file open.
		This easily avoids file-system limits for large numbers
		of channels.  
	 */
	channel_file_handle			inject_expect;
	channel_file_handle			dumplog;
	vector<entity::int_value_type>		values;
#if 1
private:
	// optional: reverse lookup map: node -> bundle, rail
	typedef	std::map<node_index_type, std::pair<size_t, size_t> >
						data_rail_map_type;
	data_rail_map_type			__node_to_rail;
#endif
public:
	channel();
	~channel();

	size_t
	bundles(void) const { return data.size(); }

	size_t
	radix(void) const {
		INVARIANT(data.size());
		return data.front().size();
	}

	void
	set_ack_active(const bool h) {
		if (h)	flags |= CHANNEL_ACK_ACTIVE_SENSE;
		else	flags &= ~CHANNEL_ACK_ACTIVE_SENSE;
	}

	bool
	get_ack_active(void) const {
		return flags & CHANNEL_ACK_ACTIVE_SENSE;
	}

	void
	set_ack_init(const bool v) {
		if (v)	flags |= CHANNEL_ACK_RESET_VALUE;
		else	flags &= ~CHANNEL_ACK_RESET_VALUE;
	}

	bool
	get_ack_init(void) const {
		return flags & CHANNEL_ACK_RESET_VALUE;
	}

	void
	process_node(const node_index_type, const uchar val);

private:
	void
	update_rail_map(void);
	// lookup node_index to bundle, rail

};	// end class channel

//=============================================================================
// wrap these into a channel_manager?

class channel_manager {
	/**
		Pool from which sim channels spawn.
		Not bothering with any sort of free-list yet.
	 */
	typedef	std::vector<channel>		channel_pool_type;

	/**
		Map type for associating channels with names.
	 */
	typedef	std::map<string, channel_index_type>
						channel_set_type;

	/**
		List of all channels that a node may be associated with.
		Every channel that is registered should maintain this map.  
		Using set structure for uniqueness.  
		invariant: should be consistent with 
			Node::flags::NODE_IN_CHANNEL.
	 */
	typedef	std::map<node_index_type, std::set<channel_index_type> >
						node_channels_map_type;

	channel_pool_type			channel_pool;
	channel_set_type			channel_index_set;
	node_channels_map_type			node_channels_map;
public:
	channel_manager();
	~channel_manager();

	bool
	new_channel(const State&, const string&, 
		const string& bn, const size_t b,
		const string& rn, const size_t r);

	bool
	set_channel_ack_valid(const bool, const bool, const bool, const bool);

};	// end class channel_manager

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_CHANNEL_H__

