/**
	\file "sim/random_time.hh"
	$Id: random_time.hh,v 1.4 2008/11/29 03:24:49 fang Exp $
 */

#ifndef	__HAC_SIM_RANDOM_TIME_H__
#define	__HAC_SIM_RANDOM_TIME_H__

#include "sim/time.hh"
#include "util/numeric/random.hh"
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
	Distribution is an exponential variate.
 */
template <>
struct random_time<unsigned short> {
	typedef	unsigned short			return_type;
	typedef	return_type			limit_type;
	typedef	unsigned short			seed_type[3];

	static const double			ln_max_val;

	return_type
	operator () (void) const {
		// const double d = rand48<double>()();
                double d;
		do
			d = rand48<double>()();
                while(d == 0.0);
		// return return_type(exp(d*ln_max_val) -1);
		const double e = -1 * log(d);
		//return return_type((0x01 << 11)*e); //(2^11)*e
		return return_type(e);
	}

	return_type
	operator () (seed_type& s) const {
		// const double d = rand48<double>()(s);
		double d;
		do
			d = rand48<double>()();
		while(d == 0.0);
		// return return_type(exp(d*ln_max_val) -1);
                const double e = -1 * log(d);
		//return return_type((0x01 << 11)*e); //(2^11)*e
 		return return_type(e);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should return positive time in range [0, 2^16).
	Distribution is an exponential variate.
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
		// const double d = rand48<double>()();
		double d;
		do
			d = rand48<double>()();
		while(d == 0.0);
		// return exp(d*ln_max_val) -1.0;
                const double e = -1 * log(d);
		//return (0x01 << 11))*e; //(2^11)*e
		return e;
	}

	return_type
	operator () (seed_type& s) const {
		// const double d = rand48<double>()(s);
		double d;
		do
			d = rand48<double>()();
		while(d == 0.0);
		// return exp(d*ln_max_val) -1.0;
                const double e = -1 * log(d);
                //return (0x01 << 11)*e; //(2^11)*e
                return e;
	}
};

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_RANDOM_TIME_H__

