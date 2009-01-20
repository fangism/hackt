/**
	\file "sim/chpsim/Trace.cc"
	$Id: Trace.cc,v 1.4.46.1 2009/01/20 02:57:05 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
/**
	Option for the paranoid.  
	Define to 1 to plan extra sanity check alignment markers
	in the trace file, e.g. at section boundaries.  
 */
#define	CHPSIM_TRACE_ALIGNMENT_MARKERS		1

#include "sim/chpsim/Trace.h"
#include "sim/chpsim/TraceStreamer.h"
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

namespace HAC {
namespace SIM {
namespace CHPSIM {
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


template <class Tag>
state_trace_point<Tag>::state_trace_point(const value_type& d, 
		const trace_index_type t, const size_t g) :
		state_trace_point_base(t, g), 
		raw_data(d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
state_trace_point<Tag>::write(ostream& o) const {
	STACKTRACE_VERBOSE;
	state_trace_point_base::write(o);
	extractor_policy::write(o, raw_data);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
state_trace_point<Tag>::read(istream& i) {
	STACKTRACE_VERBOSE;
	state_trace_point_base::read(i);
	extractor_policy::read(i, raw_data);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
state_trace_point<Tag>::dump(ostream& o) const {
	state_trace_point_base::dump(o) << '\t';
	extractor_policy::dump(o, raw_data) << endl;
	return o;
}

//=============================================================================
// class state_trace_window_base method definitions

/**
	Extracts the relevant data to save to trace and stores it in a buffer.
 */
template <class Tag>
void
state_trace_window_base<Tag>::__push_back(const var_type& v, 
		const trace_index_type t, const size_t g) {
	data_array.push_back(data_type(extractor_policy()(v), t, g));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For now we just write out the entire vector brainlessly.
	TODO: (enhancement) group by like-indexed reference.  
	Or use global-indexed map of vectors.
 */
template <class Tag>
void
state_trace_window_base<Tag>::write(ostream& o) const {
	STACKTRACE_VERBOSE;
	const size_t s = data_array.size();
	write_value(o, s);
	for_each(data_array.begin(), data_array.end(), 
		bind2nd_argval(mem_fun_ref(&data_type::write), o)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
state_trace_window_base<Tag>::read(istream& i) {
	STACKTRACE_VERBOSE;
	size_t s;
	read_value(i, s);
if (i) {
	data_array.resize(s);
	for_each(data_array.begin(), data_array.end(), 
		bind2nd_argval(mem_fun_ref(&data_type::read), i)
	);
} else {
	cerr << "Error reading data-array size." << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
state_trace_window_base<Tag>::dump(ostream& o) const {
if (data_array.size()) {
	o << "\tevent\tglobal\traw\n"
		"\tindex\tindex\tvalue" << endl;
	for_each(data_array.begin(), data_array.end(), 
		bind2nd_argval(mem_fun_ref(&data_type::dump), o)
	);
}
	return o;
}

//=============================================================================
// struct state_trace_window_base::__pseudo_const_iterator_range method definitions

/**
	Find the range over which the referenced variables were
	modified by event `ti'.  
	Iterator pair basically inch-worms forward.
	Boundary conditions OK? (end)
 */
template <class Tag>
void
state_trace_window_base<Tag>::__pseudo_const_iterator_range::advance(
		const trace_index_type ti, 
		const state_trace_window_base<Tag>& w) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("trace-entry-index = " << ti << endl);
#if 0
	p = std::equal_range(p.second, w.data_array.end(), ti, 
		state_trace_point_base::event_index_less_than());
#else
	// more efficient
	p.first = p.second;
	p.second = std::upper_bound(p.second, w.data_array.end(), ti, 
		state_trace_point_base::event_index_less_than());
	STACKTRACE_INDENT_PRINT("new distance = " <<
		std::distance(p.first, p.second) << endl);
#endif
}


//=============================================================================
// class state_trace_time_window method definitions

void
state_trace_time_window::write(ostream& o) const {
	STACKTRACE_VERBOSE;
	state_trace_window_base<bool_tag>::write(o);
	state_trace_window_base<int_tag>::write(o);
	state_trace_window_base<enum_tag>::write(o);
	state_trace_window_base<channel_tag>::write(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
state_trace_time_window::read(istream& i) {
	STACKTRACE_VERBOSE;
	state_trace_window_base<bool_tag>::read(i);
	if (!i) { cerr << "Error reading bool trace data." << endl; return; }
	state_trace_window_base<int_tag>::read(i);
	if (!i) { cerr << "Error reading int trace data." << endl; return; }
	state_trace_window_base<enum_tag>::read(i);
	if (!i) { cerr << "Error reading enum trace data." << endl; return; }
	state_trace_window_base<channel_tag>::read(i);
	if (!i) { cerr << "Error reading channel trace data." << endl; return; }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
state_trace_time_window::dump(ostream& o) const {
	o << "bool state trace:" << endl;
	state_trace_window_base<bool_tag>::dump(o);
	o << "int state trace:" << endl;
	state_trace_window_base<int_tag>::dump(o);
	o << "enum state trace:" << endl;
	state_trace_window_base<enum_tag>::dump(o);
	o << "channel state trace:" << endl;
	state_trace_window_base<channel_tag>::dump(o);
	return o;
}

//=============================================================================
// struct state_trace_time_window::pseudo_const_iterator_range method definitions

void
state_trace_time_window::pseudo_const_iterator_range::advance(
		const trace_index_type ti, 
		const state_trace_time_window& w) {
	bool_pseudo_const_iterator_range::advance(ti, w);
	int_pseudo_const_iterator_range::advance(ti, w);
	enum_pseudo_const_iterator_range::advance(ti, w);
	channel_pseudo_const_iterator_range::advance(ti, w);
}

//=============================================================================
// class trace_chunk method definitions

trace_chunk::trace_chunk() : 
		state_trace_time_window(), 
		event_trace_window() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
trace_chunk::~trace_chunk() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Non-destructive write-out of contents.  
 */
void
trace_chunk::write(ostream& o) const {
	STACKTRACE_VERBOSE;
	event_trace_window::write(o);
	state_trace_time_window::write(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
trace_chunk::read(istream& i) {
	STACKTRACE_VERBOSE;
	event_trace_window::read(i);
if (i) {
	state_trace_time_window::read(i);
	if (!i) {
		cerr << "Error reading state-trace-window." << endl;
	}
} else {
	cerr << "Error reading event-trace-window." << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param previous_events the event offset number
 */
ostream&
trace_chunk::dump(ostream& o, const size_t previous_events) const {
	event_trace_window::dump(o, previous_events);
	state_trace_time_window::dump(o);
	return o;
}

//=============================================================================
// class TraceManager method definitions

/**
	I'm lazy... global flag, initialization.
 */
bool
TraceManager::notify_flush = false;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private default constructor, only used to construct a temporary.  
 */
TraceManager::TraceManager() : 
		trace_file_name(), 
		temp_file_name(),
		trace_ostream(NULL), 
		header_ostream(NULL), 
		contents(), 
		current_chunk(),
		trace_payload_size(0), 
		previous_events(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Opens the requested file streams in binary mode for writing.  
	Caller should check good() immediately after construction.  
 */
TraceManager::TraceManager(const string& fn) : 
		trace_file_name(fn), 
#if 0
		// tmpname is flagged dangerous/deprecated on some systems.
		temp_file_name(tmpnam(NULL)), 		// libc/cstdio
#else
		temp_file_name(fn + "-tmp"),	// could append random number
#endif
		trace_ostream(new fstream(	// read and write
			temp_file_name.c_str(), 
			ios_base::binary | ios_base::trunc
				// that's silly, these should be default...
				| ios_base::in | ios_base::out
				)),
		header_ostream(new ofstream(
			trace_file_name.c_str(), ios_base::binary)), 
		contents(), 
		current_chunk(),
		trace_payload_size(0), 
		previous_events(0) {
#if 0 && USE_MKSTEMP
	// can we assume that 
	char tmp_template[] = "/tmp/hackt-chpsim-trace-XXXXXXXXXXXXX";	// null
	// convert file descriptor to fstream
#endif
	NEVER_NULL(trace_ostream);
	NEVER_NULL(header_ostream);
	// should always be allocated, but not necessarily in a good state
#if 0
	cerr << "Trace file name: " << trace_file_name;
	cerr << ((*header_ostream) ? " (opened)" : " (failed)") << endl;
	cerr << "Temp file name: " << temp_file_name;
	cerr << ((*trace_ostream) ? " (opened)" : " (failed)") << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TraceManager::~TraceManager() {
	finish();
#if 1
	// remove temp file (libc)
	remove(temp_file_name.c_str());
	// check int return value?
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if both output streams are good to go.  
 */
bool
TraceManager::good(void) const {
	return trace_ostream && header_ostream &&
		*trace_ostream && *header_ostream;
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
	// else there is nothing to flush
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This MUST be called to finish writing the trace.  
 */
void
TraceManager::finish(void) {
	STACKTRACE_VERBOSE;
if (good()) {
	flush();	// one last flush (if needed)
	// write out header to file to final file
	NEVER_NULL(header_ostream);
	contents.write(*header_ostream);
	// align to natural boundary? maybe sanity check code.
	const streampos start_of_objects = header_ostream->tellp();
	STACKTRACE_INDENT_PRINT("header written up to offset: "
		<< start_of_objects << endl);
#if CHPSIM_TRACE_ALIGNMENT_MARKERS
	static const size_t marker = 0xFFFFFFFF;
	write_value(*header_ostream, marker);
	STACKTRACE_INDENT_PRINT("starting body at: "
		<< header_ostream->tellp() << endl);
#endif
	// concatenate trace payload (from temp.) to final file.  
{
	// std::copy using stream iterators will NOT work
	static const size_t buf_size = 1<<12;	// stack buffer size
	char buf[buf_size];	// stack big enough?
	size_t trace_end = trace_ostream->tellp();
	STACKTRACE_INDENT_PRINT("trace body ends at: " << trace_end << endl);
	trace_ostream->seekg(0);
	while (trace_end > buf_size) {
		// massive transfer
		trace_ostream->read(buf, buf_size);
		header_ostream->write(buf, buf_size);
		trace_end -= buf_size;
	}
	// cleanup
	trace_ostream->read(buf, trace_end);
	header_ostream->write(buf, trace_end);
	const streampos end_of_objects = header_ostream->tellp();
	STACKTRACE_INDENT_PRINT("payload finished at offset: " <<
		end_of_objects << endl);
}
	trace_ostream = excl_ptr<fstream>(NULL);
	header_ostream = excl_ptr<ofstream>(NULL);
#if 0
	// remove the temporary file if it wasn't generated by tmpnam
	remove(temp_file_name.c_str());		// libc/stdio
	// don't care about exit status?
#endif
	if (notify_flush) {
		cout << "trace manager: trace file complete." << endl;
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true to signal error opening file. 
 */
bool
TraceManager::text_dump(const string& fn, ostream& o) {
	ifstream f(fn.c_str(), ios_base::binary);
	if (f) {
		text_dump(f, o);
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
TraceManager::text_dump(ifstream& i, ostream& o) {
#if 0
	INVARIANT(i.flags() & ios_base::binary);
	INVARIANT(!(o.flags() & ios_base::binary));
#endif
	TraceManager tm;
	// reverse the process of ::finish()
	tm.contents.read(i);
if (i) {
#if ENABLE_STACKTRACE
	const streampos start_of_objects = i.tellg();
	cerr << "header read up to offset: " << start_of_objects<< endl;
#endif
	tm.contents.dump(o);
	// note header offset? start of payload?
#if CHPSIM_TRACE_ALIGNMENT_MARKERS
{
	size_t check;
	read_value(i, check);
	INVARIANT(check == 0xFFFFFFFF);
}
#endif
//	o << "Trace events and data, by epoch:" << endl;
	size_t j = 0;
	trace_file_contents::const_iterator
		ci(tm.contents.begin()), ce(tm.contents.end());
	for ( ; ci!=ce; ++ci, ++j) {
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
			tm.current_chunk.dump(o, tm.previous_events);
			tm.previous_events += tm.current_event_count();
		} else {
			cerr << "Error encountered in reading trace payload!"
				<< endl;
			break;
		}
	}
} else {
	cerr << "Error encountered in reading trace header!" << endl;
}
}

//=============================================================================
// class TraceManager::event_streamer method definitions

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
#if CHPSIM_TRACE_ALIGNMENT_MARKERS
{
	size_t check;
	read_value(fin, check);
	INVARIANT(check == 0xFFFFFFFF);
}
#endif
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

//=============================================================================
// explicit template instantiations

template class state_trace_window_base<bool_tag>;
template class state_trace_window_base<int_tag>;
template class state_trace_window_base<enum_tag>;
template class state_trace_window_base<channel_tag>;

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

