/**
	\file "util/numeric/inttype_traits.hh"
	Integer type traits.  
	For synthsizing larger integers, see "util/numeric/bigger_ints.hh".
	$Id: inttype_traits.hh,v 1.8 2010/09/02 20:21:26 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_INTTYPE_TRAITS_H__
#define	__UTIL_NUMERIC_INTTYPE_TRAITS_H__

#include "util/size_t.h"
#include "util/inttypes.h"

namespace util {
namespace numeric {

//=============================================================================
/**
	The type of a signed integer with specified width, power of 2 only.  
	Intended to be specialized for arbitrarily integers.  
 */
template <size_t>
struct int_of_size;

/**
	The type of an unsigned integer with specified width, power of 2 only.  
	Intended to be specialized for arbitrarily integers.  
 */
template <size_t>
struct uint_of_size;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <> struct int_of_size<8> {	typedef	int8	type; };
template <> struct int_of_size<16> {	typedef	int16	type; };
template <> struct int_of_size<32> {	typedef	int32	type; };
#ifdef	HAVE_INT64_TYPE
template <> struct int_of_size<64> {	typedef	int64	type; };
#endif

template <> struct uint_of_size<8> {	typedef	uint8	type; };
template <> struct uint_of_size<16> {	typedef	uint16	type; };
template <> struct uint_of_size<32> {	typedef	uint32	type; };
#ifdef	HAVE_UINT64_TYPE
template <> struct uint_of_size<64> {	typedef	uint64	type; };
#endif

//=============================================================================
/**
	Integer type that is half the size of the argument type.  
	Defined by specializations only.  
 */
template <class T>
struct half_type;

/**
	Integer type that is half the size of the argument type.  
	Defined by specializations only.  
 */
template <class T>
struct double_type;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <> struct half_type<int16> {		typedef	int8	type;	};
template <> struct half_type<int32> {		typedef	int16	type;	};
#ifdef	HAVE_INT64_TYPE
template <> struct half_type<int64> {		typedef	int32	type;	};
#endif

template <> struct half_type<uint16> {		typedef	uint8	type;	};
template <> struct half_type<uint32> {		typedef	uint16	type;	};
#if !TYPE_EQUIV_UINT32_UNSIGNED_LONG && !TYPE_EQUIV_UINT64_UNSIGNED_LONG
template <> struct half_type<unsigned long> {	typedef	uint16	type;	};
#endif
#if !TYPE_EQUIV_UINT32_SIZE_T && !TYPE_EQUIV_UINT64_SIZE_T && !TYPE_EQUIV_SIZE_T_UNSIGNED_LONG
template <> struct half_type<size_t> {		typedef	uint16	type;	};
#endif
#ifdef	HAVE_UINT64_TYPE
template <> struct half_type<uint64> {		typedef	uint32	type;	};
#endif

template <> struct double_type<int8> {		typedef	int16	type;	};
template <> struct double_type<int16> {		typedef	int32	type;	};
#ifdef	HAVE_INT64_TYPE
template <> struct double_type<int32> {		typedef	int64	type;	};
#endif

template <> struct double_type<uint8> {		typedef	uint16	type;	};
template <> struct double_type<uint16> {	typedef	uint32	type;	};
#ifdef	HAVE_UINT64_TYPE
template <> struct double_type<uint32> {	typedef	uint64	type;	};
#endif

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_INTTYPE_TRAITS_H__

