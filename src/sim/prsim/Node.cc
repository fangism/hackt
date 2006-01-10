/**
	\file "sim/prsim/Node.cc"
	Implementation of PRS node.  
	$Id: Node.cc,v 1.1.2.5 2006/01/10 05:37:28 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
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

//=============================================================================
// class Node method definitions

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
	std::copy(&fanout[0], &fanout[fanout.size()], osi);
#else
	size_t i = 0;
	for ( ; i<fanout.size(); i++)
		o << fanout[i] << ", ";
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps stateful information about the Node only.  
 */
ostream&
Node::dump_state(ostream& o) const {
	o << "val = ";
	if (value & LOGIC_OTHER)
		o << 'X';
	else	o << size_t(value & LOGIC_VALUE);
	return o;
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

