/**
	\file "util/numeric/ctz.h"
	Count-trailing zero related functions.
	Includes most-significant-bit functions.  
	$Id: ctz.h,v 1.3 2008/11/23 17:55:16 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_CTZ_H__
#define	__UTIL_NUMERIC_CTZ_H__

#include "util/numeric/inttype_traits.h"
#if !defined(HAVE_BUILTIN_CTZ)
#include "util/numeric/nibble_tables.h"
#endif

namespace util {
namespace numeric {

//=============================================================================
template <class T>
struct trailing_zeros_counter;

template <>
struct trailing_zeros_counter<uint8> {
	typedef	uint8		arg_type;
	enum {	half_size = 4 };
	static const arg_type	half_mask = 0xF;
	char
	operator () (const arg_type c) const {
#ifdef HAVE_BUILTIN_CTZ
		// adjust for integer size difference in casting
		// unsigned casting is OK
		return char(__builtin_ctz(c));
#else	// HAVE_BUILTIN_CTZ
		const arg_type lower = c & half_mask;
		return lower ? nibble_LSB_position[lower] :
			nibble_LSB_position[c >> half_size] +half_size;
#endif	// HAVE_BUILTIN_CTZ
	}
};	// end struct trailing_zeros_counter

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
struct trailing_zeros_counter {
	typedef	T				arg_type;
	typedef	typename half_type<arg_type>::type	half_type;
	// number of bytes to number of bits
	enum {	half_size = sizeof(half_type) << 3 };
	static const half_type half_mask;		// = -1
	char
	operator () (const arg_type c) const {
		const char lowerct =
			trailing_zeros_counter<half_type>()(c & half_mask);
		return (lowerct == half_size) ?
			lowerct +trailing_zeros_counter<half_type>()(c >> half_size)
			: lowerct;
	}
};	// end struct trailing_zeros_counter

template <class U>
const typename trailing_zeros_counter<U>::half_type
trailing_zeros_counter<U>::half_mask = half_type(-1);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	SPECIALIZE_CTZ(type, func)					\
template <>								\
struct trailing_zeros_counter<type> {					\
	typedef	type					arg_type;	\
	char								\
	operator () (const arg_type s) const {				\
		/* technically, result is undefined when argument 0 */	\
		return char(func(s));					\
	}								\
};	// end struct trailing_zeros_counter

#ifdef HAVE_BUILTIN_CTZ
SPECIALIZE_CTZ(unsigned short, __builtin_ctz)
SPECIALIZE_CTZ(unsigned int, __builtin_ctz)
#endif
#ifdef HAVE_BUILTIN_CTZL
SPECIALIZE_CTZ(unsigned long, __builtin_ctzl)
#endif
#if SIZEOF_LONG_LONG && defined(HAVE_BUILTIN_CTZLL)
SPECIALIZE_CTZ(unsigned long long, __builtin_ctzll)
#endif
#undef	SPECIALIZE_CTZ

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// template-argument deduction dispatcher
template <class T>
inline
char
ctz(const T v) {
	trailing_zeros_counter<T> O;
	return O(v);
//	return trailing_zeros_counter<T>()(v);	// g++-3.3 chokes on this
}

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_CTZ_H__

