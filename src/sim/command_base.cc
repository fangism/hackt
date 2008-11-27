/**
	\file "sim/command_base.cc"
	$Id: command_base.cc,v 1.1.50.1 2008/11/27 03:40:51 fang Exp $
 */

#include <iostream>
#include <string>
#include "sim/command_base.h"
#include "util/NULL.h"

namespace HAC {
namespace SIM {

//=============================================================================
/**
	Converts simulator's error code signal into an error code
	for the interpreter.  
 */
CommandStatus
error_policy_to_status(const error_policy_enum e) {
switch (e) {
case ERROR_IGNORE:
case ERROR_WARN:
case ERROR_BREAK:
	return command_error_codes::NORMAL;
case ERROR_INTERACTIVE:
	return command_error_codes::INTERACT;
case ERROR_FATAL:
default:
	return command_error_codes::FATAL;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char*
error_policy_string(const error_policy_enum e) {
	switch (e) {
	case ERROR_IGNORE:	return "ignore";
	case ERROR_WARN:	return "warn";
	case ERROR_BREAK:	return "break";
	case ERROR_INTERACTIVE:	return "interactive";
	case ERROR_FATAL:	return "fatal";
	default:		DIE;
	}	// end switch
	 return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Too lazy to write a map...
 */
error_policy_enum
string_to_error_policy(const string& s) {
	static const string _ignore("ignore");
	static const string _warn("warn");
	static const string _notify("notify");
	static const string _break("break");
	static const string _interactive("interactive");
	static const string _fatal("fatal");
	if (s == _ignore) {
		return ERROR_IGNORE;
	} else if (s == _warn || s == _notify) {
		return ERROR_WARN;
	} else if (s == _break) {
		return ERROR_BREAK;
	} else if (s == _interactive) {
		return ERROR_INTERACTIVE;
	} else if (s == _fatal) {
		return ERROR_FATAL;
	}
	// else
	return ERROR_INVALID;
}

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

