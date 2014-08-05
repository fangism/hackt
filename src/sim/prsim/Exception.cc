/**
	\file "sim/prsim/Exception.cc"
 */

#include <iostream>
#include "sim/prsim/Exception.hh"
#include "sim/prsim/State-prsim.hh"
#include "util/IO_utils.tcc"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::endl;
using util::write_value;
using util::read_value;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Support for saving exceptions across checkpoints.  
 */
void
generic_exception::save(ostream& o) const {
	write_value(o, node_id);
	write_value(o, char(policy));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
generic_exception::load(istream& i) {
	read_value(i, node_id);
	char c;
	read_value(i, c);
	policy = error_policy_enum(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Since we aggregate exceptions, there may now be duplicate 
	messages on node-events that cause multiple exceptions.  
	TODO: (low priority) fix duplicate node diagnostics
 */
error_policy_enum
generic_exception::inspect(const State& s, ostream& o) const {
	if (policy >= ERROR_INTERACTIVE) {
		o << "Halting on node: " <<
			s.get_node_canonical_name(node_id) << endl;
	}
	return policy;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

