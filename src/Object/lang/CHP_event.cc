/**
	\file "Object/lang/CHP_event.cc"
	$Id: CHP_event.cc,v 1.1.2.4 2007/09/07 01:33:09 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include "Object/lang/CHP_event.h"
#include "Object/lang/CHP_event_alloc.h"	// for EventSuccessorDumper
#include "common/ICE.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/lang/CHP_base.h"
#include "sim/chpsim/graph_options.h"
#include "util/stacktrace.h"
#include "util/iterator_more.h"
#include "util/IO_utils.tcc"
#include "util/persistent_object_manager.tcc"

namespace HAC {
namespace entity {
namespace CHP {
#include "util/using_ostream.h"
using std::ostream_iterator;
using std::copy;
using std::back_inserter;
using std::for_each;
using util::set_inserter;
using util::write_value;
using util::read_value;
using SIM::CHPSIM::graph_options;

//=============================================================================
// class local_event method definitions

const char 
local_event::node_prefix[] = "EVENT_";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
local_event::local_event() :
		action_ptr(NULL),
		successor_events(), 
		event_type(EVENT_NULL),
		predecessors(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param a the CHP::action pointer (may be NULL)
		using shallow reference, not reference counting.
 */
local_event::local_event(const action* a, const unsigned short t) :
		action_ptr(a),
		successor_events(), 
		event_type(t),
		predecessors(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
local_event::~local_event() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Completely resets the event.  
 */
void
local_event::orphan(void) {
	this->~local_event();
	new (this) this_type();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For printing the event queue.  
	TODO: a line/position in source might be nice, 
		in case of repetition.  
 */
ostream&
local_event::dump_brief(ostream& o, const expr_dump_context& dc) const {
//	o << process_index << '\t';
	if (action_ptr) {
		action_ptr->dump_event(o, dc);
	} else {
		o << "null";
	}
	// countdown/predecessors?
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
local_event::dump_source(ostream& o, const expr_dump_context& dc) const {
	if (action_ptr) {
		action_ptr->dump(o, dc);
	} else {
		o << "null";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For diagnostics.  
	Calls to this are wrapped in auto-indent, so use indent 
	if line-breaks are ever added.  
 */
ostream&
local_event::dump_struct(ostream& o, const expr_dump_context& edc) const {
	switch (event_type) {
	case EVENT_NULL: o << (predecessors > 1 ? "join" : "null"); break;
	case EVENT_ASSIGN: o << "assign"; break;
	case EVENT_SEND: o << "send"; break;
	case EVENT_RECEIVE: o << "receive"; break;
	case EVENT_PEEK: o << "peek"; break;
	case EVENT_CONCURRENT_FORK: o << "fork"; break;
	case EVENT_SELECTION_BEGIN: o << "select"; break;
	case EVENT_CONDITION_WAIT: o << "wait"; break;
	case EVENT_FUNCTION_CALL: o << "call"; break;
	default:
		ICE(cerr, cerr << "Invalid event type enum: "
			<< event_type << endl;)
	}
	o << ": ";
	// factored out code
	dump_brief(o, edc);
	// flags?
//	o << ", pid: " << process_index;
	o << ", #pred: " << predecessors;
	// countdown? only in state dump
	// o << endl;
	o << ", succ: ";
	ostream_iterator<event_index_type> osi(o, " ");
	copy(successor_events.begin(), successor_events.end(), osi);
	o << endl;
//	block_deps.dump(o);	// includes endl already
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't print guards here, put guards on edges.  
	TODO: Caller should print pid or pass it down...
 */
ostream&
local_event::dump_dot_node(ostream& o, const event_index_type i, 
		const graph_options& g, const expr_dump_context& edc,
		const char* extra_label_text, 
		const event_index_type offset) const {
	o << node_prefix << i << '\t';
	o << "[shape=";
	switch (event_type) {
	case EVENT_NULL:
		o << ((predecessors > 1) ? "diamond" : "ellipse"); 
		break;
	case EVENT_ASSIGN: o << "box"; break;
	case EVENT_SEND: o << "house"; break;
	case EVENT_RECEIVE: o << "invhouse"; break;
	case EVENT_PEEK: o << "invhouse"; break;	// or ASSIGN shape?
	case EVENT_CONCURRENT_FORK: o << "hexagon"; break;
	case EVENT_SELECTION_BEGIN: o << "trapezium"; break;
	case EVENT_CONDITION_WAIT: o << "ellipse"; break;
	case EVENT_FUNCTION_CALL: o << "octogon"; break;
		// TODO: flag for non-deterministic? extra periphery?
	default:
		ICE(cerr, cerr << "Invalid event type enum: "
			<< event_type << endl;)
	}
	o << ", label=\"";
	if (g.show_event_index) {
		o << "[" << i << "] ";
	}
#if 0
	if (!g.process_event_clusters || !process_index) {
		// always show pid 0 because top-level is not clustered
		o << "pid=" << process_index;
	}
#else
	if (extra_label_text) {
		o << extra_label_text;
	}
#endif
	// seems a waste to do this multiple times for same process...
	// can't change until we-reorganize events into contiguous ranges.
	// and that's happening right now (-chpsim-09 branch)
	if (action_ptr) {
		action_ptr->dump_event(o << "\\n", edc);
	}
	// no edges
	// no deps
	o << "\"];" << endl;
	if (action_ptr) {
		// not SIM::CHPSIM::EventSuccessorDumper
		EventSuccessorDumper d(o, *this, i, edc);
		action_ptr->accept(d);
	} else {
		dump_successor_edges_default(o, i, offset);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just prints edge label if applicable.
	TODO: somehow print guards on edges of selection.  
		Do this in the print of selection.
 */
ostream&
local_event::dump_successor_edges_default(ostream& o, 
		const event_index_type i, 
		const event_index_type offset) const {
	// iterate over edges
	successor_list_type::const_iterator
		j(successor_events.begin()), z(successor_events.end());
	for ( ; j!=z; ++j) {
		const event_index_type h = *j +offset;
		o << node_prefix << i << " -> " << node_prefix << h <<
			';' << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
local_event::collect_transient_info_base(persistent_object_manager& m) const {
	m.assert_ptr_registered(action_ptr);	// sanity check
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
local_event::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, action_ptr);
	util::write_sequence(o, successor_events);
	write_value(o, event_type);
	write_value(o, predecessors);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
local_event::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, action_ptr);
	util::read_sequence_resize(i, successor_events);
	read_value(i, event_type);
	read_value(i, predecessors);
}

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

