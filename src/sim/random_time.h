/**
	\file "sim/random_time.h"
	$Id: random_time.h,v 1.1.2.1 2006/04/02 23:11:04 fang Exp $
 */

#ifndef	__HAC_SIM_RANDOM_TIME_H__
#define	__HAC_SIM_RANDOM_TIME_H__

#include "sim/time.h"
#include "util/numeric/random.h"
#include <cmath>

namespace HAC {
namespace SIM {
using util::numeric::rand48;

//=============================================================================
/**
	Policy for determining the maximum random time limit.
 */
template <typename>
struct random_time_limit;

template <>
struct random_time_limit<discrete_time> {
	typedef	unsigned short			type;
};

template <>
struct random_time_limit<real_time> {
	typedef	real_time			type;
};

//=============================================================================
/**
	Defined by specializations-only.  
	Interface: operator () that returns number with semi-infinite, 
	heavy-tailed distribution.  
	e.g. exp(-x), or 1/x
 */
template <typename N>
struct random_time;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should return positive time in range [0, 2^16).
 */
template <>
struct random_time<unsigned short> {
	typedef	unsigned short			return_type;
	typedef	return_type			limit_type;
	typedef	unsigned short			seed_type[3];

	static const double			ln_max_val;

	return_type
	operator () (void) const {
		const double d = rand48<double>()();
		return return_type(exp(d*ln_max_val) -1);
	}

	return_type
	operator () (seed_type& s) const {
		const double d = rand48<double>()(s);
		return return_type(exp(d*ln_max_val) -1);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should return positive time in range [0, 2^16).
 */
template <>
struct random_time<double> {
	typedef	double				return_type;
	/**
		Change this to allow higher maximum random time values.  
	 */
	typedef	unsigned short			limit_type;
	typedef	unsigned short			seed_type[3];

	/**
		This value is intentionally range-limited to 2^16.
	 */
	static const double			ln_max_val;

	return_type
	operator () (void) const {
		const double d = rand48<double>()();
		return exp(d*ln_max_val) -1.0;
	}

	return_type
	operator () (seed_type& s) const {
		const double d = rand48<double>()(s);
		return exp(d*ln_max_val) -1.0;
	}
};

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_RANDOM_TIME_H__

