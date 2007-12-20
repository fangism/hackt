/**
	\file "sim/prsim/Command-prsim-exported.h"
	$Id: Command-prsim-export.h,v 1.1.2.1 2007/12/20 18:35:49 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_COMMAND_PRSIM_EXPORTED_H__
#define	__HAC_SIM_PRSIM_COMMAND_PRSIM_EXPORTED_H__

#include "sim/prsim/State-prsim.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================

typedef	State::time_type		time_type;
typedef	State::node_type		node_type;
typedef	State::step_return_type		step_return_type;


static
inline
node_index_type
GET_NODE(const State::step_return_type& x) {
	return x.first;
}

static
inline
node_index_type
GET_CAUSE(const State::step_return_type& x) {
	return x.second;
}


ostream&
print_watched_node(ostream&, const State&, 
	const node_index_type, const string&);

ostream&
print_watched_node(ostream&, const State&, 
	const step_return_type&);

ostream&
print_watched_node(ostream&, const State&, 
	const step_return_type&, const string&);

int
prsim_advance(State&, const time_type stop_time, bool show_break);

//=============================================================================
}	// end namespace PRSIM
}	// end namepsace SIM
}	// end namespace HC

#endif	// __HAC_SIM_PRSIM_COMMAND_PRSIM_EXPORTED_H__

