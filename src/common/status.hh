/**
	\file "common/status.hh"
	Error condition/status and policy enumeration.
	This could eventually replace "util/boolean_types.h"
	$Id: status.hh,v 1.2 2010/04/30 23:58:49 fang Exp $
 */

#ifndef	__HAC_COMMON_STATUS_H__
#define	__HAC_COMMON_STATUS_H__

#include "util/size_t.h"

namespace HAC {
//=============================================================================
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just a pair of error counters.
	Could just be std::pair, but this is clearer.
	Common operations: addition
 */
struct error_count {
	size_t				warnings;
	size_t				errors;

	error_count() : warnings(0), errors(0) { }

	// adding constructor
	error_count(const error_count& l, const error_count& r) :
		warnings(l.warnings + r.warnings), 
		errors(l.errors + r.errors) { }

	// default copy-ctor
	// default POD dtor

	error_count&
	operator += (const error_count& r) {
		warnings += r.warnings;
		errors += r.errors;
		return *this;
	}

	error_count
	operator + (const error_count& r) const {
		return error_count(*this, r);
	}

};	// end struct error_count

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_COMMON_STATUS_H__

