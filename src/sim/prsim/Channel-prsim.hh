/**
	\file "sim/prsim/Channel-prsim.hh"
	Structure for managing envinronment-managed channels.  
	Primitive channel support, inspired by reference-prsim.
	TODO: piggy-back this on top of channel types from language.
	Define a channel type map to make automatic!
	auto-channel (based on consumer/producer connectivity), 
	top-level only!
	$Id: Channel-prsim.hh,v 1.29 2011/03/23 18:47:35 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_CHANNEL_H__
#define	__HAC_SIM_PRSIM_CHANNEL_H__

#include <iosfwd>
#include <string>
#include <map>
#include <set>
#include <vector>
#include "sim/common.hh"
#include "Object/expr/types.hh"
#include "sim/time.hh"
#include "sim/prsim/Exception.hh"
#include "sim/prsim/enums.hh"
#include "util/utypes.h"
#include "util/macros.h"
#include "util/memory/count_ptr.hh"
#include "util/packed_array.hh"		// for value array
#include "util/numeric/sign_traits.hh"
#include "util/tokenize_fwd.hh"		// for util::string_list

// old perm'd switches
// #define	PRSIM_CHANNEL_VALIDITY			1
// #define	PRSIM_CHANNEL_DONT_CARES		1

/**
	Define to 1 to allow interaction with dataless channels, 
	useful for watching, logging, and expecting, just not
	for sourcing and sinking.  
	Status: done, tested.
 */
// #define	PRSIM_ACKLESS_CHANNELS			1

/**
	Define to 1 to support an option to interpret data-rails
	as active-low in their inverted sense.  
	Status: done, tested, can perm.
 */
#define	PRSIM_CHANNEL_RAILS_INVERTED		1

/**
	TODO: enable ability to 'set' channel values or to 
	unset them to 'null'.
 */
#define	PRSIM_CHANNEL_SET			0

/**
	Define to 1 to enable channel-timing features.  
	This enables channels to use separate timing policies
	from the global policy.  
	A single delay value will be associated per channel, 
	so a source/sink on the same channel will use the same value.
	Status: done, can perm this
 */
#define	PRSIM_CHANNEL_TIMING			1

/**
	An option to treat a vector of nodes as a bus.
	Useful for synchronous bus interfaces.  
	This would be handy for bundled-data channels.
 */
#define	PRSIM_CHANNEL_VECTORS			0

/**
	Define to 1 to support different channel types.
	The first new channel type we support is LEDR.
	Goal: 1
	Status: done for LEDR channels, just not bundled, perm'd.
#define	PRSIM_CHANNEL_LEDR			1
 */
#define	PRSIM_CHANNEL_LEDR_BUS			0

/**
	Define to 1 to support single-track 1ofN channel types.
	Goal: 1
	Status: in one's imagination
 */
#define	PRSIM_CHANNEL_SINGLE_TRACK		0

/**
	Define to 1 to support bundled data channel types.
	There are both 4-phase and 2-phase bundled data types.
	Goal: 1
 */
#define	PRSIM_CHANNEL_BUNDLED_DATA		1

/**
	Define to 1 to interpret channel values as signed.
	Each channel can be independently configured as signed or unsigned.
	Goal: 1
	Status: done, somewhat tested
 */
#define	PRSIM_CHANNEL_SIGNED			1

/**
	Define to 1 to allow commands to take array references
	and automatically apply the command to the expanded list.  
	Goal: 1
	Status: done, basically tested.  can probably perm soon.
 */
#define	PRSIM_CHANNEL_AGGREGATE_ARGUMENTS	1

/**
	Define to 1 to enable clocked, synchronous channels.
	Goal: 1
 */
#define	PRSIM_CHANNEL_SYNC			1

