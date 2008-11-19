/**
	\file "util/numeric/ffs.h"
	Find first set bit (LSB).  
	$Id: ffs.h,v 1.2.26.1 2008/11/19 05:45:09 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_FFS_H__
#define	__UTIL_NUMERIC_FFS_H__

#include "util/numeric/inttype_traits.h"
#if !defined(HAVE_BUILTIN_FFS)
#include "util/numeric/nibble_tables.h"
#endif

namespace util {
namespace numeric {

//=============================================================================
template <class T>
struct first_set_finder;

template <>
struct first_set_finder<uint8> {
	typedef	uint8		arg_type;
	enum {	half_size = 4 };
	static const arg_type	half_mask = 0xF;
	char
	operator () (const arg_type c) const {
#ifdef HAVE_BUILTIN_FFS
		// adjust for integer size difference in casting
		// unsigned casting is OK
		return char(__builtin_ffs(c));
#else	// HAVE_BUILTIN_FFS
		if (c) {
		const arg_type lower = c & half_mask;
		return lower ? nibble_FS_position[lower] :
			nibble_FS_position[c >> half_size] +half_size;
		} else return 0;
#endif	// HAVE_BUILTIN_FFS
	}
};	// end struct first_set_finder

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
struct first_set_finder {
	typedef	T				arg_type;
	typedef	typename half_type<arg_type>::type	half_type;
	// number of bytes to number of bits
	enum {	half_size = sizeof(half_type) << 3 };
	static const half_type half_mask;		// = -1
	char
	operator () (const arg_type c) const {
		if (c) {
		const half_type lower = c & half_mask;
		return lower ?
			first_set_finder<half_type>()(lower) :
			first_set_finder<half_type>()(c >> half_size)
				+half_size;
		} else return 0;
	}
};	// end struct first_set_finder

template <class U>
const typename first_set_finder<U>::half_type
first_set_finder<U>::half_mask = half_type(-1);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	SPECIALIZE_FFS(type, func)					\
template <>								\
struct first_set_finder<type> {						\
	typedef	type					arg_type;	\
	char								\
	operator () (const arg_type s) const {				\
		/* technically, result is undefined when argument 0 */	\
		return char(func(s));					\
	}								\
};	// end struct first_set_finder

#ifdef HAVE_BUILTIN_FFS
SPECIALIZE_FFS(unsigned short, __builtin_ffs)
SPECIALIZE_FFS(unsigned int, __builtin_ffs)
#endif
#ifdef HAVE_BUILTIN_FFSL
SPECIALIZE_FFS(unsigned long, __builtin_ffsl)
#endif
#if SIZEOF_LONG_LONG && defined(HAVE_BUILTIN_FFSLL)
SPECIALIZE_FFS(unsigned long long, __builtin_ffsll)
#endif
#undef	SPECIALIZE_FFS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// template-argument deduction dispatcher
template <class T>
inline
char
ffs(const T v) {
	return first_set_finder<T>()(v);
}

template <class T>
inline
char
lsb(const T v) {
	return ffs(v) -1;
}

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_FFS_H__

