/**
	\file "util/numeric/sign_traits.h"
	Template metafunctions for signedness of integer types.  
	$Id: sign_traits.h,v 1.1 2006/07/16 03:34:59 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_SIGN_TRAITS_H__
#define	__UTIL_NUMERIC_SIGN_TRAITS_H__

#include <limits>
#include "util/inttypes.h"

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
struct signed_type<unsigned char> {	typedef	char		type; };

template <>
struct unsigned_type<char> {		typedef	unsigned char	type; };

template <>
struct unsigned_type<unsigned char> {	typedef	unsigned char	type; };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct signed_type<short> {		typedef	short		type; };

template <>
struct signed_type<unsigned short> {	typedef	short		type; };

template <>
struct unsigned_type<short> {		typedef	unsigned short	type; };

template <>
struct unsigned_type<unsigned short> {	typedef	unsigned short	type; };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct signed_type<int> {		typedef	int		type; };

template <>
struct signed_type<unsigned int> {	typedef	int		type; };

template <>
struct unsigned_type<int> {		typedef	unsigned int	type; };

template <>
struct unsigned_type<unsigned int> {	typedef	unsigned int	type; };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct signed_type<long> {		typedef	long		type; };

template <>
struct signed_type<unsigned long> {	typedef	long		type; };

template <>
struct unsigned_type<long> {		typedef	unsigned long	type; };

template <>
struct unsigned_type<unsigned long> {	typedef	unsigned long	type; };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if SIZEOF_LONG_LONG
template <>
struct signed_type<long long> {		typedef	long long	type; };

template <>
struct signed_type<unsigned long long> {	typedef	long long	type; };

template <>
struct unsigned_type<long long> {
	typedef	unsigned long long	type;
};

template <>
struct unsigned_type<unsigned long long> {
	typedef	unsigned long long	type;
};
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