namespace util {
class ifstream_manager;
}
namespace HAC {
namespace entity {
class module;
}
namespace SIM {
namespace PRSIM {
using std::vector;
using std::string;
using std::ostream;
using std::istream;
using std::ofstream;
using entity::int_value_type;
using entity::preal_value_type;
using entity::module;
using util::memory::count_ptr;
using util::packed_array;
using util::string_list;
using util::ifstream_manager;
class State;
class channel;
class channel_manager;

typedef	node_index_type				channel_index_type;
#if	PRSIM_CHANNEL_TIMING
typedef	real_time				channel_time_type;

enum channel_timing_enum {
	CHANNEL_TIMING_GLOBAL,	///< use the global timing policy
	CHANNEL_TIMING_AFTER,	///< use fixed delay timing
	CHANNEL_TIMING_RANDOM,	///< use random delay (exponential variate)
	CHANNEL_TIMING_BINARY,	///< use coin-flip delay
	CHANNEL_TIMING_DEFAULT = CHANNEL_TIMING_GLOBAL
};
#endif

/**
	tuple: node_index, next value, delay
	Events generated by the environment, channel sources and sinks.  
	The simulator state will convert these to events
	in the primary event queue.  
 */
#if PRSIM_CHANNEL_TIMING
struct env_event_type {
	node_index_type			node_index;
	value_enum			value;
	/// whether or not to use this supplied delay
	bool				use_global;
	channel_time_type		delay;

#if 0
	env_event_type(const node_index_type ni, const value_enum v) :
		node_index(ni), value(v), use_global(true) { }

	env_event_type(const node_index_type ni, const value_enum v, 
		const channel_time_type& d) :
		node_index(ni), value(v), use_global(false), delay(d) { }
#endif
	env_event_type(const node_index_type ni, const value_enum v, 
		const channel& c);
	env_event_type(const node_index_type ni, const value_enum v, 
		const channel_time_type d);

	// for sorting based on delay
	bool
	operator < (const env_event_type& r) const {
		return delay < r.delay;
	}

};	// end struct env_event_type
#else
typedef	std::pair<node_index_type, value_enum>	env_event_type;
#endif

/**
	When channel value mismatches expectation.
	Need local copy of index and iteration because, the channel
	may have already advanced by the time this exception is 
	thrown and caught (missed snapshot).
 */
struct channel_exception : public step_exception {
	const channel*			chan;
	size_t				index;
	size_t				iteration;
	int_value_type			expect;
	int_value_type			got;
	bool				got_x;

	channel_exception(
		const channel* c,
		const size_t in, const size_t it,
		const int_value_type e, const int_value_type g, 
		const bool gx) :
		chan(c), 
		index(in), iteration(it), 
		expect(e), got(g), got_x(gx) { }

	error_policy_enum
	inspect(const State&, ostream&) const;

};	// end struct channel_exception

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
	/**
		Need copy-constructible pointer type.  
	 */
	count_ptr<ofstream>			stream;
public:
	channel_file_handle();
	~channel_file_handle();

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
#if PRSIM_CHANNEL_TIMING
	friend struct env_event_type;
#endif
public:
	/**
		Treat all integers as *unsigned*, even if internal 
		integer type is signed.  This makes it easier
		to translate values to rails without worrying about negatives.  
	 */
	typedef	util::numeric::unsigned_type<int_value_type>::type
						unsigned_value_type;
	typedef	util::numeric::signed_type<int_value_type>::type
						signed_value_type;
	typedef	unsigned_value_type			value_type;
	/**
		Values kept by the array.  
		Pair implementation to support don't cares.  
		Second member bool is true means don't care for expects.  
		Signage doesn't matter for storing values, cast-OK.  
		Define to struct to be able to overload ostream& operator <<
	 */
#if 0
	struct array_value_type : public std::pair<value_type, bool> { };
#else
	typedef std::pair<value_type, bool>		array_value_type;
#endif
	/**
		Utility data structure for set of unique node indices. 
	 */
	typedef	std::set<node_index_type>	node_set_type;

