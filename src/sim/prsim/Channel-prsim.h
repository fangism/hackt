/**
	\file "sim/prsim/Channel-prsim.h"
	Structure for managing envinronment-managed channels.  
	Primitive channel support, inspired by reference-prsim.
	TODO: piggy-back this on top of channel types from language.
	Define a channel type map to make automatic!
	auto-channel (based on consumer/producer connectivity), 
	top-level only!
	$Id: Channel-prsim.h,v 1.1.2.3 2008/02/17 02:20:40 fang Exp $
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
using std::istream;
using std::ofstream;
using entity::int_value_type;
using util::memory::count_ptr;
class State;
class channel;
class channel_manager;

typedef	node_index_type				channel_index_type;

//=============================================================================
/**
	Only used for output logging.
	Never used for inputs.  
	TODO: format flags!
	Q: is there a way to query the open mode of an ios (stream)?
 */
class channel_file_handle {
	friend class channel;
	string					fname;
	count_ptr<ofstream>			stream;
#if 0
	enum __flags {
		CHANNEL_FILE_READ = 		0x01,
		CHANNEL_FILE_WRITE = 		0x02,
		CHANNEL_FILE_APPEND = 		0x04,
		/// if true, re-open file at EOF (unused)
		CHANNEL_FILE_LOOP = 		0x08
	};
	// flags
	uchar					flags;
#endif
public:
	channel_file_handle();
	~channel_file_handle();
#if 0

	bool
	is_reading(void) const {
		return flags & CHANNEL_FILE_READ;
	}

	bool
	is_writing(void) const {
		return flags & CHANNEL_FILE_WRITE;
	}

	bool
	open_read(const string&);
#endif

	// append?
	bool
	open_write(const string&);

	void
	close(void);

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

};	// end struct channel_file_handle

//=============================================================================
/**
	Crude channel type structure, for grouping signals.
	e.g. eMx1ofN
	A channel can be configured as a source or sink.  
	TODO: eventually add a configurable delay for environment 
		response time.  
	This assumes that all data rails are active high.
	TODO: consider timestamping logs to be able to reconstruct total order
 */
class channel {
	friend class channel_manager;

	enum channel_flags {
		/// the value of channel enable on reset
		CHANNEL_ACK_RESET_VALUE =	0x0001,
		/// true for active high (a), false for active low (e)
		CHANNEL_ACK_ACTIVE_SENSE =	0x0002,
#if PRSIM_CHANNEL_VALIDITY
		/// true for active high (v), false for active low (n)
		CHANNEL_VALID_ACTIVE_SENSE =	0x0004,
#endif
		/// derived mask
		CHANNEL_TYPE_FLAGS =
			CHANNEL_ACK_RESET_VALUE
			| CHANNEL_ACK_ACTIVE_SENSE
#if PRSIM_CHANNEL_VALIDITY
			| CHANNEL_VALID_RESET_VALUE
#endif
			,
		/**
			true if values are interpreted as source values.
			should be exclusive with CHANNEL_SINKING.  
		 */
		CHANNEL_SOURCING = 		0x0010,
		/**
			true if source values should repeat infinitely.
			prerequisite: CHANNEL_SOURCING | CHANNEL_EXPECTING.
		 */
		CHANNEL_VALUE_LOOP =	 	0x0020,
		/**
			true if channel is consuming tokens.
			should be mutually exclusive with CHANNEL_SOURCING.
		 */
		CHANNEL_SINKING = 		0x0040,
		/**
			true if consumed values are interpreted as
			values to expect and assert.  
			NOTE: this is independent of sinking!
		 */
		CHANNEL_EXPECTING = 		0x0080,
		/**
			Pause environment activity on a channel.  
		 */
		CHANNEL_STOPPED =		0x0100,
		/// default initial value
		CHANNEL_DEFAULT_FLAGS = 	0x0000
	};
	/// name of channel (redundant)
	string					name;
	/// node index for acknowledge/enable
	node_index_type				ack_signal;
#if PRSIM_CHANNEL_VALIDITY
	/// node index for validity/neutrality (some types)
	node_index_type				valid_signal;
#endif

