/**
	\file "util/numeric/random.h"
	Template specializations for random numbers, 
	wrapper interface to stdlib *rand48 functions.  
	$Id: random.h,v 1.4.4.2 2011/04/19 22:31:22 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_RANDOM_H__
#define	__UTIL_NUMERIC_RANDOM_H__

#include "util/libc.h"
#include <cstdlib>
#include <iosfwd>

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

	rand48() { }

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

	rand48() { }

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
#if !TYPE_EQUIV_SIZE_T_UNSIGNED_LONG && (SIZEOF_SIZE_T == SIZEOF_LONG)
/**
	Conditional specialization, depending on whether or not
	size_t is equivalent to unsigned long.
	If they are not, they must be the same size.  
 */
template <>
struct rand48<size_t> : public rand48<unsigned long> {
	typedef	rand48<unsigned long>	impl_type;
	typedef	impl_type::seed_type	seed_type;

	rand48() { }

	// inherit the rest
};	// end struct rand48
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Integer rand48 interface.  
	Q: what about ILP64 where long is 64b? See TODO.
 */
template <>
struct rand48<long> {
	typedef	unsigned short		seed_type[3];

	rand48() { }

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	50/50 coin flip random boolean generator.
 */
template <>
struct rand48<bool> {
	typedef	rand48<long>		base_rand;

	rand48() { }

	bool
	operator () (void) const {
		return base_rand()() & 0x1;	// or any bit
	}

	bool
	operator () (base_rand::seed_type& s) const {
		return base_rand()(s) & 0x1;	// or any bit
	}

};	// end struct rand48

//=============================================================================
extern
void
write_seed48(std::ostream&);

extern
void
read_seed48(std::istream&);

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_RANDOM_H__