	// global policies
	/// if true, print watched and logged nodes with timestamps
	static bool					report_time;
private:
	enum channel_types {
		CHANNEL_TYPE_1ofN,
		CHANNEL_TYPE_LEDR,	// level-encoded dual-rail
#if PRSIM_CHANNEL_BUNDLED_DATA
		CHANNEL_TYPE_BD_4P,	// bundled-data 4-phase
		CHANNEL_TYPE_BD_2P,	// bundled-data 2-phase
#endif
#if PRSIM_CHANNEL_SYNC
		CHANNEL_TYPE_CLK_SRC1,	// clock generator, source
		CHANNEL_TYPE_CLK_SRC2,	// clock generator, source (double-edge)
		CHANNEL_TYPE_CLK_1EDGE,	// single-edge (posedge or negedge)
		CHANNEL_TYPE_CLK_2EDGE,	// double-edge (anyedge)
#endif
		CHANNEL_TYPE_SINGLE_TRACK,
		CHANNEL_TYPE_NULL
	};
	enum channel_flags {
		/// the value of channel enable on reset
		CHANNEL_ACK_RESET_VALUE =	0x0001,
		/// true for active high (a), false for active low (e)
		CHANNEL_ACK_ACTIVE_SENSE =	0x0002,
		/// true for active high (v), false for active low (n)
		CHANNEL_VALID_ACTIVE_SENSE =	0x0004,
#if PRSIM_CHANNEL_RAILS_INVERTED
		/// true if data rails are inverted, i.e. active low
		CHANNEL_DATA_ACTIVE_SENSE =	0x0008,
#endif
		/// derived mask
		CHANNEL_TYPE_FLAGS =
			CHANNEL_ACK_RESET_VALUE
			| CHANNEL_ACK_ACTIVE_SENSE
			| CHANNEL_VALID_ACTIVE_SENSE
#if PRSIM_CHANNEL_RAILS_INVERTED
			| CHANNEL_DATA_ACTIVE_SENSE
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
		/**
			Set to true to ignore channel values for
			logging, and expecting.  (But watching not masked.)
		 */
		CHANNEL_IGNORED =		0x0200,
		/**
			Random values, infinite sequence, 
			override value sequence.
			Only useful for sources really.
		 */
		CHANNEL_RANDOM = 		0x0400,
		/**
			If true, print all valid channel values
			in the data-valid state.  
		 */
		CHANNEL_WATCHED =		0x0800,
		/**
			If true, and channel is both sinking and
			expecting values, then stop sinking as soon
			as values are exhausted.
		 */
		CHANNEL_STOP_ON_EMPTY = 	0x1000,
#if PRSIM_CHANNEL_SIGNED
		/**
			Set to true to interpret and report channel
			values as signed.
			By default, values are unsigned.
		 */
		CHANNEL_SIGNED =		0x2000,
#endif
		/// default initial value
		CHANNEL_DEFAULT_FLAGS = 	0x0000
	};
	/// name of channel (redundant)
	string					name;
	/// node index for acknowledge/enable
	node_index_type				ack_signal;
	/// node index for validity/neutrality (some types), also used as clk
	node_index_type				valid_signal;

#if PRSIM_CHANNEL_TIMING
	/**
		The channel timing policy.
	 */
	channel_timing_enum			timing_mode;
	/**
		Delay to used with after delay mode (fixed).
	 */
	channel_time_type			after_min;
	channel_time_type			after_max;
	/**
		For timing binary only, this determines the probability
		of choosing the min value.
	 */
	preal_value_type			timing_probability;
#endif
	/**
		Channel encoding and protocol type.
	 */
	ushort					type;
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
		Limit: 255
		When the data is active-low (inverted), this counter
		counts the number of *active* rails.  
	 */
	uchar					counter_state;
	/**
		Assuming that completetion detection is contingent 
		upon there being no unknowns, we can count unknowns
		to model unknown behavior.  
		Counts the number of X data rails.  (Limit: 255)
	 */
	uchar					x_counter;
	/**
		Size of this array is the number of bundles (rail sets). 
		Size of each element is the radix of the data rails.
		2 is for 2D array.
	 */
	typedef	util::packed_array<2, size_t, node_index_type>
						data_bundle_array_type;
	typedef	data_bundle_array_type::key_type
						data_rail_index_type;
	data_bundle_array_type			data;
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
	vector<array_value_type>		values;
	/**
		Position in values list, meaninful to source
		and expect on channels.
	 */
	size_t					value_index;
	/**
		For loops, this is the number of previous iterations
		completed, or the 0-indexed ordinal number of repeat. 
	 */
	size_t					iteration;
private:
	// optional: reverse lookup map: node -> bundle, rail
	typedef	std::map<node_index_type, data_rail_index_type>
						data_rail_map_type;
	data_rail_map_type			__node_to_rail;

public:
	channel();
	~channel();

