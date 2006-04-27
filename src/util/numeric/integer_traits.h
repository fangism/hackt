/**
	\file "util/numeric/integer_traits.h"
	The templates in this file allow compile time decisions
	based on traits of constant integer values.  
	$Id: integer_traits.h,v 1.9 2006/04/27 00:17:51 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_INTEGER_TRAITS_H__
#define	__UTIL_NUMERIC_INTEGER_TRAITS_H__

#include "util/size_t.h"
#include "util/ifthenelse.h"

//=============================================================================
// tags for natural number traits

namespace util {
namespace numeric {

struct prime_tag { };		typedef	prime_tag	not_composite_tag;
struct composite_tag { };	typedef	composite_tag	not_prime_tag;

//=============================================================================
// concept constraints: N <= M, etc...
// like assertions, but at compile time

//=============================================================================

template <size_t N>
struct is_power_of_2 {
	enum {	value = N && !(N & N-1) };
};

/**
	The number of significant bits, or position of most significant bit.
 */
template <size_t N>
struct significance {
	// static const size_t
	enum {	value = significance<(N >> 1)>::value +1 };
};

template <>
struct significance<0> {
	enum {	value = size_t(-1) };
};

template <bool B>
struct power_of_2_tag { enum { value = B}; };

// power_of_2_tag<true> and power_of_2_tag<false> are different types!

template <size_t N>
struct power_of_2_traits {
	typedef	power_of_2_tag<is_power_of_2<N>::value>	category;
};

// now can use power_of_2_traits<N> to replace multiplications and division
//	with arithmetic shifts, useful for sizeof() multiplication
// such operations would be distinguished using power_of_2_tag<true or false>

// multiplies already defined in "operators.h"
template <size_t N, class I>
inline
I
__multiply_by_constant(const I& val, power_of_2_tag<false> ) {
	return val * N;
}

template <size_t N, class I>
inline
I
__multiply_by_constant(const I& val, power_of_2_tag<true> ) {
	return val << significance<N>::value;
}

