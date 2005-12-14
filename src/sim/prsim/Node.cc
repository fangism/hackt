/**
	\file "sim/prsim/Node.cc"
	Implementation of PRS node.  
	$Id: Node.cc,v 1.1.2.1 2005/12/14 05:16:52 fang Exp $
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

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