	// should be ok for LEDR as well
	size_t
	bundles(void) const { return data.size()[0]; }

	// should be ok for LEDR as well
	// radix == 0 means data-less
	size_t
	radix(void) const { return data.size()[1]; }

	bool
	have_value(void) const { return value_index < values.size(); }

	const string&
	get_name(void) const { return name; }

	const size_t&
	current_iteration(void) const { return iteration; }

	const size_t&
	current_index(void) const { return value_index; }

	const array_value_type&
	current_value(void) const { return values[value_index]; }

	value_type
	max_value(void) const;

#if PRSIM_CHANNEL_SIGNED
	signed_value_type
	min_value(void) const;
#endif

private:
	bool
	alias_data_rails(const node_index_type) const;

#if 0
	// unused
	void
	current_data_rails(vector<node_index_type>&) const;
#endif

	void
	advance_value(void);

#if 0
	// not worth it
	void
	set_current_data_rails(vector<env_event_type>&, const uchar);
#endif

	void
	X_all_data_rails(vector<env_event_type>&);

	void
	reset_all_data_rails(vector<env_event_type>&);

	void
	set_all_data_rails(const State&, vector<env_event_type>&);

#if PRSIM_CHANNEL_SYNC
	void
	toggle_clock(const State&, vector<env_event_type>&);

	void
	set_clock_signal(const node_index_type vi) {
		valid_signal = vi;
	}
#endif

#if PRSIM_CHANNEL_BUNDLED_DATA
	void
	reset_bundled_data_rails(vector<env_event_type>&);

	void
	update_bd_data_counter(const value_enum, const value_enum);

	void
	update_data_counter(const value_enum p, const value_enum n) {
		update_bd_data_counter(p, n);
	}
#endif

	void
	initialize_all_data_rails(vector<env_event_type>&);

	bool
	set_ack_signal(const node_index_type ai) {
		ack_signal = ai;
		return alias_data_rails(ai);
	}

	bool
	set_valid_signal(const node_index_type vi) {
		valid_signal = vi;
		return alias_data_rails(vi);
	}

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

private:
	void
	set_valid_sense(const bool v) {
		if (v)	flags |= CHANNEL_VALID_ACTIVE_SENSE;
		else	flags &= ~CHANNEL_VALID_ACTIVE_SENSE;
	}

#if PRSIM_CHANNEL_SYNC
	// overloading
	void
	set_clock_sense(const bool v) {
		set_valid_sense(v);
	}

	void
	set_clk_init(const bool v) {
		set_valid_sense(v);
	}
#endif

public:
	bool
	get_valid_sense(void) const {
		return flags & CHANNEL_VALID_ACTIVE_SENSE;
	}

#if PRSIM_CHANNEL_SYNC
	bool
	get_clock_sense(void) const {
		return get_valid_sense();
	}

	bool
	get_clk_init(void) const {
		return get_valid_sense();
	}
#endif

