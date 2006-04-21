/**
	\file "sim/time.h"
	Useful time typedefs and structures.  
	Also contains some time macros and inline functions.  
	TODO: use mpz_t for really long integers.  
	$Id: time.h,v 1.3.6.2 2006/04/21 02:45:58 fang Exp $
 */

#ifndef	__HAC_SIM_TIME_H__
#define	__HAC_SIM_TIME_H__

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
typedef	long		discrete_time;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <typename T>
struct delay_policy;

template <>
struct delay_policy<real_time> {
	/**
		Use negative value for invalid time.  
	 */
	static const real_time	invalid_value;
	/**
		Default delay, when unspecified.  
	 */
	static const real_time	default_delay;
	static const real_time	zero;
};	// end struct delay_policy

template <>
struct delay_policy<discrete_time> {
	enum {	invalid_value = -1 };
	enum {	default_delay = 10 };
	enum {	zero = 0 };
};	// end struct delay_policy

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC


#endif	// __SIM_TIME_H__

