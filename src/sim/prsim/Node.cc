/**
	\file "sim/prsim/Node.cc"
	Implementation of PRS node.  
	$Id: Node.cc,v 1.20 2011/01/13 22:19:09 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <string>
#include <iterator>
#include <algorithm>
#include "sim/prsim/Node.hh"
#include "util/macros.h"
#include "util/stacktrace.hh"
#include "util/IO_utils.tcc"
#include "util/numeric/random.hh"
#include "util/STL/valarray_iterator.hh"
#include "sim/prsim/process_graph.hh"	// for faninout_struct_type
#include "Object/inst/connection_policy.hh"	// for bool_connect_policy

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.hh"
using std::ostream_iterator;
using std::string;
using util::write_value;
using util::read_value;

/**
	For debugging only.  Clearly see node boundaries.  
 */
#define	NODE_ALIGN_MARKERS				0

//=============================================================================
// class Node method definitions

Node::Node() : 
	fanin(), 
	fanout(),
	struct_flags(NODE_DEFAULT_STRUCT_FLAGS) {
	INVARIANT(!fanout.size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Node::~Node() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This grabs instance attributes from the canonical bools
	in the instance hierarchy.
	\return true on error
 */
bool
Node::import_attributes(const bool_connect_policy& b) {
if (b.is_atomic()) {
	if (has_mk_exclhi() || has_mk_excllo()) {
		cerr << "Atomic nodes may not participate in mk_excl rings."
			<< endl;
		return true;
	}
	mark_atomic();
} else {
	// the only attributes we care about:
	if (b.may_weak_interfere())	allow_weak_interference();
	if (b.may_interfere())		allow_interference();
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
Node::dump_attributes(ostream& o) const {
	if (may_interfere())	o << " ignore_interference";
	if (may_weak_interfere())	o << " ignore_weak_interference";
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps structural information about the Node only.
	Of each up/dn pair, the first is the strong rule, 
		the second is weak (may not be printed).
	TODO: dump attributes?
 */
ostream&
Node::dump_struct(ostream& o) const {
	ostream_iterator<expr_index_type> osi(o, ", ");
	o << "fanin-processes: ";
#if VECTOR_NODE_FANIN
	std::copy(fanin.begin(), fanin.end(), osi);
#else
	std::copy(begin(fanin), end(fanin), osi);
#endif
	// o << endl;
	// TODO: expand process fanins to rule expresions?
	o << " fanout: ";
#if 1
//	o << '<' << fanout.size() << "> ";
	std::copy(fanout.begin(), fanout.end(), osi);
	// std::copy(&fanout[0], &fanout[fanout.size()], osi);
#else
	size_t i = 0;
	for ( ; i<fanout.size(); ++i)
		o << fanout[i] << ", ";
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints a dot-edge from this node to each fanout expression.  
	\param s is the name of the node corresponding to the tail 
	of the edge.  
 */
ostream&
Node::dump_fanout_dot(ostream& o, const string& s) const {
	const_fanout_iterator i(fanout.begin());
	const const_fanout_iterator e(fanout.end());
	for ( ; i!=e; ++i) {
		o << s << " -> EXPR_" << *i << ';' << endl;
	}
	return o;
}

//=============================================================================
// class fanin_state_type method definitions

/**
	Ripped from ExprState::dump_state()
 */
ostream&
fanin_state_type::dump_state(ostream& o) const {
	return o << "ctdn: " << countdown << " X: " << unknowns << "(/" <<
		size << ')' << " pull: " << size_t(pull());
}

//=============================================================================
// class NodeState method definitions

const uchar
NodeState::value_to_char[3] = { '0', '1', 'X' };

// also used for inverting pull state
const value_enum
NodeState::invert_value[3] = { LOGIC_HIGH, LOGIC_LOW, LOGIC_OTHER };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This just initializes all nodes with unknown values.  
	This preserves the watchpoint and breakpoint state of all nodes.  
	Transition counts are reset to 0.
 */
void
NodeState::initialize(void) {
	x_value_and_cause();
	tcount = 0;
#if PRSIM_TRACK_LAST_EDGE_TIME
	last_edge_time[LOGIC_LOW] = -1.0;
	last_edge_time[LOGIC_HIGH] = -1.0;
	last_edge_time[LOGIC_OTHER] = -1.0;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This just initializes all nodes with unknown values.  
	This preserves the watchpoint and breakpoint state of all nodes.  
 */
void
NodeState::x_value_and_cause(void) {
	event_index = INVALID_EVENT_INDEX;
	causes.initialize();
	value = LOGIC_OTHER;
	state_flags |= NODE_INITIALIZE_SET_MASK;
	state_flags &= ~NODE_INITIALIZE_CLEAR_MASK;
	pull_up_state STR_INDEX(NORMAL_RULE).initialize();
	pull_dn_state STR_INDEX(NORMAL_RULE).initialize();
#if PRSIM_WEAK_RULES
	pull_up_state STR_INDEX(WEAK_RULE).initialize();
	pull_dn_state STR_INDEX(WEAK_RULE).initialize();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
__count_fanins(fanin_state_type& s, const fanin_array_type& v) {
	s.size += v.size();
}

void
NodeState::count_fanins(const faninout_struct_type& f) {
	__count_fanins(pull_up_state STR_INDEX(NORMAL_RULE), 
		f.pull_up STR_INDEX(NORMAL_RULE));
	__count_fanins(pull_dn_state STR_INDEX(NORMAL_RULE), 
		f.pull_dn STR_INDEX(NORMAL_RULE));
#if PRSIM_WEAK_RULES
	__count_fanins(pull_up_state STR_INDEX(WEAK_RULE), 
		f.pull_up STR_INDEX(WEAK_RULE));
	__count_fanins(pull_dn_state STR_INDEX(WEAK_RULE), 
		f.pull_dn STR_INDEX(WEAK_RULE));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This initializes the state and also wipes the watch/break flags.  
	Also wipes out IN_CHANNEL.
 */
void
NodeState::reset(void) {
	initialize();
	state_flags = NODE_INITIAL_STATE_FLAGS;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps stateful information about the Node only.  
 */
ostream&
NodeState::dump_value(ostream& o) const {
	return o << value_to_char[value & LOGIC_MASK];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps stateful information about the Node only.  
 */
ostream&
NodeState::dump_state(ostream& o) const {
	o << "val = ";
	return dump_value(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lexes char to node value.  
	NOTE: reserving H/L for weak logic levels.  
	\return 0, 1, 2 (X), or LOGIC_INVALID on error.  
 */
value_enum
NodeState::char_to_value(const char v) {
	switch (v) {
	case 'f':	// fall-through
	case 'F':	// fall-through
	case '0': return LOGIC_LOW;

	case 't':	// fall-through
	case 'T':	// fall-through
	case '1': return LOGIC_HIGH;

	case 'X':	// fall-through
	case 'x':	// fall-through
	case 'U':	// fall-through
	case 'u':	// fall-through
		return LOGIC_OTHER;
	case '?': {
		util::numeric::rand48<bool> r;
		return r() ? LOGIC_HIGH : LOGIC_LOW;
	}
	default:
		return LOGIC_INVALID;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_enum
NodeState::char_to_pull(const char v) {
	switch (v) {
	case '0': return PULL_OFF;

	case '1': return PULL_ON;

	case 'X':	// fall-through
	case 'x':	// fall-through
	case 'U':	// fall-through
	case 'u':	// fall-through
		return PULL_WEAK;
	default:
		return PULL_INVALID;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lexes string to node value.  
	TODO: add synonymous character mappings.  
	\return 0, 1, 2 (X), or LOGIC_INVALID on error.  
 */
value_enum
NodeState::string_to_value(const string& v) {
	if (v.length() != 1) {
		return LOGIC_INVALID;
	} else {
		return char_to_value(v[0]);
	}
}

pull_enum
NodeState::string_to_pull(const string& v) {
	if (v.length() != 1) {
		return PULL_INVALID;
	} else {
		return char_to_pull(v[0]);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if node is strongly (definitely) interfering.
	Either strong vs. strong, or weak vs. weak.
	But not strong vs. weak.  
 */
bool
NodeState::interfering(void) const {
#if PRSIM_WEAK_RULES
	// check strong vs. strong rules
	const pull_enum u(get_pull_struct(true, NORMAL_RULE).pull());
	const pull_enum d(get_pull_struct(false, NORMAL_RULE).pull());
	if (u == PULL_ON && d == PULL_ON) { return true; }
	// else check weak vs. weak rules
	const pull_enum wu(get_pull_struct(true, WEAK_RULE).pull());
	const pull_enum wd(get_pull_struct(false, WEAK_RULE).pull());
	return (wu == PULL_ON && wd == PULL_ON);
#else
	const pull_enum u(get_pull_struct(true).pull());
	const pull_enum d(get_pull_struct(false).pull());
	return (u == PULL_ON && d == PULL_ON);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if node is weakly (possibly) interfering.
	This excludes nodes that are strongly interfering.  
 */
bool
NodeState::weak_interfering(void) const {
#if PRSIM_WEAK_RULES
	// check strong vs. strong rules
	const pull_enum u(get_pull_struct(true, NORMAL_RULE).pull());
	const pull_enum d(get_pull_struct(false, NORMAL_RULE).pull());
	if (u != PULL_OFF && d != PULL_OFF &&
		(u == PULL_WEAK || d == PULL_WEAK)) { return true; }
	// else check weak vs. weak rules
	const pull_enum wu(get_pull_struct(true, WEAK_RULE).pull());
	const pull_enum wd(get_pull_struct(false, WEAK_RULE).pull());
	return (wu != PULL_OFF && wd != PULL_OFF &&
		(wu == PULL_WEAK || wd == PULL_WEAK));
#else
	const pull_enum u(get_pull_struct(true).pull());
	const pull_enum d(get_pull_struct(false).pull());
	return (u != PULL_OFF && d != PULL_OFF &&
		(u == PULL_WEAK || d == PULL_WEAK));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_enum
NodeState::drive_state(void) const {
	// check strong vs. strong rules
	const pull_enum u(get_pull_struct(true, NORMAL_RULE).pull());
	const pull_enum d(get_pull_struct(false, NORMAL_RULE).pull());
#if PRSIM_WEAK_RULES
	// else check weak vs. weak rules
	const pull_enum wu(get_pull_struct(true, WEAK_RULE).pull());
	const pull_enum wd(get_pull_struct(false, WEAK_RULE).pull());
#endif
	if (u == PULL_ON || d == PULL_ON) { return PULL_ON; }
#if PRSIM_WEAK_RULES
	else if (wu == PULL_ON || wd == PULL_ON) { return PULL_ON; }
#endif
	else if (u == PULL_WEAK || d == PULL_WEAK) { return PULL_WEAK; }
#if PRSIM_WEAK_RULES
	else if (wu == PULL_WEAK || wd == PULL_WEAK) { return PULL_WEAK; }
#endif
	else return PULL_OFF;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: we don't save the event index because it may change upon 
	reconstruction due to event allocation.  
	It is reconstructed by load_event, which is called during
	event queue reconstruction.  
	Otherwise, the event_index is left as INVALID_EVENT_INDEX
 */
void
NodeState::save_state(ostream& o) const {
#if NODE_ALIGN_MARKERS
	static const char cc = 0xCC;
	write_value(o, cc);
#endif
	write_value(o, value);
	write_value(o, state_flags);
//	omit event index, which is reconstructed
	causes.save_state(o);
#if PRSIM_TRACK_LAST_EDGE_TIME
	write_value(o, last_edge_time[0]);
	write_value(o, last_edge_time[1]);
	write_value(o, last_edge_time[2]);
#endif
	write_value(o, tcount);
#if NODE_ALIGN_MARKERS
	static const char dd = 0xDD;
	write_value(o, dd);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The node state must be reeconstructed *before* the event queue
	is reconstructed, because it overwrites the event_index field.  
	\pre node is reset to unknown value to be able to reconstruct
		intermediate expression state through propagation.  
 */
void
NodeState::load_state(istream& i) {
	// no longer true with global !GND and !Vdd
	// INVARIANT(value == LOGIC_OTHER);
#if NODE_ALIGN_MARKERS
	char dd;
	read_value(i, dd);
#endif
	read_value(i, value);
	read_value(i, state_flags);
//	omit event index, which is reconstructed
	INVARIANT(event_index == INVALID_EVENT_INDEX);
	causes.load_state(i);
#if PRSIM_TRACK_LAST_EDGE_TIME
	read_value(i, last_edge_time[0]);
	read_value(i, last_edge_time[1]);
	read_value(i, last_edge_time[2]);
#endif
	read_value(i, tcount);
#if NODE_ALIGN_MARKERS
	read_value(i, dd);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
NodeState::dump_checkpoint_state_header(ostream& o) {
	o << "value\tflags\tcause\ttcount";
#if PRSIM_TRACK_LAST_EDGE_TIME
	o << "\ttime(0,1,X)";
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
NodeState::dump_checkpoint_state(ostream& o, istream& i) {
	this_type temp;
	temp.load_state(i);
	return temp.dump_debug(o, false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
NodeState::dump_debug(ostream& o, const bool h) const {
if (h) {
	dump_checkpoint_state_header(o) << endl;
}
	dump_value(o) << "\t0x" << std::hex << size_t(state_flags) <<
		'\t';
	causes.dump_checkpoint_state(o);
	o << '\t' << tcount;
#if PRSIM_TRACK_LAST_EDGE_TIME
	o << "\t(" << last_edge_time[LOGIC_LOW] << ',' 
		<< last_edge_time[LOGIC_HIGH] << ',' 
		<< last_edge_time[LOGIC_OTHER] << ')';
#endif
	return o;
}

//=============================================================================
#if 0
/**
	Hmmm... need canonical name.  
 */
void
NodeState::status_dumper::operator () (const NodeState& n) {
	if (n.current_value() == match_val) {
		os << ...;
	}
}
#endif

//=============================================================================
// class pull_set method definitions
ostream&
pull_set::dump(ostream& o) const {
	o << "pull up:" << up << " dn:" << dn;
#if PRSIM_WEAK_RULES
	o << " wup:" << wup << " wdn:" << wdn;
#endif
	return o;
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