	/// \return true if data is active-low
	bool
	get_data_sense(void) const {
#if PRSIM_CHANNEL_RAILS_INVERTED
		return flags & CHANNEL_DATA_ACTIVE_SENSE;
#else
		return false;
#endif
	}

#if PRSIM_CHANNEL_RAILS_INVERTED
	void
	set_data_sense(const bool al) {
		if (al)	{ flags |= CHANNEL_DATA_ACTIVE_SENSE; }
		else	{ flags &= ~CHANNEL_DATA_ACTIVE_SENSE; }
	}
#endif

	// TODO: once wider LEDR channels are supported, 
	// cannot use a single repeat rail any more, 
	// need one per bit.  
	const node_index_type&
	repeat_rail(void) const { return valid_signal; }

	// ALERT: ack_active is overloaded as reset/initial_value
	void
	set_data_init(const bool a) {
		set_ack_active(a);
	}

	bool
	get_data_init(void) const {
		return get_ack_active();
	}

	// validity rail is overloaded to function as repeat rail
	void
	set_repeat_init(const bool a) {
		set_valid_sense(a);
	}

	bool
	get_repeat_init(void) const {
		return get_valid_sense();
	}

#if PRSIM_CHANNEL_BUNDLED_DATA
	// valid-sense is also overloaded for req-init
	void
	set_req_init(const bool r) {
		set_valid_sense(r);
	}

	bool
	get_req_init(void) const {
		return get_valid_sense();
	}
#endif
#if PRSIM_CHANNEL_SYNC
	node_index_type
	clock_signal(void) const {
		return valid_signal;
	}
#endif

	bool
	four_phase(void) const {
	switch (type) {
		case CHANNEL_TYPE_1ofN: return true;
		// CHANNEL_TYPE_LEDR
#if PRSIM_CHANNEL_BUNDLED_DATA
		case CHANNEL_TYPE_BD_4P: return true;
#endif
		// CHANNEL_TYPE_SINGLE_TRACK
		default: return false;
	}
	}

	bool
	two_phase(void) const {
		// Q: what about 1-phase? (ack-less)
		return !four_phase();
	}

private:
	// the parity specified by the initial empty state
	bool
	ledr_empty_parity(void) const {
		INVARIANT(type == CHANNEL_TYPE_LEDR);
		return get_data_init() ^ get_repeat_init() ^ get_ack_init();
	}

#if PRSIM_CHANNEL_BUNDLED_DATA
	bool
	bd2p_empty_parity(void) const {
		INVARIANT(type == CHANNEL_TYPE_BD_2P);
		return get_req_init() ^ get_ack_init();
	}
#endif

	bool
	ledr_full_parity(void) const {
		return !ledr_empty_parity();
	}

	value_enum
	current_ledr_parity(const State& s) const;

#if PRSIM_CHANNEL_BUNDLED_DATA
	value_enum
	current_bd2p_parity(const State& s) const;
#endif

	// this suffices until LEDR supports bus of rails
	node_index_type
	ledr_data_rail(void) const { return data.front(); }


public:
	bool
	is_clocked(void) const {
#if PRSIM_CHANNEL_SYNC
		return type == CHANNEL_TYPE_CLK_SRC1 ||
			type == CHANNEL_TYPE_CLK_SRC2 ||
			type == CHANNEL_TYPE_CLK_1EDGE ||
			type == CHANNEL_TYPE_CLK_2EDGE;
#else
		return false;
#endif
	}

