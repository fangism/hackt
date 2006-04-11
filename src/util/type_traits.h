/**
	\file "util/type_traits.h"
	Snippets of type_traits from TR1 / Boost.  
	Don't want to rely on new compiler (gcc4) or Boost Libraries.  
	Really should make this configuration dependent.  
	TODO: consider relocating this to the STL subdir.  
	TODO: somehow use typedefs to remap deficient traits.  
		Kludge: template typedefs.  
	See also "util/memory/pointer_traits.h"
	Add more traits as needed.  
	$Id: type_traits.h,v 1.4 2006/04/11 07:54:48 fang Exp $
 */

#ifndef	__UTIL_TYPE_TRAITS_H__
#define	__UTIL_TYPE_TRAITS_H__

#include "config.h"
#include "util/cppcat.h"
// #include "util/static_assert.h"

#if	defined(HAVE_TYPE_TRAITS_H)
#include <type_traits.h>
#endif

#if	defined(HAVE_BITS_TYPE_TRAITS_H)
#include <bits/type_traits.h>
#endif

#if	defined(HAVE_TR1_TYPE_TRAITS_H)
#include <tr1/type_traits.h>
#endif

#if	defined(HAVE_BOOST_TYPE_TRAITS_HPP)
#include "boost/type_traits.hpp"
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

//=============================================================================
template <class T, class S>
struct is_same : public false_value { };

template <class T>
struct is_same<T, T> : public true_value { };

//=============================================================================
}	// end namespace util

#endif	// __UTIL_TYPE_TRAITS_H__


