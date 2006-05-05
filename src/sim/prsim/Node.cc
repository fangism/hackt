/**
	\file "sim/prsim/Node.cc"
	Implementation of PRS node.  
	$Id: Node.cc,v 1.5.6.8 2006/05/05 04:55:40 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <string>
#include <iterator>
#include <algorithm>
#include "sim/prsim/Node.h"
#include "util/macros.h"
#include "util/stacktrace.h"
#include "util/IO_utils.tcc"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
using std::ostream_iterator;
using std::string;
using util::write_value;
using util::read_value;

//=============================================================================
// class Node method definitions

Node::Node() : pull_up_index(INVALID_EXPR_INDEX),
	pull_dn_index(INVALID_EXPR_INDEX), fanout(),
	struct_flags(NODE_DEFAULT_STRUCT_FLAGS) {
	INVARIANT(!fanout.size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Node::~Node() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: add ONLY if not already found in fanout list?
	Realloc-ing on every push_back could be slow... consider vector.  
 */
void
Node::push_back_fanout(const expr_index_type ei) {
	STACKTRACE("Node::push_back_fanout()");
#if 0
	fanout.resize(fanout.size() +1);
	const size_t i = fanout.size() -1;
	fanout[i] = ei;
#else
	fanout.push_back(ei);	// automatically resizes and reallocs
#endif
	STACKTRACE_INDENT_PRINT("fanout[" << fanout.size() -1
		<< "]=" << fanout.back() << endl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
Node::contains_fanout(const expr_index_type i) const {
	return (std::find(fanout.begin(), fanout.end(), i) != fanout.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps structural information about the Node only.
 */
ostream&
Node::dump_struct(ostream& o) const {
	o << "up: ";
	if (pull_up_index)	o << pull_up_index;
	else			o << '-';	// irrelevant
	o << ", dn: ";
	if (pull_dn_index)	o << pull_dn_index;
	else			o << '-';	// irrelevant
	o << " fanout: ";
#if 1
//	o << '<' << fanout.size() << "> ";
	ostream_iterator<expr_index_type> osi(o, ", ");
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
// class NodeState method definitions

const char
NodeState::value_to_char[3] = { '0', '1', 'X' };

const char
NodeState::invert_value[3] = { LOGIC_HIGH, LOGIC_LOW, LOGIC_OTHER };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This just initializes all nodes with unknown values.  
	This preserves the watchpoint and breakpoint state of all nodes.  
 */
void
NodeState::initialize(void) {
	event_index = INVALID_EVENT_INDEX;
	caused_by_node = INVALID_NODE_INDEX;
	value = LOGIC_OTHER;
	tcount = 0;
	state_flags |= NODE_INITIALIZE_SET_MASK;
	state_flags &= ~NODE_INITIALIZE_CLEAR_MASK;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This initializes the state and also wipes the watch/break flags.  
 */
void
NodeState::reset(void) {
	event_index = INVALID_EVENT_INDEX;
	caused_by_node = INVALID_NODE_INDEX;
	value = LOGIC_OTHER;
	tcount = 0;
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
	\return 0, 1, 2 (X), or -1 on error.  
 */
char
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
	default:
		return -1;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lexes string to node value.  
	TODO: add synonymous character mappings.  
	\return 0, 1, 2 (X), or -1 on error.  
 */
char
NodeState::string_to_value(const string& v) {
	if (v.length() != 1) {
		return -1;
	} else {
		return char_to_value(v[0]);
	}
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
	write_value(o, value);
	write_value(o, state_flags);
//	omit event index, which is reconstructed
	write_value(o, caused_by_node);
	write_value(o, tcount);
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
	INVARIANT(value == LOGIC_OTHER);
	read_value(i, value);
	read_value(i, state_flags);
//	omit event index, which is reconstructed
	INVARIANT(event_index == INVALID_EVENT_INDEX);
	read_value(i, caused_by_node);
	read_value(i, tcount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
NodeState::dump_checkpoint_state_header(ostream& o) {
	return o << "value\tflags\tcause\ttcount";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
NodeState::dump_checkpoint_state(ostream& o, istream& i) {
	this_type temp;
	temp.load_state(i);
	return temp.dump_value(o) << '\t' << size_t(temp.state_flags) <<
		'\t' << temp.caused_by_node << '\t' << temp.tcount;
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
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

