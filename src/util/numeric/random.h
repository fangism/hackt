/**
	\file "util/numeric/random.h"
	Template specializations for random numbers, 
	wrapper interface to stdlib *rand48 functions.  
	$Id: random.h,v 1.2 2006/04/03 05:30:40 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_RANDOM_H__
#define	__UTIL_NUMERIC_RANDOM_H__

#include "util/libc.h"
#include <cstdlib>

namespace util {
namespace numeric {
//=============================================================================
/**
	Interface that returns a uniform distribution random value.  
	Defined by specialization only.  
 */
template <typename N>
struct rand48;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Double-precision rand48 interface.  
 */
template <>
struct rand48<double> {
	typedef	unsigned short		seed_type[3];
	/**
		\return value from [0.0, 1.0)
	 */
	double
	operator () (void) const {
		return drand48();
	}

	/**
		Uses an externally stored seed.  
	 */
	double
	operator () (seed_type& s) const {
		return erand48(s);
	}

};	// end struct rand48

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Integer rand48 interface.  
	Q: what about ILP64 where long is 64b? See TODO.
 */
template <>
struct rand48<unsigned long> {
	typedef	unsigned short		seed_type[3];
	/**
		\return value from [0, 2^31 -1]
	 */
	unsigned long
	operator () (void) const {
		return lrand48();
	}

	/**
		Uses an externally stored seed.  
	 */
	unsigned long
	operator () (seed_type& s) const {
		return nrand48(s);
	}

};	// end struct rand48

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Integer rand48 interface.  
	Q: what about ILP64 where long is 64b? See TODO.
 */
template <>
struct rand48<long> {
	typedef	unsigned short		seed_type[3];
	/**
		\return value from [-2^31, 2^31 -1]
	 */
	long
	operator () (void) const {
		return mrand48();
	}

	/**
		Uses an externally stored seed.  
	 */
	long
	operator () (seed_type& s) const {
		return jrand48(s);
	}

};	// end struct rand48


//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_RANDOM_H__

