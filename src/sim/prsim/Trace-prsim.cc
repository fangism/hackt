/**
	\file "sim/prsim/Trace-prsim.cc"
	$Id: Trace-prsim.cc,v 1.3 2009/04/29 05:33:41 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "sim/prsim/Trace-prsim.h"
// #include "sim/prsim/TraceStreamer.h"
#include "sim/prsim/State-prsim.h"
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <functional>
#include <fstream>
#include "util/stacktrace.h"
#include <cstdio>			// for tmpnam
#include "util/libc_temp.h"	// for temp file functions
#include "util/IO_utils.tcc"	// .tcc?
#include "util/binders.h"

/**
	Define to 1 to include initial and final checkpoints.
 */
#define	TRACE_INITIAL_FINAL_STATES		1

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
using std::for_each;
using std::ios_base;
using std::copy;
using std::ifstream;
using util::write_value;
using util::read_value;
using std::streampos;
using std::mem_fun_ref;
using util::bind1st_argval;
using util::bind2nd_argval;

//=============================================================================
// class state_trace_point method definitions

void
state_trace_point::write_no_value(ostream& o) const {
	event_trace_point::write(o);
// TODO: only write if event_id is 0, else can be derived from rule index
	write_value(o, node_index);
	// omit node_value
}

// inline
void
state_trace_point::write_value_only(ostream& o) const {
	write_value(o, node_value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
state_trace_point::read_no_value(istream& i) {
	event_trace_point::read(i);
// TODO: only write if event_id is 0, else can be derived from rule index
	read_value(i, node_index);
	// omit node_value
}

void
state_trace_point::read_value_only(istream& i) {
	read_value(i, node_value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
state_trace_point::__dump(ostream& o) const {
	return event_trace_point::__dump(o) << '\t' << node_index <<
		'\t' << NodeState::value_to_char[size_t(new_value())];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
state_trace_point::dump(ostream& o) const {
	return dump(o) << endl;
}

//=============================================================================
// class trace_chunk method definitions

#if 0
trace_chunk::trace_chunk() : event_array_type() {
	// reserve (preallocate)?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
trace_chunk::~trace_chunk() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Non-destructive write-out of contents.  
 */
void
trace_chunk::write(ostream& o) const {
	STACKTRACE_VERBOSE;
	const size_t s = size();
	write_value(o, s);
	for_each(begin(), end(), bind2nd_argval(
		mem_fun_ref(&state_trace_point::write_no_value), o));
	for_each(begin(), end(), bind2nd_argval(
		mem_fun_ref(&state_trace_point::write_value_only), o));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: write signature that confirms the architecture of the binary.
 */
void
trace_chunk::read(istream& i) {
	STACKTRACE_VERBOSE;
	size_t s = 0;
	read_value(i, s);
	resize(s);
	for_each(begin(), end(), bind2nd_argval(
		mem_fun_ref(&state_trace_point::read_no_value), i));
	for_each(begin(), end(), bind2nd_argval(
		mem_fun_ref(&state_trace_point::read_value_only), i));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param previous_events the event offset number
 */
ostream&
trace_chunk::dump(ostream& o, const trace_index_type previous_events, 
		const State& s) const {
#if 0
	for_each(begin(), end(), bind2nd_argval(
		mem_fun_ref(&state_trace_point::dump), o));
#else
	o << "\tevent\t\tevent\tcause\tnode\tnode" << endl;
	o << "\tindex\ttime\tnode\tindex\tindex\tvalue";
// if (v) {
	o << "\trule/node";
// }
	o << endl;

	event_array_type::const_iterator i(begin()), e(end());
	size_t j = previous_events;
	for ( ; i!=e; ++i, ++j) {
		i->__dump(o << '\t' << j);
		// TODO: print human-comprehensible text: rule, node...
		const rule_index_type& r = i->event_id;
		const node_index_type& ni = i->node_index;
		if (r) {
			s.dump_rule(o << '\t', r, false, false);
		} else if (ni) {
			// just print (node:value)
			s.dump_node_canonical_name(o << "\t(", ni) << ':' <<
			NodeState::value_to_char[size_t(i->new_value())] << ')';
		} else {
			o << "\t(null)";
		}
		o << endl;
	}
#endif
	return o;
}

//=============================================================================
// class TraceManager method definitions

/**
	Private default constructor, only used to construct a temporary.  
 */
TraceManager::TraceManager(const checkpoint_type& np) : 
		trace_manager_base(), 
		current_chunk(), 
		checkpoint(np) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Opens the requested file streams in binary mode for writing.  
	Caller should check good() immediately after construction.  
 */
TraceManager::TraceManager(const string& fn, const checkpoint_type& np) : 
		trace_manager_base(fn), 
		current_chunk(), 
		checkpoint(np) {
if (good()) {
#if TRACE_INITIAL_FINAL_STATES
	// write out initial state of all nodes
	__write_checkpoint();
#endif
	// reserve the 0th event as a NULL event?
	const trace_index_type i = push_back_event(state_trace_point(
		delay_policy<trace_time_type>::zero, 
		INVALID_EXPR_INDEX, INVALID_TRACE_INDEX, 
		INVALID_NODE_INDEX, LOGIC_OTHER, LOGIC_OTHER));
	INVARIANT(!i);		// must have been index 0
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TraceManager::~TraceManager() {
if (good()) {
	flush();
#if TRACE_INITIAL_FINAL_STATES
	// write out final state of all nodes
	__write_checkpoint();
#endif
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write out a checkpoint for the value of all nodes. 
	\pre was just flushed, start of brand new section.  
 */
void
TraceManager::__write_checkpoint(void) {
	const streampos old_size = trace_ostream->tellp();
	node_value_array init(checkpoint.size());
	transform(checkpoint.begin(), checkpoint.end(), init.begin(), 
		mem_fun_ref(&NodeState::current_value));
	util::write_sequence(*trace_ostream, init);
	// force flush out write
	trace_ostream->flush();
	trace_payload_size = trace_ostream->tellp();
	const size_t prev = previous_events;
	contents.push_back(trace_file_contents::entry(prev, 
		delay_policy<trace_time_type>::zero, 
		old_size, trace_payload_size -old_size));
	// restart chunk, recycling memory (placement dtor and ctor)
	current_chunk.~trace_chunk();
	new (&current_chunk) trace_chunk();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes out the current contents of the chunk to (temp) file.  
	Append an entry to the table of contents.  
	This also destructively resets the current chunk 
	(releasing memory) so new data can be appended.  
	With regular flushing, memory usage can be capped.  
 */
void
TraceManager::flush(void) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(trace_ostream);
	// write out to temp file
	// INVARIANT, there must be at least ONE event in the trace
	// otherwise, this flush is just wasted (no-op)
if (current_chunk.event_count()) {
	__force_flush();
}
	// else there is nothing to flush
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write out a section, even if it is empty, no events transpired.
 */
void
TraceManager::__force_flush(void) {
	if (notify_flush) {
		cout << "trace manager: flushing out another epoch of trace."
			<< endl;
	}
	const trace_time_type start_time = current_chunk.start_time();
	const streampos old_size = trace_ostream->tellp();
	const size_t prev = previous_events;
	previous_events += current_chunk.event_count();
	current_chunk.write(*trace_ostream);
	trace_ostream->flush();
	trace_payload_size = trace_ostream->tellp();
	STACKTRACE_INDENT_PRINT("chunk written to offsets [" << old_size <<
		':' << trace_payload_size << "]." << endl);
	// append entry to contents
	contents.push_back(trace_file_contents::entry(prev, 
		start_time, old_size, trace_payload_size -old_size));
	// restart chunk, recycling memory (placement dtor and ctor)
	current_chunk.~trace_chunk();
	new (&current_chunk) trace_chunk();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
TraceManager::dump_checkpoint(istream& i, ostream& o) {
	node_value_array init;
	util::read_sequence_resize(i, init);
	INVARIANT(init.size() == checkpoint.size());	// sanity check
	std::ostream_iterator<char> osi(o, " ");
	node_value_array::const_iterator ii(init.begin()), ie(init.end());
	// wrap 40 per line:
	static const int width = 40;
	for ( ; ie -ii >= width; ii += width) {
		transform(ii, ii +width, osi, 
			&NodeState::translate_value_to_char);
		o << endl;
	}
	if (ii != ie) {
		transform(ii, ie, osi, 
			&NodeState::translate_value_to_char);
		o << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true to signal error opening file. 
 */
bool
TraceManager::text_dump(const string& fn, ostream& o, const State& s) {
	ifstream f(fn.c_str(), ios_base::binary);
	if (f) {
		text_dump(f, o, s);
		return false;
	} else {
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads in trace file and produces a massive text dump, which you're
	supposed to correlate with the CHP event graph.  Fun.  
	This is the reverse of the finish() method.  
	\param i binary input stream of the trace file
	\param o text output stream for human(?) grokability.  
 */
void
TraceManager::text_dump(ifstream& i, ostream& o, const State& s) {
#if 0
	INVARIANT(i.flags() & ios_base::binary);
	INVARIANT(!(o.flags() & ios_base::binary));
#endif
	TraceManager tm(s.get_node_pool());
	// reverse the process of ::finish()
	tm.contents.read(i);
if (i) {
#if ENABLE_STACKTRACE
	const streampos start_of_objects = i.tellg();
	cerr << "header read up to offset: " << start_of_objects<< endl;
#endif
	tm.contents.dump(o);
	// note header offset? start of payload?
//	o << "Trace events and data, by epoch:" << endl;
	size_t j = 0;
	trace_file_contents::const_iterator
		ci(tm.contents.begin()), ce(tm.contents.end());
#if TRACE_INITIAL_FINAL_STATES
	const trace_file_contents::const_iterator
		cc(ce -1);	// final checkpoint
	// ci points to initial value checkpoint
	o << "Epoch " << j << ": (initial values)" << endl;
	tm.dump_checkpoint(i, o);
	++ci; ++j;
#else
	const trace_file_contents::const_iterator cc(ce);
#endif
	for ( ; ci!=cc; ++ci, ++j) {
		o << "Epoch " << j << ':' << endl;
#if ENABLE_STACKTRACE
		const streampos head = i.tellg();
#endif
		tm.current_chunk.read(i);
		if (i) {
#if ENABLE_STACKTRACE
			const streampos tail = i.tellg();
			cerr << "read chunk of size: " << tail -head << endl;
#endif
			tm.current_chunk.dump(o, tm.previous_events, s);
			tm.previous_events += tm.current_event_count();
		} else {
			cerr << "Error encountered in reading trace payload!"
				<< endl;
			break;
		}
	}
#if TRACE_INITIAL_FINAL_STATES
	// ci points to end checkpoint section
	o << "Epoch " << j << ": (final values)" << endl;
	tm.dump_checkpoint(i, o);
#endif
} else {
	cerr << "Error encountered in reading trace header!" << endl;
}
}

//=============================================================================
// class TraceManager::event_streamer method definitions

#if 0
TraceManager::entry_streamer::entry_streamer(const string& fn) :
		fin(fn.c_str(), ios_base::binary),
		tracefile(),
		epoch_iter(),
		event_iter(), 
		_index(0) {
	init();	// establishes iterators
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial constructor, leaving iterators uninitialized.  
 */
TraceManager::entry_streamer::entry_streamer(
		const string& fn, partial_init_tag) :
		fin(fn.c_str(), ios_base::binary),
		tracefile(),
		epoch_iter(),
		event_iter(), 
		_index(0) {
	// do nothing
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// default dtor manager everything
TraceManager::entry_streamer::~entry_streamer() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This partial initialization only reads the header, 
	and not any of the payload, thus leaving the epoch
	iterator and trace entry interators uninitialized.  
	Direct use is really reserved for derivative classes.  
 */
good_bool
TraceManager::entry_streamer::partial_init(void) {
if (fin) {
	tracefile.contents.read(fin);
	if (fin) {
		return good_bool(true);
	}
}
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes trace-stream state's nested iterator.
	Reads the table of contents and the first chunk.  
 */
good_bool
TraceManager::entry_streamer::init(void) {
	if (partial_init().good) {
		// continue initializing iterators
		epoch_iter = tracefile.contents.begin();
		if (epoch_iter != tracefile.contents.end()) {
			tracefile.current_chunk.read(fin);
			event_iter = tracefile.current_chunk.begin();
			// make sure iterator is valid
			return good_bool(
				event_iter != tracefile.current_chunk.end());
		}
	}
	// catch-all
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the status of the stream, true if good.  
 */
bool
TraceManager::entry_streamer::good(void) const {
	STACKTRACE_VERBOSE;
	return fin &&
		(epoch_iter != tracefile.contents.end()) &&
		(event_iter != tracefile.current_chunk.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre all 
 */
const event_trace_point&
TraceManager::entry_streamer::current_event_record(void) const {
	INVARIANT(good());
	return *event_iter;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Advances to next entry, iterator-like.
	\return bad if there are no more events, we've reached end of stream, 
		and the current_event_record() is thus invalid.  
	This return value is useless...
 */
good_bool
TraceManager::entry_streamer::advance(void) {
	STACKTRACE_VERBOSE;
	++event_iter;
	++_index;
	if (event_iter == tracefile.current_chunk.end()) {
		STACKTRACE_INDENT_PRINT("at end of epoch\n");
		// load next epoch
		// safety guard, in case user tries to abuse
		if (epoch_iter != tracefile.contents.end()) {
			STACKTRACE_INDENT_PRINT("loading next epoch\n");
			++epoch_iter;
			if (epoch_iter != tracefile.contents.end()) {
				tracefile.current_chunk.read(fin);
				event_iter = tracefile.current_chunk.begin();
				return good_bool(event_iter !=
					tracefile.current_chunk.end());
			}
		}
	}
	// catch-all
	return good_bool(false);
}

//=============================================================================
// class TraceManager::entry_reverse_streamer method definitions

TraceManager::entry_reverse_streamer::entry_reverse_streamer(
		const string& s) : 
		parent_type(s, partial_init_tag()),
		start_of_epochs(0), _total_entries(0) {
	STACKTRACE_VERBOSE;
	init();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const event_trace_point&
TraceManager::entry_reverse_streamer::current_event_record(void) const {
	INVARIANT(good());
	return event_iter[-1];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
TraceManager::entry_reverse_streamer::good(void) const {
	return fin &&
		((epoch_iter != tracefile.contents.begin()) ||
		(event_iter != tracefile.current_chunk.begin()));
	// could just look at index...
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
TraceManager::entry_reverse_streamer::init(void) {
	STACKTRACE_VERBOSE;
	if (partial_init().good) {
		// remember the position to adjust seek pointers
		start_of_epochs = fin.tellg();
		INVARIANT(!tracefile.contents.empty());
		epoch_iter = --tracefile.contents.end();
		const size_t start_of_end = epoch_iter->start_index;
		// seek to last epoch, and read its whole chunk
		fin.seekg(epoch_iter->file_offset + start_of_epochs);
		tracefile.current_chunk.read(fin);
		INVARIANT(!tracefile.current_chunk.empty());
		// point to one-past-the-end
		event_iter = tracefile.current_chunk.end();
		_index = start_of_end +tracefile.current_chunk.event_count() -1;
		_total_entries = epoch_iter->start_index
			+tracefile.current_chunk.event_count();
	}
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
TraceManager::entry_reverse_streamer::retreat(void) {
	STACKTRACE_VERBOSE;
	if (_index) {
		// pre-decrement
		--event_iter;
	} else {
		if (event_iter != tracefile.current_chunk.begin()) {
			--event_iter;		// to terminate
		}
		return good_bool(false);
	}
	if (event_iter == tracefile.current_chunk.begin()) {
		INVARIANT(epoch_iter != tracefile.contents.begin());
		--epoch_iter;
		fin.seekg(epoch_iter->file_offset + start_of_epochs);
		tracefile.current_chunk.read(fin);
		event_iter = tracefile.current_chunk.end();
		INVARIANT(_index == 
			(epoch_iter->start_index +
			tracefile.current_chunk.event_count()));
	}
	--_index;
	return good_bool(true);
}

//=============================================================================
// class TraceManager::random_accessor method definitions

/**
	Initialize to last epoch to get _total_entries count.
 */
TraceManager::random_accessor::random_accessor(const string& fn) :
		parent_type(fn) {
	// parent's constructor will initialize to the last epoch
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if trace file handle is in valid state. 
	Doesn't use the epoch-local event iterator.
 */
bool
TraceManager::random_accessor::good(void) const {
	return fin &&
		(epoch_iter != tracefile.contents.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param ei event_index, MUST be valid, not checked here.
	\return a reference to the indexed entry.
	The caller should NOT store the reference long, it is at best
		short-lived, and thus should be copied.
 */
const event_trace_point&
TraceManager::random_accessor::operator [] (const size_t ei) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("ei = " << ei << endl);
	INVARIANT(ei < _total_entries);
	const size_t si = epoch_iter->start_index;
	if ((ei >= si) && (ei < si +tracefile.current_chunk.event_count())) {
		// hit in same epoch
		const size_t offset = ei - si;
		return tracefile.current_chunk.get_event(offset);
	} else {
		// lookup by epoch
		trace_file_contents::const_iterator
			e_iter(std::upper_bound(tracefile.contents.begin(),
				tracefile.contents.end(), ei,
				trace_file_contents::entry::event_index_less_than()));
		INVARIANT(e_iter != tracefile.contents.begin());
		--e_iter;
		STACKTRACE_INDENT_PRINT("epoch: " <<
			distance(tracefile.contents.begin(), e_iter) << endl);
		INVARIANT(e_iter != epoch_iter);	// else we would've hit
		epoch_iter = e_iter;
		fin.seekg(start_of_epochs + epoch_iter->file_offset);
		tracefile.current_chunk.read(fin);
		const size_t offset = ei - epoch_iter->start_index;
		return tracefile.current_chunk.get_event(offset);
	}
}

//=============================================================================
// class TraceManager::state_change_streamer method definitions

/**
	Do full initialization of parent class, opening stream for forward
	reading, one epoch at a time.  
 */
TraceManager::state_change_streamer::state_change_streamer(const string& fn) :
		parent_type(fn), state_iter() {
	if (good()) {
		begin();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets the state-change iterator to the beginning of the chunk.  
	(Not the same as parent_type::init()
 */
void
TraceManager::state_change_streamer::begin(void) {
	STACKTRACE_VERBOSE;
	// this could be combined into a single set operation, who cares...
	state_iter.begin(tracefile.current_chunk);
	state_iter.advance(_index, tracefile.current_chunk);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\post if stream is valid, state_iter 'points' to a valid
		set of modifications.  
 */
good_bool
TraceManager::state_change_streamer::advance(void) {
	STACKTRACE_VERBOSE;
	const trace_file_contents::const_iterator last_epoch(epoch_iter);
	parent_type::advance();
	const good_bool ret(parent_type::good());
	if (ret.good) {
		// reference is still good
		// _index should have incremented
		if (last_epoch != epoch_iter) {
			STACKTRACE_INDENT_PRINT("new epoch\n");
			// if epoch changed, re-establish the state-iterator
			begin();
		} else {
			STACKTRACE_INDENT_PRINT("same epoch\n");
			state_iter.advance(_index, tracefile.current_chunk);
		}
	} else {
		STACKTRACE_INDENT_PRINT("fin: " << size_t(bool(fin)) << endl);
		STACKTRACE_INDENT_PRINT("!end-epoch: " <<
			size_t(epoch_iter != tracefile.contents.end()) << endl);
		STACKTRACE_INDENT_PRINT("!end-entry: " <<
			size_t(event_iter != tracefile.current_chunk.end())
			<< endl);
		STACKTRACE_INDENT_PRINT("good? " << size_t(ret.good) << endl);
	}
	return ret;
}
#endif

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

