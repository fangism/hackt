/**
	\file "sim/command_error_codes.h"
	$Id: command_error_codes.h,v 1.3 2009/02/19 02:58:34 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_ERROR_CODES_H__
#define	__HAC_SIM_COMMAND_ERROR_CODES_H__

#include "util/string_fwd.h"

namespace HAC {
namespace SIM {

//-----------------------------------------------------------------------------
/**
	These error codes are to be returned by the simulator methods.  
	Policy enumeration for determining simulation behavior
	in the event of a delay-insensitivity violation.  
	Absolute values matter, in increasing order of severity.
 */
enum error_policy_enum {
	ERROR_IGNORE = 0,
	ERROR_NONE = ERROR_IGNORE,
	ERROR_WARN = 1,
	ERROR_NOTIFY = ERROR_WARN,
	ERROR_BREAK = 2,
	/// return control to user temporarily before resuming script
	ERROR_INTERACTIVE = 3,
	/// halt the simulation immediately
	ERROR_FATAL = 4,
	ERROR_INVALID
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	These codes are used by command interpreter.  
	A few reserved exit codes for main functions.  
	Should use an int for base integer type.
	TODO: validate the range of these values w.r.t. 
		return type size or standard enums.
 */
struct command_error_codes {
enum CommandStatus {
	FATAL = -5,	///< terminate immediately (e.g. assert fail)
	INTERACT = -4,	///< open a sub-shell to inspect state
	BADFILE = -3,	///< source file not found
	SYNTAX = -2,	///< bad syntax
	UNKNOWN = -1,	///< unknown command
	NORMAL = 0,	///< command executed fine
	BADARG = 1,	///< other error with input
	END = 0xFF	///< normal exit, such as EOF
};	// end enum CommandStatus
};	// end struct command_error_codes

typedef	command_error_codes::CommandStatus	CommandStatus;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// the following functions are defined in "sim/command_base.cc"

extern
const char*
error_policy_string(const error_policy_enum);

extern
error_policy_enum
string_to_error_policy(const std::string&);

inline
bool
valid_error_policy(const error_policy_enum e) {
	return e != ERROR_INVALID;
}

extern
CommandStatus
error_policy_to_status(const error_policy_enum);

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_ERROR_CODES_H__

