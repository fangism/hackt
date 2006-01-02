/**
	\file "sim/prsim/Node.cc"
	Implementation of PRS node.  
	$Id: Node.cc,v 1.1.2.3 2006/01/02 23:13:36 fang Exp $
 */

#include <iostream>
#include <iterator>
#include <algorithm>
#include "sim/prsim/Node.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
using std::ostream_iterator;

//=============================================================================
// class Node method definitions

Node::Node() : pull_up_index(0), pull_dn_index(0), fanout() { }

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
	fanout.resize(fanout.size() +1);
	fanout[fanout.size() -1] = ei;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps structural information about the Node only.
 */
ostream&
Node::dump_struct(ostream& o) const {
	o << "up: " << pull_up_index << ", dn: " << pull_dn_index <<
		" fanout: ";
	ostream_iterator<expr_index_type> osi(o, ", ");
	std::copy(&fanout[0], &fanout[fanout.size()], osi);
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

