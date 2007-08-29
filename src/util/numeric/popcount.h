/**
	\file "util/numeric/popcount.h"
	Population count of bits.  
	$Id: popcount.h,v 1.1 2007/08/29 04:45:54 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_POPCOUNT_H__
#define	__UTIL_NUMERIC_POPCOUNT_H__

#include "util/numeric/inttype_traits.h"
#if !defined(HAVE_BUILTIN_POPCOUNT)
#include "util/numeric/nibble_tables.h"
#endif

namespace util {
namespace numeric {

//=============================================================================
template <class T>
struct population_counter;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Auxiliary implementation when built-in is unavailable.  
	Terminal case for template recursion.
 */
template <>
struct population_counter<uint8> {
	typedef	uint8		arg_type;
	enum {	half_size = 4 };
	static const arg_type	half_mask = 0xF;

	/**
		Note that this does NOT check the range of c.
		The caller should guarantee that it is NOT zero.  
		\param c the byte to lookup MSB.
		\pre c must be non-zero.
	 */
	char
	operator () (const arg_type c) const {
#if HAVE_BUILTIN_POPCOUNT
		return __builtin_popcount(c);
#else	// HAVE_BUILTIN_POPCOUNT
		// unsigned shift is OK
		return nibble_popcount[(c >> half_size)]
			+nibble_popcount[c & half_mask];
#endif	// HAVE_BUILTIN_POPCOUNT
	}
};	// end struct population_counter

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	General implementation for evaluating the position of the MSB.  
	Recursively halves the bit field in question.  
 */
template <class U>
struct population_counter {
	typedef	U					arg_type;
	typedef	typename half_type<arg_type>::type	half_type;
	// number of bytes to number of bits
	enum {	half_size = sizeof(half_type) << 3 };
	static const half_type half_mask;		// = -1

	/**
		\param c the int to lookup MSB.
		\pre c must be non-zero.
	 */
	char
	operator () (const arg_type s) const {
		return population_counter<half_type>(s >> half_size)
			+population_counter<half_type>(s & half_mask);
	}
};	// end struct population_counter

template <class U>
const typename population_counter<U>::half_type
population_counter<U>::half_mask = half_type(-1);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	SPECIALIZE_POPULATION_COUNTER(type, func)			\
template <>								\
struct population_counter<type> {					\
	typedef	type					arg_type;	\
	char								\
	operator () (const arg_type s) const {				\
		return func(s);						\
	}								\
};	// end struct population_counter

#ifdef HAVE_BUILTIN_POPCOUNT
SPECIALIZE_POPULATION_COUNTER(unsigned short, __builtin_popcount)
SPECIALIZE_POPULATION_COUNTER(unsigned int, __builtin_popcount)
#endif
#ifdef HAVE_BUILTIN_POPCOUNTL
SPECIALIZE_POPULATION_COUNTER(unsigned long, __builtin_popcountl)
#endif
#if SIZEOF_LONG_LONG && defined(HAVE_BUILTIN_POPCOUNTLL)
SPECIALIZE_POPULATION_COUNTER(unsigned long long, __builtin_popcountll)
#endif
#undef	SPECIALIZE_POPULATION_COUNTER

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// template-argument deduction dispatcher
template <class T>
inline
char
popcount(const T v) {
	return population_counter<T>()(v);
}

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_POPCOUNT_H__

