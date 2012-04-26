/**
	\file "sim/prsim/Channel-prsim.cc"
	$Id: Channel-prsim.cc,v 1.44 2011/03/30 20:59:25 fang Exp $
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
#include "AST/expr_base.h"
#include "Object/ref/meta_reference_union.h"
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
#include "util/wtf.h"
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
using parser::inst_ref_expr;
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
using entity::global_indexed_reference;

static const char bundled_data_global_timing_warning[] =
"Warning (FIXME): global-randomized timing policy is not yet fully supported\n"
"on bundled-data channel sources; the request signal may fire prematurely.";

//=============================================================================
#if PRSIM_CHANNEL_TIMING

#if 0
/**
	\returns the default value if given value is 0
	Stolen from State-prsim.cc:__get_delay()
 */
static
inline
const channel_time_type&
__get_channel_delay(const channel& c,
		channel_time_type channel::* m, 
		const channel_time_type& def) {
	return !State::time_traits::is_zero(c.*m) ? c.*m : def;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overridden with delay value.  
 */
env_event_type::env_event_type(const node_index_type ni, 
		const value_enum v, const channel_time_type d) :
		node_index(ni), value(v), use_global(false), delay(d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Use channel timing policy to determine delay.
 */
env_event_type::env_event_type(const node_index_type ni, 
		const value_enum v, const channel& c) :
		node_index(ni), value(v) {
	switch (c.timing_mode) {
	case CHANNEL_TIMING_GLOBAL:
		use_global = true; break;
	case CHANNEL_TIMING_AFTER:
		use_global = false;
		delay = c.after_min; break;	// same as after_max
	case CHANNEL_TIMING_RANDOM: {
		use_global = false;
#if PRSIM_AFTER_RANGE
		typedef	State::time_traits		time_traits;
		typedef	channel_time_type		time_type;
		const time_type min_val = c.after_min;
	//	__get_channel_delay(c, &channel::after_min, default_after_min);
		const time_type max_val = c.after_max;
	//	__get_channel_delay(c, &channel::after_max, default_after_max);
		const bool have_min = !time_traits::is_zero(min_val);
		const bool have_max = !time_traits::is_zero(max_val);
		if (have_max) {
			if (have_min) {
				delay = min_val +
					(max_val -min_val)
					* State::uniform_random_delay();
			} else {
				delay = max_val * State::uniform_random_delay();
			}
		} else {
			delay = ((0x01 << 11) * State::exponential_random_delay());
			if (have_min) delay += min_val;
		}
#else
		if (c.after < 0) {	// negative signals unbounded
		delay = State::exponential_random_delay(); break;
		// should this match State::get_delay_up? (normalized?)
		} else {
		delay = State::uniform_random_delay() *c.after; break;
		}
#endif
		break;
	}
	case CHANNEL_TIMING_BINARY: {
		use_global = false;
		delay = (State::uniform_random_delay() < c.timing_probability)
			? c.after_min : c.after_max; 
		break;
	}
	default: DIE;
	}
}
#endif

//=============================================================================
// class channel_exception method definitions

error_policy_enum
channel_exception::inspect(const State& s, ostream& o) const {
	NEVER_NULL(chan);
	o << "ERROR: value assertion failed on channel `" <<
		chan->get_name() << "\' at index [" << index;
		// chan->current_index();	// may have already advanced
	if (chan->is_looping()) {
		o << "] of iteration [" << iteration;
		// chan->current_iteration(); // may have already advanced
	}
	o << "]." << endl;
	chan->print_data_value(o << "\texpected: ", expect);
	o << ", got: ";
	if (got_x) {
		o << 'X' << endl;
	} else {
		chan->print_data_value(o, got) << endl;
	}
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

// default global policy
bool
channel::report_time = false;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel::channel() :
		name(), 
		ack_signal(INVALID_NODE_INDEX), 
		valid_signal(INVALID_NODE_INDEX), 
#if PRSIM_CHANNEL_TIMING
		timing_mode(CHANNEL_TIMING_DEFAULT),
		after_min(State::rule_type::default_unspecified_delay),
		after_max(State::rule_type::default_unspecified_delay),
		timing_probability(0.5),
#endif
		type(CHANNEL_TYPE_1ofN),	// default
		flags(CHANNEL_DEFAULT_FLAGS), 
		counter_state(0), 	// invalid
		x_counter(0),		// invalid
		data(), 
		inject_expect_file(), 
		dumplog(), 
		values(), 
		value_index(0), 
		iteration(0) {
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
#if !PRSIM_CHANNEL_SIGNED
/**
	This printer is always unsigned.
 */
ostream&
operator << (ostream& o, const channel::array_value_type& p) {
	if (p.second) {
		return o << 'X';
	} else {
		return o << p.first;
	}
}
#endif

#if PRSIM_CHANNEL_SIGNED
class channel_data_dumper {
//	public std::iterator<std::output_iterator_tag, void, void, void, void>
	typedef	channel_data_dumper		this_type;
	ostream&		os;
	const bool		is_signed;
	const char*		delim;

public:
	channel_data_dumper(ostream& o, bool s, const char* d) :
		os(o), is_signed(s), delim(d) { }

	ostream&
	operator () (const channel::array_value_type& p) {
		if (p.second) {
			os << 'X';
		} else {
			if (is_signed) {
				os << channel::signed_value_type(p.first);
			} else {
				os << p.first;
			}
		}
		return os << delim;
	}

};	// end struct channel_data_dumper
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel::__dump_ack_valid_type(ostream& o) const {
	if (ack_signal) {
	o << (get_ack_active() ? ".a" : ".e");
	o << "(init:" << (get_ack_init() ? '1' : '0') << ')';
	o << ' ';
	}
	if (valid_signal) {
		o << (get_valid_sense() ? ".v" : ".n");
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't bother printing out node indices.
 */
ostream&
channel::dump(ostream& o) const {
	o << name << " : ";
	const size_t r = radix();
switch (type) {
case CHANNEL_TYPE_1ofN:
	__dump_ack_valid_type(o);
	// didn't store names of bundles and rails
	o << ' ' << bundles() << 'x';
#if PRSIM_CHANNEL_RAILS_INVERTED
	if (get_data_sense()) { o << '~'; }
#endif
	o << "1of" << radix();
	break;
case CHANNEL_TYPE_LEDR:
	// FIXME: bundle-count Nx
	o << "LEDR (init";
	// FIXME: doesn't actually store/use the name of the 
	// member rails that were given when channel was declared.
	if (ack_signal) {
		o << " .e:" << (get_ack_init() ? '1' : '0');
	}
	o << " .";
#if PRSIM_CHANNEL_RAILS_INVERTED
	if (get_data_sense()) { o << '~'; }
	// else { o << "+"; }
#endif
	o << "d:" << (get_data_init() ? '1' : '0');
	o << " .r:" << (get_repeat_init() ? '1' : '0');
	// empty-parity?
	o << ")";
	break;
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_4P:
	o << "bundled-4p ";
	__dump_ack_valid_type(o);
if (r) {
	o << ' ';
#if PRSIM_CHANNEL_RAILS_INVERTED
	if (get_data_sense()) { o << '~'; }
#endif
	o << 'x' << r;
}
	break;
case CHANNEL_TYPE_BD_2P:	// similar to LEDR
	o << "bundled-2p";
	o << " .e:" << (get_ack_init() ? '1' : '0');	// ack
	o << " .v:" << (get_req_init() ? '1' : '0');	// request
if (r) {
	o << ' ';
#if PRSIM_CHANNEL_RAILS_INVERTED
	if (get_data_sense()) { o << '~'; }
#endif
	o << 'x' << r;
}
	break;
#endif	// PRSIM_CHANNEL_BUNDLED_DATA
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_SRC1:
	o << "clock-source " << (get_clock_sense() ? "posedge" : "negedge");
	break;
case CHANNEL_TYPE_CLK_SRC2:
	o << "clock-source anyedge:" << (get_clk_init() ? '1' : '0');	// init
	break;
case CHANNEL_TYPE_CLK_1EDGE:
	o << "clocked-" << (get_clock_sense() ? "posedge " : "negedge ");
#if PRSIM_CHANNEL_RAILS_INVERTED
	if (get_data_sense()) { o << '~'; }
#endif
	o << 'x' << r;
	break;
case CHANNEL_TYPE_CLK_2EDGE:
	o << "clocked-anyedge:" << (get_clk_init() ? '1' : '0') << ' ';	// init
#if PRSIM_CHANNEL_RAILS_INVERTED
	if (get_data_sense()) { o << '~'; }
#endif
	o << 'x' << r;
	break;
#endif	// PRSIM_CHANNEL_SYNC
// case CHANNEL_TYPE_SINGLE_TRACK:
default:
	DIE;
}	// end switch
#if PRSIM_CHANNEL_SIGNED
	if (can_be_signed()) {
		if (is_signed())
			o << " signed";
		else	o << " unsigned";
	}
#endif
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
	if (stopping_on_empty()) {
		o << ",stop-on-empty";
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
	if (is_clock_source()) {
		// doesn't take real values
		o << " @" << value_index << '/';
		if (is_looping()) {
			o << '*';
		} else {
			o << values.size();
		}
	} else {
		o << " {";
#if PRSIM_CHANNEL_SIGNED
		for_each(values.begin(), values.end(),
			channel_data_dumper(o, is_signed(), ","));
#else
		copy(values.begin(), values.end(),
			ostream_iterator<array_value_type>(o, ","));
#endif
		o << '}';
		if (is_looping()) o << '*';
		o << " @" << value_index;
		if (is_looping()) o << " #" << iteration;
		if (inject_expect_file.length()) {
			o << " < " << inject_expect_file;	// source/expect
		}
		// values may come from command arguments
	}
	} else if (is_random()) {
#if 0
		o << " {" << values.front() << '}';
#endif
	}
	if (dumplog.stream) {
		o << " > " << dumplog.fname;
	}
	return o;
}	// end channel::dump

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
	Summarize information about the state of a channel.
	\pre the value of x_counter and counter_state must be up-to-date 
		to reflect the current state of the data-rails and validity.  
 */
channel::status_summary
channel::summarize_status(const State& s) const {
	STACKTRACE_BRIEF;
	status_summary ret;
if (type != CHANNEL_TYPE_LEDR) {
	if (ack_signal) {
		const value_enum a = s.get_node(ack_signal).current_value();
		ret.x_ack = (a == LOGIC_OTHER);
		ret.ack_active = get_ack_active() ^ (a == LOGIC_LOW);
	}
	if (valid_signal) {	// use ev handshake protocol
		// is also applicable to clock channels/sources
		const value_enum v = s.get_node(valid_signal).current_value();
		ret.x_valid = (v == LOGIC_OTHER);
		ret.valid_active = get_valid_sense() ^ (v == LOGIC_LOW);
	}
	// this also sufficiently covers clocked channel types as well
	// for bundled-data, set the full/empty state regardless of data rails
#if PRSIM_CHANNEL_BUNDLED_DATA
switch (type) {
case CHANNEL_TYPE_BD_4P: { 	// treat req/ack as ev-handshake!
	INVARIANT(ack_signal);
	INVARIANT(valid_signal);
	// doesn't care about state of data rails
	if (!ret.x_valid && !ret.x_ack) {
		// shared validity, validity may follow rails
		if (ret.valid_active ^ ret.ack_active)
			ret.waiting_receiver = true;
		else	ret.waiting_sender = true;
	}
	break;
}
case CHANNEL_TYPE_BD_2P: { 	// treat req/ack as 2p handshake
	INVARIANT(ack_signal);
	INVARIANT(valid_signal);
	const value_enum p = current_bd2p_parity(s);
	// doesn't care about state of data rails
	if (!ret.x_valid && !ret.x_ack) {
		const bool empty = bd2p_empty_parity() ^ (p == LOGIC_LOW);
		ret.set_empty(empty);
	}
	break;
}
default: break;
}	// end switch
#endif
}	// otherwise not applicable
if (!x_counter) {
	// then we can infer the state of the handshake
switch (type) {
case CHANNEL_TYPE_1ofN: {
	// 4-phase
	if (!counter_state) {
		// data is neutral
	if (ack_signal && !ret.x_ack) {
		if (valid_signal) {
		if (!ret.x_valid) {
			// shared validity, validity may follow rails
			if (ret.valid_active ^ ret.ack_active)
				ret.waiting_receiver = true;
			else	ret.waiting_sender = true;
		}
		} else {
			// no shared validity
			if (ret.ack_active)
				ret.waiting_receiver = true;
			else	ret.waiting_sender = true;
		}
	} // else nothing else
	} else if (counter_state == bundles()) {
		// data is valid
		// ALERT: shared validity protocol validity *follows* data rails
		ret.current_value = data_rails_value(s);
	if (ack_signal && !ret.x_ack) {
		if (valid_signal) {
		if (!ret.x_valid) {
			// shared-validity
			if (ret.valid_active ^ ret.ack_active)
				ret.waiting_receiver = true;
			else	ret.waiting_sender = true;
		}
		} else {
			// no shared-validity
			if (ret.ack_active)
				ret.waiting_sender = true;
			else	ret.waiting_receiver = true;
		}
	}
	} else {
		// data is in between
		ret.value_transitioning = true;
		if (ack_signal && !ret.x_ack) {
			// data is always sender's responsibility
			ret.waiting_sender = true;
		}
	}
	break;
}
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_SRC1:		// fall-through
case CHANNEL_TYPE_CLK_SRC2:
	// no data
	break;
case CHANNEL_TYPE_CLK_1EDGE:		// fall-through
case CHANNEL_TYPE_CLK_2EDGE:		// fall-through
	// fall-through
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_4P: 	// treat req/ack as ev-handshake!
	// fall-through
case CHANNEL_TYPE_BD_2P:	// treat req/ack as 2p handshake
	// just always read data rails?
	ret.current_value = data_rails_value(s);
	break;
#endif
case CHANNEL_TYPE_LEDR: {
	// 2-phase, or 1-phase
	const value_enum p = current_ledr_parity(s);
	// FIXME: can be X if we didn't check the state of the ack
	INVARIANT(p != LOGIC_OTHER);
	if (ack_signal) {
	// full/empty
		const bool empty = ledr_empty_parity() ^ (p == LOGIC_LOW);
		ret.set_empty(empty);
		if (!empty) {
			ret.current_value = data_rails_value(s);
		}
	} else {
		// is a 1-phase channel, only report data rails
		ret.current_value = data_rails_value(s);
	}
	break;
}
case CHANNEL_TYPE_SINGLE_TRACK: {
	FINISH_ME(Fang);
	break;
}
default: break;
}	// end switch
} else {
	// if there are X's do nothing, don't bother summarizing
}
#if ENABLE_STACKTRACE
	ret.dump_raw(cerr);
#endif
	return ret;
}	// end channel::summarize_status

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Raw dump of status summary, only really used for debugging.
 */
ostream&
channel::status_summary::dump_raw(ostream& o) const {
	o << "current value: " << current_value << endl;
	o << "value transitioning: " << value_transitioning << endl;
	o << "full: " << full << endl;
	o << "x_ack: " << x_ack << endl;
	o << "ack_active: " << ack_active << endl;
	o << "x_valid: " << x_valid << endl;
	o << "valid_active: " << valid_active << endl;
	o << "valid_following: " << valid_following << endl;
	o << "waiting sender: " << waiting_sender << endl;
	o << "waiting receiver: " << waiting_receiver << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints state information about channel, w.r.t. phase of handshake.
	TODO: tests to cover all states
 */
ostream&
channel::dump_status(ostream& o, const State& s) const {
	// compute status summary first
	const status_summary stat(summarize_status(s));
if (!x_counter) {
	// then we can infer the state of the handshake
switch (type) {
case CHANNEL_TYPE_1ofN: {
	if (!counter_state) {
		// data is neutral
		o << "data is neutral";
	if (valid_signal) {
		if (stat.x_valid)
			o << ", validity is X";
		// validity is catching up
		else if (stat.valid_active)
			o << ", validity resetting";
		// else validity already reflects the state of data rails
	}
	if (ack_signal && !(valid_signal && stat.valid_active)) {
		o << ", " << (stat.x_ack ? "ack is X" :
			(stat.ack_active ? "waiting for receiver to neg-ack"
			: "waiting for sender to produce data"));
	} // else nothing else
	} else if (counter_state == bundles()) {
		// data is valid
		// ALERT: shared validity protocol validity *follows* data rails
		o << "data is valid (" << stat.current_value << ")";
	if (valid_signal) {
		if (stat.x_valid)
			o << ", validity is X";
		// validity is catching up
		else if (!stat.valid_active)
			o << ", validity setting";
		// else validity already reflects the state of data rails
	}
	if (ack_signal && !(valid_signal && !stat.valid_active)) {
		o << ", " << (stat.x_ack ? "ack is X" :
			(stat.ack_active ? "waiting for sender to remove data"
			: "waiting for receiver to ack"));
	}
	} else {
		// data is in between
		INVARIANT(stat.value_transitioning);
		o << "data is transitioning to " <<
			(stat.ack_active ? "neutral" : "valid");
	}
	break;
}
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_4P: { 	// treat req/ack as ev-handshake!
	// does not use counter_state
	if (stat.x_valid)
		o << ", request is X";
	// validity is catching up
	else if (stat.valid_active)
		o << ", request resetting";
	// else validity already reflects the ready-state of data rails
	if (stat.valid_active) {
		o << ", " << (stat.x_ack ? "ack is X" :
			(stat.ack_active ? "waiting for sender to neg-request"
			: "waiting for receiver to ack"));
	} else {
		o << ", " << (stat.x_ack ? "ack is X" :
			(stat.ack_active ? "waiting for receiver to neg-ack"
			: "waiting for sender request"));
	}
	break;
}
case CHANNEL_TYPE_BD_2P:
	// fall-through, same as LEDR!
	INVARIANT(ack_signal);
#endif
case CHANNEL_TYPE_LEDR: {
	if (ack_signal) {
	// full/empty
		if (!stat.full) {
			o << "channel is empty, waiting for sender";
		} else {
			o << "channel is full (" << stat.current_value <<
				"), waiting for receiver ack";
		}
	} else {
		// is a 1-phase channel, only report data rails
		o << "channel value (" << stat.current_value << ")";
	}
	break;
}
case CHANNEL_TYPE_SINGLE_TRACK: {
	FINISH_ME(Fang);
	break;
}
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_SRC1:		// fall-through
case CHANNEL_TYPE_CLK_1EDGE:
	o << "clock is " << (stat.valid_active ? "active" : "inactive");
	break;
case CHANNEL_TYPE_CLK_SRC2:		// fall-through
case CHANNEL_TYPE_CLK_2EDGE:
	o << "clock is 2-edged";	// always
	break;
#endif
default: break;
}	// end switch
} else {
	// for all channel types, even bundled-data
	o << "unknown, because there are " << size_t(x_counter) <<
		" X rails";
}
	return o;
}	// end channel::dump_status

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function.
	Always prints a diagnostic on failure (non-maskable), 
	but up to caller how to handle.  
	\return false if assertion fails.  
 */
bool
channel::__assert_channel_value(const channel::value_type& expect, 
		const channel::value_type& got,
		const bool x, 
		const bool confirm) const {
	static const char cmd[] = "channel-assert";
if (radix()) {
	if (expect != got) {
		print_data_value(cerr << cmd <<
			": value assertion failed on channel " <<
			name << ", expected: ", expect);
		cerr << ", but got: ";
		if (x) {
			cerr << 'X' << endl;
		} else {
			print_data_value(cerr, got) << endl;
		}
		return false;
	} else if (confirm) {
		print_data_value(cout << "channel " << name << " has value ", 
			expect) << ", as expected." << endl;
	}
}	// else is data-less, and thus not applicable
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return false if assertion fails.  
 */
bool
channel::__assert_value(const status_summary& stat, const value_type& expect, 
		const bool confirm) const {
	static const char cmd[] = "channel-assert";
switch (type) {
case CHANNEL_TYPE_1ofN: {
	if (counter_state == bundles()) {
		if (!__assert_channel_value(expect, stat.current_value, false,
				confirm))
			return false;
	} else {
		cerr << cmd << ": cannot assert value on channel " <<
			name << " because " << size_t(counter_state) << " of "
			<< bundles() << " bundles are valid." << endl;
		return false;
	}
	break;
}	// end case CHANNEL_TYPE_1ofN
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_SRC1:
case CHANNEL_TYPE_CLK_SRC2:
	// value-less
	break;
case CHANNEL_TYPE_CLK_1EDGE:
case CHANNEL_TYPE_CLK_2EDGE:
	// general warning: values may be transient due to synchronous nature
	if (!__assert_channel_value(expect, stat.current_value, x_counter, confirm))
		return false;
	break;
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_4P: {
	// snapshot of data rails
	// the real window is when request is active and ack is inactive
	if (!stat.valid_active || stat.ack_active) {
		cerr << cmd <<
": [warning] asserting value of bundled-data channel when data rails\n"
"may be transient." << endl;
	}
	if (!__assert_channel_value(expect, stat.current_value, x_counter, confirm))
		return false;
	break;
}
case CHANNEL_TYPE_BD_2P:
	// fall-through, same handling as LEDR 2-phase!
	INVARIANT(ack_signal);
#endif
case CHANNEL_TYPE_LEDR: {
	if (ack_signal) {
	// 2-phase
	// full/empty
	if (stat.full) {
		if (!__assert_channel_value(expect, stat.current_value, false, 
				confirm))
			return false;
	} else {
		cerr << cmd << ": cannot assert value on channel " <<
			name << " because it is now empty." << endl;
		return false;
	}
	} else {
	// 1-phase (ackless)
		if (!__assert_channel_value(expect, stat.current_value,
				false, confirm))
			return false;
	}
	break;
}	// end case CHANNEL_TYPE_LEDR
// case CHANNEL_TYPE_SINGLE_TRACK:
default:
	FINISH_ME(Fang);
	break;
}	// end switch (type)
	return true;
}	// end channel::assert_value

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
__assert_validity_diagnostic(ostream& o, const string& name, 
		const bool expect_valid) {
	static const char cmd[] = "channel-assert";
	o << cmd << ": expected channel " << name;
	if (expect_valid) {
		o << " valid, but is neutral." << endl;
	} else {
		o << " neutral, but is valid." << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return false if assertion failed.
 */
bool
channel::__assert_validity(const status_summary& stat,
		const bool expect_valid, const bool confirm) const {
	static const char cmd[] = "channel-assert";
switch (type) {
case CHANNEL_TYPE_1ofN: {
	if (valid_signal) {
	if (stat.x_valid) {
		cerr << cmd << ": validity signal of " << name
			<< " is X, cannot assert." << endl;
		return false;
	} else if (expect_valid ^ stat.valid_active) {
		__assert_validity_diagnostic(cerr, name, expect_valid);
		return false;
	} else if (confirm) {
		cout << "channel " << name << " is " <<
			(expect_valid ? "valid" : "neutral") <<
			", as expected." << endl;
	}
	} else {
		// no validity signal, have to examine data rails
		if (counter_state == bundles()) {
			// data is valid
		if (!expect_valid) {
			__assert_validity_diagnostic(cerr, name, expect_valid);
			return false;
		} else if (confirm) {
			cout << "channel " << name <<
				" is valid, as expected." << endl;
		}
		} else if (!counter_state) {
			// data is neutral
		if (expect_valid) {
			__assert_validity_diagnostic(cerr, name, expect_valid);
			return false;
		} else if (confirm) {
			cout << "channel " << name <<
				" is neutral, as expected." << endl;
		}
		} else {
			// data is transitional
			cerr << cmd << ": channel " << name <<
				" is transitional (neither valid nor "
				"neutral), cannot assert." << endl;
			return false;
		}
	}
	break;
}	// end case CHANNEL_TYPE_1ofN
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_4P: {
	INVARIANT(valid_signal);
	if (stat.x_valid) {
		cerr << cmd << ": validity signal of " << name
			<< " is X, cannot assert." << endl;
		return false;
	} else if (expect_valid ^ stat.valid_active) {
		__assert_validity_diagnostic(cerr, name, expect_valid);
		return false;
	} else if (confirm) {
		cout << "channel " << name << " is " <<
			(expect_valid ? "valid" : "neutral") <<
			", as expected." << endl;
	}
	break;
}
case CHANNEL_TYPE_BD_2P:
	INVARIANT(ack_signal);
	// fall-through to 2p-LEDR
#endif
case CHANNEL_TYPE_LEDR: {
	if (ack_signal) {
	// 2-phase
	// full/empty
	if (stat.full ^ expect_valid) {
		__assert_validity_diagnostic(cerr, name, expect_valid);
		return false;
	} else if (confirm) {
		cout << "channel " << name << " is " <<
			(expect_valid ? "valid" : "neutral") <<
			", as expected." << endl;
	}
	} else {
		// 1-phase (ackless)
		cerr <<
"Validity is not applicable to (acknowledgeless) single-phase channel "
			<< name << "." << endl;
		return false;
	}
	break;
}	// end case CHANNEL_TYPE_LEDR
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_SRC1:
case CHANNEL_TYPE_CLK_SRC2:
case CHANNEL_TYPE_CLK_1EDGE:
case CHANNEL_TYPE_CLK_2EDGE:
	cerr << "Validity is not applicable to clocked channel "
		<< name << "." << endl;
	return false;
	break;
#endif
// case CHANNEL_TYPE_SINGLE_TRACK:
default:
	FINISH_ME(Fang);
	break;
}	// end switch
	return true;
}	// end channel::__assert_validity

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	User error checking on channels' state.
	\param o error stream
	\param s global state
	\param v keyword for state to assert.
 */
error_policy_enum
channel::assert_status(ostream& o, const State& s, const string& v) const {
	static const char cmd[] = "channel-assert";
	const error_policy_enum E = s.get_channel_expect_fail_policy();
	const status_summary stat(summarize_status(s));
	const bool confirm = s.confirm_asserts();
if (x_counter) {
	o << "Error: Cannot assert the state of a channel with X rails."
		<< endl;
	return E;
}
// SINGLE_TRACK not implemented yet
if (type == CHANNEL_TYPE_SINGLE_TRACK) {
	FINISH_ME(Fang);
	return E;
}
	value_type expect;
// check for numerical string
if (!string_to_num(v, expect)) {
	// then we have a value
	if (!__assert_value(stat, expect, confirm))
		return E;
} else if (v == "valid" || v == "neutral") {
	if (!__assert_validity(stat, (v == "valid"), confirm))
		return E;
} else if (v == "full" || v == "empty") {
	// if (two_phase()) ... ?
	// for now make synonymous with valid/neutral
	if (!__assert_validity(stat, (v == "full"), confirm))
		return E;
} else if (v == "ack" || v == "neg-ack") {
	// four-phase only
	if (!four_phase() || !ack_signal) {
		cerr << cmd <<
": \'ack\' and \'neg-ack\' are applicable to only four-phase channels\n"
"with acknowledge signals, of which channel " << name << " is not."
			<< endl;
		return E;
	}
	const bool expect_ack = (v == "ack");
	if (expect_ack ^ stat.ack_active) {
		cerr << cmd << ": channel " << name << " acknowledge expected ";
		if (expect_ack) {
			cerr << "active, but is negative." << endl;
		} else {
			cerr << "negative, but is active." << endl;
		}
		return E;
	} else if (confirm) {
		cout << "channel " << name << " ackowledge is " <<
			(expect_ack ? "active" : "negative") <<
			", as expected." << endl;
	}
} else if (v == "waiting-sender") {
	// four-phase or two-phase
	if (!stat.waiting_sender) {
		cerr << cmd << ": channel " << name <<
			" is expecting to be waiting for the sender, "
			"but is not." << endl;
		return E;
	} else if (confirm) {
		cout << "channel " << name <<
			" is waiting for the sender, as expected." << endl;
	}
} else if (v == "waiting-receiver") {
	// four-phase or two-phase
	if (!stat.waiting_receiver) {
		cerr << cmd << ": channel " << name <<
			" is expecting to be waiting for the receiver, "
			"but is not." << endl;
		return E;
	} else if (confirm) {
		cout << "channel " << name <<
			" is waiting for the receiver, as expected." << endl;
	}
} else {
	cerr << cmd << ": unknown keyword or value \"" << v << "\"" << endl;
	return ERROR_FATAL;
//	return ERROR_BREAK;
}
	return ERROR_NONE;
}	// end channel::assert_status

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param T signed or unsigned number type
	See: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=39802
	In older versions, parsing -1 into an unsigned type
	would fail-to-fail.  
 */
template <class T>
static
bool
lex_channel_value(const string& tok, T& val, bool& x) {
	val = 0;
	x = false;
	if (tok == "X" || tok == "x") {
		x = true;
	} else if (string_to_num(tok, val)) {
		// passing a negative number to an unsigned will error, nice!
		cerr << "Error: invalid value: " << tok << "." << endl;
		return true;
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
static
bool
__scan_value()
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param max, if value exceeds max, issue a warning.
	\return true on error.
 */
bool
channel::read_values_from_list(const string_list& s) {
//		const channel::value_type max,
//		vector<channel::array_value_type>& v
	STACKTRACE_VERBOSE;
	static const value_type half_max =
		std::numeric_limits<value_type>::max() >> 1;
	const value_type max = max_value();
	const signed_value_type smax = signed_value_type(max);
	const signed_value_type smin = min_value();
//	const value_type min = value_type(smin);
	values.reserve(s.size());
	string_list::const_iterator j(s.begin()), e(s.end());
	for ( ; j!=e; ++j) {
		const string& tok(*j);
		array_value_type p;
		value_type& i(p.first);
#if PRSIM_CHANNEL_SIGNED
		// type-punned alias
	if (is_signed()) {
		signed_value_type&
			si(reinterpret_cast<signed_value_type&>(i));
		if (lex_channel_value(tok, si, p.second)) {
			return true;
		}
		if (si > smax) {
			cerr <<
"Warning: value " << si << " exceeds the maximum value encoded, " << smax << ".\n"
"Higher significant bits may be ignored." << endl;
		}
		if (si < smin) {
			cerr <<
"Warning: value " << si << " exceeds the minimum value encoded, " << smin << ".\n"
"Higher significant bits may be ignored." << endl;
		}
	} else {	// unsigned
#endif
		if (lex_channel_value(tok, i, p.second)) {
			return true;
		}
		if (i > half_max) {
			cerr << "Warning: value " << i << " is greater than "
				"max(unsigned value_type)/2, which may screw "
				"up ldiv() when translating to rails." << endl;
		}
		if (i > max) {
			cerr <<
"Warning: value " << i << " exceeds the maximum value encoded, " << max << ".\n"
"Higher significant bits may be ignored." << endl;
		}
		// minimum is 0
#if PRSIM_CHANNEL_SIGNED
	}	// end if signed
#endif
		values.push_back(p);
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
bool
channel::read_values_from_file(const string& fn) {
//		const channel::value_type max, 
//		vector<channel::array_value_type>& v
	STACKTRACE_VERBOSE;
	values.clear();
	ifstream f(fn.c_str());
	if (!f) {
		cerr << "Error opening file \"" << fn << "\" for reading."
			<< endl;
		return true;
	}
	// honor '#' comments
	string_list s;
	size_t i = 1;
	bool warned = false;
while (1) {
	string line;
	getline(f, line);
	if (!f) break;
	if ((line.length() > 0) && (line[0] != '#')) {
		util::string_list toks;
		util::tokenize(line, toks);
		s.push_back(toks.front());
	if (toks.size() > 1 && !warned) {
		cerr << "Warning: line " << i <<
		" contains additional ignored tokens (suppressing further)."
			<< endl;
		warned = true;
	}
	}
	++i;
}
	return read_values_from_list(s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_SIGNED
void
channel::set_signed(void) {
	if (can_be_signed()) {
		flags |= CHANNEL_SIGNED;
	} else {
		cerr <<
"Warning: ignoring attempt to treat non-radix-2 values as signed,\n"
"instead leaving as unsigned.  (Only wide radix-2 channels can be signed.)"
		<< endl;
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel::value_type
channel::max_value(void) const {
	switch (type) {
	case CHANNEL_TYPE_SINGLE_TRACK:
	case CHANNEL_TYPE_1ofN:
		// can't assume radix is 2
		if (is_signed()) {
			INVARIANT(radix() == 2);
			return value_type((1 << (bundles() -1)) -1);
		} else {
			return value_type(pow(radix(), bundles())) -1;
		}
#if PRSIM_CHANNEL_SYNC
//	case CHANNEL_TYPE_CLK_SRC1:
//	case CHANNEL_TYPE_CLK_SRC2:
//		return 1;
	case CHANNEL_TYPE_CLK_1EDGE:		// fall-through
	case CHANNEL_TYPE_CLK_2EDGE:		// fall-through
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
	case CHANNEL_TYPE_BD_2P:	// fall-through
	case CHANNEL_TYPE_BD_4P:
		if (is_signed()) {
			return value_type((1 << (radix() -1)) -1);
		} else {
			// unsigned
			return value_type((1 << radix()) -1);
		}
#endif
	case CHANNEL_TYPE_LEDR:
		// TODO: support LEDR buses
	default:
		return 1;	// 0 or 1
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_SIGNED
/**
	Returns smallest value encoded by data rails.
 */
channel::signed_value_type
channel::min_value(void) const {
	switch (type) {
	case CHANNEL_TYPE_SINGLE_TRACK:
	case CHANNEL_TYPE_1ofN:
		if (is_signed()) {
			INVARIANT(radix() == 2);
			return signed_value_type(-1 << (bundles() -1));
		} else {
			return 0;
		}
#if PRSIM_CHANNEL_SYNC
//	case CHANNEL_TYPE_CLK_SRC1:
//	case CHANNEL_TYPE_CLK_SRC2:
//		return 1;
	case CHANNEL_TYPE_CLK_1EDGE:		// fall-through
	case CHANNEL_TYPE_CLK_2EDGE:		// fall-through
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
	case CHANNEL_TYPE_BD_2P:	// fall-through
	case CHANNEL_TYPE_BD_4P:
		if (is_signed()) {
			return signed_value_type((-1 << (radix() -1)));
		} else {
			// unsigned
			return 0;
		}
#endif
	case CHANNEL_TYPE_LEDR:
		// TODO: support LEDR buses
	default:
		return 1;	// 0 or 1
	}
}
#endif

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
	if (read_values_from_file(file_name)) return true;
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
	if (read_values_from_list(v)) {
		return true;
	}
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
#if PRSIM_CHANNEL_SYNC
/**
	Configure source to just emit random values.  
 */
bool
channel::set_clock_source(const State& s, const int N) {
	STACKTRACE_VERBOSE;
	if (__configure_source(s, N < 0))	return true;
	value_index = 0;
	if (N < 0) {
	values.resize(1);	// fill? don't care about garbage values?
	} else {
	values.resize(N);	// fill? don't care about garbage values?
	}
	std::fill(values.begin(), values.end(), array_value_type(0, false));
	return false;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel::can_source(void) const {
	return ack_signal || type == CHANNEL_TYPE_LEDR
#if PRSIM_CHANNEL_SYNC
		|| type == CHANNEL_TYPE_CLK_SRC1
		|| type == CHANNEL_TYPE_CLK_SRC2
		|| type == CHANNEL_TYPE_CLK_1EDGE
		|| type == CHANNEL_TYPE_CLK_2EDGE
#endif
	;
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
	if (loop) {
		flags |= CHANNEL_VALUE_LOOP;
	}

	// safety checks on signal directions
if (ack_signal) {
	if (!s.get_node(ack_signal).has_fanin()) {
		cerr << "Warning: channel acknowledge `" << name <<
			(get_ack_active() ? ".a" : ".e") <<
			"\' has no fanin!" << endl;
	}
} else {
// TODO: support acknowledgeless sources for the other channel types
if (!can_source()) {
	cerr << "Error: acknowledgeless channels cannot be sourced!" << endl;
	return true;
}
}
	bool maybe_externally_driven = false;
if (valid_signal) {
if (type != CHANNEL_TYPE_LEDR && !is_clocked()) {
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
	if (!vn.has_fanout() && !is_clocked()) {
		// this warning might be excessive
		cerr << "Warning: channel validity `" << name <<
			(get_valid_sense() ? ".v" : ".n") <<
			"\' has no fanout, but is being sourced." << endl;
	}
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
}	// end channel::__configure_source

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
	if (is_clocked()) {
		// be nice, don't error out
		cerr <<
			"Warning: Ignoring.  What exactly do you expect a sync. sink to do?" << endl;
		return false;
	} else {
		// no ack, no handshake, no sink
		cerr << "Error: acknowledgeless channel cannot consume tokens!"
			<< endl;
		return true;
	}
}
	// set actual flag
	flags |= CHANNEL_SINKING;
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
#if 0
	value_type max = 0;
	switch (type) {
	case CHANNEL_TYPE_SINGLE_TRACK:
	case CHANNEL_TYPE_1ofN:
		max = bundles() * radix();
		break;
	case CHANNEL_TYPE_LEDR:
	default:
		max = 2;
		break;
	}
#endif
	if (read_values_from_file(fn)) return true;
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
	if (read_values_from_list(v)) {
		return true;
	}
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
	For 1ofN channels only? also useful for LEDR.
	Immediately upon registering a channel, initialize the state 
	of the data counter according the current values of all
	data rail nodes.  
	x_counter does not count the validity nor acknowledges.
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
switch (type) {
case CHANNEL_TYPE_SINGLE_TRACK:
case CHANNEL_TYPE_1ofN: {
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
	break;
}
#if PRSIM_CHANNEL_SYNC
//	case CHANNEL_TYPE_CLK_SRC1:
//	case CHANNEL_TYPE_CLK_SRC2:
//		break;
case CHANNEL_TYPE_CLK_1EDGE:			// fall-through
case CHANNEL_TYPE_CLK_2EDGE:			// fall-through
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_2P:		// fall-through
case CHANNEL_TYPE_BD_4P: {
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
	break;
}
#endif
case CHANNEL_TYPE_LEDR:
	// TODO: account for multiple bundles
	// don't care about counter_state, leave 0
	if (s.get_node(ledr_data_rail()).current_value() == LOGIC_OTHER)
		++x_counter;
	if (s.get_node(repeat_rail()).current_value() == LOGIC_OTHER)
		++x_counter;
	break;
default: break;
}	// end switch
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
	// same applies for 1ofN codes and LEDR channels
	counter_state = 0;
	// independent of data-rail sense
	// would rather call initialize_data_counter(const State&)
switch (type) {
case CHANNEL_TYPE_SINGLE_TRACK:
case CHANNEL_TYPE_1ofN:
	x_counter = bundles() * radix();
	break;
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_SRC1:
case CHANNEL_TYPE_CLK_SRC2:
	break;
case CHANNEL_TYPE_CLK_1EDGE:			// fall-through
case CHANNEL_TYPE_CLK_2EDGE:			// fall-through
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_2P:		// fall-through
case CHANNEL_TYPE_BD_4P:
	x_counter = radix();
	break;
#endif
case CHANNEL_TYPE_LEDR:
	x_counter = 2;		// data and repeat rail
	// TODO: account for wider LEDR bundles
	break;
default: DIE;
}	// end switch
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
	static const string __binary("binary");
	if (m == __random) {
		timing_mode = CHANNEL_TIMING_RANDOM;
		if (a.size()) {
#if PRSIM_AFTER_RANGE
			return State::parse_min_max_delay(a.front(), 
				after_min, after_max);
#else
			// bounded, uniform
			return string_to_num(a.front(), after);
#endif
			// ignore trailing values
		} else {
#if PRSIM_AFTER_RANGE
			// just report bounds, don't change
			dump_timing(cout << "channel " << name << ": ") << endl;
#else
			after = -1;
			// unbounded, exponential var.
#endif
		}
	} else if (m == __global) {
		timing_mode = CHANNEL_TIMING_GLOBAL;
		// no need to set after
#if PRSIM_CHANNEL_BUNDLED_DATA
		if (data_is_bundled()) {
			cerr << bundled_data_global_timing_warning << endl;
		}
#endif
	} else if (m == __after) {
		timing_mode = CHANNEL_TIMING_AFTER;
		if (a.size()) {
			const bool ret = string_to_num(a.front(), after_min);
			if (!ret) after_max = after_min;
			return ret;
		} else {
			// alert user that using old value
			dump_timing(cout << "channel " << name << ": ") << endl;
		}
	} else if (m == __binary) {	// copied from State::set_timing
		// syntax: [min]:[max] prob
		timing_mode = CHANNEL_TIMING_BINARY;
		if (a.size() == 2) {
			if (State::parse_min_max_delay(a.front(),
				after_min, after_max)) {
				cerr << "Error parsing min:max." << endl;
					return true;
			}
			if (string_to_num(a.back(), timing_probability)) {
				cerr << "Error lexing probability." << endl;
				return true;
			}
			return false;
		} else {
			cerr << "Expecting min:max prob arguments." << endl;
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
	case CHANNEL_TIMING_AFTER: o << "after=" << after_min; break;
	case CHANNEL_TIMING_RANDOM: {
		o << "random"; 
		const bool mz = State::time_traits::is_zero(after_max);
		if (mz)
			o << "(exp.) ";
		else	o << "(uniform) ";
		o << "[" << after_min << ",";
		if (mz)
			o << "+INF";
		else	o << after_max;
		o << "]";
		break;
	}
	case CHANNEL_TIMING_BINARY: {
		o << "binary"; 
		o << " [" << after_min << "," << after_max << "] @"
			<< timing_probability;
		break;
	}
	default: o << "unknown"; DIE;
	}
	return o;
	// NOTE: no endl
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel::help_timing(ostream& o) {
o << "available channel timing modes:\n"
	<< "\tglobal : use the global policy set by \'timing\'\n"
	<< "\trandom [[min]:[max]] :\n"
	<< "\tafter [val] : use fixed delay\n"
	<< "\tbinary min:max prob :\n"
"if max and min given, uniform bounded, else exponential variate"
"if min given, use min as lower bound." << endl;
o << "\tafter [del] : if del given, set fixed delay, else use prev. value"
	<< endl;
	return o;
}
#endif	// PRSIM_CHANNEL_TIMING

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Binding constructor functor.
 */
#if PRSIM_CHANNEL_TIMING
#define	__node_setter(x)	__node_setter__(x, *this)
#define	__node_setter_decl(y, x)	__node_setter__ y(x, *this)
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
};	// end struct __node_setter__

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
		// reset data rails to all 0s (arbitrary), just not X.
		// even for bundled-data
		initialize_all_data_rails(events);
	switch (type) {
#if PRSIM_CHANNEL_BUNDLED_DATA
	// Q: does timing matter here?
#if PRSIM_CHANNEL_SYNC
	case CHANNEL_TYPE_CLK_SRC1:
		// always start in inactive phase
		events.push_back(ENV_EVENT(clock_signal(), 
			(get_clock_sense() ? LOGIC_LOW : LOGIC_HIGH)));
		break;
	case CHANNEL_TYPE_CLK_SRC2:
		events.push_back(ENV_EVENT(clock_signal(), 
			(get_clk_init() ? LOGIC_HIGH : LOGIC_LOW)));
		break;
	case CHANNEL_TYPE_CLK_1EDGE:		// fall-through
	case CHANNEL_TYPE_CLK_2EDGE:		// fall-through
		// does NOT drive the clock rail because
		// clocks are often globally shared.
		// use a separate clock source
		break;
#endif
	case CHANNEL_TYPE_BD_4P:
		// just reset the request to neutral
		events.push_back(ENV_EVENT(valid_signal, 
			(get_valid_sense() ? LOGIC_LOW : LOGIC_HIGH)));
		break;
	case CHANNEL_TYPE_BD_2P:
		events.push_back(ENV_EVENT(valid_signal, 
			(get_req_init() ? LOGIC_HIGH : LOGIC_LOW)));
		// reset request to initial value
		break;
#endif
	default: break;
		// once nodes all become neutral, the validity should be reset
	}	// end switch
	}
	if (is_sinking()) {
	switch (type) {
#if PRSIM_CHANNEL_SYNC
	case CHANNEL_TYPE_CLK_SRC1:		// fall-through
	case CHANNEL_TYPE_CLK_SRC2:		// fall-through
	case CHANNEL_TYPE_CLK_1EDGE:		// fall-through
	case CHANNEL_TYPE_CLK_2EDGE:		// fall-through
		// no ack signal
		break;
#endif
	case CHANNEL_TYPE_1ofN:	// fall-through
#if PRSIM_CHANNEL_BUNDLED_DATA
	case CHANNEL_TYPE_BD_2P: // fall-through
	case CHANNEL_TYPE_BD_4P: // fall-through
#endif
	case CHANNEL_TYPE_LEDR:
		INVARIANT(ack_signal);
		// ack-less cannot be configured as source/sink
		events.push_back(ENV_EVENT(ack_signal, 
			(get_ack_init() ? LOGIC_HIGH
				: LOGIC_LOW)));
		// use global timing policy
		break;
	case CHANNEL_TYPE_SINGLE_TRACK:
		FINISH_ME_EXIT(Fang);
		break;
	default:
		DIE;
	}	// end switch
	}
	// else nothing else to do
	stop();	// freeze this channel until it is resumed
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For channel sources only.  
	Called by channel::reset()
 */
void
channel::initialize_all_data_rails(vector<env_event_type>& events) {
switch (type) {
case CHANNEL_TYPE_SINGLE_TRACK:	// fall-through
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_1EDGE:	// fall-through
case CHANNEL_TYPE_CLK_2EDGE:	// fall-through
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_2P: // fall-through
case CHANNEL_TYPE_BD_4P: // fall-through
	reset_bundled_data_rails(events);
	break;
#endif
case CHANNEL_TYPE_1ofN:
	reset_all_data_rails(events);
	break;
case CHANNEL_TYPE_LEDR:
	events.push_back(__node_setter(
		get_data_init() ? LOGIC_HIGH : LOGIC_LOW)(ledr_data_rail()));
	events.push_back(__node_setter(
		get_repeat_init() ? LOGIC_HIGH : LOGIC_LOW)(repeat_rail()));
	break;
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_SRC1:		// fall-through
case CHANNEL_TYPE_CLK_SRC2:		// fall-through
	break;
#endif
default:
	DIE;
}	// end switch
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
			++iteration;
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
#if 0
// unused
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Responds to an (active) acknowledge by neutralizing data rails
	in a return-to-zero protocol.
	For 4-phase protocols, this means returning data rail bundles to
	the NULL state.
	For 2-phase protocols, this does nothing!
 */
inline
void
channel::reset_all_data_rails(vector<env_event_type>& events) {
switch (type) {
case CHANNEL_TYPE_SINGLE_TRACK:
case CHANNEL_TYPE_1ofN:
	transform(data.begin(), data.end(), back_inserter(events),
		__node_setter(get_data_sense() ? LOGIC_HIGH : LOGIC_LOW));
	break;
default:
// case CHANNEL_TYPE_LEDR:
// two phase, LEDR: no resetting
#if PRSIM_CHANNEL_SYNC
// case CHANNEL_TYPE_CLK_SRC1:		// fall-through
// case CHANNEL_TYPE_CLK_SRC2:		// fall-through
// case CHANNEL_TYPE_CLK_1EDGE:
// case CHANNEL_TYPE_CLK_2EDGE:
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
// case CHANNEL_TYPE_BD_2P:
// case CHANNEL_TYPE_BD_4P:
#endif
	break;
}	// end switch
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Responds to an (active) acknowledge by neutralizing data rails
	in a return-to-zero protocol.
	For 4-phase protocols, this means returning data rail bundles to
	the NULL state.
	For 2-phase protocols, this does nothing!
 */
inline
void
channel::reset_bundled_data_rails(vector<env_event_type>& events) {
if (radix()) {
switch (type) {
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_1EDGE:		// fall-through
case CHANNEL_TYPE_CLK_2EDGE:		// fall-through
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_2P:
	// fall-through
case CHANNEL_TYPE_BD_4P:
#endif
	transform(data.begin(), data.end(), back_inserter(events),
		__node_setter(get_data_sense() ? LOGIC_HIGH : LOGIC_LOW));
	break;
// case CHANNEL_TYPE_SINGLE_TRACK:
// case CHANNEL_TYPE_1ofN:
default:
// case CHANNEL_TYPE_CLK_SRC1:		// fall-through
// case CHANNEL_TYPE_CLK_SRC2:		// fall-through
// case CHANNEL_TYPE_LEDR:
// two phase, LEDR: no resetting
	break;
}	// end switch
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This applies to all channel types.
 */
inline
void
channel::X_all_data_rails(vector<env_event_type>& events) {
	const __node_setter_decl(X_it, LOGIC_OTHER);
switch (type) {
case CHANNEL_TYPE_SINGLE_TRACK:	// fall-through
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_1EDGE:		// fall-through
case CHANNEL_TYPE_CLK_2EDGE:		// fall-through
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_2P: // fall-through
case CHANNEL_TYPE_BD_4P: // fall-through
#endif
case CHANNEL_TYPE_1ofN:
	transform(data.begin(), data.end(), back_inserter(events), X_it);
	break;
case CHANNEL_TYPE_LEDR:
	events.push_back(X_it(ledr_data_rail()));
	events.push_back(X_it(repeat_rail()));
	break;
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_SRC1:		// fall-through
case CHANNEL_TYPE_CLK_SRC2:		// fall-through
	break;
#endif
default:
	DIE;
}	// end switch
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        \return an event for toggling the value of node.
	Technically shouldn't bother with X nodes, as inverse of X is X, 
	but will get filtered out.
 */
env_event_type
channel::toggle_node(const State& s, const node_index_type ni) const {
        const value_enum rv(s.get_node(ni).current_value());
	return ENV_EVENT(ni, NodeState::invert_value[size_t(rv)]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Applicable to 1ofN and single-track codes only.
	Similar to current_data_rails, but emits events for setting
	all data rails high or low, according to the current value.  
	If there is no current value, reset all data rails.  
	This is useful for coming out of an uninitialized state (resume).
	When run out of data values, clear the sourcing flag, 
	and clear the value array.  
	\param s state needed for parity-encoded LEDR channels.
 */
void
channel::set_all_data_rails(const State& s, vector<env_event_type>& r) {
	STACKTRACE_BRIEF;
	typedef	State::node_type		node_type;
if (have_value()) {
switch (type) {
case CHANNEL_TYPE_SINGLE_TRACK:
case CHANNEL_TYPE_1ofN: {
	const int_value_type rdx = radix();
	// NOTE: div is *signed*
	data_rail_index_type k;
if (rdx == 2) {
	// optimize for binary, and handle signedness
	k[0] = 0;
	value_type v = DATA_VALUE(current_value());
	for ( ; k[0] < bundles(); ++k[0]) {
		k[1] = 0;
		const node_index_type ni0 = data[k];	// node index
		k[1] = 1;
		const node_index_type ni1 = data[k];	// node index
		const value_enum b =
			((v & 0x1) ^ get_data_sense()) ? LOGIC_HIGH : LOGIC_LOW;
		const value_enum nb = node_type::invert_value[size_t(b)];
		// TODO: should neutralize data-rail first?
		// to prevent momentary violation of exclusion?
		// don't need to worry for RTZ protocols
		r.push_back(ENV_EVENT(ni0, nb));
		r.push_back(ENV_EVENT(ni1, b));
		v >>= 1;
	}
} else {
	div_type<int_value_type>::return_type qr;
	qr.quot = DATA_VALUE(current_value());
	qr.rem = 0;	// unused
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
}
	break;
}
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_1EDGE:		// fall-through
case CHANNEL_TYPE_CLK_2EDGE:		// fall-through
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_2P:		// fall-through
case CHANNEL_TYPE_BD_4P: {
	data_rail_index_type k;
	const size_t rdx = radix();
	k[0] = 0;
	value_type v = DATA_VALUE(current_value());
	for (k[1]=0; k[1] < rdx; ++k[1]) {
		const node_index_type ni = data[k];	// node index
		const value_enum b =
			((v & 0x1) ^ get_data_sense()) ? LOGIC_HIGH : LOGIC_LOW;
		r.push_back(ENV_EVENT(ni, b));
		v >>= 1;
	}
	break;
}
#endif
case CHANNEL_TYPE_LEDR: {
	// TODO: eventually support wider bundled ledr values
	const node_index_type& dn(ledr_data_rail());
#if PRSIM_CHANNEL_RAILS_INVERTED
	// LSB
	const bool v = (DATA_VALUE(current_value()) & 0x01) ^ get_data_sense();
#else
	const bool v = DATA_VALUE(current_value()) & 0x01;	// LSB
#endif
	switch (s.get_node(dn).current_value()) {
	case LOGIC_LOW:
		if (v) {
			// different value, toggle data rail
			r.push_back(ENV_EVENT(dn, LOGIC_HIGH));
		} else {
			// same value as before, toggle repeat
			r.push_back(toggle_node(s, repeat_rail()));
		}
		break;
	case LOGIC_HIGH:
		if (v) {
			// same value as before, toggle repeat
			r.push_back(toggle_node(s, repeat_rail()));
		} else {
			// different value, toggle data rail
			r.push_back(ENV_EVENT(dn, LOGIC_LOW));
		}
		break;
	default: break;	// do nothing, silently
	}
	break;
}
default: DIE;
}	// end switch
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
}	// end set_all_data_rails

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
	if (x_counter) {
		return false;
	}
	switch (type) {
	case CHANNEL_TYPE_1ofN: return (counter_state == bundles());
#if PRSIM_CHANNEL_SYNC
	case CHANNEL_TYPE_CLK_1EDGE:		// fall-through
	case CHANNEL_TYPE_CLK_2EDGE:		// fall-through
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
	// data can always be sampled in any phase of handshake
	case CHANNEL_TYPE_BD_2P:
	case CHANNEL_TYPE_BD_4P:
#endif
	case CHANNEL_TYPE_LEDR: return true;
//	case CHANNEL_TYPE_CLK_SRC1:		// fall-through
//	case CHANNEL_TYPE_CLK_SRC2:		// fall-through
	default: break;
	}
	// should not be reached
	return false;
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
	STACKTRACE_BRIEF;
	typedef	State::node_type	node_type;
if (!radix()) {
	return 0;			// data-less, don't care
}
	value_type ret = 0;
switch (type) {
case CHANNEL_TYPE_SINGLE_TRACK:	// use 1ofN
case CHANNEL_TYPE_1ofN: {
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
	break;
}
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_SRC1:		// fall-through
case CHANNEL_TYPE_CLK_SRC2:		// fall-through
	return 0;			// data-less
case CHANNEL_TYPE_CLK_1EDGE:		// fall-through
case CHANNEL_TYPE_CLK_2EDGE:		// fall-through
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_2P:		// fall-through
case CHANNEL_TYPE_BD_4P: {
	data_rail_index_type k;
	k[0] = 0;		// no bundles
	k[1] = radix();		// bus width
	ret = 0;
	do {
		ret <<= 1;
		--k[1];
		bool have_hi = false;
		const node_type& n(s.get_node(data[k]));
		switch (n.current_value()) {
		case LOGIC_LOW:
		if (get_data_sense()) {
			have_hi = true;
		}
			break;
		case LOGIC_HIGH:
		if (!get_data_sense()) {
			have_hi = true;
		}
			break;
		default: DIE;
		}
		if (have_hi) {
			ret |= 0x1;
		}
	} while (k[1]);
	break;
}
#endif
case CHANNEL_TYPE_LEDR:
	// value is that of just the data rail
#if PRSIM_CHANNEL_RAILS_INVERTED
	ret = ((s.get_node(ledr_data_rail()).current_value() == LOGIC_LOW) ^ get_data_sense() ? 0 : 1);
#else
	ret = ((s.get_node(ledr_data_rail()).current_value() == LOGIC_LOW) ? 0 : 1);
#endif
	break;
default: DIE;
}	// end switch
#if PRSIM_CHANNEL_SIGNED
	// possible sign extension
	if (is_signed()) {
		INVARIANT(radix() == 2 || data_is_bundled());
		// detect sign bit
		const size_t width = data_is_bundled() ? radix() : bundles();
		const bool sign = ret & (1 << (width -1));
		if (sign) {
			// then sign extend negative number
			ret |= (int_value_type(-1) << width);
		}
	}
#endif
	return ret;
}	// end channel::data_rails_value()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel::print_data_value(ostream& o, const value_type v) const {
#if PRSIM_CHANNEL_SIGNED
	if (is_signed()) {
		return o << signed_value_type(v);
	} else
#endif
		return o << v;
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
		if (valid_signal && (ni == valid_signal)
#if PRSIM_CHANNEL_SYNC
				&& !is_clock_source()
#endif
				) {
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
		switch (type) {
		case CHANNEL_TYPE_1ofN:
		if (valid_signal && is_data_rail) {
			// for ev-handshake channels:
			// source's validity must respond to its own data rails
			return true;
		}
			break;
#if PRSIM_CHANNEL_SYNC
		// clock sources respond to self
		case CHANNEL_TYPE_CLK_SRC1:		// fall-through
		case CHANNEL_TYPE_CLK_SRC2:		// fall-through
		// synchronous sources respond to clock when sourcing
		case CHANNEL_TYPE_CLK_1EDGE:		// fall-through
		case CHANNEL_TYPE_CLK_2EDGE:		// fall-through
			return (ni == clock_signal());
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
		case CHANNEL_TYPE_BD_2P:
		case CHANNEL_TYPE_BD_4P:
		// bundled-data sources do not respond to own data rails
		// fall-through
#endif
		default: break;
		}
	}
	if (is_sinking()) {
		if (valid_signal) {
			// ack only responds to validity, not data rails
			// this also applies to bundled-data channels
			// this also applies to clocked channels
			if (ni == valid_signal)
				return true;
		} else {
			// if there is no validity or request rail
			// ack responds directly to data rails
			if (is_data_rail && !valid_signal) {
				return true;
			}
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
	\param ni the node driven by channel to query.
	\param limit is the recursion limit.
	\param dir is the direction of the node being checked.
	\param why_not is true to ask 'why-not'?
	\param verbose is true to pretty-print trees.
	\param u, v are just visited sets of nodes (stack, seen)
 */
ostream&
channel::__node_why_not(const State& s, ostream& o, const node_index_type ni, 
		const size_t limit, const bool dir, 
		const bool why_not, const bool verbose, 
		node_set_type& u, node_set_type& v) const {
	typedef	State::node_type		node_type;
	const indent __ind_outer(o, verbose ? " " : "");
	// const node_type& n(s.get_node(ni));
if (is_sourcing()) {
	if (stopped()) {
	o << auto_indent << "(channel " << name << " is stopped.)" << endl;
	} else {
	switch (type) {
	case CHANNEL_TYPE_1ofN: {
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
		break;
	}
	case CHANNEL_TYPE_LEDR: {
		// channel is empty
#if 0
		const value_enum p = current_ledr_parity(s);
		bool empty = false;
		switch (p) {
		case LOGIC_OTHER:
			break;
		default:
			empty = (p == LOGIC_HIGH) ^ ledr_empty_parity();
			break;
		}
#endif
#if 0
		// channel is full
		s.__node_why_not(o, ack_signal, limit, 
			dir ^ , 
			why_not, verbose, u, v);
#endif
		FINISH_ME(Fang);
		break;
	}
#if PRSIM_CHANNEL_SYNC
	case CHANNEL_TYPE_CLK_SRC1:		// fall-through
	case CHANNEL_TYPE_CLK_SRC2:		// fall-through
	case CHANNEL_TYPE_CLK_1EDGE:
	case CHANNEL_TYPE_CLK_2EDGE:
		FINISH_ME(Fang);
		break;
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
	case CHANNEL_TYPE_BD_2P:
	case CHANNEL_TYPE_BD_4P:
		FINISH_ME(Fang);
		break;
#endif
	case CHANNEL_TYPE_SINGLE_TRACK:
		FINISH_ME(Fang);
		break;
	default:
		break;
	}	// end switch
	}
}
if (is_sinking() && ack_signal && (ni == ack_signal)) {
	// no other signal should be driven by sink
if (stopped()) {
	o << auto_indent << "(channel " << name << " is stopped.)" << endl;
} else {
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
}
}	// end if sinking
	return o;
}	// end channel::__node_why_not

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Asks why the data rails are/not in their present state.
	This analysis will depend on the sense of the data signals.  
	4-phase only for now.
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
	4-phase only for now.
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
	4-phase only for now.
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
	If any participating rails are X, return LOGIC_OTHER.
	LOGIC_HIGH is parity=1, LOGIC_LOW is parity=0.
 */
value_enum
channel::current_ledr_parity(const State& s) const {
	bool parity = false;
if (ack_signal) {
	switch (s.get_node(ack_signal).current_value()) {
	case LOGIC_HIGH:
		parity = !parity;
		break;
	case LOGIC_LOW:
		break;
	// case LOGIC_OTHER:
	default:
		return LOGIC_OTHER;
	}
}
	// parity is unaffected by sense of data rail
	switch (s.get_node(ledr_data_rail()).current_value()) {
	case LOGIC_HIGH:
		parity = !parity;
		break;
	case LOGIC_LOW:
		break;
	// case LOGIC_OTHER:
	default:
		return LOGIC_OTHER;
	}
	switch (s.get_node(repeat_rail()).current_value()) {
	case LOGIC_HIGH:
		parity = !parity;
		break;
	case LOGIC_LOW:
		break;
	// case LOGIC_OTHER:
	default:
		return LOGIC_OTHER;
	}
	return parity ? LOGIC_HIGH : LOGIC_LOW;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_BUNDLED_DATA
/**
	If any participating rails are X, return LOGIC_OTHER.
	LOGIC_HIGH is parity=1, LOGIC_LOW is parity=0.
	FIXME: The X overrule may prohibit use of this when
		one of the signals is initially X.
 */
value_enum
channel::current_bd2p_parity(const State& s) const {
	bool parity = false;
	switch (s.get_node(ack_signal).current_value()) {
	case LOGIC_HIGH:
		parity = !parity;
		break;
	case LOGIC_LOW:
		break;
	// case LOGIC_OTHER:
	default:
		return LOGIC_OTHER;
	}
	switch (s.get_node(valid_signal).current_value()) {
	case LOGIC_HIGH:
		parity = !parity;
		break;
	case LOGIC_LOW:
		break;
	// case LOGIC_OTHER:
	default:
		return LOGIC_OTHER;
	}
	return parity ? LOGIC_HIGH : LOGIC_LOW;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/**
	Convenience function for bundled-data sources.
	\pre have_value() is true
	This sets data rails and request rail.
 */
void
channel::set_bd_data_req(const State& s, const status_summary& stat,
		vector<env_event_type>& new_events) {
	STACKTRACE_BRIEF;
	// set data rails to next data value
	set_all_data_rails(s, new_events);
	advance_value();
	value_enum reqv = LOGIC_OTHER;
	switch (type) {
	case CHANNEL_TYPE_BD_2P: {
		if (!stat.x_ack) {
			reqv = s.get_node(ack_signal).current_value();
			if (!bd2p_empty_parity()) {
				reqv = NodeState::invert_value[size_t(reqv)];
			}
		}
		break;
	}
	case CHANNEL_TYPE_BD_4P:
		reqv = get_valid_sense() ? LOGIC_HIGH : LOGIC_LOW;
		break;
	default:
		DIE;
		break;
	}
	// followed by setting request
	if (new_events.empty()) {
		// possible if next data token has same value
		// then use any delay value
		new_events.push_back(ENV_EVENT(valid_signal, reqv));
	} else {
		// must guarantee request is last
		// use delay equal to max of data rail events
		const vector<env_event_type>::const_iterator
			i(new_events.begin()), 
			e(new_events.end());
		const vector<env_event_type>::const_iterator
			m(std::max_element(i, e));
		new_events.push_back(env_event_type(valid_signal, 
			reqv, m->delay));
	}
}
#endif	// PRSIM_CHANNEL_BUNDLED_DATA

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is the primary callback function for channel activity.  
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
	TODO: we current forbid aliases between rails, 
	but it is conceivable to process each node for every repeated 
	occurrence in the rail set (future).  
 */
void
channel::process_node(const State& s, const node_index_type ni, 
		const value_enum prev, const value_enum next, 
		vector<env_event_type>& new_events) throw(channel_exception) {
	STACKTRACE_BRIEF;
	typedef	State::node_type	node_type;
#if ENABLE_STACKTRACE
	cout << s.get_node_canonical_name(ni) << " : " << size_t(prev) << 
		" -> " << size_t(next) << endl;
#endif
	// can't call summarize_status until x_counter and counter_state
	// have been updated (data-rails, below)
switch (type) {
// case CHANNEL_TYPE_SINGLE_TRACK: ?
case CHANNEL_TYPE_1ofN: {
// first identify which channel node member this node is
if (ack_signal && (ni == ack_signal)) {
	STACKTRACE_INDENT_PRINT("got ack update" << endl);
	const status_summary stat(summarize_status(s));
	// only need to take action if this is a source
	if (is_sourcing() && !stopped()) {
	STACKTRACE_INDENT_PRINT("source responding..." << endl);
	if (stat.x_ack) {
		// set all data to X
		// do not advance
		X_all_data_rails(new_events);
	} else if (stat.ack_active) {
		reset_all_data_rails(new_events);
	} else {	// is neg-ack
		// \pre all data rails are neutral
		// set data rails to next data value
		set_all_data_rails(s, new_events);
		advance_value();
	}
	}
	// logging and expect mode don't care
} else if (valid_signal && (ni == valid_signal)) {
	STACKTRACE_INDENT_PRINT("1ofN: got validity update" << endl);
	const status_summary stat(summarize_status(s));
	if (!stat.x_valid && stat.valid_active) {
		process_data(s);
	}
	// only need to take action if this is a sink
	if (is_sinking() && !stopped()) {
		INVARIANT(ack_signal);
		if (stat.x_valid) {
			new_events.push_back(ENV_EVENT(ack_signal, LOGIC_OTHER));
		} else if (stat.valid_active) {
			// valid, ack
			new_events.push_back(ENV_EVENT(ack_signal, 
				get_ack_active() ? LOGIC_HIGH : LOGIC_LOW));
		} else {
			// neutral, reset ack
			new_events.push_back(ENV_EVENT(ack_signal, 
				get_ack_active() ? LOGIC_LOW : LOGIC_HIGH));
		}
	}
} else {
	STACKTRACE_INDENT_PRINT("got data-rail update" << endl);
	// invariant: must be data rail
	// update state counters
	update_data_counter(prev, next);
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
	break;
}	// end case CHANNEL_TYPE_1ofN
case CHANNEL_TYPE_LEDR: {
	const bool ep = ledr_empty_parity();
if (ack_signal && (ni == ack_signal)) {
	STACKTRACE_INDENT_PRINT("ledr: got ack update" << endl);
	// 2-phase, respond on either edge of ack
	if (is_sourcing() && !stopped()) {
	// check current parity vs. empty parity to determine action
	bool empty = false;
	switch (current_ledr_parity(s)) {
		// assumes that data rails are active high
	case LOGIC_LOW:
		if (!ep) empty = true;
		// else warn that acknowledge changed while channel was full
		break;
	case LOGIC_HIGH:
		if (ep) empty = true;
		break;
	default:
		// set all data to X
		// do not advance
		X_all_data_rails(new_events);
		break;
	}
	if (empty) {
		set_all_data_rails(s, new_events);
		advance_value();
	}
	// else 2-phase does not have a reset phase on full, do nothing
	}	// end if sourcing
} else {
	INVARIANT(ni == repeat_rail() || ni == ledr_data_rail());
	STACKTRACE_INDENT_PRINT("ledr: got data or repeat-rail update" << endl);
	switch (prev) {
	case LOGIC_OTHER: --x_counter; break;
	default: break;
	}
	switch (next) {
	case LOGIC_OTHER: ++x_counter; break;
	default: break;
	}	// end switch
	// an acknowledgeless LEDR (1-phase) source just continuously
	// sources values using the channel timing.  
	if (!ack_signal && is_sourcing() && !stopped()) {
	if (!x_counter) {
		set_all_data_rails(s, new_events);
		advance_value();
	} else {
		X_all_data_rails(new_events);
	}
	}
	if (ack_signal && is_sinking() && !stopped()) {
	// reply with ack
	if (x_counter) {
		// propagate Xs
		new_events.push_back(ENV_EVENT(ack_signal, LOGIC_OTHER));
	} else {
		// toggle the ack (2-phase)
		new_events.push_back(toggle_node(s, ack_signal));
	}	// end switch
	}
	// else there is no ack, then source is single-phase continuous source
	// watch, log, expect...
	process_data(s);
}
	break;
}	// end case CHANNEL_TYPE_LEDR
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_SRC1:
case CHANNEL_TYPE_CLK_SRC2: {
	STACKTRACE_INDENT_PRINT("clk-src: got clock tick" << endl);
	const status_summary stat(summarize_status(s));
	INVARIANT(ni == clock_signal());
	if (is_sourcing() && !stopped() && have_value()) {
		if (!stat.x_valid && 
			(type == CHANNEL_TYPE_CLK_SRC2 || stat.valid_active)) {
			advance_value();
		}
		new_events.push_back(toggle_node(s, clock_signal()));
	}
	break;
}
case CHANNEL_TYPE_CLK_2EDGE: {
// first identify which channel node member this node is
if (ni == clock_signal()) {
	STACKTRACE_INDENT_PRINT("clk2: got clk update" << endl);
	const status_summary stat(summarize_status(s));
	if (!stat.x_valid) {
		// check data on both clock edges
		process_data(s);	// check expects
	}
	// only need to take action if this is a source
	if (is_sourcing() && !stopped()) {
	STACKTRACE_INDENT_PRINT("source responding..." << endl);
	// FIXME: doesn't handle 0-X-0 or 1-X-1 transitions (undefined)
	if (stat.x_valid) {
		STACKTRACE_INDENT_PRINT("X-ing all data." << endl);
		// set all data to X, also request rail
		// do not advance
		X_all_data_rails(new_events);
	} else {
		STACKTRACE_INDENT_PRINT("setting up data." << endl);
		// clock is double-edged, setup data
		if (have_value()) {
			// set data rails to next data value
			set_all_data_rails(s, new_events);
			advance_value();
		}
	}
	}
	// logging and expect mode don't care
	// synchronous channels don't sink
} else {
	STACKTRACE_INDENT_PRINT("bd4p: got data-rail update" << endl);
	// invariant: must be data rail
	// update state counters
	update_bd_data_counter(prev, next);
	// generally, no need to spawn any events after data
	// which can always be transient
}
	break;
}
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_2P: {
if (ni == ack_signal) {
	// much of this code copied from LEDR above, re-factor later
	STACKTRACE_INDENT_PRINT("bd2p: got ack update" << endl);
	const status_summary stat(summarize_status(s));
	// 2-phase, respond on either edge of ack
	if (is_sourcing() && !stopped()) {
	// check current parity vs. empty parity to determine action
	if (stat.x_ack) {
		X_all_data_rails(new_events);
		new_events.push_back(ENV_EVENT(valid_signal, LOGIC_OTHER));
	} else if (!stat.full) {
		if (have_value()) {
			set_bd_data_req(s, stat, new_events);
		}
	}
	// else 2-phase does not have a reset phase on full, do nothing
	}	// end if sourcing
} else if (ni == valid_signal) {
	STACKTRACE_INDENT_PRINT("bd2p: got validity update" << endl);
	const status_summary stat(summarize_status(s));
	if (!stat.x_valid && stat.full) {
		process_data(s);
	}
	// only need to take action if this is a sink
	if (is_sinking() && !stopped()) {
		if (ack_signal) {
		if (stat.x_valid) {
			new_events.push_back(ENV_EVENT(ack_signal, LOGIC_OTHER));
		} else {
			new_events.push_back(toggle_node(s, ack_signal));
		}
		}	// else synchronous channel has no ack
	}
} else {
	// is a data-rail
	STACKTRACE_INDENT_PRINT("bd2p: got data rail update" << endl);
	update_bd_data_counter(prev, next);
	// nothing responds directly to data-rail
}
	break;
}	// end case CHANNEL_TYPE_BD_2P
#endif
// follows similarly to 1ofN case (ev-handshake)
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_1EDGE: {
// first identify which channel node member this node is
if (ni == clock_signal()) {
	STACKTRACE_INDENT_PRINT("clk1: got clk update" << endl);
	const status_summary stat(summarize_status(s));
	if (!stat.x_valid && stat.valid_active) {
		process_data(s);	// check expects
	}
	// only need to take action if this is a source
	if (is_sourcing() && !stopped()) {
	STACKTRACE_INDENT_PRINT("source responding..." << endl);
	if (stat.x_valid) {
		STACKTRACE_INDENT_PRINT("X-ing all data." << endl);
		// set all data to X, also request rail
		// do not advance
		X_all_data_rails(new_events);
	} else if (stat.valid_active) {
		STACKTRACE_INDENT_PRINT("holding data." << endl);
		// reset_all_data_rails(new_events);	// does nothing
	} else {
		STACKTRACE_INDENT_PRINT("setting up data." << endl);
		// clock is in inactive phase, setup data
		if (have_value()) {
			// set data rails to next data value
			set_all_data_rails(s, new_events);
			advance_value();
		}
	}
	}
	// logging and expect mode don't care
	// synchronous channels don't sink
} else {
	STACKTRACE_INDENT_PRINT("bd4p: got data-rail update" << endl);
	// invariant: must be data rail
	// update state counters
	update_bd_data_counter(prev, next);
	// generally, no need to spawn any events after data
	// which can always be transient
}
	break;
}
#endif
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_4P: {
// first identify which channel node member this node is
if (ni == ack_signal) {
	STACKTRACE_INDENT_PRINT("bd4p: got ack update" << endl);
	const status_summary stat(summarize_status(s));
	// only need to take action if this is a source
	if (is_sourcing() && !stopped()) {
	STACKTRACE_INDENT_PRINT("source responding..." << endl);
	if (stat.x_ack) {
		// set all data to X, also request rail
		// do not advance
		X_all_data_rails(new_events);
		new_events.push_back(ENV_EVENT(valid_signal, LOGIC_OTHER));
	} else if (stat.ack_active) {
		reset_all_data_rails(new_events);	// does nothing
		// followed by resetting request
		new_events.push_back(ENV_EVENT(valid_signal, 
			get_valid_sense() ? LOGIC_LOW : LOGIC_HIGH));
	} else {
		if (timing_mode == CHANNEL_TIMING_GLOBAL &&
				s.timing_is_randomized()) {
			cerr << bundled_data_global_timing_warning << endl;
		}
		if (have_value()) {
			// set data rails to next data value
			set_bd_data_req(s, stat, new_events);
		}
	}
	}
	// logging and expect mode don't care
} else if (ni == valid_signal) {
	STACKTRACE_INDENT_PRINT("bd4p: got validity update" << endl);
	const status_summary stat(summarize_status(s));
	if (!stat.x_valid && stat.valid_active) {
		process_data(s);
	}
	// only need to take action if this is a sink
	if (is_sinking() && !stopped()) {
		if (ack_signal) {
		if (stat.x_valid) {
			new_events.push_back(ENV_EVENT(ack_signal, LOGIC_OTHER));
		} else if (stat.valid_active) {
			// valid, ack
			new_events.push_back(ENV_EVENT(ack_signal, 
				get_ack_active() ? LOGIC_HIGH : LOGIC_LOW));
		} else {
			// neutral, reset ack
			new_events.push_back(ENV_EVENT(ack_signal, 
				get_ack_active() ? LOGIC_LOW : LOGIC_HIGH));
		}
		}	// else synchronous channel is ack-less
	}
} else {
	STACKTRACE_INDENT_PRINT("bd4p: got data-rail update" << endl);
	// invariant: must be data rail
	// update state counters
	update_bd_data_counter(prev, next);
	// generally, no need to spawn any events after data
	// which can always be transient
}
	break;
}	// end case CHANNEL_TYPE_BD_4P
#endif	// PRSIM_CHANNEL_BUNDLED_DATA
default: DIE;
}	// end switch
}	// end channel::process_node

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_BUNDLED_DATA || PRSIM_CHANNEL_SYNC
/**
	\param prev previous value of data rail
	\param next new value of data rail
 */
void
channel::update_bd_data_counter(const value_enum prev, const value_enum next) {
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
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	React to data value.
	This is written generically for all channel types.
	Print it if it is watched, log it if it is logged.  
	Check it if it is expected.  
	Suppress if ignored.
	TODO: what to do if data is in bad state (validity protocol only).
 */
void
channel::process_data(const State& s) throw (channel_exception) {
	STACKTRACE_BRIEF;
	const bool v = data_is_valid();
	if (watched()) {
		// even if channel is ignored?
		cout << "channel\t" << name << " (.data) : ";
		if (v) {
			print_data_value(cout, data_rails_value(s));
		} else {	// data is in invalid state
			cout << 'X';
		}
		if (report_time) cout << " @ " << s.time();
		if (ignored()) {
			cout << " (ignored)";
		}
		cout << endl;
	}
	if (!ignored() && dumplog.stream && *dumplog.stream) {
		// TODO: format me, hex, dec, bin, etc...
		// should be able to just setbase()
		if (v || data_is_bundled()) {
		print_data_value(*dumplog.stream, data_rails_value(s));
		} else {
		(*dumplog.stream) << 'X';
		}
		if (report_time) (*dumplog.stream) << " @ " << s.time();
		(*dumplog.stream) << endl;
		// really flush every line?
	}
	if (is_expecting() && !ignored()) {
		STACKTRACE_INDENT_PRINT("expecting and not ignored" << endl);
	if (have_value()) {
		STACKTRACE_INDENT_PRINT("have value" << endl);
		const array_value_type& expect = current_value();
		const size_t cur_index = current_index();	// save it
		const size_t cur_iter = iteration;	// save it
		if (!expect.second) {
		STACKTRACE_INDENT_PRINT("expect value is not X" << endl);
		const error_policy_enum e(s.get_channel_expect_fail_policy());
		if (v) {
	// FIXME: bundled-data: data may be valid, but X!
		STACKTRACE_INDENT_PRINT("channel data is valid" << endl);
		const value_type got = data_rails_value(s);
		advance_value();
		// want to advance after checking, 
		// but need to guarantee progress in the event of exception
		if (!__assert_channel_value(DATA_VALUE(expect),
				got, x_counter, s.confirm_asserts())) {
			STACKTRACE_INDENT_PRINT("data mismatch" << endl);
			const channel_exception
				ex(this, cur_index, cur_iter,
					DATA_VALUE(expect), got, x_counter);
			if (e == ERROR_WARN) {
				ex.inspect(s, cout);
			} else if (e > ERROR_WARN) {
#if PRSIM_AGGREGATE_EXCEPTIONS
				s.record_exception(State::exception_ptr_type(
					new channel_exception(ex)));
#else
				throw ex;
#endif
			}
		}
		} else {	// cannot expect invalid value
			STACKTRACE_INDENT_PRINT("channel data is invalid" << endl);
			const channel_exception
				ex(this, cur_index, cur_iter,
					DATA_VALUE(expect), 0xDEADBEEF, true);
			if (e == ERROR_WARN) {
				ex.inspect(s, cout);
			} else if (e > ERROR_WARN) {
#if PRSIM_AGGREGATE_EXCEPTIONS
				s.record_exception(State::exception_ptr_type(
					new channel_exception(ex)));
#else
				throw ex;
#endif
			}
		}
		} else {	// else don't care
			// don't print confirmed values for don't cares
			STACKTRACE_INDENT_PRINT("expect is X (don't care)" << endl);
			if (v || data_is_bundled()) {
				// on valid data, advance
				STACKTRACE_INDENT_PRINT("advancing value" << endl);
				advance_value();
			}
		}
		// configure to stop sinking on empty expect (2-phase)
		if (two_phase() && !have_value() &&
				is_sinking() && stopping_on_empty()) {
			stop();
		}
	} else {
		// exhausted values, disable expecting
		flags &= ~CHANNEL_EXPECTING;
		// might as well release memory...
		if (!values.empty()) {
			values.clear();
		}
		// configure to stop sinking on empty expect (4-phase)
		if (four_phase() && is_sinking() && stopping_on_empty()) {
			stop();
		}
	}
	}	// end if is_expecting
}	// end method process_data

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	respond to 4-phase channel sink by toggling acknowledge.
	Applicable to:
		CHANNEL_TYPE_1ofN, CHANNEL_TYPE_BD_4P
	Refactored out from channel::resume().
 */
void
channel::__resume_4p_sink(const status_summary& stat,
		vector<env_event_type>& events) {
if (is_sinking()) {
	INVARIANT(ack_signal);
	if (valid_signal) {
		// only react to the valid signal
		if (stat.x_valid) {
			events.push_back(ENV_EVENT(ack_signal, LOGIC_OTHER));
		} else if (stat.valid_active) {
			// ack
			events.push_back(ENV_EVENT(ack_signal, 
				get_ack_active() ? LOGIC_HIGH : LOGIC_LOW));
		} else {
			// reset ack
			events.push_back(ENV_EVENT(ack_signal, 
				get_ack_active() ? LOGIC_LOW : LOGIC_HIGH));
		}
	} else {
		// no validity rail, just react to data rails
	if (x_counter) {
		events.push_back(ENV_EVENT(ack_signal, LOGIC_OTHER));
	} else if (!counter_state) {
		// data is neutral, reset ack
		events.push_back(ENV_EVENT(ack_signal, 
			get_ack_active() ? LOGIC_LOW : LOGIC_HIGH));
	} else if (counter_state == bundles()) {
		// data is valid, ack
		events.push_back(ENV_EVENT(ack_signal, 
			get_ack_active() ? LOGIC_HIGH : LOGIC_LOW));
	}
	// else in some intermediate state, leave acknowledge alone
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Channel is no longer stopped, and is thus free to respond
	to its current state and spawn events.  
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
	const status_summary stat(summarize_status(s));
switch (type) {
case CHANNEL_TYPE_1ofN: {
if (is_sourcing()) {
	INVARIANT(ack_signal);
	// validity should be set after all data rails are valid/neutral
	if (stat.x_ack) {
		X_all_data_rails(events);
	} else if (stat.ack_active) {
		reset_all_data_rails(events);
	} else {
		if (counter_state && (counter_state != bundles())) {
			// ambiguous
			cerr << ambiguous_data << endl;
		}
		if ((counter_state != bundles() || x_counter)) {
			set_all_data_rails(s, events);
			advance_value();
		}
		if (valid_signal) {
			if (x_counter) {
				events.push_back(ENV_EVENT(valid_signal, 
					LOGIC_OTHER));
			} else if (!counter_state) {
				events.push_back(ENV_EVENT(valid_signal, 
					get_valid_sense() ?
						LOGIC_LOW : LOGIC_HIGH));
			} else if (counter_state == bundles()) {
				events.push_back(ENV_EVENT(valid_signal, 
					get_valid_sense() ?
						LOGIC_HIGH : LOGIC_LOW));
			}
			// else leave alone in intermediate state
		}
	}
}
// could also be sinking at the same time
	__resume_4p_sink(stat, events);
	break;
}	// end case
#if PRSIM_CHANNEL_BUNDLED_DATA
case CHANNEL_TYPE_BD_4P: {
	// should be very similar to ev-protocol
if (is_sourcing()) {
	INVARIANT(ack_signal);
	INVARIANT(valid_signal);
	// validity should be set after all data rails are valid/neutral
	if (stat.x_ack) {
		X_all_data_rails(events);
	} else if (stat.ack_active) {
		reset_all_data_rails(events);	// does nothing!
		// TODO: could optionally change data to X!?
		// request should be lowered after data is 'reset'
		events.push_back(ENV_EVENT(valid_signal, 
			get_valid_sense() ? LOGIC_LOW : LOGIC_HIGH));
	} else {
		// neg-ack: set all data rails to next data value
		// AND set the request/validity (which needs to happen *last*)
		// even in the event of random-timing
		if (timing_mode == CHANNEL_TIMING_GLOBAL &&
				s.timing_is_randomized()) {
			cerr << bundled_data_global_timing_warning << endl;
		}
		if (have_value()) {
			// set data rails to next data value
			set_bd_data_req(s, stat, events);
		}
	}
}
// could also be sinking at the same time
	__resume_4p_sink(stat, events);
	break;
}	// end case CHANNEL_TYPE_BD_4P
case CHANNEL_TYPE_BD_2P: {
	// very similar to LEDR, below
if (is_sourcing()) {
	// only respond if is currently at empty
	STACKTRACE_INDENT_PRINT("bd2p: releasing source" << endl);
	if (stat.x_ack) {
		STACKTRACE_INDENT_PRINT("bd2p: ack is X" << endl);
		X_all_data_rails(events);
		events.push_back(ENV_EVENT(valid_signal, LOGIC_OTHER));
	} else if (!stat.full) {
		STACKTRACE_INDENT_PRINT("bd2p: channel is empty, so filling..." << endl);
		if (have_value()) {
			// set data rails to next data value
			set_bd_data_req(s, stat, events);
		}
	} else {
	//	possibly issue a warning if channel resumed in wrong phase?
	}
}
if (is_sinking()) {
	STACKTRACE_INDENT_PRINT("bd2p: releasing sink" << endl);
	// check for full parity
	if (stat.x_valid) {
		STACKTRACE_INDENT_PRINT("bd2p: req is X" << endl);
		events.push_back(ENV_EVENT(ack_signal, LOGIC_OTHER));
	} else if (stat.full) {
		STACKTRACE_INDENT_PRINT("bd2p: channel is full, so emptying..." << endl);
		// Q: what if ack is currently X?
		events.push_back(toggle_node(s, ack_signal));
	} else {
	//	possibly issue a warning if channel resumed in wrong phase?
	}
	// else there's no acknowledge signal to toggle!
}
	break;
}	// end case CHANNEL_TYPE_BD_2P
#endif	// PRSIM_CHANNEL_BUNDLED_DATA
#if PRSIM_CHANNEL_SYNC
case CHANNEL_TYPE_CLK_SRC1:
case CHANNEL_TYPE_CLK_SRC2:
	// copied from process_node, above
	if (is_sourcing() && have_value()) {
		if (!stat.x_valid && 
			(type == CHANNEL_TYPE_CLK_SRC2 || stat.valid_active)) {
			advance_value();
		}
		events.push_back(toggle_node(s, clock_signal()));
	}
	break;
case CHANNEL_TYPE_CLK_1EDGE:
case CHANNEL_TYPE_CLK_2EDGE:
if (is_sourcing()) {
	// for single-edge clocks, respond when clock is in inactive phase
	// for double-edged clocks, always respond
	// never respond to X
	bool respond = !stat.x_valid &&
		(type == CHANNEL_TYPE_CLK_2EDGE || !stat.valid_active);
	if (respond) {
		set_all_data_rails(s, events);
		advance_value();
	}
}
	// sinking not applicable; there's no ack
	break;
#endif
case CHANNEL_TYPE_LEDR: {
if (is_sourcing()) {
	bool respond = true;	// whether or not to respond with valid data
	if (ack_signal) {
		const bool ep = ledr_empty_parity();
		const value_enum p(current_ledr_parity(s));
		// only respond if is currently at empty parity
		switch (p) {
		case LOGIC_LOW: if (ep) respond = false; break;
		case LOGIC_HIGH: if (!ep) respond = false; break;
		default:
			X_all_data_rails(events);
			respond = false;
			break;
		}
	}
	if (respond) {
		set_all_data_rails(s, events);
		advance_value();
	} else {
	//	possibly issue a warning if channel resumed in wrong phase?
	//	cerr << "Warning: 2-phase channel\'s acknowledge...";
	}
}
if (is_sinking() && ack_signal) {
	// check for full parity
	const bool fp = ledr_full_parity();
	const value_enum p(current_ledr_parity(s));
	bool respond = false;
	switch (p) {
	case LOGIC_LOW: if (!fp) respond = true; break;
	case LOGIC_HIGH: if (fp) respond = true; break;
	default:
		events.push_back(ENV_EVENT(ack_signal, LOGIC_OTHER));
	}
	if (respond) {
		events.push_back(toggle_node(s, ack_signal));
	} else {
//	possibly issue a warning if channel resumed in wrong phase?
	}
	// else there's no acknowledge signal to toggle!
}
}
	break;
case CHANNEL_TYPE_SINGLE_TRACK:
	FINISH_ME_EXIT(Fang);
	break;
default: DIE;
}	// end switch
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
	write_value(o, after_min);
	write_value(o, after_max);
	write_value(o, timing_probability);
#endif
	write_value(o, type);
	write_value(o, flags);
	write_value(o, counter_state);
	write_value(o, x_counter);
	write_value(o, bundles());	// size_t
	write_value(o, radix());	// size_t
	util::write_range(o, data.begin(), data.end());
	write_value(o, value_index);
	write_value(o, iteration);
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
	read_value(i, after_min);
	read_value(i, after_max);
	read_value(i, timing_probability);
#endif
	read_value(i, type);
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
	read_value(i, iteration);
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

/**
	Allocates and maps the channel data rails,
	and initializes the data counter state.
	\param rail_name if empty, means that channel is data-less!
	\return true if error
 */
bool
channel_manager::allocate_data_rails(State& state, const module& m,
	const size_t key /* channel index */,
	const string& bundle_name, const size_t _num_bundles,
	const string& rail_name, const size_t _num_rails) {
	// because 0 is used to denote scalars
	const size_t num_bundles = _num_bundles ? _num_bundles : 1;
	const size_t num_rails = _num_rails ? _num_rails : 1;
	channel& c(channel_pool[key]);
	const string& base(c.name);
	// allocate data rail references:
	set<size_t> rail_aliases;		// check for aliases (forbidden)
	channel::data_rail_index_type dk;
if (rail_name.length()) {
	dk[0] = num_bundles;
	dk[1] = num_rails;
	c.data.resize(dk);
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
			// may throw exception
			const node_index_type ni =
				parse_node_to_index(n.str(), m).index;
			if (ni) {
				c.data[dk] = ni;
				// flag node for consistency
				state.__get_node(ni).set_in_channel();
				c.__node_to_rail[ni] = dk;
				// lookup from node to channels
				node_channels_map[ni].insert(key);
				// check for rail uniqueness
				if (!rail_aliases.insert(ni).second) {
					cerr <<
"Error: channels rails are forbidden from aliasing (implementation limitation)."
						<< endl;
					cerr << "Aliased channel rail: "
						<< n.str() << endl;
					return true;
				}
			} else {
				cerr << "Error: no such node `" << n.str() <<
					"\' in channel." << endl;
				return true;
			}
		}	// end for each rail
	}	// end for each bundle
	// initialize data-rail state counters from current values
	c.initialize_data_counter(state);
} else {	// else skip, is data-less
	// explicitly resize to 0
	dk[0] = 0;
	dk[1] = 0;
	c.data.resize(dk);
}
	return false;
}	// end allocate_data_rails

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a channel, but leaves the acknowledgement and validity
	rails uninitialized.  
	TODO: this does not account for working directory prefix (ACX-PR-5414)
	\return true on error.
 */
bool
channel_manager::new_channel(State& state, const string& _base, 
		const string& bundle_name, const size_t _num_bundles, 
		const string& rail_name, const size_t _num_rails, 
		const bool active_low, 
		const bool have_ack, const bool ack_sense, const bool ack_init, 
		const bool have_validity, const bool validity_sense) {
	STACKTRACE_VERBOSE;
	// make sure base is a legitmate scalar channel name first
	const entity::module& m(state.get_module());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	parser::expanded_global_references_type refs;
	if (parser::expand_global_references(_base, m, refs)) {
		return true;
	}
	parser::expanded_global_references_type::const_iterator
		ri(refs.begin()), re(refs.end());
for ( ; ri!=re; ++ri) {
	const entity::global_indexed_reference& g(ri->second);
	ostringstream oss;
//	cr.inst_ref()->dump(oss, expr_dump_context::default_value);
	ri->first->dump(oss);
	// base name needs to expanded to scalar reference strings...
	const string& base(oss.str());
#else
	const string& base(_base);
	const entity::global_indexed_reference
		g(parser::parse_global_reference(base, m));
#endif
	if (g.first != entity::META_TYPE_PROCESS || !g.second) {
		cerr << "Error: base reference is not a valid channel." << endl;
		return true;
	}
	const size_t key = channel_pool.size();
	const pair<channel_set_type::iterator, bool>
		i(channel_index_set.insert(make_pair(base, key)));
	if (!i.second) {
		cerr << "Error: channel " << base <<
			" already registered." << endl;
		return true;
	}
	channel_pool.resize(key +1);	// default construct
	channel& c(channel_pool.back());
	c.type = channel::CHANNEL_TYPE_1ofN;
	c.name = base;
#if PRSIM_CHANNEL_RAILS_INVERTED
	c.set_data_sense(active_low);
#endif
try {
	if (allocate_data_rails(state, m, key, 
			bundle_name, _num_bundles, rail_name, _num_rails)) {
		return true;
	}
	// now setup ack and validity
	// really shouldn't have to lookup channel again here...
	if (set_channel_ack_valid(state, base, 
			have_ack, ack_sense, ack_init, 
			NULL, have_validity, validity_sense)) {
		return true;
	}
} catch (...) {
	cerr << "Error referencing channel `" << base
		<< "\' structure members." << endl;
	return true;
}
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
}	// end for all references
#endif
	return false;
}	// end new_channel

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::set_channel_2p_ack(State& state, const size_t key,
	const string& a, const bool ack_init) {
	channel& c(channel_pool[key]);
	const node_index_type ni =
		parse_node_to_index(a, state.get_module()).index;
	if (ni) {
		c.ack_signal = ni;
		state.__get_node(ni).set_in_channel();
		node_channels_map[ni].insert(key);
	} else {
		cerr << "Error: no such node `" << a <<
			"\' in channel." << endl;
		return true;
	}
	c.set_ack_init(ack_init);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The repeat rail is overloaded to be the validity/request rail.
 */
bool
channel_manager::set_channel_2p_req(State& state, const size_t key,
	const string& r, const bool req_init) {
	channel& c(channel_pool[key]);
	// assign repeat rail (use validity signal)
	const node_index_type ni =
		parse_node_to_index(r, state.get_module()).index;
	if (ni) {
		c.valid_signal = ni;	// doubles as repeat signal
		state.__get_node(ni).set_in_channel();
		node_channels_map[ni].insert(key);
	} else {
		cerr << "Error: no such node `" << r <<
			"\' in channel." << endl;
		return true;
	}
	c.set_repeat_init(req_init);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a LEDR channel.
	\return true on error.
 */
bool
channel_manager::new_channel_ledr(State& state, const string& _base, 
		const string& ack_name, const bool ack_init, 
		const string& bundle_name, const size_t _num_bundles, 
		const string& data_name, const bool data_init, 
		const bool active_low,
		const string& repeat_name, const bool repeat_init) {
	STACKTRACE_VERBOSE;
	const entity::module& m(state.get_module());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	parser::expanded_global_references_type refs;
	if (parser::expand_global_references(_base, m, refs)) {
		return true;
	}
	parser::expanded_global_references_type::const_iterator
		ri(refs.begin()), re(refs.end());
for ( ; ri!=re; ++ri) {
//	const entity::global_indexed_reference& g(ri->second);
	ostringstream oss;
//	cr.inst_ref()->dump(oss, expr_dump_context::default_value);
	ri->first->dump(oss);
	// base name needs to expanded to scalar reference strings...
	const string& base(oss.str());
#else
	const string& base(_base);
#endif
//	const size_t num_bundles = _num_bundles ? _num_bundles : 1;
	const size_t key = channel_pool.size();
	const pair<channel_set_type::iterator, bool>
		i(channel_index_set.insert(make_pair(base, key)));
if (i.second) {
	channel_pool.resize(key +1);	// default construct
	channel& c(channel_pool.back());
	c.type = channel::CHANNEL_TYPE_LEDR;
	c.name = base;
#if PRSIM_CHANNEL_RAILS_INVERTED
	c.set_data_sense(active_low);
#endif
{
	// assign ack rail (optional)
	if (ack_name.length()) {
		const string a(base + '.' + ack_name);
		if (set_channel_2p_ack(state, key, a, ack_init)) {
			return true;
		}
	}
}{
	// assign repeat rail (use validity signal)
	const string r(base + '.' + repeat_name);
	if (set_channel_2p_req(state, key, r, repeat_init)) {
		return true;
	}
}{
	// TODO: handle bundles
	if (_num_bundles) {
		cerr <<
"Sorry, I have not finished implementing bundled LEDR channels yet." << endl;
		FINISH_ME(Fang);
		return true;
	}
}
{
#if 1
	// allocate data rail references:
	channel::data_rail_index_type dk;
	dk[0] = 1;	// bundles
	dk[1] = 1;	// rails
	c.data.resize(dk);	// only 1 data rail
	// assign data rail
	dk[0] = 0;
	dk[1] = 0;
	const string n(base + "." + data_name);
	const node_index_type ni = parse_node_to_index(n, m).index;
	if (ni) {
		c.data[dk] = ni;
		// flag node for consistency
		state.__get_node(ni).set_in_channel();
		c.__node_to_rail[ni] = dk;	// need this?
		// lookup from node to channels
		node_channels_map[ni].insert(key);
	} else {
		cerr << "Error: no such node `" << n <<
			"\' in channel." << endl;
		return true;
	}
#else
	// this doesn't account for repeat rail (yet)
	if (allocate_data_rails(state, m, key, 
			"", 0, data_name, 0)) {
		return true;
	}
#endif
	c.set_data_init(data_init);
}
	// initialize data-rail state counters from current values
	c.initialize_data_counter(state);
} else {
	// channel not successfully inserted; already exists
	return true;
}
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
}	// end for each
#endif
	return false;
}	// end new_channel

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_BUNDLED_DATA
/**
	Creates a bundled-data (4-phase) channel.
	\return true on error.
 */
bool
channel_manager::new_channel_bd4p(State& state, const string& _base, 
		const bool ack_sense, const bool ack_init, 
		const bool req_sense,
//		const bool req_init, 
		const string& data_name, const size_t _num_rails, 
		const bool active_low) {
	STACKTRACE_VERBOSE;
	const entity::module& m(state.get_module());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	parser::expanded_global_references_type refs;
	if (parser::expand_global_references(_base, m, refs)) {
		return true;
	}
	parser::expanded_global_references_type::const_iterator
		ri(refs.begin()), re(refs.end());
for ( ; ri!=re; ++ri) {
//	const entity::global_indexed_reference& g(ri->second);
	ostringstream oss;
//	cr.inst_ref()->dump(oss, expr_dump_context::default_value);
	ri->first->dump(oss);
	// base name needs to expanded to scalar reference strings...
	const string& base(oss.str());
#else
	const string& base(_base);
#endif
	const size_t key = channel_pool.size();
	const pair<channel_set_type::iterator, bool>
		i(channel_index_set.insert(make_pair(base, key)));
if (i.second) {
	channel_pool.resize(key +1);	// default construct
	channel& c(channel_pool.back());
	c.type = channel::CHANNEL_TYPE_BD_4P;
	// default timing mode is after, not global
	c.timing_mode = CHANNEL_TIMING_AFTER;
	c.name = base;
#if PRSIM_CHANNEL_RAILS_INVERTED
	c.set_data_sense(active_low);
#endif
	if (allocate_data_rails(state, m, key, 
			"", 0, data_name, _num_rails)) {
		return true;
	}
	// now setup ack and validity
	if (set_channel_ack_valid(state, base, 
			true, ack_sense, ack_init, 
			NULL, true, req_sense)) {
		return true;
	}
} else {
	// channel not successfully inserted; already exists
	return true;
}
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
}	// end for each
#endif
	return false;
}	// end new_channel_bd4p

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::new_channel_bd2p(State& state, const string& _base,
		const string& ack_name, const bool ack_init,
		const string& req_name, const bool req_init, 
		const string& data_name, const size_t _num_rails,
		const bool active_low) {
	STACKTRACE_VERBOSE;
	const entity::module& m(state.get_module());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	parser::expanded_global_references_type refs;
	if (parser::expand_global_references(_base, m, refs)) {
		return true;
	}
	parser::expanded_global_references_type::const_iterator
		ri(refs.begin()), re(refs.end());
for ( ; ri!=re; ++ri) {
//	const entity::global_indexed_reference& g(ri->second);
	ostringstream oss;
//	cr.inst_ref()->dump(oss, expr_dump_context::default_value);
	ri->first->dump(oss);
	// base name needs to expanded to scalar reference strings...
	const string& base(oss.str());
#else
	const string& base(_base);
#endif
	const size_t key = channel_pool.size();
	const pair<channel_set_type::iterator, bool>
		i(channel_index_set.insert(make_pair(base, key)));
if (i.second) {
	channel_pool.resize(key +1);	// default construct
	channel& c(channel_pool.back());
	c.type = channel::CHANNEL_TYPE_BD_2P;
	// default timing mode is after, not global
	c.timing_mode = CHANNEL_TIMING_AFTER;
	c.name = base;
	if (set_channel_2p_ack(state, key, base + '.' +ack_name, ack_init)) {
		return true;
	}
	if (set_channel_2p_req(state, key, base + '.' +req_name, req_init)) {
		return true;
	}
#if PRSIM_CHANNEL_RAILS_INVERTED
	c.set_data_sense(active_low);
#endif
	if (allocate_data_rails(state, m, key, 
			"", 0, data_name, _num_rails)) {
		return true;
	}
} else {
	// channel not successfully inserted; already exists
	return true;
}
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
}	// end for each
#endif
	return false;
}	// end new_channel_bd2p
#endif	// PRSIM_CHANNEL_BUNDLED_DATA

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_SYNC
/**
	Creates a single-edge sensitive clocked channel.  
	The clock signal poses as a request signal.  
	There is no acknowledge.
	Same params as other channels.  
 */
bool
channel_manager::new_channel_clocked_1edge(State& state, const string& _base, 
		const string& clkname, const bool clk_sense,
		const string& data_name, const size_t _num_rails, 
		const bool active_low) {
	STACKTRACE_VERBOSE;
	const entity::module& m(state.get_module());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	parser::expanded_global_references_type refs;
	if (parser::expand_global_references(_base, m, refs)) {
		return true;
	}
	parser::expanded_global_references_type::const_iterator
		ri(refs.begin()), re(refs.end());
for ( ; ri!=re; ++ri) {
//	const entity::global_indexed_reference& g(ri->second);
	ostringstream oss;
//	cr.inst_ref()->dump(oss, expr_dump_context::default_value);
	ri->first->dump(oss);
	// base name needs to expanded to scalar reference strings...
	const string& base(oss.str());
#else
	const string& base(_base);
#endif
	const size_t key = channel_pool.size();
	const pair<channel_set_type::iterator, bool>
		i(channel_index_set.insert(make_pair(base, key)));
if (i.second) {
	channel_pool.resize(key +1);	// default construct
	channel& c(channel_pool.back());
	c.type = channel::CHANNEL_TYPE_CLK_1EDGE;
	// default timing mode is after, not global
	c.timing_mode = CHANNEL_TIMING_AFTER;
	c.name = base;
#if PRSIM_CHANNEL_RAILS_INVERTED
	c.set_data_sense(active_low);
#endif
	if (allocate_data_rails(state, m, key, 
			"", 0, data_name, _num_rails)) {
		return true;
	}
	// now setup clk
	if (set_channel_ack_valid(state, base, 
			false, false, false, 	// don't care, no ack
			clkname.c_str(), true, clk_sense)) {	// clk == req
		return true;
	}
} else {
	// channel not successfully inserted; already exists
	return true;
}
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
}	// end for each
#endif
	return false;
}	// end new_channel_clocked_1edge

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a double-edge sensitive clocked channel.  
	The clock signal poses as a request signal.  
	There is no acknowledge.
	Same params as other channels.  
 */
bool
channel_manager::new_channel_clocked_2edge(State& state, const string& _base, 
		const string& clkname, const bool clk_init, 
		const string& data_name, const size_t _num_rails, 
		const bool active_low) {
	STACKTRACE_VERBOSE;
	const entity::module& m(state.get_module());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	parser::expanded_global_references_type refs;
	if (parser::expand_global_references(_base, m, refs)) {
		return true;
	}
	parser::expanded_global_references_type::const_iterator
		ri(refs.begin()), re(refs.end());
for ( ; ri!=re; ++ri) {
//	const entity::global_indexed_reference& g(ri->second);
	ostringstream oss;
//	cr.inst_ref()->dump(oss, expr_dump_context::default_value);
	ri->first->dump(oss);
	// base name needs to expanded to scalar reference strings...
	const string& base(oss.str());
#else
	const string& base(_base);
#endif
	const size_t key = channel_pool.size();
	const pair<channel_set_type::iterator, bool>
		i(channel_index_set.insert(make_pair(base, key)));
if (i.second) {
	channel_pool.resize(key +1);	// default construct
	channel& c(channel_pool.back());
	c.type = channel::CHANNEL_TYPE_CLK_2EDGE;
	// default timing mode is after, not global
	c.timing_mode = CHANNEL_TIMING_AFTER;
	c.name = base;
#if PRSIM_CHANNEL_RAILS_INVERTED
	c.set_data_sense(active_low);
#endif
	if (allocate_data_rails(state, m, key, 
			"", 0, data_name, _num_rails)) {
		return true;
	}
	// now setup clk
	if (set_channel_ack_valid(state, base, 
			false, false, false, 	// don't care, no ack
			clkname.c_str(), true, clk_init)) {	// clk == req
		// valid_sense is overloaded as clk_init
		return true;
	}
} else {
	// channel not successfully inserted; already exists
	return true;
}
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
}	// end for each
#endif
	return false;
}	// end new_channel_clocked_2edge

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a clock-source.
 */
bool
channel_manager::new_clock_source(State& state,
		const string& clkname, const bool clk_2edge, 
		const bool clk_sense, const bool clk_init,
		const int cycles) {
	STACKTRACE_VERBOSE;
	const entity::module& m(state.get_module());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	parser::expanded_global_references_type refs;
	if (parser::expand_global_references(clkname, m, refs)) {
		return true;
	}
	parser::expanded_global_references_type::const_iterator
		ri(refs.begin()), re(refs.end());
for ( ; ri!=re; ++ri) {
	ostringstream oss;
	ri->first->dump(oss);
	const string& base(oss.str());
#else
	const string& base(clkname);
#endif
	const node_index_type vi =
		parse_node_to_index(base, state.get_module()).index;
	if (!vi) {
		cerr << "Error: no such node `" << base << "\'." << endl;
		return true;
	}
	// TODO: warn if has fanin
	size_t key = channel_pool.size();
	const pair<channel_set_type::iterator, bool>
		i(channel_index_set.insert(make_pair(base, key)));
	channel* c = NULL;
if (i.second) {
	channel_pool.resize(key +1);	// default construct
	c = &channel_pool.back();
	c->name = base;
	c->timing_mode = CHANNEL_TIMING_AFTER;
} else {
	// reconfigure existing channel
	key= i.first->second;
	c = &channel_pool[key];
}
	c->type = clk_2edge ? channel::CHANNEL_TYPE_CLK_SRC2
		: channel::CHANNEL_TYPE_CLK_SRC1;
	// overloaded valid_sense flag
	if (clk_2edge) {
		c->set_clk_init(clk_init);
	} else {
		c->set_clock_sense(clk_sense);
	}
	c->set_clock_signal(vi);	
//	c->stop_on_empty();		// not needed for sources
	// TODO: make sure not driven by other alias?
	state.__get_node(vi).set_in_channel();		// flag in channel
	node_channels_map[vi].insert(key);
	c->set_clock_source(state, cycles);
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
}	// end for each
#endif
	return false;
}	// end new_clock_source
#endif	// PRSIM_CHANNEL_SYNC

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_index_type
channel_manager::get_channel_index(const channel& c) const {
	return std::distance(&channel_pool[0], &c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\returns a modify-able pointer to the named channel.
 */
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
/**
	\returns a read-only pointer to the named channel.
 */
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
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
/**
	Does an expanded lookup on the name, which can be an array reference.
	\return true on error
 */
bool
channel_manager::lookup_expand(const string& _base, const module& m, 
		vector<const channel*>& ret) const {
	parser::expanded_global_references_type refs;
	if (parser::expand_global_references(_base, m, refs)) {
		return true;
	}
	parser::expanded_global_references_type::const_iterator
		ri(refs.begin()), re(refs.end());
for ( ; ri!=re; ++ri) {
	ostringstream oss;
	ri->first->dump(oss);
	const string& base(oss.str());
	const channel* const c = lookup(base);	// re-parse :S
	if (c) {
		ret.push_back(c);
	} else {
		// already have error message
		return true;
	}
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return ret array of modify-able channel pointers.  
 */
bool
channel_manager::lookup_expand(const string& _base, const module& m, 
		vector<channel*>& ret) {
	parser::expanded_global_references_type refs;
	if (parser::expand_global_references(_base, m, refs)) {
		return true;
	}
	parser::expanded_global_references_type::const_iterator
		ri(refs.begin()), re(refs.end());
for ( ; ri!=re; ++ri) {
	ostringstream oss;
	ri->first->dump(oss);
	const string& base(oss.str());
	channel* const c = lookup(base);	// re-parse :S
	if (c) {
		ret.push_back(c);
	} else {
		// already have error message
		return true;
	}
}
	return false;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// convenient macro to reduce copy-pasting...

#if 0
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
#else
#define	GET_NAMED_CHANNEL(chan, name)					\
	channel* c_ptr = lookup(name);					\
	if (!c_ptr) return true;					\
	channel& chan(*c_ptr);

#define	GET_NAMED_CHANNEL_CONST(chan, name)				\
	const channel* c_ptr = lookup(name);				\
	if (!c_ptr) return true;					\
	const channel& chan(*c_ptr);
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param vname optional name to override validity rail (e.g. clk)
	\pre channel has already been created by new_channel()
	\return true on error.
	This can also be used for clocked channels/sources, where
		the valid_signal serves as the reference clock.
 */
bool
channel_manager::set_channel_ack_valid(State& state, const string& base, 
		const bool have_ack, const bool ack_sense, const bool ack_init, 
		const char* vname,	// optional override if not NULL
		const bool have_validity, const bool validity_sense) {
	STACKTRACE_VERBOSE;
	// TODO: should just pass in channel from caller instead of lookup
	GET_NAMED_CHANNEL(c, base)
#if 0
	const channel_index_type ci = f->second;
#else
	const channel_index_type ci = get_channel_index(c);
#endif
	const entity::module& m(state.get_module());
if (have_ack) {
	c.set_ack_active(ack_sense);
	c.set_ack_init(ack_init);
	// ack and enable rail names are hard-coded for now :(
	const string ack_name(base + (ack_sense ? ".a" : ".e"));
	const node_index_type ai = parse_node_to_index(ack_name, m).index;
	if (!ai) {
		cerr << "Error: no such node `" << ack_name <<
			"\' in channel `" << base << "\'." << endl;
		return true;
	}
	if (c.set_ack_signal(ai)) {
		cerr << "Channel acknowledge is not allowed to alias "
			"any of its data rails!" << endl;
		return true;
	}
	state.__get_node(ai).set_in_channel();		// flag in channel
	// reverse lookup
	node_channels_map[ai].insert(ci);
}
if (have_validity) {
	c.set_valid_sense(validity_sense);
	const string v_name((vname ? base +"." +vname :
		base +(validity_sense ? ".v" : ".n")));
	const node_index_type vi = parse_node_to_index(v_name, m).index;
	if (!vi) {
		cerr << "Error: no such node `" << v_name <<
			"\' in channel `" << base << "\'." << endl;
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
	node_channels_map[vi].insert(ci);
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
/**
	Produce i as an iterator referring to a channel-pointer.
	\param type is channel or const channel.
	\param name is the name of the channel(s).
 */
#define	CHANNEL_FOR_EACH(T, name)					\
	vector<T*> __tmp;						\
	if (lookup_expand(name, s.get_module(), __tmp))			\
		{ return true; }					\
	vector<T*>::const_iterator					\
		i(__tmp.begin()), e(__tmp.end());			\
	for ( ; i!=e; ++i)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
/**
	Configure a registered channel to source values from a file.
 */
bool
channel_manager::source_channel_file(const State& s, 
		const string& channel_name, 
		const string& file_name, const bool loop) {
	STACKTRACE_VERBOSE;
#if 0 && PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
CHANNEL_FOR_EACH(channel, channel_name) {
	channel& c(**i);
	if (c.set_source_file(s, file_name, loop)) return true;
	if (check_source(c)) return true;
}
	return false;
#else
	GET_NAMED_CHANNEL(c, channel_name)
	if (c.set_source_file(s, file_name, loop)) return true;
	return check_source(c);
#endif
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
#if 0 && PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
CHANNEL_FOR_EACH(channel, channel_name) {
	channel& c(**i);
	if (c.set_source_args(s, values, loop)) return true;
	if (check_source(c)) return true;
}
	return false;
#else
	GET_NAMED_CHANNEL(c, channel_name)
	if (c.set_source_args(s, values, loop)) return true;
	return check_source(c);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Configure a registered channel to source values randomly, 
	and infinitely.
 */
bool
channel_manager::rsource_channel(const State& s, const string& channel_name) {
	STACKTRACE_VERBOSE;
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
CHANNEL_FOR_EACH(channel, channel_name) {
	channel& c(**i);
	if (c.set_rsource(s)) return true;
	if (check_source(c)) return true;
}
	return false;
#else
	GET_NAMED_CHANNEL(c, channel_name)
	if (c.set_rsource(s)) return true;
	return check_source(c);
#endif
}
#endif // !PRSIM_CHANNEL_AGGREGATE_ARGUMENTS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if there is any connection error.
 */
bool
channel_manager::check_source(const channel& c
//		, const string& chan_name
		) const {
	// warn if channel happens to be connected in wrong direction
	// TODO: check that data/validity are not driven by other sources!
if (c.valid_signal) {
	STACKTRACE_VERBOSE;
#if 0
	__GET_NAMED_CHANNEL(chan_name)
	const channel_index_type ci = f->second;
#else
	const channel_index_type ci = get_channel_index(c);
#endif
	node_channels_map_type::const_iterator
		m(node_channels_map.find(c.valid_signal));
	INVARIANT(m != node_channels_map.end());
	set<channel_index_type>::const_iterator
		ti(m->second.begin()), te(m->second.end());
	for ( ; ti!=te; ++ti) {
	if (*ti != ci) {
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
	\return true on error (no error condition exists yet).
 */
bool
channel_manager::check_sink(const channel& c) const {
if (c.ack_signal) {
	const channel_index_type ci = get_channel_index(c);
	// check if signal is registered with other sinking channels?
	node_channels_map_type::const_iterator
		m(node_channels_map.find(c.ack_signal));
	INVARIANT(m != node_channels_map.end());

	set<channel_index_type>::const_iterator
		ti(m->second.begin()), te(m->second.end());
	for ( ; ti!=te; ++ti) {
	if (*ti != ci) {
		const channel& ch(channel_pool[*ti]);
		if (ch.is_sinking() && (ch.ack_signal == c.ack_signal)) {
			cerr << "Warning: channel acknowledge is already "
				"being driven by sink on channel `" <<
				ch.name << "\'." << endl;
		}
	}
	}
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
/**
	Configure a registered channel to sink values, 
	with an optional file of expected values.
	\param loop is only applicable if file_name refers to a file of values.
 */
bool
channel_manager::sink_channel(const State& s, const string& channel_name) {
	STACKTRACE_VERBOSE;
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
CHANNEL_FOR_EACH(channel, channel_name) {
	channel& c(**i);
#else
	GET_NAMED_CHANNEL(c, channel_name)
#endif
	if (c.set_sink(s)) return true; // does many checks
	if (check_sink(c)) return true;
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
}	// end for each
#endif
	return false;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Do not aggregate arguments for logs because multiple channels
	would then write to the same file name!
 */
bool
channel_manager::log_channel(const string& channel_name, 
		const string& file_name) {
	STACKTRACE_VERBOSE;
	GET_NAMED_CHANNEL(c, channel_name)
	return c.set_log(file_name);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
bool
channel_manager::expect_channel_file(const string& channel_name, 
		const string& file_name, const bool loop) {
	STACKTRACE_VERBOSE;
#if 0 && PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
CHANNEL_FOR_EACH(channel, channel_name) {
	channel& c(**i);
	if (c.set_expect_file(file_name, loop)) return true;
}
	return false;
#else
	GET_NAMED_CHANNEL(c, channel_name)
	return c.set_expect_file(file_name, loop);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_manager::expect_channel_args(const string& channel_name, 
		const string_list& values, const bool loop) {
	STACKTRACE_VERBOSE;
#if 0 && PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
CHANNEL_FOR_EACH(channel, channel_name) {
	channel& c(**i);
	if (c.set_expect_args(values, loop)) return true;
}
	return false;
#else
	GET_NAMED_CHANNEL(c, channel_name)
	return c.set_expect_args(values, loop);
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_manager::apply_all(void (channel::*f)(void)) {
	STACKTRACE_VERBOSE;
	for_each(channel_pool.begin(), channel_pool.end(), mem_fun_ref(f));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Caller should also State::flush_channel_events() after this!
 */
bool
channel_manager::resume_channel(const State& s, const string& channel_name, 
		vector<env_event_type>& events) {
	STACKTRACE_VERBOSE;
#if 0 && PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
CHANNEL_FOR_EACH(channel, channel_name) {
	channel& c(**i);
	c.resume(s, events);
}
#else
	GET_NAMED_CHANNEL(c, channel_name)
	c.resume(s, events);
#endif
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Caller should also State::flush_channel_events() after this!
 */
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
/**
	Caller should also State::flush_channel_events() after this!
 */
bool
channel_manager::reset_channel(const string& channel_name, 
		vector<env_event_type>& events) {
	STACKTRACE_VERBOSE;
#if 0 && PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
CHANNEL_FOR_EACH(channel, channel_name) {
	channel& c(**i);
	c.reset(events);
}
#else
	GET_NAMED_CHANNEL(c, channel_name)
	c.reset(events);
#endif
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Caller should also State::flush_channel_events() after this!
 */
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
	STACKTRACE_BRIEF;
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
	Just show a single channel.
 */
bool
channel_manager::__dump_channel(ostream& o, const string& channel_name, 
		const bool state) const {
#if 0 && PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
CHANNEL_FOR_EACH(const channel, channel_name) {
	const channel& c(**i);
#else
	GET_NAMED_CHANNEL_CONST(c, channel_name)
#endif
	c.dump(o) << endl;		// contains channel name
	if (state) {
		c.dump_state(o << '\t') << endl;
	}
#if 0 && PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
}	// end for each
#endif
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
bool
channel_manager::dump_channel_state(ostream& o, const State& s, 
		const string& channel_name) const {
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
CHANNEL_FOR_EACH(const channel, channel_name) {
	const channel& c(**i);
#else
	GET_NAMED_CHANNEL_CONST(c, channel_name)
#endif
	c.dump_status(o << "channel " << c.name << ": ", s) << endl;
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
}	// end for each
#endif
	return false;
}
#endif

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
// #undef	__GET_NAMED_CHANNEL
#undef	GET_NAMED_CHANNEL
#undef	GET_NAMED_CHANNEL_CONST
#undef	DATA_VALUE

}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

