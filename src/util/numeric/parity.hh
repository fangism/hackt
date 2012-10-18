/**
	\file "util/numeric/parity.hh"
	Parity counter.  
	TODO: fallback implementation should look for popcount, 
	and use its LSB as the result (& 1).  
	$Id: parity.hh,v 1.4 2010/04/19 02:46:17 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_PARITY_H__
#define	__UTIL_NUMERIC_PARITY_H__

#include "util/numeric/inttype_traits.hh"
#if !defined(HAVE_BUILTIN_PARITY)
#include "util/numeric/nibble_tables.hh"
#endif

namespace util {
namespace numeric {

//=============================================================================
template <class T>
struct parity_counter;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Auxiliary implementation when built-in is unavailable.  
	Terminal case for template recursion.
 */
template <>
struct parity_counter<uint8> {
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
#ifdef HAVE_BUILTIN_PARITY
		return char(__builtin_parity(c));
#else	// HAVE_BUILTIN_PARITY
		// unsigned shift is OK
		return nibble_parity[(c >> half_size)]
			^ nibble_parity[c & half_mask];
#endif	// HAVE_BUILTIN_PARITY
	}
};	// end struct parity_counter

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	General implementation for evaluating the position of the MSB.  
	Recursively halves the bit field in question.  
 */
template <class U>
struct parity_counter {
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
		return parity_counter<half_type>()(s >> half_size)
			^ parity_counter<half_type>()(s & half_mask);
	}
};	// end struct parity_counter

template <class U>
const typename parity_counter<U>::half_type
parity_counter<U>::half_mask = half_type(-1);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	SPECIALIZE_PARITY_COUNTER(type, func)				\
template <>								\
struct parity_counter<type> {						\
	typedef	type					arg_type;	\
	char								\
	operator () (const arg_type s) const {				\
		return char(func(s));						\
	}								\
};	// end struct parity_counter

#ifdef HAVE_BUILTIN_PARITY
SPECIALIZE_PARITY_COUNTER(unsigned short, __builtin_parity)
SPECIALIZE_PARITY_COUNTER(unsigned int, __builtin_parity)
#endif
#ifdef HAVE_BUILTIN_PARITYL
SPECIALIZE_PARITY_COUNTER(unsigned long, __builtin_parityl)
#endif
#if SIZEOF_LONG_LONG && defined(HAVE_BUILTIN_PARITYLL)
SPECIALIZE_PARITY_COUNTER(unsigned long long, __builtin_parityll)
#endif
#undef	SPECIALIZE_PARITY_COUNTER

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// template-argument deduction dispatcher
template <class T>
inline
char
parity(const T& v) {
//	const
	parity_counter<T> O;
	return O(v);
//	return parity_counter<T>()(v);	// g++-3.3 chokes
}

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_PARITY_H__

