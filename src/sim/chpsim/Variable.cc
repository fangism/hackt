/**
	\file "sim/chpsim/Variable.cc"
	$Id: Variable.cc,v 1.1.2.1 2006/12/11 00:40:23 fang Exp $
 */

#include "sim/chpsim/Variable.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
//=============================================================================
// class VariableBase method definitions

VariableBase::VariableBase() : event_subscribers() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VariableBase::~VariableBase() { }

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

