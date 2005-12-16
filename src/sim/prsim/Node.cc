/**
	\file "sim/prsim/Node.cc"
	Implementation of PRS node.  
	$Id: Node.cc,v 1.1.2.2 2005/12/16 02:43:20 fang Exp $
 */

#include "sim/prsim/Node.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================
// class Node method definitions

Node::Node() : fanout() { }

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

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