	// is valarray copy-safe? who cares for now...
	typedef	vector<node_index_type>		rails_array_type;
	/**
		General attribute and mode flags.  
	 */
	ushort					flags;
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
	uchar					counter_state;
	/**
		Counts the number of X data rails.  
	 */
	uchar					x_counter;
	/**
		Size of this array is the number of bundles (rail sets). 
		Size of each element is the radix of the data rails.
		TODO: use packed_array<T, 2>!!
	 */
	vector<rails_array_type>		data;
	/**
		Inject/expect files don't need to persist
		once they are loaded into values.  
		In non-loop mode, once values are exhausted, 
		this should be cleared, along with flags.  
		This way a checkpoint restore won't re-open them.  
		Could also use value_index as a check.  
	 */
	string					inject_expect_file;
	/**
		Actions associated between channels and files.
		Q: can some of these be overloaded?  
		e.g. don't need to inject and dump/expect.
		Also once loaded into values cache, no need to keep file open.
		This easily avoids file-system limits for large numbers
		of channels.  
	 */
	channel_file_handle			dumplog;
	/**
		The values to expect or inject.  
	 */
	vector<int_value_type>			values;
	/**
		Position in values list.
	 */
	size_t					value_index;
private:
	// optional: reverse lookup map: node -> bundle, rail
	typedef	std::map<node_index_type, std::pair<size_t, size_t> >
						data_rail_map_type;
	data_rail_map_type			__node_to_rail;

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

private:
	void
	set_ack_active(const bool h) {
		if (h)	flags |= CHANNEL_ACK_ACTIVE_SENSE;
		else	flags &= ~CHANNEL_ACK_ACTIVE_SENSE;
	}

public:
	bool
	get_ack_active(void) const {
		return flags & CHANNEL_ACK_ACTIVE_SENSE;
	}

private:
	void
	set_ack_init(const bool v) {
		if (v)	flags |= CHANNEL_ACK_RESET_VALUE;
		else	flags &= ~CHANNEL_ACK_RESET_VALUE;
	}

public:
	bool
	get_ack_init(void) const {
		return flags & CHANNEL_ACK_RESET_VALUE;
	}

#if PRSIM_CHANNEL_VALIDITY
private:
	void
	set_valid_sense(const bool v) {
		if (v)	flags |= CHANNEL_VALID_ACTIVE_SENSE;
		else	flags &= ~CHANNEL_VALID_ACTIVE_SENSE;
	}

public:
	bool
	get_valid_sense(void) const {
		return flags & CHANNEL_VALID_ACTIVE_SENSE;
	}
#endif

	bool
	is_sourcing(void) const {
		return flags & CHANNEL_SOURCING;
	}

	bool
	is_sinking(void) const {
		return flags & CHANNEL_SINKING;
	}

	bool
	is_expecting(void) const {
		return flags & CHANNEL_EXPECTING;
	}

	bool
	is_looping(void) const {
		return (is_sourcing() || is_expecting()) &&
			(flags & CHANNEL_VALUE_LOOP);
	}

	bool
	stopped(void) const { return flags & CHANNEL_STOPPED; }

	void
	stop(void) { flags |= CHANNEL_STOPPED; }

	void
	resume(void) { flags &= ~CHANNEL_STOPPED; }

	bool
	set_source(const State&, const string&, const bool);

	bool
	set_sink(const State&);

	bool
	set_log(const string&);

	bool
	set_expect(const string&, const bool);

	void
	close_stream(void);

	void
	initialize_data_counter(const State&);

	void
	process_node(const node_index_type, const uchar val);

#if 0
private:
	void
	update_rail_map(void);
	// lookup node_index to bundle, rail
#endif

	ostream&
	dump(ostream&) const;

	ostream&
	dump_state(ostream&) const;

private:
	void
	get_all_nodes(vector<node_index_type>&) const;

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

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
	new_channel(State&, const string&, 
		const string& bn, const size_t b,
		const string& rn, const size_t r);

	bool
	set_channel_ack_valid(State&, const string&, 
		const bool, const bool, const bool, const bool);

	ostream&
	__dump(ostream&, const bool) const;

	ostream&
	dump(ostream& o, const State&) const {
		return __dump(o, false);
	}

	bool
	source_channel(const State&, const string&, const string&, const bool);

	bool
	sink_channel(const State&, const string&);

	bool
	expect_channel(const string&, const string&, const bool);

	bool
	log_channel(const string&, const string&);

	bool
	close_channel(const string&);

	void
	close_all_channels(void);

	bool
	stop_channel(const string&);
	
	void
	stop_all_channels(void);
	
	bool
	resume_channel(const string&);

	void
	resume_all_channels(void);

	ostream&
	dump_memory_usage(ostream&) const;

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

	ostream&
	dump_checkpoint_state(ostream& o) const {
		return __dump(o, true);
	}

};	// end class channel_manager

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_CHANNEL_H__

