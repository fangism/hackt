/**
	\file "sim/command.cc"
	$Id: command.cc,v 1.2 2007/01/21 06:00:12 fang Exp $
 */

#include <iostream>
#include "sim/command.h"
#include "util/null.h"

namespace HAC {
namespace SIM {
//=============================================================================
// class CommandBase method definitions

CommandBase::CommandBase() : _name(), _brief(), _usage(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CommandBase::CommandBase(const string& _n, const string& _b,
		const usage_ptr_type _u) :
		_name(_n), _brief(_b), _usage(_u) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CommandBase::~CommandBase() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
CommandBase::usage(ostream& o) const {
	if (_usage) {
		(*_usage)(o);
	} else {
		o << "help/usage unavailable." << std::endl;
	}
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