	bool
	can_source(void) const;

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
	is_random(void) const {
		return (flags & CHANNEL_RANDOM) && is_sourcing();
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
	resume(const State&, vector<env_event_type>&);

	void
	reset(vector<env_event_type>&);

	bool
	watched(void) const { return flags & CHANNEL_WATCHED; }

	void
	watch(void) { flags |= CHANNEL_WATCHED; }

	void
	unwatch(void) { flags &= ~CHANNEL_WATCHED; }

	bool
	ignored(void) const { return flags & CHANNEL_IGNORED; }

	void
	ignore(void) { flags |= CHANNEL_IGNORED; }

	void
	heed(void) { flags &= ~CHANNEL_IGNORED; }

	void
	set_stop_on_empty(const bool t) {
		if (t)	{ flags |= CHANNEL_STOP_ON_EMPTY; }
		else	{ flags &= ~CHANNEL_STOP_ON_EMPTY; }
	}

	void
	stop_on_empty(void) {
		set_stop_on_empty(true);
	}
	void
	continue_on_empty(void) {
		set_stop_on_empty(false);
	}

	bool
	stopping_on_empty(void) const {
		return flags & CHANNEL_STOP_ON_EMPTY;
	}

#if PRSIM_CHANNEL_SIGNED
	bool
	can_be_signed(void) const {
		return
#if PRSIM_CHANNEL_BUNDLED_DATA
			(data_is_bundled() && radix()) ||
#endif
			((radix() == 2) && (bundles() > 1));
	}

	void
	set_signed(void);

	void
	set_unsigned(void) {
		flags &= ~CHANNEL_SIGNED;
	}

	bool
	is_signed(void) const {
		return flags & CHANNEL_SIGNED;
	}
#endif

private:
	bool
	__configure_source(const State&, const bool);

	void
	__configure_expect(const bool);

	bool
	read_values_from_list(const string_list&);

	bool
	read_values_from_file(const string&, ifstream_manager&);

public:
	bool
	set_source_file(const State&, const string&, const bool);

	bool
	set_source_args(const State&, const string_list&, const bool);

#if PRSIM_CHANNEL_SYNC
	bool
	set_clock_source(const State&, const int);

	bool
	is_clock_source(void) const {
		return type == CHANNEL_TYPE_CLK_SRC1 ||
			type == CHANNEL_TYPE_CLK_SRC2;
	}
#endif

	bool
	set_rsource(const State&);

	bool
	set_sink(const State&);

	bool
	set_log(const string&);

	bool
	set_expect_file(const string&, const bool, ifstream_manager&);

	bool
	set_expect_args(const string_list&, const bool);

#if PRSIM_CHANNEL_TIMING
	bool
	set_timing(const string&, const string_list&);

	static
	ostream&
	help_timing(ostream&);

	ostream&
	dump_timing(ostream&) const;
#endif

	void
	close_stream(void);

	void
	initialize_data_counter(const State&);

	void
	initialize(void);

	void
	clobber(void);		// simulation reset

	bool
	data_is_valid(void) const;

	bool
	data_is_bundled(void) const {
		return (type == CHANNEL_TYPE_BD_4P) ||
			(type == CHANNEL_TYPE_BD_2P);
	}

	value_type
	data_rails_value(const State&) const;

	// handles signed case
	ostream&
	print_data_value(ostream&, const value_type) const;

	bool
	may_drive_node(const node_index_type) const;

	bool
	reads_node(const node_index_type) const;

	void
	__get_fanins(const node_index_type, node_set_type&) const;

	ostream&
	__node_why_not(const State&, ostream&, const node_index_type, 
		const size_t, const bool d, const bool wn, const bool v, 
		node_set_type&, node_set_type&) const;

	ostream&
	__node_why_X(const State&, ostream&, const node_index_type, 
		const size_t, const bool v, 
		node_set_type&, node_set_type&) const;

private:
	ostream&
	__node_why_not_data_rails(const State&, ostream&, 
		const node_index_type, const bool a, 
		const size_t, const bool d, const bool wn, const bool v, 
		node_set_type&, node_set_type&) const;

	ostream&
	__node_why_X_data_rails(const State&, ostream&, 
		const bool a, 
		const size_t, const bool v, 
		node_set_type&, node_set_type&) const;

public:
	void
	process_node(const State&, const node_index_type, 
		const value_enum, const value_enum, 
		vector<env_event_type>&) throw(channel_exception);

private:
#if 0
	void
	update_rail_map(void);
	// lookup node_index to bundle, rail
#endif
	ostream&
	__dump_ack_valid_type(ostream&) const;

public:
	ostream&
	dump(ostream&) const;

	ostream&
	dump_state(ostream&) const;

	/**
		Summarize channel state that is not already
		in the channel structure.  
	 */
	struct status_summary {
		/// snapshot of the current data value, may be garbage
		value_type		current_value;
		/// true if channel data is known to be in transient state
		bool			value_transitioning;
		/// true if channel is considered to 'have' a token
		bool			full;	// or empty
		/// true if ack is X
		bool			x_ack;
		/// true if ack is active (asserted)
		bool			ack_active;
		/// true if valid/request is X
		bool			x_valid;
		/// true if valid/request is active (asserted)
		bool			valid_active;
		/// true if validity/req is due to change next (ev handshake)
		bool			valid_following;
		/// true if the next action is expected to be from sender
		bool			waiting_sender;
		/// true if the next action is expected to be from receiver
		bool			waiting_receiver;

		status_summary() :	// defaults
			current_value(0), 
			value_transitioning(false), 
			full(false), 
			x_ack(false), 
			ack_active(false), 
			x_valid(false), 
			valid_active(false), 
			valid_following(false), 
			waiting_sender(false), 
			waiting_receiver(false)
			{ }

		// only applicable to 2-phase channels
		void
		set_empty(bool e) {
			full = !e;
		if (e) {
			waiting_sender = true;
			valid_active = false;
			ack_active = true;
		} else {
			waiting_receiver = true;
			valid_active = true;
			ack_active = false;
		}
		}

		void
		set_full(bool f) {
			set_empty(!f);
		}

		ostream&
		dump_raw(ostream&) const;
	};	// end struct status_summary

	status_summary
	summarize_status(const State&) const;

	ostream&
	dump_status(ostream&, const State&) const;

	error_policy_enum
	assert_status(ostream&, const State&, const string&) const;

private:
// don't bother passing ostream& to these assert functions for now
	bool
	__assert_channel_value(const value_type& expect, 
		const value_type& got, const bool x, const bool confirm) const;

	bool
	__assert_value(const status_summary&, const value_type&, 
		const bool) const;

	bool
	__assert_validity(const status_summary&,
		const bool, const bool) const;

	bool
	__assert_full(const status_summary&,
		const bool, const bool) const;

#if PRSIM_CHANNEL_BUNDLED_DATA
	void
	set_bd_data_req(const State&, const status_summary&,
		vector<env_event_type>&);
#endif

	void
	__resume_4p_sink(const status_summary&, vector<env_event_type>&);

	env_event_type
	toggle_node(const State& s, const node_index_type ni) const;

	void
	process_data(const State&) throw(channel_exception);

	void
	get_all_nodes(vector<node_index_type>&) const;

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

};	// end class channel

#if !PRSIM_CHANNEL_SIGNED
ostream&
operator << (ostream&, const channel::array_value_type&);
#endif

//=============================================================================
// wrap these into a channel_manager?

class channel_manager {
	/**
		For __get_X_fanins.
	 */
	typedef	std::set<node_index_type>	node_set_type;
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
		const string& rn, const size_t r, const bool al, 
		const bool, const bool, const bool, const bool, const bool);

private:
	bool
	set_channel_ack_valid(State&, const string&, 
		const bool have_ack, const bool ack_sense, const bool ack_init,
		const char* vname,
		const bool have_validity, const bool validity_sense);

