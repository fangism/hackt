/**
	\file "util/type_traits.hh"
	Snippets of type_traits from TR1 / Boost.  
	Don't want to rely on new compiler (gcc4) or Boost Libraries.  
	Really should make this configuration dependent.  
	TODO: consider relocating this to the STL subdir.  
	TODO: somehow use typedefs to remap deficient traits.  
		Kludge: template typedefs.  
	See also "util/memory/pointer_traits.hh"
	Add more traits as needed.  
	$Id: type_traits.hh,v 1.6 2006/05/06 04:18:58 fang Exp $
 */

#ifndef	__UTIL_TYPE_TRAITS_H__
#define	__UTIL_TYPE_TRAITS_H__

#include "config.h"
#include "util/cppcat.h"
// #include "util/static_assert.hh"

#if 0
// include these once we need them
#if	defined(HAVE_TYPE_TRAITS_H)
#include <type_traits.h>
#endif

#if	defined(HAVE_BITS_TYPE_TRAITS_H)
#include <bits/type_traits.h>
#endif

#if	defined(HAVE_TR1_TYPE_TRAITS)
#include <tr1/type_traits>
#endif

#if	defined(HAVE_BOOST_TYPE_TRAITS_HPP)
#include "boost/type_traits.hpp"
#endif
#endif

namespace util {
//=============================================================================
/**
	Convenient short hand.  
 */
template <class T>
struct identity { typedef T     type; };

template <bool B>
struct bool_value { enum { value = B }; typedef	bool_value type; };

typedef	bool_value<true>	true_value;
typedef	bool_value<false>	false_value;

//=============================================================================
/**
	Template default for non-const type.  
 */
template <class T>
struct remove_const : public identity<T> { };

/**
	Specialization that removes the const from a type.  
 */
template <class T>
struct remove_const<T const> : public identity<T> { };

template <class T>
struct add_const : public identity<T const> { };

template <class T>
struct add_const<T const> : public identity<T const> { };

template <class T>
struct is_const : public false_value { };

template <class T>
struct is_const<T const> : public true_value { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Template default for non-volatile type.  
 */
template <class T>
struct remove_volatile : public identity<T> { };

/**
	Specialization that removes the volatile from a type.  
 */
template <class T>
struct remove_volatile<T volatile> : public identity<T> { };

template <class T>
struct add_volatile : public identity<T volatile> { };

template <class T>
struct add_volatile<T volatile> : public identity<T volatile> { };

template <class T>
struct is_volatile : public false_value { };

template <class T>
struct is_volatile<T volatile> : public true_value { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Could use MPL compose... fancy-schmancy...
 */
template <class T>
struct remove_cv : public remove_const<typename remove_volatile<T>::type> { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
struct remove_reference : public identity<T> { };

template <class T>
struct remove_reference<T&> : public identity<T> { };

template <class T>
struct add_reference : public identity<T&> { };

/**
	Useful for call_traits.  
 */
template <class T>
struct add_reference<T&> : public identity<T&> { };

template <class T>
struct is_reference : public false_value { };

template <class T>
struct is_reference<T&> : public true_value { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
struct remove_pointer : public identity<T> { };

template <class T>
struct remove_pointer<T*> : public identity<T> { };

template <class T>
struct remove_all_pointers : public identity<T> { };

template <class T>
struct remove_all_pointers<T*> :
	public identity<typename remove_all_pointers<T>::type> { };

/**
	Note, this creates pointer-to-pointer-to...
 */
template <class T>
struct add_pointer : public identity<T*> { };

template <class T>
struct is_pointer : public false_value { };

template <class T>
struct is_pointer<T*> : public true_value { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// this could conceivably belong in "util/numeric/inttype_traits.h"...
// NOTE: __is_integral and __is_floating_point (double-underscore) are reserved
// thus, we use triple-underscore

namespace detail {
template <class T>
struct ___is_integral : public false_value { };

template <>
struct ___is_integral<char> : public true_value { };

template <>
struct ___is_integral<unsigned char> : public true_value { };

template <>
struct ___is_integral<short> : public true_value { };

template <>
struct ___is_integral<unsigned short> : public true_value { };

template <>
struct ___is_integral<int> : public true_value { };

template <>
struct ___is_integral<unsigned int> : public true_value { };

template <>
struct ___is_integral<long> : public true_value { };

template <>
struct ___is_integral<unsigned long> : public true_value { };

#if	SIZEOF_LONG_LONG
template <>
struct ___is_integral<long long> : public true_value { };

template <>
struct ___is_integral<unsigned long long> : public true_value { };
#endif
}	// end namespace detail

template <class T>
struct is_integral : public detail::___is_integral<
	typename remove_cv<typename remove_reference<T>::type>::type > { };

// UTIL_STATIC_ASSERT(is_integral<size_t>::value)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace detail {

template <class T>
struct ___is_floating_point : public false_value { };

template <>
struct ___is_floating_point<float> : public true_value { };

template <>
struct ___is_floating_point<double> : public true_value { };

// better not use this for now, if we're issuing warnings about long-double
#if	0 && SIZEOF_LONG_DOUBLE
template <>
struct ___is_floating_point<long double> : public true_value { };
#endif
}	// end namespace detail

template <class T>
struct is_floating_point : public detail::___is_floating_point<
	typename remove_cv<typename remove_reference<T>::type>::type > { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
struct is_arithmetic : public bool_value<
		is_integral<T>::value || is_floating_point<T>::value
	> { };

//=============================================================================
template <class T, class S>
struct is_same : public false_value { };

template <class T>
struct is_same<T, T> : public true_value { };

//=============================================================================
}	// end namespace util

#endif	// __UTIL_TYPE_TRAITS_H__


