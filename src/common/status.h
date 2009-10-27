/**
	\file "common/status.h"
	Error condition/status and policy enumeration.
	This could eventually replace "util/boolean_types.h"
	$Id: status.h,v 1.1 2009/10/27 18:21:48 fang Exp $
 */

#ifndef	__HAC_COMMON_STATUS_H__
#define	__HAC_COMMON_STATUS_H__

namespace HAC {
/**
	Used for both setting error-handling policy and returning error value.
 */
enum error_status {
	STATUS_NORMAL = 0,
	OPTION_IGNORE = STATUS_NORMAL,
	// issue a diagnostic warning message and continue
	STATUS_WARNING = 1,
	OPTION_WARN = STATUS_WARNING,
	// signal an error condition
	STATUS_ERROR = 2,
	OPTION_ERROR = STATUS_ERROR
#if 0
	// violently error!
	STATUS_FATAL = 3,
	OPTION_FATAL = STATUS_FATAL
#endif
};

typedef	error_status			error_policy;

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_COMMON_STATUS_H__

