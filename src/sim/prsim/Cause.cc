/**
	\file "sim/prsim/Cause.cc"
	Implementation of EventCause node.  
	$Id: Cause.cc,v 1.3 2008/11/05 23:03:43 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include "sim/prsim/Node.hh"
#include "util/IO_utils.tcc"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.hh"
using util::write_value;
using util::read_value;

//=============================================================================
ostream&
EventCause::dump_raw(ostream& o) const {
	typedef	NodeState	node_type;
	return o << "node: " << node << " -> val: " <<
		node_type::value_to_char[size_t(val)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
#if 0 && PRSIM_TRACK_CAUSE_TIME
	write_value(o, cause_time[0]);
	write_value(o, cause_time[1]);
	write_value(o, cause_time[2]);
#endif
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
#if 0 && PRSIM_TRACK_CAUSE_TIME
	read_value(i, cause_time[0]);
	read_value(i, cause_time[1]);
	read_value(i, cause_time[2]);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
LastCause::dump_checkpoint_state(ostream& o) const {
	return o << '(' << caused_by_node[0] << ',' <<
		NodeState::value_to_char[caused_by_value[0]] << "),(" <<
		caused_by_node[1] << ',' <<
		NodeState::value_to_char[caused_by_value[1]] << "),(" <<
		caused_by_node[2] << ',' <<
		NodeState::value_to_char[caused_by_value[2]] << ')';
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

