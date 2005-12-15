/**
	\file "sim/time.h"
	Useful time typedefs and structures.  
	Also contains some time macros and inline functions.  
	TODO: use mpz_t for really long integers.  
	$Id: time.h,v 1.1.2.1 2005/12/15 04:46:04 fang Exp $
 */

#ifndef	__SIM_TIME_H__
#define	__SIM_TIME_H__

namespace HAC {
namespace SIM {
//=============================================================================
/**
	Floating point time.  
 */
typedef	double		real_time;

/**
	Integer time.
	TODO: should be fixed at 32 or 64 b, depending on uintxx_t.
 */
typedef	long		int_time;

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC


#endif	// __SIM_TIME_H__