template <size_t N, class I>
inline
I
multiply_by_constant(const I& val) {
	return __multiply_by_constant<N,I>(val,
		typename power_of_2_traits<N>::category());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t N, class I>
inline
I
__divide_by_constant(const I& val, power_of_2_tag<false> ) {
	return val / N;
}

template <size_t N, class I>
inline
I
__divide_by_constant(const I& val, power_of_2_tag<true> ) {
	return val >> significance<N>::value;
}

template <size_t N, class I>
inline
I
divide_by_constant(const I& val) {
	return __divide_by_constant<N,I>(val,
		typename power_of_2_traits<N>::category());
}

//=============================================================================
template <size_t N, size_t L = 1, size_t H = N>
struct sqrt_floor {
private:
	// can cause internal compiler error on instantiation
	// static const size_t	value = size_t(sqrt(N));
#if 1
	enum {
		mid = (L + H + 1) >> 1		// average, round up
	};
#else
	// this causes gcc-3.2 to ICE:
	static const size_t mid = (L + H + 1) >> 1;
//	../numeric/integer_traits.h:98: Internal compiler error in 
//	instantiate_decl, at cp/pt.c:10052
#endif

public:
	// CAUTION: gcc-3.2 seems to treat enum values as signed, 
	// in which case the resulting comparison is between signed 
	// and unsigned, in which case compilation will die a horrible death.  
	typedef	typename ifthenelse_type<
				(N < mid*mid), 
				sqrt_floor<N,L,mid-1>,
				sqrt_floor<N,mid,H>
			>::result_type
		value_type;

	enum {
		value = value_type::value
	};
};

template <size_t N, size_t R>
struct sqrt_floor<N,R,R> {
	enum {	value = R	};
};

template <size_t L, size_t H>
struct sqrt_floor<0,L,H> {
	enum {	value = 0	};
};

template <size_t N>
struct is_square {
	// can cause internal compiler error on instantiation
	enum {
		value = (N == sqrt_floor<N>::value *sqrt_floor<N>::value)
	};
};

//=============================================================================
/**
	For unsigned int (size_t), N >= 14 will overflow.
	Is there a way to catch?
 */
template <size_t N>
struct factorial {
	enum { value = N * factorial<N-1>::value };
};

template <>
struct factorial<0> {
	enum { value = 1 };
};

template <>
struct factorial<1> {
	enum { value = 1 };
};

/**
	Can be smart to trying to prevent overflow.
	\param N total number.
	\param M subset, must be <= N.
 */
template <size_t N, size_t M>
struct combinations {
	/// this is the dumb way of computing
	enum { value = factorial<N>::value /
		(factorial<N-M>::value * factorial<M>::value)
	};
};

//=============================================================================

template <size_t N>
struct nth_fibonacci {
	enum {
		value = nth_fibonacci<N-1>::value + nth_fibonacci<N-2>::value
	};
};

template <>
struct nth_fibonacci<0> {
	enum { value = 1 };
};

template <>
struct nth_fibonacci<1> {
	enum { value = 1 };
};

template <>
struct nth_fibonacci<2> {
	enum { value = 2 };
};

// struct is_fibonacci

//=============================================================================
// primality: not ready yet

template <size_t>
struct is_prime;

#if 0
template <size_t>
struct prime_after_prime;
#endif

// the greatest odd number strictly less than
template <size_t N>
struct next_odd {
	enum { value = ((N-1) | 1) +2 };
};

// the greatest odd number strictly less than
template <size_t N>
struct prev_odd {
	enum { value = (N & size_t(-2)) -1 };
};

#if 0
template <>
struct prime_after_prime<2> {	typedef	enum { value = 3 } value_type;	};

template <>
struct prime_after_prime<3> {	typedef	enum { value = 5 } value_type;	};
#endif

/**
	Value is the greatest prime number strictly less than N.
	\param N must be odd if it is greater than 2.  
 */
template <size_t N>
struct prev_prime {
	enum {
		value = ifthenelse_value<is_prime<N-2>::value, 
			size_t, N-2, prev_prime<N-2>::value>::value
	};
};

template <>
struct prev_prime<3> {	typedef	enum { value = 2 } value_type;	};

template <>
struct prev_prime<2> {	typedef	enum { value = 2 } value_type;	};

template <>
struct prev_prime<1> {	typedef	enum { value = 2 } value_type;	};

template <>
struct prev_prime<0> {	typedef	enum { value = 2 } value_type;	};

#if 1
/**
	Stops when N is prime.
	\param N is not prime, must be odd (except 2).
 */
template <size_t N>
struct next_prime {
	enum {
		value = ifthenelse_value<is_prime<N+2>::value, 
			size_t, N+2, next_prime<N+2>::value>::value
	};
};

template <>
struct next_prime<0> {	typedef	enum { value = 2 } value_type;	};

template <>
struct next_prime<1> {	typedef	enum { value = 2 } value_type;	};

template <>
struct next_prime<2> {	typedef	enum { value = 3 } value_type;	};
#endif

#if 0
/**
	General algorithm for finding next prime at compile time.  
	If this is prime, the result is itself, to prevent unbounded
	recursion.
	\param N is prime.
 */
template <size_t N>
struct prime_after_prime {
	...
};
#endif

template <size_t N>
struct prime_before_prime {
	enum { value = prev_prime<N-2>::value };
};

template <>
struct prime_before_prime<3> { enum { value = 2 }; };

/**
	Assumes that all primes < MIN have been tested.
	\param N the integer in question.  
	\param Q the next prime factor to try.
 */
template <size_t N, size_t Q>
struct reverse_prime_test_loop {
	/**
		Value is false if there are factors.  
	 */
	enum {
		// order of evaluation should be switched...
		value = (N % Q) &&
			reverse_prime_test_loop<
				N, prime_before_prime<Q>::value
			>::value
	};
};

// specialization terminate counting down
template <size_t N>
struct reverse_prime_test_loop<N, 2> {
	enum {	value = (N % 2) };
};

#if 0
/**
	Forward (incremental) test loop.
 */
template <size_t N, size_t Q>
struct prime_test_loop<N, 2> {
	enum {
		value = (N % Q) &&
			prime_test_loop<
				// ideally, next_prime<Q>
				N, next_odd<Q>::value
			>::value
	};
};

template <size_t N>
struct prime_test_loop<N, last_prime_to_check<N>::value> {
	enum {	value = (N % last_prime_to_check<N>::value) };
};
#endif

template <size_t N>
struct is_prime {
	// start with first prime less than or equal to...
	enum {	value = reverse_prime_test_loop<N, 
			prev_prime<
				next_odd<sqrt_floor<N>::value>::value
			>::value
		>::value
	};
};

template <>
struct is_prime<0> {	enum { value = false };	};

template <>
struct is_prime<1> {	enum { value = false };	};

template <>
struct is_prime<2> {	enum { value = true };	};

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_INTEGER_TRAITS_H__

