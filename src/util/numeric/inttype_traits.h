/**
	\file "util/numeric/inttype_traits.h"
	Integer type traits.  
	$Id: inttype_traits.h,v 1.1.2.1 2005/08/05 21:08:30 fang Exp $
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
template <> struct int_of_size<64> {	typedef	int64	type; };

template <> struct uint_of_size<8> {	typedef	uint8	type; };
template <> struct uint_of_size<16> {	typedef	uint16	type; };
template <> struct uint_of_size<32> {	typedef	uint32	type; };
template <> struct uint_of_size<64> {	typedef	uint64	type; };

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
template <> struct half_type<int64> {		typedef	int32	type;	};

template <> struct half_type<uint16> {		typedef	uint8	type;	};
template <> struct half_type<uint32> {		typedef	uint16	type;	};
template <> struct half_type<uint64> {		typedef	uint32	type;	};

template <> struct double_type<int8> {		typedef	int16	type;	};
template <> struct double_type<int16> {		typedef	int32	type;	};
template <> struct double_type<int32> {		typedef	int64	type;	};

template <> struct double_type<uint8> {		typedef	uint16	type;	};
template <> struct double_type<uint16> {	typedef	uint32	type;	};
template <> struct double_type<uint32> {	typedef	uint64	type;	};

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_INTTYPE_TRAITS_H__

