/**
	\file "sim/prsim/Node.cc"
	Implementation of PRS node.  
	$Id: Node.cc,v 1.2.26.5 2006/03/31 23:47:07 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <string>
#include <iterator>
#include <algorithm>
#include "sim/prsim/Node.h"
#include "util/macros.h"
#include "util/stacktrace.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
using std::ostream_iterator;
using std::string;

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
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "fanout[" << fanout.size() -1
		<< "]=" << fanout.back() << endl;
#endif
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
	else			o << '-';
	o << ", dn: ";
	if (pull_dn_index)	o << pull_dn_index;
	else			o << '-';
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
	value = LOGIC_OTHER;
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
	value = LOGIC_OTHER;
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
	Lexes string to node value.  
	TODO: add synonymous character mappings.  
	\return 0, 1, 2 (X), or -1 on error.  
 */
char
NodeState::string_to_value(const string& v) {
	if (v.length() != 1) {
		return -1;
	} else {
		switch (v[0]) {
		case '0': return LOGIC_LOW;
		case '1': return LOGIC_HIGH;
		case 'X':
		case 'x':
			return LOGIC_OTHER;
		default:
			return -1;
		}
	}
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

