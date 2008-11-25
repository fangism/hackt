/**
	\file "sim/command_error_codes.h"
	$Id: command_error_codes.h,v 1.1.2.2 2008/11/25 08:36:39 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_ERROR_CODES_H__
#define	__HAC_SIM_COMMAND_ERROR_CODES_H__

#include "sim/prsim/devel_switches.h"

namespace HAC {
namespace SIM {

/**
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

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_ERROR_CODES_H__

