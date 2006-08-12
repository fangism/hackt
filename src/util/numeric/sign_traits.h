/**
	\file "util/numeric/sign_traits.h"
	Template metafunctions for signedness of integer types.  
	$Id: sign_traits.h,v 1.2 2006/08/12 00:36:39 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_SIGN_TRAITS_H__
#define	__UTIL_NUMERIC_SIGN_TRAITS_H__

#include <limits>
#include "util/inttypes.h"
#include "util/utypes.h"

namespace util {
namespace numeric {

/**
	Synonymous wrapper uses static const bool numeric_limits::is_signed
	\param T should be some integer type
 */
template <typename T>
struct is_signed {
	enum { value = std::numeric_limits<T>::is_signed };
};

/**
	Type metafunction.  
	Defined by specializations only.  
 */
template <typename>
struct signed_type { };

/**
	Unsigned type variants.  
 */
template <typename>
struct unsigned_type { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct signed_type<char> {		typedef	char		type; };

template <>
struct signed_type<uchar> {		typedef	char		type; };

template <>
struct unsigned_type<char> {		typedef	uchar		type; };

template <>
struct unsigned_type<uchar> {		typedef	uchar		type; };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct signed_type<short> {		typedef	short		type; };

template <>
struct signed_type<ushort> {		typedef	short		type; };

template <>
struct unsigned_type<short> {		typedef	ushort		type; };

template <>
struct unsigned_type<ushort> {		typedef	ushort		type; };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct signed_type<int> {		typedef	int		type; };

template <>
struct signed_type<uint> {		typedef	int		type; };

template <>
struct unsigned_type<int> {		typedef	uint		type; };

template <>
struct unsigned_type<uint> {		typedef	uint		type; };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct signed_type<long> {		typedef	long		type; };

template <>
struct signed_type<ulong> {		typedef	long		type; };

template <>
struct unsigned_type<long> {		typedef	ulong		type; };

template <>
struct unsigned_type<ulong> {		typedef	ulong		type; };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if SIZEOF_LONG_LONG
template <>
struct signed_type<long long> {		typedef	long long	type; };

template <>
struct signed_type<ulonglong> {		typedef	long long	type; };

template <>
struct unsigned_type<long long> {	typedef	ulonglong	type; };

template <>
struct unsigned_type<ulonglong> {	typedef	ulonglong	type; };
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if defined(HAVE_INT64_TYPE) && defined(HAVE_UINT64_TYPE)
template <>
struct signed_type<int64> {	typedef	int64		type; };

template <>
struct signed_type<uint64> {	typedef	int64		type; };

template <>
struct unsigned_type<int64> {	typedef	uint64		type; };

template <>
struct unsigned_type<uint64> {	typedef	uint64		type; };
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_SIGN_TRAITS_H__

