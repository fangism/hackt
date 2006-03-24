/**
	\file "sim/prsim/Node.cc"
	Implementation of PRS node.  
	$Id: Node.cc,v 1.2.26.2 2006/03/24 00:01:50 fang Exp $
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

const char
Node::value_to_char[3] = { '0', '1', 'X' };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Node::Node() : pull_up_index(0), pull_dn_index(0), fanout() {
	INVARIANT(!fanout.size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Node::~Node() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the state of the node.
	Not expected to be called frequently.  
 */
void
Node::initialize(void) {
	value = LOGIC_OTHER;
	breakpoint = 0;
	tcount = 0;
}

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
	Dumps stateful information about the Node only.  
 */
ostream&
Node::dump_value(ostream& o) const {
#if 0
	return  (value & LOGIC_OTHER) ?
		// or do you prefer 'U'?
		o << 'X' :
		o << size_t(value & LOGIC_VALUE);
#else
	return o << value_to_char[value & LOGIC_MASK];
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps stateful information about the Node only.  
 */
ostream&
Node::dump_state(ostream& o) const {
	o << "val = ";
	return dump_value(o);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lexes string to node value.  
	TODO: add synonymous character mappings.  
	\return 0, 1, 2 (X), or -1 on error.  
 */
char
Node::string_to_value(const string& v) {
	if (v.length() != 1) {
		return -1;
	} else {
		switch (v[0]) {
		case '0': return 0;
		case '1': return 1;
		case 'X':
		case 'x':
			return 2;
		default:
			return -1;
		}
	}
}

//=============================================================================
// class NodeState method definitions

void
NodeState::initialize(void) {
	parent_type::initialize();
	event_index = INVALID_EVENT_INDEX;
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

