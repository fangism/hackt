/**
	\file "util/timer.h"
	$Id: $
 */

#ifndef	__UTIL_TIMER_H__
#define	__UTIL_TIMER_H__

#include <iosfwd>
#include <ctime>
#include "util/attributes.h"

namespace util {
using std::ostream;
using std::time_t;

/**
	A scope-based timer class for measure time spent in functions.
	TODO: enable time-out to pause timer and resume timer
	to time self without time in subroutines.  
 */
struct timer {
	const bool			enable;
	ostream&			os;
	/// string is light, not copied
	const char*			id;
	time_t				start;

	explicit
	timer(const bool, ostream&, const char*);

	~timer();

	const time_t&
	start_time(void) const { return start; }

} __ATTRIBUTE_UNUSED__ ;	// end struct timer

}	// end namespace util

#endif	// __UTIL_TIMER_H__
