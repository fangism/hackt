/**
	\file "util/numeric/clz.h"
	Count-leading zero related functions.
	Includes most-significant-bit functions.  
	$Id: clz.h,v 1.3 2008/11/23 17:55:16 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_CLZ_H__
#define	__UTIL_NUMERIC_CLZ_H__

#include "util/numeric/inttype_traits.h"
#if !defined(HAVE_BUILTIN_CLZ)
#include "util/numeric/nibble_tables.h"
#endif

namespace util {
namespace numeric {

/**
	Define to 1 to redefine MSB_position in terms of clz.  
	Rationale: reduce mess of definitions.  
 */
#define	DEFINE_MSB_FROM_CLZ				1

//=============================================================================
template <class T>
struct leading_zeros_counter;

template <>
struct leading_zeros_counter<uint8> {
	typedef	uint8		arg_type;
	enum {	half_size = 4 };
	static const arg_type	half_mask = 0xF;
#ifdef HAVE_BUILTIN_CLZ
	enum {	sizediff = (sizeof(int) -sizeof(arg_type)) << 3 };
#endif
	char
	operator () (const arg_type c) const {
#ifdef HAVE_BUILTIN_CLZ
		// adjust for integer size difference in casting
		// unsigned casting is OK
		return char(__builtin_clz(c) -sizediff);
		// return c ? __builtin_clz(c) -sizediff : sizeof(arg_type) << 3;
		// make 0 -> numbits? no, leave as undefined
#else	// HAVE_BUILTIN_CLZ
		const arg_type upper = c >> half_size;
		return upper ? nibble_LZ[upper] :
			nibble_LZ[c & half_mask] +half_size;
#endif	// HAVE_BUILTIN_CLZ
	}
};	// end struct leading_zeros_counter

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
struct leading_zeros_counter {
	typedef	T				arg_type;
	typedef	typename half_type<arg_type>::type	half_type;
	// number of bytes to number of bits
	enum {	half_size = sizeof(half_type) << 3 };
	static const half_type half_mask;		// = -1
	char
	operator () (const arg_type c) const {
		const half_type upper = c >> half_size;
		return upper ?
			leading_zeros_counter<half_type>()(upper) :
			leading_zeros_counter<half_type>()(c & half_mask) +half_size;
	}
};	// end struct leading_zeros_counter

template <class U>
const typename leading_zeros_counter<U>::half_type
leading_zeros_counter<U>::half_mask = half_type(-1);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	SPECIALIZE_CLZ(type, func)					\
template <>								\
struct leading_zeros_counter<type> {					\
	typedef	type					arg_type;	\
	char								\
	operator () (const arg_type s) const {				\
		/* technically, result is undefined when argument 0 */	\
		return char(func(s));					\
	}								\
};	// end struct leading_zeros_counter

#ifdef HAVE_BUILTIN_CLZ
// unsigned short needs custom specialization
SPECIALIZE_CLZ(unsigned int, __builtin_clz)
#endif
#ifdef HAVE_BUILTIN_CLZL
SPECIALIZE_CLZ(unsigned long, __builtin_clzl)
#endif
#if SIZEOF_LONG_LONG && defined(HAVE_BUILTIN_CLZLL)
SPECIALIZE_CLZ(unsigned long long, __builtin_clzll)
#endif
#undef	SPECIALIZE_CLZ

template <class T>
char
clz(const T v) {
	return leading_zeros_counter<T>()(v);
}

//=============================================================================
#if !DEFINE_MSB_FROM_CLZ
/**
	MSB position template functors.  
	We provide specializations for some basic unsigned types.  
 */
template <class T>
struct MSB_position;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialized implementation of evaluating the position of the MSB.  
 */
template <>
struct MSB_position<uint8> {
	typedef	uint8		arg_type;
	enum {	half_size = 4 };
	enum {	Nbits = sizeof(arg_type) << 3 };
	static const arg_type	half_mask = 0xF;

	/**
		Note that this does NOT check the range of c.
		The caller should guarantee that it is NOT zero.  
		\param c the byte to lookup MSB.
		\pre c must be non-zero.
	 */
	char
	operator () (const arg_type c) const {
#ifdef HAVE_BUILTIN_CLZ
		// for unsigned types, safe to upcast to larger integer
		return Nbits -1 -clz(c);
		// return c ? Nbits -1 -clz(c) : Nbits;
		// leave undefined for arg 0
#else	// HAVE_BUILTIN_CLZ
		return (c > half_mask) ?
			nibble_MSB_position[(c >> half_size)] +half_size :
			nibble_MSB_position[c];
#endif	// HAVE_BUILTIN_CLZ
	}
};	// end struct MSB_position

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	General implementation for evaluating the position of the MSB.  
	Recursively halves the bit field in question.  
 */
template <class U>
struct MSB_position {
	typedef	U					arg_type;
	typedef	typename half_type<arg_type>::type	half_type;
	// number of bytes to number of bits
	enum {	half_size = sizeof(half_type) << 3 };
	enum {	Nbits = sizeof(arg_type) << 3 };
	static const half_type half_mask;		// = -1

	/**
		\param c the int to lookup MSB.
		\pre c must be non-zero.
		\return 0-indexed position of MSB.
	 */
	char
	operator () (const arg_type s) const {
#if 0
		return (s > half_mask) ?
			MSB_position<half_type>()(half_type(s >> half_size))
				+half_size :
			MSB_position<half_type>()(half_type(s));
#else
		return Nbits -1 -clz(s);
#endif
	}
};	// end struct MSB_position

template <class U>
const typename MSB_position<U>::half_type
MSB_position<U>::half_mask = half_type(-1);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	SPECIALIZE_MSB_POSITION(type, func)				\
template <>								\
struct MSB_position<type> {						\
	typedef	type					arg_type;	\
	enum {	Nbits = sizeof(arg_type) << 3 };			\
	char								\
	operator () (const arg_type s) const {				\
		/* technically, result is undefined when argument 0 */	\
		return s ? Nbits -1 -func(s) : Nbits;			\
	}								\
};	// end struct MSB_position

#ifdef HAVE_BUILTIN_CLZ
// can't specialize short this way because counts leading zeros
// SPECIALIZE_MSB_POSITION(unsigned short, __builtin_clz)
SPECIALIZE_MSB_POSITION(unsigned int, __builtin_clz)
#endif
#ifdef HAVE_BUILTIN_CLZL
SPECIALIZE_MSB_POSITION(unsigned long, __builtin_clzl)
#endif
#if SIZEOF_LONG_LONG && defined(HAVE_BUILTIN_CLZLL)
SPECIALIZE_MSB_POSITION(unsigned long long, __builtin_clzll)
#endif
#undef	SPECIALIZE_MSB_POSITION
#endif	// DEFINE_MSB_FROM_CLZ

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	template-argument deduction dispatcher.
	most significant bit position.  
	Undefined if argument is 0.  
 */
template <class T>
inline
char
msb(const T v) {
#if DEFINE_MSB_FROM_CLZ
	return (sizeof(T) << 3) -1 -clz(v);
	// undefined: results in -1 for (v == 0)
#else
	return MSB_position<T>()(v);
#endif
}

/**
	Synonym: find-last-set.
 */
template <class T>
inline
char
fls(const T v) {
	return msb(v);
}

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_CLZ_H__