	bool
	allocate_data_rails(State&, const module&, const size_t ci,
		const string& bn, const size_t nb, 
		const string& rn, const size_t nr);

	bool
	set_channel_2p_ack(State&, const size_t ci,
		const string& an, const bool ai);

	bool
	set_channel_2p_req(State&, const size_t ci,
		const string& rn, const bool ri);

public:
	bool
	new_channel_ledr(State&, const string&, 
		const string& an, const bool ai,
		const string& bn, const size_t, 
		const string& dn, const bool di, const bool ds,
		const string& rn, const bool ri);

#if PRSIM_CHANNEL_BUNDLED_DATA
	// for now, name is hard-coded based on sense
	bool
	new_channel_bd4p(State&, const string&, 
//		const string& an, 
		const bool as, const bool ai,
//		const string& rn, 
		const bool rs, 
//		const bool ri,
		const string& dn, const size_t nr, const bool ds);

	bool
	new_channel_bd2p(State&, const string&, 
		const string& an, 
		const bool ai, 
		const string& rn, 
		const bool ri,
		const string& dn, const size_t nr, const bool ds);
#endif

#if PRSIM_CHANNEL_SYNC
	bool
	new_channel_clocked_1edge(State&, const string&,
		const string& cn, const bool cs,
		const string& dn, const size_t nr, const bool ds);

