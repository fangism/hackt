/**
	\file "sim/prsim/Cause.cc"
	Implementation of EventCause node.  
	$Id: Cause.cc,v 1.2 2006/08/12 00:36:28 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include "sim/prsim/Node.h"
#include "util/IO_utils.tcc"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;

//=============================================================================
void
EventCause::save_state(ostream& o) const {
	write_value(o, node);
	write_value(o, val);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventCause::load_state(istream& i) {
	read_value(i, node);
	read_value(i, val);
}

//=============================================================================
void
LastCause::save_state(ostream& o) const {
	write_value(o, caused_by_node[0]);
	write_value(o, caused_by_node[1]);
	write_value(o, caused_by_node[2]);
	write_value(o, caused_by_value[0]);
	write_value(o, caused_by_value[1]);
	write_value(o, caused_by_value[2]);
	write_value(o, caused_by_value[3]);	// unused
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
LastCause::load_state(istream& i) {
	read_value(i, caused_by_node[0]);
	read_value(i, caused_by_node[1]);
	read_value(i, caused_by_node[2]);
	read_value(i, caused_by_value[0]);
	read_value(i, caused_by_value[1]);
	read_value(i, caused_by_value[2]);
	read_value(i, caused_by_value[3]);	// unused
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
LastCause::dump_checkpoint_state(ostream& o) const {
	return o << '(' << caused_by_node[0] << ',' <<
		NodeState::value_to_char[caused_by_value[0]] << "), (" <<
		caused_by_node[1] << ',' <<
		NodeState::value_to_char[caused_by_value[1]] << "), (" <<
		caused_by_node[2] << ',' <<
		NodeState::value_to_char[caused_by_value[2]] << ')';
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