	bool
	new_channel_clocked_2edge(State&, const string&,
		const string& cn, const bool ci,
		const string& dn, const size_t nr, const bool ds);

	bool
	new_clock_source(State&, const string&, const bool e,
		const bool cs, const bool ci, const int n);
#endif

	ostream&
	__dump(ostream&, const bool) const;

	bool
	__dump_channel(ostream&, const string&, const bool) const;

	ostream&
	dump(ostream& o, const State&) const {
		return __dump(o, false);
	}

#if !PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	bool
	dump_channel(ostream& o, const State&, const string& c) const {
		return __dump_channel(o, c, true);
	}

	bool
	dump_channel_state(ostream&, const State&, const string&) const;
#endif

private:
	channel_index_type
	get_channel_index(const channel&) const;

public:
	bool
	check_source(const channel& c) const;

	bool
	check_sink(const channel& c) const;

#if !PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	// most of these functions differ in only the channel method called
	bool
	source_channel_file(const State&, const string&, 
		const string&, const bool);

	bool
	source_channel_args(const State&, const string&, 
		const string_list&, const bool);

	bool
	rsource_channel(const State&, const string&);

	bool
	sink_channel(const State&, const string&);

	bool
	expect_channel_file(const string&, const string&, const bool);

	bool
	expect_channel_args(const string&, const string_list&, const bool);
#endif

	bool
	log_channel(const string&, const string&);

	// TODO: reduce these functions to take channel::*function
	channel*
	lookup(const string&);

	const channel*
	lookup(const string&) const;

#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	bool
	lookup_expand(const string&, const module&,
		vector<const channel*>&) const;

	bool
	lookup_expand(const string&, const module&,
		vector<channel*>&);
#endif

#if !PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	bool
	apply_one(const string&, void (channel::*)(void));
#endif

	void
	apply_all(void (channel::*)(void));

	bool
	resume_channel(const State&, const string&, vector<env_event_type>&);

	void
	resume_all_channels(const State&, vector<env_event_type>&);

	bool
	reset_channel(const string&, vector<env_event_type>&);

	void
	reset_all_channels(vector<env_event_type>&);

	void
	process_node(const State&, const node_index_type, 
		const value_enum, const value_enum, 
		vector<env_event_type>&) throw (channel_exception);

	bool
	node_has_fanin(const node_index_type) const;

	bool
	node_has_fanout(const node_index_type) const;

	ostream&
	dump_node_fanin(ostream&, const node_index_type) const;

	ostream&
	dump_node_fanout(ostream&, const node_index_type) const;

	void
	__get_X_fanins(const State&, const node_index_type, 
		node_set_type&) const;

	ostream&
	__node_why_not(const State&, ostream&, const node_index_type, 
		const size_t, const bool d, const bool wn, const bool v, 
		node_set_type&, node_set_type&) const;

	void
	initialize(void);

	void
	clobber_all(void);

	ostream&
	__node_why_X(const State&, ostream&, const node_index_type, 
		const size_t, const bool v, 
		node_set_type&, node_set_type&) const;

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
